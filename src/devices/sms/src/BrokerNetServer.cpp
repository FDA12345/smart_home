#include "BrokerNetServer.h"

using namespace net_server;
using namespace broker;

class BrokerServerImpl : public Server
{
public:
	void Subscribe(const ServerEvents::Ptr& owner) override
	{
	}

	void Unsubscribe(const ServerEvents::Ptr& owner) override
	{
	}

	bool RouteAdd(const std::string& routePath) override
	{
		return false;
	}

	bool RouteRemove(const std::string& path) override
	{
		return false;
	}

	bool Start() override
	{
		return false;
	}

	void Stop() override
	{
	}
};


Ptr net_server::broker::CreateServer()
{
	return std::make_unique<BrokerServerImpl>();
}
