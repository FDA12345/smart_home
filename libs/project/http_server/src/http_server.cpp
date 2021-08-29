#include "stdafx.h"

class HttpServer : public Server
{
public:
	HttpServer(const Params& params)
		: m_params(std::make_shared<Params>(params))
		, m_acceptor(boost::beast::net::make_strand(m_io))
	{
	}

	bool RouteAdd(const std::string& routePath, RouteFn routeFn) override
	{
		std::lock_guard lock(*m_mx);

		auto it = m_routes->find(routePath);
		if (it != m_routes->end())
		{
			return false;
		}

		m_routes->emplace(std::make_pair(routePath, routeFn));
		return true;
	}

	bool RouteRemove(const std::string& routePath) override
	{
		std::lock_guard lock(*m_mx);

		auto it = m_routes->find(routePath);
		if (it == m_routes->end())
		{
			return false;
		}

		m_routes->erase(it);
		return true;
	}

	bool Start() override
	{
		Stop();

		while (true)
		{
			boost::system::error_code ec;

			const auto& address = m_params->address.empty() ? "0.0.0.0" : m_params->address;
			logINFO(__FUNCTION__, "starting http server at " << address << ":" << m_params->port);

			const auto& addr = ip::make_address(address, ec);
			if (ec)
			{
				logERROR(__FUNCTION__, "address " << address << " not parsed");
				break;
			}

			const auto& ep = tcp::endpoint(addr, m_params->port);

			m_acceptor.open(ep.protocol(), ec);
			if (ec)
			{
				logERROR(__FUNCTION__, "acceptor open error " << ec.message());
				break;
			}

			m_acceptor.bind(ep, ec);
			if (ec)
			{
				logERROR(__FUNCTION__, "acceptor bind error " << ec.message());
				break;
			}

			m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
			if (ec)
			{
				logERROR(__FUNCTION__, "acceptor listen error " << ec.message());
				break;
			}

			StartAccept();

			for (size_t i = 0; i < m_params->threadsCount; ++i)
			{
				m_threads.emplace_back([this]() { m_io.run(); });
			}

			return true;
		}

		Stop();
		return false;
	}

	void Stop() override
	{
		if (m_acceptor.is_open())
		{
			m_acceptor.close();
		}

		if (!m_io.stopped())
		{
			m_io.stop();
		}

		for (auto& t : m_threads)
		{
			t.join();
		}
		m_threads.clear();

		m_io.reset();
	}

private:
	void StartAccept()
	{
		m_acceptor.async_accept(boost::beast::net::make_strand(m_io),
			std::bind(&HttpServer::OnAccept, this, std::placeholders::_1, std::placeholders::_2));// // async_accept();
	}

	void OnAccept(const boost::system::error_code& ec, tcp::socket&& peer)
	{
		if (ec)
		{
			logERROR(__FUNCTION__, "accept error " << ec.message());
			return;
		}

		auto requestFn = [mx = m_mx, routes = m_routes](const HttpRequest& req, HttpResponse& rsp)
		{
			return OnRequest(*mx, *routes, req, rsp);
		};

		auto session = HttpSession::Create(std::move(requestFn), m_params, std::move(peer));
		session->Run();

		StartAccept();
	}

	static bool OnRequest(std::mutex& mx, const std::map<std::string, RouteFn>& routes, const HttpRequest& req, HttpResponse& rsp)
	{
		std::lock_guard lock(mx);

		auto it = routes.find(req.Route());
		if (it == routes.end())
		{
			rsp.Result(ResultCodes::CODE_NOT_FOUND);
			return true;
		}

		return it->second(req, rsp);
	}

private:
	const std::shared_ptr<Params> m_params;
	const logger::Ptr m_log = logger::Create();

	std::shared_ptr<std::mutex> m_mx = std::make_shared<std::mutex>();
	std::shared_ptr<std::map<std::string, RouteFn>> m_routes = std::make_shared<std::map<std::string, RouteFn>>();

	boost::asio::io_service m_io;
	std::vector<std::thread> m_threads;
	tcp::acceptor m_acceptor;
};

Ptr net_server::http::CreateServer(const Params& params)
{
	return std::make_unique<HttpServer>(params);
}
