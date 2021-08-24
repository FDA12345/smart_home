#include <iostream>

#include "MqttBroker.h"
#include "BrokerNetServer.h"

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
		for (int k = 0; k < 3; ++k)
		{
			DongleDeviceConnection devConn;
			devConn.mode = DongleMode::F_TX;
			devConn.channel = 10;

			dongle->SwitchOff(devConn);
			std::this_thread::sleep_for(std::chrono::seconds(2));
			dongle->SwitchOn(devConn);
			std::this_thread::sleep_for(std::chrono::seconds(2));
		}
	}
	dongle->Stop();

	auto broker = broker::mqtt::Create("tcp://mqtt.eclipseprojects.io:1883", "supervisor");
	auto server = net_server::broker::CreateServer(std::move(broker));

	//server->RouteAdd("fda/servers", [](const net_server::Request& req, net_server::Response& rsp)
	server->RouteAdd("$SYS/broker/version", [](const net_server::Request& req, net_server::Response& rsp)
	{
		std::cout << std::string(req.Payload()) << std::endl;
		return false;
	});

	server->RouteAdd("fda/noolite", [](const net_server::Request& req, net_server::Response& rsp)
	{
		return false;
	});

	server->Start();

	std::this_thread::sleep_for(std::chrono::seconds(15));

	server->Stop();
	server.reset();
}