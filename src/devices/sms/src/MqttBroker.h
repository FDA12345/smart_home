#pragma once

#include "Broker.h"

class MqttBroker
{
public:
	static Broker::Ptr Create();
};