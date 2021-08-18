#include <iostream>

#include "MqttBroker.h"


class OnBrokerEvents : public broker::BrokerEvents
{
public:
	void OnConnected(broker::BaseBroker& broker) override
	{
		std::cout << "connected" << std::endl;
		broker.SubscribeTopic("$SYS/");
	}

	void OnDisconnected(broker::BaseBroker& broker) override
	{
		std::cout << "disconnected" << std::endl;
	}

	void OnMsgRecv(broker::BaseBroker& broker, const broker::Msg& msg) override
	{
		std::cout << "msg " << std::string(msg.Topic()) << std::endl;
	}

	void OnMsgSent(broker::BaseBroker& broker, const broker::Msg& msg) override
	{
	}
};

int main()
{
	OnBrokerEvents brokerEvents;

	auto broker = MqttBroker::Create("tcp://mqtt.eclipseprojects.io:1883", "fda123");

	broker->SubscribeEvents(brokerEvents);
	//broker->SubscribeTopic("$SYS/#");
	broker->SubscribeTopic("#");

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