#include "stdafx.h"

/*
#include "json_http_server.h"
#include "logger.h"

#include "noolite.h"

#include <thread>
#include <map>
#include <optional>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include "json_object.h"

const std::string g_routePrefix = "/smart_home";

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
	//auto root = json::CreateRoot();
	//root->Tree()["aaa"]["bbb"]["ccc"];

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
		struct DeviceInfo
		{
			uint8_t channel = 0;
			std::optional<noolite::ChannelInfo0> info0;
			std::optional<noolite::ChannelInfo1> info1;
			std::optional<noolite::ChannelInfo2> info2;
		};
		std::map<uint8_t, std::map<uint32_t, DeviceInfo>> channels;

		for (uint8_t channel = 0; channel < noolite::MAX_CHANNELS; ++channel)
		{
			std::vector<noolite::ChannelInfo0> infos0;
			if (noo->ReadChannelInfo0(channel, infos0))
			{
				for (const auto& info : infos0)
				{
					channels[channel][info.id].info0 = info;
				}

				std::vector<noolite::ChannelInfo1> infos1;
				if (noo->ReadChannelInfo1(channel, infos1))
				{
					for (const auto& info : infos1)
					{
						channels[channel][info.id].info1 = info;
					}

					std::vector<noolite::ChannelInfo2> infos2;
					if (noo->ReadChannelInfo2(channel, infos2))
					{
						for (const auto& info : infos2)
						{
							channels[channel][info.id].info2 = info;
						}
					}
				}
			}
		}


		rapidjson::Document doc(rapidjson::kObjectType);

		rapidjson::Value jsonChannels{rapidjson::kArrayType};
		for (const auto& channel : channels)
		{
			rapidjson::Value jsonChannel(rapidjson::kArrayType);
			for (const auto& device : channel.second)
			{
				rapidjson::Value d(rapidjson::kObjectType);
				d.AddMember("id", device.first, doc.GetAllocator());

				if (!device.second.info0)
				{
					d.AddMember("info0", rapidjson::Value{rapidjson::kNullType}, doc.GetAllocator());
				}
				else
				{
					rapidjson::Value devInfo0{ rapidjson::kObjectType };

					rapidjson::Value info0{ rapidjson::kObjectType };
					info0.AddMember("deviceType", device.second.info0->info0.deviceType, doc.GetAllocator());
					info0.AddMember("firmware", device.second.info0->info0.firmware, doc.GetAllocator());
					info0.AddMember("state", size_t(device.second.info0->info0.state), doc.GetAllocator());
					info0.AddMember("bindMode", size_t(device.second.info0->info0.bindMode), doc.GetAllocator());
					info0.AddMember("lightLevel", size_t(device.second.info0->info0.lightLevel), doc.GetAllocator());

					devInfo0.AddMember("info0", std::move(info0), doc.GetAllocator());
					devInfo0.AddMember("answer", size_t(device.second.info0->answer), doc.GetAllocator());

					d.AddMember("info0", std::move(devInfo0), doc.GetAllocator());
				}

				jsonChannel.PushBack(std::move(d), doc.GetAllocator());
			}

			jsonChannels.PushBack(std::move(jsonChannel), doc.GetAllocator());
		}
		doc.AddMember("channels", std::move(jsonChannels), doc.GetAllocator());

		rapidjson::StringBuffer sb;
		rapidjson::Writer writer(sb);
		doc.Accept(writer);

		rsp.Result(net_server::ResultCodes::CODE_OK);
		rsp.ResultMsg("OK");
		rsp.Payload(sb.GetString());

		return true;
	});

	server->Start();

	std::this_thread::sleep_for(std::chrono::hours(1));

	server->Stop();
	noo->Stop();
	return 0;
}
*/

#include "mqtt_broker.h"
#include "broker_server.h"
#include "wirenboard.h"
#include "logger.h"
#include "http_client.h"

#include <thread>

http_client::Headers httpHeaders =
{
	{"User-Agent", "Wirenboard service client 1.0"},
	{"Content-Type", "text/html"},
	{"Accept", "*/*"},
};

int main()
{
	logger::SetLogLevel(logger::LogLevel::Trace);

	auto m_log = logger::Create();

	//auto httpClient = http_client::Create(http_client::AuthMode::Digest, "fda123", "litcaryno", "192.168.41.11", 1880);
	auto httpClient = http_client::Create("ya.ru", 443);
	if (!httpClient)
	{
		logERROR(__FUNCTION__, "http client create failed");
		return -1;
	}

	httpClient->Get(httpHeaders, "/", [](bool result, const std::string& answer)
	{
	});


	auto driver = serial::wirenboard::Create();
	if (driver->Open("COM7"))
	{
		serial::wirenboard::WB_MAP3H wbMap3H = { 0 };

		for (;;)
		{
			if (driver->Read(14, wbMap3H))
			{
				logINFO(__FUNCTION__, "p_total " << wbMap3H.p_total.all << ", p_l1 " << wbMap3H.p_total.l1
					<< ", p_l2 " << wbMap3H.p_total.l2 << ", p_l3 " << wbMap3H.p_total.l3);

				httpClient->Post(httpHeaders, "/update", [](bool result, const std::string& answer) {}, "");
			}
			else
			{
				logERROR(__FUNCTION__, "read failed");
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		driver->Close();
	}

	/*
	auto&& broker = broker::mqtt::Create("127.0.0.1", "client123");
	auto server = net_server::broker::CreateServer(std::move(broker));

	if (server->Start())
	{
	}

	server->Stop();
	server.reset();
	*/
}
