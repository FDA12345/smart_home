#include <iostream>

#include "MqttBroker.h"

int main()
{
	auto broker = MqttBroker::Create();
  std::cout << "hello world " << std::endl;
}