#include <iostream>

#include "MqttBroker.h"


class OnBrokerEvents : public broker::BrokerEvents
{
public:
	void OnConnected(broker::Broker& broker) override
	{
		std::cout << "connected" << std::endl;
	}

	void OnDisconnected(broker::Broker& broker) override
	{
		std::cout << "disconnected" << std::endl;
	}

	void OnMsgRecv(broker::Broker& broker, const broker::TopicMsg& topicMsg) override
	{
		std::cout << "msg " << std::string(topicMsg.Topic()) << std::endl;
	}

	void OnMsgSent(broker::Broker& broker, const broker::TopicMsg& topicMsg) override
	{
	}
};

int main()
{
	OnBrokerEvents brokerEvents;

	auto broker = MqttBroker::Create("tcp://mqtt.eclipseprojects.io:1883", "fda123");

	broker->SubscribeEvents(brokerEvents);
	broker->SubscribeTopic("$SYS/#");

	if (broker->Start())
	{
		std::cout << "broker started" << std::endl;
	}
	else
	{
		std::cout << "start broker failed" << std::endl;
	}

	std::this_thread::sleep_for(std::chrono::seconds(60));

	broker->Stop();
	broker.reset();
}