#include <iostream>

#include "MqttBroker.h"

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

class OnBrokerEvents : public broker::BrokerEvents
{
public:
	void OnConnected(broker::BaseBroker& broker) override
	{
	}

	void OnDisconnected(broker::BaseBroker& broker) override
	{
	}

	void OnMsgRecv(broker::BaseBroker& broker, const broker::Msg& msg) override
	{
	}

	void OnMsgSent(broker::BaseBroker& broker, const broker::Msg& msg) override
	{
	}
};

int main()
{
	for (const auto& f : boost::filesystem::directory_iterator("c:/"))
	{
		std::cout << f.path().string() << std::endl;
	}

	boost::asio::io_service io;

	OnBrokerEvents brokerEvents;

	auto broker = MqttBroker::Create("tcp://mqtt.eclipseprojects.io:1883", "fda123");

	broker->SubscribeEvents(brokerEvents);

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