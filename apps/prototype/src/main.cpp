#include <iostream>

//#include "MqttBroker.h"
//#include "broker_server.h"
#include "json_http_server.h"

#include "noolite.h"
#include "logger.h"

int main()
{
	logger::SetLogLevel(logger::LogLevel::Trace);

	auto m_log = logger::Create();
	logINFO("app", "started");

	serial::Params serialParams;
	serialParams.serialName = "COM6";
	serialParams.baudRate = 9600;
	serialParams.stopBits = serial::StopBits::_1_0;
	serialParams.parity = serial::Parity::NONE;
	serialParams.flowControl = serial::FlowControl::NONE;
	serialParams.characterSize = 8;

	auto dongle = noolite::CreateDongle();
	if (dongle && dongle->Start(serialParams) && dongle->ForceInit())
	{
		for (int i = 0;false && i < 64; ++i)
		{
			std::vector<noolite::ChannelInfo0> infos0;
			if (dongle->ReadChannelInfo0(i, infos0))
			{
				std::vector<noolite::ChannelInfo1> infos1;

				if (dongle->ReadChannelInfo1(i, infos1))
				{
					std::vector<noolite::ChannelInfo2> infos2;
					if (dongle->ReadChannelInfo2(i, infos2))
					{
						int k = 0;
					}
				}
			}
		}
	}
	dongle->Stop();

	//auto broker = broker::mqtt::Create("tcp://mqtt.eclipseprojects.io:1883", "supervisor");
	//auto server = net_server::broker::CreateServer(std::move(broker));

	auto server = net_server::http::json::CreateServer({"", 9898, "Prototype server ver 1.0", 1});

	server->RouteAdd("fda/servers", [](const net_server::Request& req, net_server::Response& rsp)
	//server->RouteAdd("$SYS/broker/version", [](const net_server::Request& req, net_server::Response& rsp)
	{
		std::cout << std::string(req.Payload()) << std::endl;
		return false;
	});

	server->RouteAdd("fda/noolite", [](const net_server::Request& req, net_server::Response& rsp)
	{
		return false;
	});

	server->Start();

	std::this_thread::sleep_for(std::chrono::seconds(150));

	server->Stop();
	server.reset();
}