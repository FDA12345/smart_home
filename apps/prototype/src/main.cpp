#include <iostream>

#include "MqttBroker.h"
#include "BrokerNetServer.h"

#include "noolite.h"
#include "modbus.h"
#include "finglai_fths01.h"
#include "wirenboard.h"
#include "huawei.h"
#include "serial.h"

int main()
{
	auto v1 = noolite_version();
	auto v2 = modbus_version();
	auto v3 = finglai_fths01_version();
	auto v4 = wirenboard_version();
	auto v5 = huawei_version();


	auto serial = serial::Create({});


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