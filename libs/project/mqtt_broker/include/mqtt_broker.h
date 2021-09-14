#pragma once

#include "broker.h"

namespace broker
{
namespace mqtt
{

	//class Mqtt

Ptr Create(const std::string& address, const std::string& clientId);

};//mqtt
};//broker
