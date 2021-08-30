#pragma once

namespace net_server
{
	namespace http
	{
		class HttpRequestFullAccess : public HttpRequest
		{
		public:
			virtual void Route(const std::string& route) = 0;
			virtual void Payload(std::vector<char>&& payload) = 0;
			virtual void Method(const std::string& method) = 0;
			virtual void Version(int version) = 0;
			virtual HeaderList& Headers() = 0;
		};


		using HttpRequestPtr = std::unique_ptr<HttpRequestFullAccess>;
		HttpRequestPtr CreateRequest();
	};
};
