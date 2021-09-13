#pragma once

namespace http_client
{
	enum class AuthMode
	{
		None,
		Basic,
		Digest,
	};

	struct Params
	{
		std::string scheme = "https";

		std::string host;
		uint16_t port = 80;

		AuthMode auth = AuthMode::None;
		std::string login;
		std::string password;

		size_t timeout = 5;
	};

	using Headers = std::map<std::string, std::string>;

	using ResultFn = std::function<void(bool result, const std::string& answer)>;

	class HttpClient
	{
	public:
		virtual ~HttpClient() = default;

		virtual void Get(const Headers& headers, const std::string& resource, ResultFn resultFn, const std::string& body = std::string()) = 0;
		virtual void Post(const Headers& headers, const std::string& resource, ResultFn resultFn, const std::string& body) = 0;
		virtual void Put(const Headers& headers, const std::string& resource, ResultFn resultFn, const std::string& body) = 0;
		virtual void Delete(const Headers& headers, const std::string& resource, ResultFn resultFn, const std::string& body = std::string()) = 0;
	};

	using Ptr = std::unique_ptr<HttpClient>;

	Ptr Create(const std::string& host, uint16_t port);
	Ptr Create(AuthMode auth, const std::string& login,	const std::string& password, const std::string& host, uint16_t port);
};
