#include <iostream>

#include "MqttBroker.h"
#include "BrokerNetServer.h"

#include "noolite.h"

int main()
{
	serial::Params serialParams;
	serialParams.serialName = "COM6";
	serialParams.baudRate = 9600;
	serialParams.stopBits = serial::STOPBITS_1_0;
	serialParams.parity = serial::PARITY_NONE;
	serialParams.flowControl = serial::FLOW_CONTROL_NONE;
	serialParams.characterSize = 8;

	auto dongle = noolite::CreateDongle();
	if (dongle && dongle->Start(serialParams) && dongle->ForceInit())
	{
		DongleDeviceConnection devConn;
		devConn.mode = DongleMode::F_TX;
		devConn.channel = 5;
		dongle->SwitchOn(devConn);
	}

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