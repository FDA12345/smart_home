#include "MqttBroker.h"
#include "MQTTClient.h"

Broker::Ptr MqttBroker::Create()
{
	MQTTClient client;

	MQTTClient_create(&client, "tcp://192.168.41.11:1883", "ExampleClientPub",
		MQTTCLIENT_PERSISTENCE_NONE, NULL);

	return nullptr;
}
