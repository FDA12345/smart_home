#pragma once

#include "common/NetServer.h"
#include "common/Broker.h"

namespace net_server
{
namespace broker
{

//create server via specified broker instance
Ptr CreateServer(::broker::Ptr&& broker);

};//broker
};//net_server
