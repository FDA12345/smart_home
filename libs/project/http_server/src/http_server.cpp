#include "stdafx.h"

class HttpServer : public net_server::Server
{
public:
	HttpServer(const Params& params)
		: m_params(std::make_shared<Params>(params))
		, m_acceptor(m_io)
	{
	}

	bool RouteAdd(const std::string& routePath, const RouteFn& routeFn) override
	{
		return false;
	}

	bool RouteRemove(const std::string& routePath) override
	{
		return false;
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
		m_acceptor.async_accept(std::bind(&HttpServer::OnAccept, this, std::placeholders::_1, std::placeholders::_2));// // async_accept();
	}

	void OnAccept(const boost::system::error_code& ec, tcp::socket& peer)
	{
		if (ec)
		{
			logERROR(__FUNCTION__, "accept error " << ec.message());
			return;
		}

		auto session = HttpSession::Create(m_params, std::move(peer));
		session->ReadHeader();
	}

private:
	const std::shared_ptr<Params> m_params;
	const logger::Ptr m_log = logger::Create();

	boost::asio::io_service m_io;
	std::vector<std::thread> m_threads;
	tcp::acceptor m_acceptor;
};

Ptr net_server::http::CreateServer(const Params& params)
{
	return std::make_unique<HttpServer>(params);
}
