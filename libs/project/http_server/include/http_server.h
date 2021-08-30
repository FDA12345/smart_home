#pragma once

#include "net_server.h"

namespace net_server
{
	namespace http
	{
		extern std::string ReqRspType;

		struct Header
		{
			Header(const std::string& name_, const std::string& value_)
				: name(name_)
				, value(value_)
			{}

			Header(std::string&& name_, std::string&& value_)
				: name(std::move(name_))
				, value(std::move(value_))
			{}

			std::string name;
			std::string value;
		};

		using HeaderList = std::deque<Header>;

		class HttpRequest : public Request
		{
		public:
			virtual const std::string& Method() const = 0;
			virtual int Version() const = 0;
			virtual const HeaderList& Headers() const = 0;
		};

		class HttpResponse : public Response
		{
		public:
			virtual int Version() const = 0;
			virtual void Version(int version) = 0;

			virtual const HeaderList& Headers() const = 0;
			virtual HeaderList& Headers() = 0;
		};


		struct Params
		{
			std::string address; //empty for tcp_v4 on all network interfaces
			uint16_t port = 0;

			std::string serverName;
			size_t threadsCount = 1;
		};


		net_server::Ptr CreateServer(const Params& params);

	};//namespace http
};//namespace net_server
