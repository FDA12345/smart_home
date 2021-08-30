#include "stdafx.h"

#include "http_request.h"
#include "http_response.h"

class HttpSessionImpl
	: public HttpSession
	, public std::enable_shared_from_this<HttpSessionImpl>
{
public:
	using Ptr = std::shared_ptr<HttpSession>;

public:
	HttpSessionImpl(RouteFn routeFn, const std::shared_ptr<Params>& params, tcp::socket&& peer)
		: m_routeFn(routeFn)
		, m_params(params)
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
		boost::beast::net::dispatch(m_stream.get_executor(),
			std::bind(&HttpSessionImpl::OnReadHeaders, shared_from_this()));
	}

private:
	void OnReadHeaders()
	{
		m_stream.expires_after(std::chrono::seconds(30));

		//for new request we resetting parser aka new operation
		m_parser = std::make_unique<beast_http::request_parser<beast_http::string_body>>();

		beast_http::async_read_header(m_stream, m_buf, *m_parser, std::bind(&HttpSessionImpl::OnHeaders, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void OnHeaders(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
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

		const auto& parserReq = m_parser->get();

		auto req = CreateRequest();
		req->Version(parserReq.version());
		req->Method(parserReq.method_string().to_string());
		req->Route(parserReq.target().to_string());
		req->Payload({ &parserReq.body()[0], &parserReq.body()[parserReq.body().size()] });

		for (const auto& f : parserReq)
		{
			req->Headers().emplace_back(f.name_string().to_string(), f.value().to_string());
		}

		auto serverErrorRspMsg = PrepareResponseMsg(parserReq, CreateResponseMsg({}), ResultCodes::CODE_INTERNAL_ERROR).second;

		auto rsp = CreateResponse();
		if (!m_routeFn(*req, *rsp))
		{
			WriteResponseMsg(std::move(serverErrorRspMsg));
			return;
		}


		if (rsp->Payload().empty())
		{
			auto&& preparedRspMsg = PrepareResponseMsg(parserReq, CreateResponseMsg(rsp->Headers()), rsp->Result());
			WriteResponseMsg(preparedRspMsg.first ? std::move(preparedRspMsg.second) : std::move(serverErrorRspMsg));
		}
		else
		{
			auto&& preparedRspMsg = PrepareResponseMsg(parserReq, CreateResponseMsg(rsp->Headers(), rsp->Payload()), rsp->Result());
			if (preparedRspMsg.first)
			{
				WriteResponseMsg(std::move(preparedRspMsg.second));
			}
			else
			{
				WriteResponseMsg(std::move(serverErrorRspMsg));
			}
		}
	}

	template <typename ResponseMsg>
	void WriteResponseMsg(ResponseMsg&& rspMsg)
	{
		logINFO(__FUNCTION__, "write response msg");

		using type = std::remove_reference_t<decltype(rspMsg)>;
		std::shared_ptr<type::element_type> sharedRspMsg = std::move(rspMsg);

		beast_http::async_write(m_stream, *sharedRspMsg, std::bind(&HttpSessionImpl::OnWriteMsg<decltype(sharedRspMsg)>,
			shared_from_this(), sharedRspMsg, std::placeholders::_1, std::placeholders::_2));
	}

	template <typename ResponseMsg>
	void OnWriteMsg(const ResponseMsg& rspMsg, const boost::system::error_code& ec, size_t transferred)
	{
		logINFO(__FUNCTION__, "on write msg");
		if (ec)
		{
			logERROR(__FUNCTION__, "write error " << ec.message());
			return;
		}

		if (rspMsg->need_eof())
		{
			Close();
			return;
		}

		OnReadHeaders();
	}

	template <typename Request, typename Response>
	std::pair<bool, Response> PrepareResponseMsg(const Request& req, Response&& rsp, const ResultCodes result)
	{
		bool ok = true;
		rsp->set(beast_http::field::server, m_params->serverName);

		switch (result)
		{
		case ResultCodes::CODE_OK:				rsp->result(beast_http::status::ok); break;
		case ResultCodes::CODE_NOT_FOUND:		rsp->result(beast_http::status::not_found); break;
		case ResultCodes::CODE_INTERNAL_ERROR:	rsp->result(beast_http::status::internal_server_error); break;
		case ResultCodes::CODE_BUSY:			rsp->result(beast_http::status::service_unavailable); break;
		default:
			logERROR(__FUNCTION__, "unsupported result code " << size_t(result));
			ok = false;
			break;
		}

		rsp->version(req.version());
		rsp->keep_alive(req.keep_alive());

		return std::make_pair(ok, std::move(rsp));
	}

	static std::unique_ptr<beast_http::message<false, beast_http::string_body>> CreateResponseMsg(const HeaderList& headers, const std::string_view& body)
	{
		auto rsp = CreateResponseMsgImpl<beast_http::string_body>(headers);

		rsp->body() = body;
		rsp->prepare_payload();
		rsp->content_length(rsp->body().size());

		return std::move(rsp);
	}

	static std::unique_ptr<beast_http::message<false, beast_http::empty_body>> CreateResponseMsg(const HeaderList& headers)
	{
		return CreateResponseMsgImpl<beast_http::empty_body>(headers);
	}

	template <typename BodyType>
	static std::unique_ptr<beast_http::message<false, BodyType>> CreateResponseMsgImpl(const HeaderList& headers)
	{
		auto& rsp = std::make_unique<beast_http::message<false, BodyType>>();

		for (const auto h : headers)
		{
			rsp->set(h.name, h.value);
		}

		return std::move(rsp);
	}

private:
	const logger::Ptr m_log = logger::Create();

	const RouteFn m_routeFn;
	const std::shared_ptr<Params> m_params;
	boost::beast::tcp_stream m_stream;

	std::unique_ptr<beast_http::request_parser<beast_http::string_body>> m_parser;
	boost::beast::flat_buffer m_buf;
};



HttpSession::Ptr HttpSession::Create(RouteFn routeFn, const std::shared_ptr<Params>& params, tcp::socket&& peer)
{
	return std::make_shared<HttpSessionImpl>(routeFn, params, std::move(peer));
}
