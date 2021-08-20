#include <iostream>

#include "MqttBroker.h"
#include "BrokerNetServer.h"

int main()
{
	auto dlgServer = net_server::broker::CreateServer(broker::mqtt::Create("tcp://mqtt.eclipseprojects.io:1883", "supervisor"));
}