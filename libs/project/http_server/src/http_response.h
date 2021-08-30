#pragma once

namespace net_server
{
	namespace http
	{
		class HttpResponseFullAccess : public HttpResponse
		{
		public:
			virtual void Route(const std::string& route) = 0;
		};

		using HttpResponsePtr = std::unique_ptr<HttpResponseFullAccess>;
		HttpResponsePtr CreateResponse();
	};
};
