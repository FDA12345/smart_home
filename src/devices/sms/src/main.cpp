#include <iostream>

#include "MqttBroker.h"
#include "BrokerNetServer.h"

int main()
{
	auto broker = broker::mqtt::Create("tcp://mqtt.eclipseprojects.io:1883", "supervisor");
	auto server = net_server::broker::CreateServer(std::move(broker));

	server->RouteAdd("servers");
	server->RouteAdd("noolite");

	server->Start();

	std::this_thread::sleep_for(std::chrono::seconds(15));

	server->Stop();
	server.reset();
}