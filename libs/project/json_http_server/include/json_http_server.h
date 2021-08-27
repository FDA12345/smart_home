#pragma once

#include "http_server.h"

namespace net_server
{
namespace http
{
namespace json
{

net_server::Ptr CreateServer(const net_server::http::Params& params);

};//namespace json
};//namespace http
};//namespace net_server
