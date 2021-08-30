#include "stdafx.h"
#include "http_request.h"

class HttpRequestFullAccessImpl : public HttpRequestFullAccess
{
public:
	//Request
	const std::string& Type() const override
	{
		return ReqRspType;
	}

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


	//HttpRequestFullAccess
	void Route(const std::string& route) override
	{
		m_route = route;
	}

	void Payload(std::vector<char>&& payload) override
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

	void Method(const std::string& method) override
	{
		m_method = method;
	}

	void Version(int version) override
	{
		m_version = version;
	}

	HeaderList& Headers() override
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

net_server::http::HttpRequestPtr net_server::http::CreateRequest()
{
	return std::make_unique<HttpRequestFullAccessImpl>();
}
