#include "stdafx.h"

class HttpSessionImpl
	: public HttpSession
	, public std::enable_shared_from_this<HttpSessionImpl>
{
public:
	using Ptr = std::shared_ptr<HttpSession>;

public:
	HttpSessionImpl(const std::shared_ptr<Params>& params, tcp::socket&& peer)
		: m_params(params)
		, m_stream(std::move(peer))
	{
		logINFO(__FUNCTION__, "c_tor");
	}

	~HttpSessionImpl()
	{
		logINFO(__FUNCTION__, "d_tor");
	}

	void Run()
	{
		logINFO(__FUNCTION__, "read header");
		boost::beast::net::dispatch(m_stream.get_executor(),
			std::bind(&HttpSessionImpl::OnReadHeaders, shared_from_this()));
	}

private:
	void OnReadHeaders()
	{
		logINFO(__FUNCTION__, "on read header");
		m_stream.expires_after(std::chrono::seconds(30));

		//for new request we resetting parser aka new operation
		m_parser = std::make_unique<beast_http::request_parser<beast_http::string_body>>();

		beast_http::async_read_header(m_stream, m_buf, *m_parser, std::bind(&HttpSessionImpl::OnHeaders, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void OnHeaders(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		logINFO(__FUNCTION__, "on headers, bytes_transferred " << bytes_transferred);
		if (ec == beast_http::error::end_of_stream)
		{
			Close();
			return;
		}

		if (ec)
		{
			logERROR(__FUNCTION__, "read headers error - " << ec.message());
			return;
		}

		if (bytes_transferred == 0)
		{
			logERROR(__FUNCTION__, "read headers error - bytes_transferred == 0");
			return;
		}

		ParseHeaders();
	}

	void ParseHeaders()
	{
		logINFO(__FUNCTION__, "parse header");

		const auto& req = m_parser->get();
		const auto& method = req.method();
		const auto& resource = req.target().to_string();

		logINFO(__FUNCTION__, "resource " << resource << ", method " << method);

		ReadBody();
	}

	void Close()
	{
		logINFO(__FUNCTION__, "close");

		boost::beast::error_code ec;
		m_stream.socket().shutdown(tcp::socket::shutdown_send, ec);
	}

	void ReadBody()
	{
		logINFO(__FUNCTION__, "read body");
		beast_http::async_read_some(m_stream, m_buf, *m_parser, std::bind(&HttpSessionImpl::OnBody, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void OnBody(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		logINFO(__FUNCTION__, "on body");
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
		logINFO(__FUNCTION__, "parse body");
		const auto& req = m_parser->get();
		const auto& method = req.method();
		const auto& resource = req.target().to_string();

		static int counter = 0;
		WriteResponse(PrepareResponse(req, CreateResponse("text/html", "route path not found - " + std::to_string(++counter)), beast_http::status::not_found));
		//WriteResponse(PrepareResponse(req, CreateResponse(), beast_http::status::not_found));
	}

	template <typename Response>
	void WriteResponse(Response&& rsp)
	{
		logINFO(__FUNCTION__, "write response");
		using type = std::remove_reference_t<decltype(rsp)>;
		std::shared_ptr<type::element_type> sharedRsp = std::move(rsp);

		beast_http::async_write(m_stream, *sharedRsp, std::bind(&HttpSessionImpl::OnWrite<decltype(sharedRsp)>,
			shared_from_this(), sharedRsp, std::placeholders::_1, std::placeholders::_2));
	}

	template <typename Response>
	void OnWrite(const Response& rsp, const boost::system::error_code& ec, size_t transferred)
	{
		logINFO(__FUNCTION__, "on write");
		if (ec)
		{
			logERROR(__FUNCTION__, "write error " << ec.message());
			return;
		}

		if (rsp->need_eof())
		{
			Close();
			return;
		}

		OnReadHeaders();
	}

	template <typename Request, typename Response>
	Response PrepareResponse(const Request& req, Response&& rsp, const beast_http::status result)
	{
		rsp->set(beast_http::field::server, m_params->serverName);

		rsp->result(result);
		rsp->version(req.version());
		rsp->keep_alive(req.keep_alive());

		return std::move(rsp);
	}

	static std::unique_ptr<beast_http::message<false, beast_http::string_body>> CreateResponse(const std::string& contentType, const std::string_view& body)
	{
		auto rsp = CreateResponseImpl<beast_http::string_body>();

		rsp->set(beast_http::field::content_type, contentType);

		rsp->body() = body;
		rsp->prepare_payload();
		rsp->content_length(rsp->body().size());

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

	std::unique_ptr<beast_http::request_parser<beast_http::string_body>> m_parser;
	boost::beast::flat_buffer m_buf;
};



HttpSession::Ptr HttpSession::Create(const std::shared_ptr<Params>& params, tcp::socket&& peer)
{
	return std::make_shared<HttpSessionImpl>(params, std::move(peer));
}
