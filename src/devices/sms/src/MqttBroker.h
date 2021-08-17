#pragma once

#include "Broker.h"

class MqttBroker : private Broker
{
public:
	static Ptr Create();
};