#pragma once

class Broker
{
public:
	using Ptr = std::unique_ptr<Broker>;

public:
	virtual ~Broker() = default;

	virtual bool Start() = 0;
	virtual void Stop() = 0;
};
