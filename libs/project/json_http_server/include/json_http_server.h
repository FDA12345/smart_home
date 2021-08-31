#pragma once

#include "http_server.h"

/*

json accepting server

json request format
method POST
Header "Content-Type: application/json"
Header "Accept: application/json"
{
	"route":"route123",
	"payload":"abc"
}

json answer format
{
	"request":
	{
		"route":"route123",
		"payload":"abc"
	},
	"response":
	{
		"route":"route234",
		"payload":"cde"
	},
	"result":
	{
		"resultMsg":"msg",
		"resultCode":"OK"
	}
}
*/

namespace net_server
{
	namespace http
	{
		namespace json
		{
			extern std::string ReqRspType;

			net_server::Ptr CreateServer(const net_server::http::Params& params);

		};//namespace json
	};//namespace http
};//namespace net_server
