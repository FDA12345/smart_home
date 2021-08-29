#pragma once

/*
Base net server aka web server request response
*/

namespace net_server
{

class Request
{
public:
	virtual ~Request() = default;

	virtual const std::string& Route() const = 0;
	virtual const std::string_view& Payload() const = 0;
};

class Response
{
public:
	virtual ~Response() = default;

	virtual const std::string& Route() const = 0;

	virtual const std::string_view& Payload() const = 0;
	virtual void Payload(const std::string_view& payload) = 0;

	virtual size_t Result() const = 0;
	virtual void Result(size_t code) = 0;
};

class Server
{
public:
	using RouteFn = std::function<bool(const Request& req, Response& rsp)>;

public:
	virtual ~Server() = default;

	//add route - path to resource, routeFn - async callback on route
	virtual bool RouteAdd(const std::string& routePath, RouteFn routeFn) = 0;
	//remove route - path to resource
	virtual bool RouteRemove(const std::string& routePath) = 0;

	//start server
	virtual bool Start() = 0;
	//stop server
	virtual void Stop() = 0;
};

using Ptr = std::unique_ptr<Server>;

};//namespace net_server
