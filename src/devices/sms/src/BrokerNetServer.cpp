#include "BrokerNetServer.h"

using namespace net_server;
using namespace net_server::broker;

class BrokerServerImpl : public Server
{
public:
	BrokerServerImpl(::broker::Ptr&& broker)
		: m_broker(std::move(broker))
	{
	}

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

private:
	::broker::Ptr m_broker;
};


Ptr net_server::broker::CreateServer(::broker::Ptr&& broker)
{
	return std::make_unique<BrokerServerImpl>(std::move(broker));
}
