#include "stdafx.h"


class HttpRequestImpl : public HttpRequest
{
public:
	//Request
	const std::string& Route() const override
	{
		return m_route;
	}

	const std::string_view& Payload() const override
	{
		return m_payloadView;
	}


	//HttpRequest
	const std::string& Method() const override
	{
		return m_method;
	}

	int Version() const override
	{
		return m_version;
	}

	const HeaderList& Headers() const override
	{
		return m_headers;
	}


	//own methods
	void Route(const std::string& route)
	{
		m_route = route;
	}

	void Payload(std::vector<char>&& payload)
	{
		m_payload = std::move(payload);

		if (!m_payload.empty())
		{
			m_payloadView = std::string_view(&m_payload[0], m_payload.size());
		}
		else
		{
			m_payloadView = std::string_view();
		}
	}

	void Method(const std::string& method)
	{
		m_method = method;
	}

	void Version(int version)
	{
		m_version = version;
	}

	HeaderList& Headers()
	{
		return m_headers;
	}

private:
	std::string m_method;
	int m_version = 11;
	HeaderList m_headers;
	std::string m_route;
	std::vector<char> m_payload;
	std::string_view m_payloadView;
};

class HttpResponseImpl : public HttpResponse
{
public:
	//Response
	const std::string& Route() const override
	{
		return m_route;
	}

	ResultCodes Result() const override
	{
		return m_result;
	}

	void Result(ResultCodes code) override
	{
		m_result = code;
	}

	const std::string& ResultMsg() const override
	{
		return m_resultMsg;
	}

	void ResultMsg(const std::string& msg) override
	{
		m_resultMsg = msg;
	}

	const std::string_view& Payload() const override
	{
		return m_payloadView;
	}

	void Payload(const std::string_view& payload) override
	{
		m_payload = { payload.data(), payload.data() + payload.size() };

		if (!m_payload.empty())
		{
			m_payloadView = std::string_view(&m_payload[0], m_payload.size());
		}
		else
		{
			m_payloadView = std::string_view();
		}
	}


	//HttpResponse
	int Version() const override
	{
		return m_version;
	}

	void Version(int version) override
	{
		m_version = version;
	}

	const HeaderList& Headers() const override
	{
		return m_headers;
	}

	HeaderList& Headers() override
	{
		return m_headers;
	}


	//own methods
	void Route(const std::string& route)
	{
		m_route = route;
	}

private:
	int m_version = 11;
	HeaderList m_headers;
	std::string m_route;
	std::vector<char> m_payload;
	std::string_view m_payloadView;
	ResultCodes m_result = ResultCodes::CODE_OK;
	std::string m_resultMsg;
};



class HttpSessionImpl
	: public HttpSession
	, public std::enable_shared_from_this<HttpSessionImpl>
{
public:
	using Ptr = std::shared_ptr<HttpSession>;

public:
	HttpSessionImpl(RequestFn requestFn, const std::shared_ptr<Params>& params, tcp::socket&& peer)
		: m_requestFn(requestFn)
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

		HttpRequestImpl req;
		req.Version(parserReq.version());
		req.Method(parserReq.method_string().to_string());
		req.Route(parserReq.target().to_string());
		req.Payload({ &parserReq.body()[0], &parserReq.body()[parserReq.body().size()] });

		for (const auto& f : parserReq)
		{
			req.Headers().emplace_back(f.name_string().to_string(), f.value().to_string());
		}

		auto serverErrorRsp = PrepareResponse(parserReq, CreateResponse({}), ResultCodes::CODE_INTERNAL_ERROR).second;

		HttpResponseImpl rsp;
		if (!m_requestFn(req, rsp))
		{
			WriteResponse(std::move(serverErrorRsp));
			return;
		}


		if (rsp.Payload().empty())
		{
			auto&& preparedRsp = PrepareResponse(parserReq, CreateResponse(rsp.Headers()), rsp.Result());
			WriteResponse(preparedRsp.first ? std::move(preparedRsp.second) : std::move(serverErrorRsp));
		}
		else
		{
			auto&& preparedRsp = PrepareResponse(parserReq, CreateResponse(rsp.Headers(), rsp.Payload()), rsp.Result());
			if (preparedRsp.first)
			{
				WriteResponse(std::move(preparedRsp.second));
			}
			else
			{
				WriteResponse(std::move(serverErrorRsp));
			}
		}
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
	std::pair<bool, Response> PrepareResponse(const Request& req, Response&& rsp, const ResultCodes result)
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

	static std::unique_ptr<beast_http::message<false, beast_http::string_body>> CreateResponse(const HeaderList& headers, const std::string_view& body)
	{
		auto rsp = CreateResponseImpl<beast_http::string_body>(headers);

		rsp->body() = body;
		rsp->prepare_payload();
		rsp->content_length(rsp->body().size());

		return std::move(rsp);
	}

	static std::unique_ptr<beast_http::message<false, beast_http::empty_body>> CreateResponse(const HeaderList& headers)
	{
		return CreateResponseImpl<beast_http::empty_body>(headers);
	}

	template <typename BodyType>
	static std::unique_ptr<beast_http::message<false, BodyType>> CreateResponseImpl(const HeaderList& headers)
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

	const RequestFn m_requestFn;
	const std::shared_ptr<Params> m_params;
	boost::beast::tcp_stream m_stream;

	std::unique_ptr<beast_http::request_parser<beast_http::string_body>> m_parser;
	boost::beast::flat_buffer m_buf;
};



HttpSession::Ptr HttpSession::Create(RequestFn requestFn, const std::shared_ptr<Params>& params, tcp::socket&& peer)
{
	return std::make_shared<HttpSessionImpl>(requestFn, params, std::move(peer));
}
