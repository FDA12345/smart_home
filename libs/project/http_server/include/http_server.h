#pragma once

#include "net_server.h"

namespace net_server
{
	namespace http
	{
		struct Params
		{
			std::string address; //empty for tcp_v4 on all network interfaces
			uint16_t port = 0;
		};


		net_server::Ptr CreateServer(const Params& params);

	};//namespace http
};//namespace net_server
