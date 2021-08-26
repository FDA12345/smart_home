#include "stdafx.h"

//#include <boost/beast/core.hpp>
//#include <boost/bind/bind.hpp>

class HttpSessionImpl
	: public HttpSession
	, private std::enable_shared_from_this<HttpSessionImpl>
{
public:
	using Ptr = std::shared_ptr<HttpSession>;

public:
	HttpSessionImpl(const std::shared_ptr<Params>& params, tcp::socket&& peer)
		: m_params(params)
		, m_stream(std::move(peer))
	{
	}

	void ReadHeader()
	{
		beast_http::async_read_header(m_stream, m_buf, m_parser, std::bind(&HttpSessionImpl::OnHeaders, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
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
		/*
		const auto& req = m_parser.get();
		const auto& method = req.method();
		const auto& resource = req.target().to_string();
		*/

		ReadBody();
	}

	void Close()
	{
		boost::beast::error_code ec;
		m_stream.socket().shutdown(tcp::socket::shutdown_send, ec);
	}

	void ReadBody()
	{
		beast_http::async_read_some(m_stream, m_buf, m_parser, std::bind(&HttpSessionImpl::OnBody, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
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
		const auto& req = m_parser.get();
		const auto& method = req.method();
		const auto& resource = req.target().to_string();

		auto rsp = PrepareResponse(req, CreateResponse("text/html", "route path not found"), beast_http::status::not_found);

		//beast_http::async_write(m_stream, *rsp, std::bind(&HttpSessionImpl::OnWrite<decltype(rsp)>, shared_from_this(), rsp, std::placeholders::_1, std::placeholders::_2));
		//beast_http::async_write(m_stream, *rsp, boost::bind(&HttpSessionImpl::OnWrite2, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2));

		//beast_http::async_write(m_stream, *rsp.get(),
		//	boost::beast::bind_front_handler(
		//		&HttpSessionImpl::OnWrite2,
		//		shared_from_this()));

		//WriteResponse(std::move(rsp));
	}

	template <typename Response>
	void WriteResponse(Response&& rsp)
	{
		beast_http::async_write(m_stream, *rsp,
			boost::beast::bind_front_handler(
				&HttpSessionImpl::OnWrite2,
				shared_from_this()));
	}

	template <typename Response>
	void OnWrite(const Response& rsp, const boost::system::error_code& ec, size_t transferred)
	{
	}

	void OnWrite2(const boost::system::error_code& ec, size_t transferred)
	{
	}

	template <typename Request, typename Response>
	Response PrepareResponse(const Request& req, Response&& rsp, const beast_http::status status)
	{
		rsp->set(beast_http::field::server, m_params->serverName);

		rsp->result(status);
		rsp->version(req.version());
		rsp->keep_alive(req.keep_alive());

		WriteResponse(std::move(rsp));

		return std::move(rsp);
	}

	static std::unique_ptr<beast_http::message<false, beast_http::string_body>> CreateResponse(const std::string& contentType, const std::string_view& body)
	{
		auto rsp = CreateResponseImpl<beast_http::string_body>();

		rsp->set(beast_http::field::content_type, contentType);

		rsp->body() = body;
		rsp->prepare_payload();

		return std::move(rsp);
	}

	static std::unique_ptr<beast_http::message<false, beast_http::empty_body>> CreateResponse()
	{
		return CreateResponseImpl<beast_http::empty_body>();
	}

	template <typename BodyType>
	static std::unique_ptr<beast_http::message<false, BodyType>> CreateResponseImpl()
	{
		return std::make_unique<beast_http::message<false, BodyType>>();
	}

private:
	const logger::Ptr m_log = logger::Create();

	const std::shared_ptr<Params> m_params;
	boost::beast::tcp_stream m_stream;

	beast_http::request_parser<beast_http::string_body> m_parser;
	boost::beast::flat_buffer m_buf;
};



HttpSession::Ptr HttpSession::Create(const std::shared_ptr<Params>& params, tcp::socket&& peer)
{
	return std::make_shared<HttpSessionImpl>(params, std::move(peer));
}
