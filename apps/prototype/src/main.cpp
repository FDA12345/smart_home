#include <iostream>

#include "MqttBroker.h"
#include "BrokerNetServer.h"

#include "noolite.h"

int main()
{
	auto dongle = noolite::CreateDongle();
	if (dongle && dongle->Start())
	{
		dongle->Init();
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