#pragma once

class HttpSession
{
public:
	using RequestFn = std::function<void(const HttpRequest& req, HttpResponse& rsp)>;

	using Ptr = std::shared_ptr<HttpSession>;
	static Ptr Create(RequestFn requestFn, const std::shared_ptr<Params>& params, tcp::socket&& peer);

public:
	virtual ~HttpSession() = default;

	virtual void Run() = 0;
};
