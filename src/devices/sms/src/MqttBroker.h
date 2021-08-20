#pragma once

#include "common/Broker.h"

namespace broker
{
namespace mqtt
{

Ptr Create(const std::string& address, const std::string& clientId);

};//mqtt
};//broker
