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
#include "finglai_fths01.h"
#include "http_client.h"
#include "logger.h"
#include <thread>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

std::vector<int> g_devices{ 50, 51, 52, 53 };

std::string MakeSensorMsg(uint8_t address, const serial::fths01::Telemetry& telemetry)
{
	/*
	{
		"request": {
			"route": "sensors",
			"command" : "update",
			"payload" : {
				"version": "1.0",
				"sensors" : {
				"total": 1,
				"items" :
					[
						{
							"name": "",
							"address" : "fths01/temp_1st_floor",
							"type" : "float",
							"value" : 15,
							"timestamp" : 0
						}
					]
				}
			}
		}
	}
	*/

	rapidjson::Document doc(rapidjson::kObjectType);

	rapidjson::Value items(rapidjson::kArrayType);

	auto addItemFn = [&doc, &items](const std::string& name, float value)
	{
		rapidjson::Value item(rapidjson::kObjectType);

		item.AddMember("name", rapidjson::Value("", doc.GetAllocator()), doc.GetAllocator());
		item.AddMember("address", rapidjson::Value(name.c_str(), name.size(), doc.GetAllocator()), doc.GetAllocator());
		item.AddMember("type", rapidjson::Value("float", doc.GetAllocator()), doc.GetAllocator());
		item.AddMember("value", value, doc.GetAllocator());

		using clock = std::chrono::system_clock;
		const auto secondsUtc = std::chrono::duration_cast<std::chrono::seconds>(clock::now().time_since_epoch()).count();
		item.AddMember("value", secondsUtc, doc.GetAllocator());

		items.PushBack(std::move(item), doc.GetAllocator());
	};

	addItemFn("fths01://" + std::to_string(address) + "/temp_1st_floor", telemetry.temperature);
	addItemFn("fths01://" + std::to_string(address) + "/humi_1st_floor", telemetry.humidity);

	rapidjson::Value sensors(rapidjson::kObjectType);
	sensors.AddMember("total", 2, doc.GetAllocator());
	sensors.AddMember("items", std::move(items), doc.GetAllocator());

	rapidjson::Value payload(rapidjson::kObjectType);
	payload.AddMember("version", rapidjson::Value("1.0", doc.GetAllocator()), doc.GetAllocator());
	payload.AddMember("sensors", std::move(sensors), doc.GetAllocator());

	rapidjson::Value request(rapidjson::kObjectType);
	request.AddMember("route", rapidjson::Value("sensors", doc.GetAllocator()), doc.GetAllocator());
	request.AddMember("command", rapidjson::Value("update", doc.GetAllocator()), doc.GetAllocator());
	request.AddMember("payload", std::move(payload), doc.GetAllocator());

	doc.AddMember("request", std::move(request), doc.GetAllocator());

	rapidjson::StringBuffer sb;
	rapidjson::Writer writer(sb);

	if (!doc.Accept(writer))
	{
		return "";
	}

	return sb.GetString();
}

int main()
{
	//logger::SetLogLevel(logger::LogLevel::Trace);
	auto m_log = logger::Create();

	auto httpClient = http_client::Create(http_client::AuthMode::Digest, "fda", "litcaryno123", "192.168.41.11", 1880);

	auto driver = serial::fths01::Create();
	if (driver->Open("COM6"))
	{
		for (;;)
		{
			for (const auto address : g_devices)
			{
				logINFO(__FUNCTION__, "reading address " << address);

				serial::fths01::Telemetry telemetry = { 0 };
				if (driver->ReadTelemetry(address, telemetry))
				{
					logINFO(__FUNCTION__, "address:" << address << ", temp: " << telemetry.temperature << ", humi: " << telemetry.humidity);

					httpClient->Post(
						{
							{"User-Agent", "FTHS01 sensors service 1.0"},
							{"Content-Type", "application/json; charset=utf-8"},
							{"Accept", "application/json"},
						},
						"/send",
						[](bool result, const std::string& answer)
						{

						},
						MakeSensorMsg(address, telemetry)
					);
				}
				else
				{
					logERROR(__FUNCTION__, "read error, address " << address);
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
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
