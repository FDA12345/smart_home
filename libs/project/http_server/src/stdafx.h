#pragma once

#include <string>
#include <functional>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
namespace ip = boost::asio::ip;
using tcp = ip::tcp;
namespace beast_http = boost::beast::http;

#include "logger.h"

#include "http_server.h"
using namespace net_server;
using namespace net_server::http;

#include "http_session.h"
