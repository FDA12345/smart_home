#include "stdafx.h"

#include "json_http_server.h"
#include "noolite.h"
#include "logger.h"

#include <thread>

const std::string g_routePrefix = "smart_home";

const uint16_t g_svcPort = 10001;
const std::string g_svcPrefix = "noolite";
const std::string g_svcSrvName = "noolite service";

const std::string g_svcRoutePrefix = g_routePrefix + (g_svcPrefix.empty() ? "" : "/" + g_svcPrefix);

class LoggedServer : public net_server::Server
{
public:
	static net_server::Ptr Create(net_server::Ptr&& srv)
	{
		return std::unique_ptr<LoggedServer>(new LoggedServer(std::move(srv)));
	}

public:
	~LoggedServer()
	{
		logINFO(__FUNCTION__, "remove");
		m_srv.reset();
		logINFO(__FUNCTION__, "removed");
	}

	bool RouteAdd(const std::string& routePath, net_server::RouteFn routeFn) override
	{
		logINFO(__FUNCTION__, "add route " << routePath);
		return m_srv->RouteAdd(routePath, routeFn);
	}

	bool RouteRemove(const std::string& routePath) override
	{
		logINFO(__FUNCTION__, "remove route " << routePath);
		return m_srv->RouteRemove(routePath);
	}

	bool Start() override
	{
		logINFO(__FUNCTION__, "start");
		const bool ret = m_srv->Start();
		logINFO(__FUNCTION__, "started");
		return ret;
	}

	void Stop() override
	{
		logINFO(__FUNCTION__, "stop");
		m_srv->Stop();
		logINFO(__FUNCTION__, "stopped");
	}

private:
	LoggedServer(net_server::Ptr&& srv)
		: m_srv(std::move(srv))
	{
	}

private:
	const logger::Ptr m_log = logger::Create();
	net_server::Ptr m_srv;
};

int main()
{
	const auto m_log = logger::Create();

	const auto noo = noolite::CreateDongle();

	serial::Params serialParams;
	serialParams.serialName = "COM6";
	serialParams.baudRate = 9600;
	serialParams.characterSize = 8;
	serialParams.flowControl = serial::FlowControl::NONE;
	serialParams.parity = serial::Parity::NONE;
	serialParams.stopBits = serial::StopBits::_1_0;

	noo->Start(serialParams);


	const auto server = LoggedServer::Create(net_server::http::json::CreateServer({ "", g_svcPort, g_svcSrvName }));
	server->RouteAdd(g_svcRoutePrefix + "/dongle/discoveryAllChannels", [&noo](const net_server::Request& req, net_server::Response& rsp)->bool
	{
		for (uint8_t channel = 0; channel < noolite::MAX_CHANNELS; ++channel)
		{
			std::vector<noolite::ChannelInfo0> infos0;
			noo->ReadChannelInfo0(channel, infos0);
		}
		return false;
	});

	server->Start();

	std::this_thread::sleep_for(std::chrono::hours(1));

	server->Stop();
	noo->Stop();
	return 0;
}
