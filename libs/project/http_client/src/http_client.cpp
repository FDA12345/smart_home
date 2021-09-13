#include "stdafx.h"
#include "json_http_client.h"
#include "logger.h"

/*
#include <algorithm>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"

std::string net_server::http::json::ReqRspType = "JSON";

using namespace net_server::http;

class JsonRequest : public net_server::Request
{
public:
	JsonRequest(std::string&& route, std::string&& payload)
		: m_route(std::move(route))
		, m_payload(std::move(payload))
		, m_payloadView(&m_payload[0], m_payload.size())
	{
	}

	const std::string& Type() const override
	{
		return net_server::http::json::ReqRspType;
	}

	const std::string& Route() const override
	{
		return m_route;
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


class JsonResponse : public net_server::Response
{
public:
	JsonResponse(const std::string& route)
		: m_route(route)
	{
	}

	const std::string& Type() const override
	{
		return net_server::http::json::ReqRspType;
	}

	const std::string& Route() const override
	{
		return m_route;
	}

	net_server::ResultCodes Result() const override
	{
		return m_result;
	}

	void Result(net_server::ResultCodes result) override
	{
		m_result = result;
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
		m_payload = std::string(payload.data(), payload.size());
		m_payloadView = std::string_view(&m_payload[0], m_payload.size());
	}

private:
	const std::string m_route;
	net_server::ResultCodes m_result = net_server::ResultCodes::CODE_OK;
	std::string m_resultMsg;
	std::string m_payload;
	std::string_view m_payloadView;
};



class JsonHttpServer : public net_server::Server
{
public:
	JsonHttpServer(const Params& params)
		: m_httpServer(CreateServer(params))
	{
	}

	bool RouteAdd(const std::string& routePath, net_server::RouteFn routeFn) override
	{
		auto jsonRouteFn = [routeFn](const net_server::Request& req, net_server::Response& rsp)
		{
			if (
				(req.Type() != net_server::http::ReqRspType) ||
				(rsp.Type() != net_server::http::ReqRspType)
				)
			{
				return false;
			}

			auto& httpReq = static_cast<const HttpRequest&>(req);
			auto& httpRsp = static_cast<HttpResponse&>(rsp);

			return OnHttpRoute(routeFn, httpReq, httpRsp);
		};

		return m_httpServer->RouteAdd(routePath, std::move(jsonRouteFn));
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
	static bool OnHttpRoute(const net_server::RouteFn& routeFn, const net_server::http::HttpRequest& req, net_server::http::HttpResponse& rsp)
	{
		return ProcessJsonRequest(routeFn, req, rsp);

		if (req.Method() == "POST")
		{
			const auto& headers = req.Headers();
			if (ValidateHeaders(headers,
					{
						{ "Content-Type", "application/json" },
						{ "Accept", "application/json" },
					}
				))
			{
				return ProcessJsonRequest(routeFn, req, rsp);
			}
		}

		return false;
	}

	static bool ProcessJsonRequest(const net_server::RouteFn& routeFn, const net_server::http::HttpRequest& req, net_server::http::HttpResponse& rsp)
	{
		return ParseJsonRequest(routeFn, req, rsp, "", "");

		logger::Ptr m_log = logger::Create();
		rapidjson::Document doc;

		try
		{
			doc.Parse(req.Payload().data(), req.Payload().size());
			if (doc.HasParseError())
			{
				throw std::exception(("not parsed, pos " + std::to_string(doc.GetErrorOffset()) + ", msg " + rapidjson::GetParseError_En(doc.GetParseError())).c_str());
			}

			auto routeIt = doc.FindMember("route");
			if (routeIt == doc.MemberEnd() || !routeIt->value.IsString())
			{
				return false;
			}

			auto payloadIt = doc.FindMember(rapidjson::Value{ "payload" });
			if (payloadIt == doc.MemberEnd() || !payloadIt->value.IsString())
			{
				return false;
			}

			return ParseJsonRequest(routeFn, req, rsp, routeIt->value.GetString(), payloadIt->value.GetString());
		}
		catch (const std::exception& e)
		{
			logERROR(__FUNCTION__, "json parse error - " << e.what());
		}

		return false;
	}

	static bool ParseJsonRequest(const net_server::RouteFn& routeFn, const net_server::http::HttpRequest& req,
		net_server::http::HttpResponse& rsp, std::string&& route, std::string&& payload)
	{
		JsonRequest jsonReq{std::move(route), std::move(payload)};
		JsonResponse jsonRsp{ jsonReq.Route() };

		const bool ret = routeFn(jsonReq, jsonRsp);

		rapidjson::Document doc(rapidjson::kObjectType);

		static auto addStringMemberFn = [](auto& doc, auto& root, const std::string& name, const std::string& value)
		{
			root.AddMember(
				rapidjson::Value{ name.c_str(), name.size(), doc.GetAllocator() },
				rapidjson::Value{ value.c_str(), value.size(), doc.GetAllocator() },
				doc.GetAllocator()
			);
		};

		rapidjson::Value request(rapidjson::kObjectType);
		addStringMemberFn(doc, request, "route", jsonReq.Route());
		addStringMemberFn(doc, request, "payload", std::string(jsonReq.Payload().data(), jsonReq.Payload().size()));

		rapidjson::Value response(rapidjson::kObjectType);
		addStringMemberFn(doc, response, "route", jsonRsp.Route());
		addStringMemberFn(doc, response, "payload", std::string(jsonRsp.Payload().data(), jsonRsp.Payload().size()));

		rapidjson::Value result(rapidjson::kObjectType);
		addStringMemberFn(doc, result, "resultMsg", jsonRsp.ResultMsg());
		addStringMemberFn(doc, result, "resultCode", GetResultAsText(jsonRsp.Result()));

		doc.AddMember("request", std::move(request), doc.GetAllocator());
		doc.AddMember("response", std::move(response), doc.GetAllocator());
		doc.AddMember("result", std::move(result), doc.GetAllocator());

		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		doc.Accept(writer);

		const std::string jsonText = sb.GetString();

		rsp.Result(net_server::ResultCodes::CODE_OK);
		rsp.ResultMsg("OK");
		rsp.Payload(jsonText);
		rsp.Headers().push_back({"Content-Type", "application/json"});
		return true;
	}

	static std::string GetResultAsText(const net_server::ResultCodes code)
	{
		switch (code)
		{
		case net_server::ResultCodes::CODE_OK:				return "OK";
		case net_server::ResultCodes::CODE_NOT_FOUND:		return "NOT_FOUND";
		case net_server::ResultCodes::CODE_INTERNAL_ERROR:	return "INTERNAL_ERROR";
		case net_server::ResultCodes::CODE_BUSY:			return "BUSY";
		}
		return "UNKNOWN(" + std::to_string(size_t(code)) + ")";
	}

	static bool ValidateHeaders(const HeaderList& headers, const std::list<Header>& testHeaders)
	{
		for (const auto& h : testHeaders)
		{
			if (!ValidateHeaderValue(headers, h.name, h.value))
			{
				return false;
			}
		}

		return true;
	}

	static bool ValidateHeaderValue(const HeaderList& headers, const std::string& name, const std::string& value)
	{
		auto[found, it] = FindHeader(headers, name);
		return found && (it->value == value);
	}

	static std::tuple<bool, typename HeaderList::const_iterator> FindHeader(const HeaderList& headers, const std::string& name)
	{
		auto&& it = std::find_if(headers.cbegin(), headers.cend(),
			[](const net_server::http::Header& hdr)
			{
				return hdr.name == "Content-Type";
			});

		return std::make_tuple(it != headers.end(), std::move(it));
	}

private:
	net_server::Ptr m_httpServer;
};

net_server::Ptr net_server::http::json::CreateServer(const net_server::http::Params& params)
{
	return std::make_unique<JsonHttpServer>(params);
}
*/