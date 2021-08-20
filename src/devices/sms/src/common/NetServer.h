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

	virtual const std::string_view& Payload() const = 0;
	virtual void Payload(const std::string_view& payload) = 0;

	enum ResultCodes
	{
		CODE_OK,
		CODE_NOT_FOUND,
		CODE_INTERNAL_ERROR,
		CODE_BUSY,
	};
	virtual ResultCodes Result() const = 0;
	virtual void Result(ResultCodes code) = 0;

	virtual const std::string& ResultMsg() const = 0;
	virtual void ResultMsg(const std::string& msg) = 0;
};

class ServerEvents
{
public:
	using Ptr = std::shared_ptr<ServerEvents>;

public:
	virtual ~ServerEvents() = default;

	virtual void OnRequest(const Request& req, Response& rsp) = 0;
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
	virtual bool RouteRemove(const std::string& routePath) = 0;

	//start server
	virtual bool Start() = 0;
	//stop server
	virtual void Stop() = 0;
};

using Ptr = std::unique_ptr<Server>;

};//namespace net_server
