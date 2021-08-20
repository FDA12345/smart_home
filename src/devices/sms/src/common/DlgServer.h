#pragma once

/*
Base dialog server aka web server request response
*/

namespace dlg_server
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

class Server
{
public:
	virtual ~Server() = default;

	//add route - path to resource.
	//routeFn - async callback on route
	virtual bool AddRoute(const std::string& routePath, std::function<void(const std::string_view& payload)> routeFn) = 0;

	//remove route - path to resource
	virtual bool RemoveRoute(const std::string& path) = 0;

	virtual bool Start() = 0;
	virtual void Stop() = 0;
};

};//namespace dlg_server
