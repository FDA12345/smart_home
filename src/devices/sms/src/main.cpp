#include <iostream>

#include "MqttBroker.h"

int main()
{
	auto broker = MqttBroker::Create();

	if (broker->Start())
	{
		std::cout << "broker started" << std::endl;
	}
	else
	{
		std::cout << "start broker failed" << std::endl;
	}

	std::this_thread::sleep_for(std::chrono::seconds(1000));

	broker->Stop();
	broker.reset();
}