#pragma once

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

namespace http_client
{
	namespace json
	{
		//extern std::string ReqRspType;

		//net_server::Ptr CreateServer(const net_server::http::Params& params);

	};//namespace json
};//namespace http_client
