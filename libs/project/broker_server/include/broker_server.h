#pragma once

#include "net_server.h"
#include "broker.h"

namespace net_server
{
	namespace broker
	{

		extern std::string ReqRspType;

		//create server via specified broker instance
		Ptr CreateServer(const std::shared_ptr<::broker::Broker>& broker);

	};//broker
};//net_server
