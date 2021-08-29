#include "stdafx.h"
#include "json_http_server.h"

#include "rapidjson/document.h"

class JsonRequest : public net_server::Request
{
public:
	const std::string& Route() const override
	{
	}

	const std::string_view& Payload() const override
	{
		return m_payloadView;
	}

private:
	const std::string m_route;

	const std::string m_payload;
	const std::string_view m_payloadView;
};


class JsonHttpServer : public net_server::Server
{
public:
	JsonHttpServer(const net_server::http::Params& params)
		: m_httpServer(net_server::http::CreateServer(params))
	{
	}

	bool RouteAdd(const std::string& routePath, RouteFn routeFn) override
	{
		return m_httpServer->RouteAdd(routePath, routeFn);
	}

	bool RouteRemove(const std::string& routePath) override
	{
		return m_httpServer->RouteRemove(routePath);
	}

	bool Start() override
	{
		return m_httpServer->Start();
	}

	void Stop() override
	{
		m_httpServer->Stop();
	}

private:
	net_server::Ptr m_httpServer;
};

net_server::Ptr net_server::http::json::CreateServer(const net_server::http::Params& params)
{
	return std::make_unique<JsonHttpServer>(params);
}