#include <iostream>

#include "MqttBroker.h"
#include "BrokerDlgServer.h"

int main()
{
	auto dlgServer = BrokerDlgServer::Create();

	dlgServer->AddRoute("services", [&dlgServer](const std::string_view& payload)
	{
		//dlgServer->Send("services", "me1234");
	});

	auto broker = MqttBroker::Create("tcp://mqtt.eclipseprojects.io:1883", "fda123");

	//broker->SubscribeEvents(brokerEvents);

	if (broker->Start())
	{
		std::cout << "broker started" << std::endl;
	}
	else
	{
		std::cout << "start broker failed" << std::endl;
	}

	std::this_thread::sleep_for(std::chrono::seconds(10));

	broker->Stop();
	broker.reset();
}