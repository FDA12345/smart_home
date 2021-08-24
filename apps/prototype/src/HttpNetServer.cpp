#include "HttpNetServer.h"
#include "logger.h"

#include <boost/asio.hpp>
#include <boost/beast.hpp>

using namespace net_server;
using namespace net_server::http;

using tcp = boost::asio::ip::tcp;


class HttpServer : public net_server::Server
{
public:
	HttpServer()
		: m_acceptor(m_io)//, tcp::endpoint(tcp::v4(), 9898))
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
		boost::system::error_code ec;
		const auto& ep = tcp::endpoint(tcp::v4(), 9898);
		m_acceptor.open(ep.protocol(), ec);
		m_acceptor.bind(ep, ec);
		m_acceptor.listen(-1, ec);
		m_acceptor.accept(ec);// listen(-1, ec);
		return false;
	}

	void Stop() override
	{
	}

private:
	const logger::Ptr m_log = logger::Create();

	boost::asio::io_service m_io;
	boost::asio::ip::tcp::acceptor m_acceptor;
};

Ptr net_server::http::CreateServer()
{
	return std::make_unique<HttpServer>();
}

