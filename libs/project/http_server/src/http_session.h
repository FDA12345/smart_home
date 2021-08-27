#pragma once

struct SessionRequest
{
	int version = 11;
	std::string method;
	std::vector<std::pair<std::string, std::string>> headers;
	std::string resource;
	std::string body;
};

struct SessionResponse
{
	int version = 11;
	std::vector<std::pair<std::string, std::string>> headers;
	std::string body;
	boost::beast::http::status result;
};

class HttpSession
{
public:
	using RequestFn = std::function<void(const SessionRequest& sessReq, SessionResponse& sessResp)>;

	using Ptr = std::shared_ptr<HttpSession>;
	static Ptr Create(RequestFn requestFn, const std::shared_ptr<Params>& params, tcp::socket&& peer);

public:
	virtual ~HttpSession() = default;

	virtual void Run() = 0;
};
