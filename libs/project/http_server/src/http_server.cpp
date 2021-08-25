#include "http_server.h"
#include "logger.h"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
//#include <boost/bind/bind.hpp>

using namespace net_server;
using namespace net_server::http;

namespace ip = boost::asio::ip;
using tcp = ip::tcp;

namespace beast_http = boost::beast::http;

class HttpSession : public std::enable_shared_from_this<HttpSession>
{
public:
	using Ptr = std::shared_ptr<HttpSession>;

public:
	HttpSession(tcp::socket&& peer)
		: m_stream(std::move(peer))
	{
	}

	void ReadHeader()
	{
		beast_http::async_read_header(m_stream, m_buf, m_parser, std::bind(&HttpSession::OnHeaders, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

private:
	void OnHeaders(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec == beast_http::error::end_of_stream)
		{
			Close();
			return;
		}

		if (ec)
		{
			logERROR(__FUNCTION__, "read header error " << ec.message());
			return;
		}

		ParseHeaders();
	}

	void ParseHeaders()
	{
		const auto& req = m_parser.get();
		const auto& method = req.method();
		const auto& resource = req.target().to_string();

		ReadBody();
	}

	void Close()
	{
		boost::beast::error_code ec;
		m_stream.socket().shutdown(tcp::socket::shutdown_send, ec);
	}

	void ReadBody()
	{
		beast_http::async_read_some(m_stream, m_buf, m_parser, std::bind(&HttpSession::OnBody, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void OnBody(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec == beast_http::error::end_of_stream)
		{
			Close();
			return;
		}

		if (ec)
		{
			logERROR(__FUNCTION__, "read body error " << ec.message());
			return;
		}

		ParseBody();
	}

	void ParseBody()
	{
	}

	void WriteResponse()
	{
	}

private:
	const logger::Ptr m_log = logger::Create();
	boost::beast::tcp_stream m_stream;

	beast_http::request_parser<beast_http::string_body> m_parser;
	boost::beast::flat_buffer m_buf;
};


class HttpServer : public net_server::Server
{
public:
	HttpServer(const Params& params)
		: m_params(params)
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

			const auto& address = m_params.address.empty() ? "0.0.0.0" : m_params.address;
			logINFO(__FUNCTION__, "starting http server at " << address << ":" << m_params.port);

			const auto& addr = ip::make_address(address, ec);
			if (ec)
			{
				logERROR(__FUNCTION__, "address " << address << " not parsed");
				break;
			}

			const auto& ep = tcp::endpoint(addr, m_params.port);

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

			m_threadIo = std::thread
			{ [this]()
			{
				m_io.run();
			} };

			return true;
		}

		Stop();
		return false;
	}

	void Stop() override
	{
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

		HttpSession::Ptr session = std::make_shared<HttpSession>(std::move(peer));
		session->ReadHeader();
	}

private:
	const Params m_params;
	const logger::Ptr m_log = logger::Create();

	boost::asio::io_service m_io;
	std::thread m_threadIo;
	tcp::acceptor m_acceptor;
};

Ptr net_server::http::CreateServer(const Params& params)
{
	return std::make_unique<HttpServer>(params);
}
