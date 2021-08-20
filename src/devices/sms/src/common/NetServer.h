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
};

class Response
{
public:
	virtual ~Response() = default;
};

class ServerEvents
{
public:
	using Ptr = std::shared_ptr<ServerEvents>;

public:
	virtual ~ServerEvents() = default;
};

class Server
{
public:
	virtual ~Server() = default;

	//subscribing to events
	virtual void Subscribe(const ServerEvents::Ptr& owner) = 0;
	//unsubscribing to events
	virtual void Unsubscribe(const ServerEvents::Ptr& owner) = 0;

	//add route - path to resource, routeFn - async callback on route
	virtual bool RouteAdd(const std::string& routePath) = 0;
	//remove route - path to resource
	virtual bool RouteRemove(const std::string& path) = 0;

	//start server
	virtual bool Start() = 0;
	//stop server
	virtual void Stop() = 0;
};

using Ptr = std::unique_ptr<Server>;

};//namespace net_server