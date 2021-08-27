#pragma once

class HttpSession
{
public:
	using Ptr = std::shared_ptr<HttpSession>;
	static Ptr Create(const std::shared_ptr<Params>& params, tcp::socket&& peer);

public:
	virtual ~HttpSession() = default;

	virtual void Run() = 0;
};
