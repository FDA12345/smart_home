#include "stdafx.h"
#include "http_response.h"

using namespace net_server::http;

class HttpResponseFullAccessImpl : public HttpResponseFullAccess
{
public:
	const std::string& Type() const override
	{
		return ReqRspType;
	}

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


	//HttpResponseFullAccess
	void Route(const std::string& route) override
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


net_server::http::HttpResponsePtr net_server::http::CreateResponse()
{
	return std::make_unique<HttpResponseFullAccessImpl>();
}
