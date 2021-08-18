#pragma once

#include "Broker.h"

class MqttBroker
{
public:
	static broker::Broker::Ptr Create(const std::string& address, const std::string& clientId);
};