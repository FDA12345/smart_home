#include "http_server.h"
#include "logger.h"

#include <boost/asio.hpp>
#include <boost/beast.hpp>

using namespace net_server;
using namespace net_server::http;

namespace ip = boost::asio::ip;
using tcp = ip::tcp;


class HttpServer : public net_server::Server
{
public:
	HttpServer(const Params& params)
		: m_params(params)
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

			const auto& address = m_params.address.empty() ? "0.0.0.0" : m_params.address;
			logINFO(__FUNCTION__, "starting http server at " << address << ":" << m_params.port);

			const auto& addr = ip::make_address(address, ec);
			if (ec)
			{
				logERROR(__FUNCTION__, "address " << address << " not parsed");
				break;
			}

			const auto& ep = tcp::endpoint(addr, m_params.port);

			m_io = std::make_unique<boost::asio::io_service>();
			m_acceptor = std::make_unique<tcp::acceptor>(*m_io);

			m_acceptor->open(ep.protocol(), ec);
			if (ec)
			{
				logERROR(__FUNCTION__, "acceptor open error " << ec.message());
				break;
			}

			m_acceptor->bind(ep, ec);
			if (ec)
			{
				logERROR(__FUNCTION__, "acceptor bind error " << ec.message());
				break;
			}

			m_acceptor->listen(boost::asio::socket_base::max_listen_connections, ec);
			if (ec)
			{
				logERROR(__FUNCTION__, "acceptor listen error " << ec.message());
				break;
			}

			return true;
		}
		//m_acceptor.accept(ec);// listen(-1, ec);

		Stop();
		return false;
	}

	void Stop() override
	{
	}

private:
	const Params m_params;
	const logger::Ptr m_log = logger::Create();

	std::unique_ptr<boost::asio::io_service> m_io;
	std::unique_ptr<tcp::acceptor> m_acceptor;
};

Ptr net_server::http::CreateServer(const Params& params)
{
	return std::make_unique<HttpServer>(params);
}
