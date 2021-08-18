#include <iostream>

#include "MqttBroker.h"


class OnTopicEvents : public broker::TopicEvents
{
public:
	void OnMsgRecv(const broker::TopicMsg& topicMsg) override
	{
		std::cout << "msg " << std::string(topicMsg.Topic()) << std::endl;
	}

	void OnMsgSent(const broker::TopicMsg& topicMsg) override
	{
	}
};

int main()
{
	OnTopicEvents onTopicEvents;

	auto broker = MqttBroker::Create("tcp://mqtt.eclipseprojects.io:1883", "fda123");

	broker->SubscribeEvents(onTopicEvents);
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