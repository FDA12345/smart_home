#include "BrokerNetServer.h"

using namespace ::broker;
using namespace net_server;

class BrokerServerImpl :
	public Server,
	private BrokerEvents
{
public:
	BrokerServerImpl(::broker::Ptr&& broker)
		: m_broker(std::move(broker))
	{
		m_broker->SubscribeEvents(*this);
	}

	~BrokerServerImpl()
	{
		m_broker->UnsubscribeEvents(*this);
	}

	//::broker::BrokerEvents
	void OnConnected(BaseBroker& broker) override
	{
	}

	void OnDisconnected(BaseBroker& broker) override
	{
	}

	void OnMsgRecv(BaseBroker& broker, const Msg& msg) override
	{
	}

	void OnMsgSent(BaseBroker& broker, const Msg& msg) override
	{
	}


	//Server
	void Subscribe(const ServerEvents::Ptr& owner) override
	{
		std::lock_guard lock(m_mx);
		m_owners.push_back(owner);
	}

	void Unsubscribe(const ServerEvents::Ptr& owner) override
	{
		std::lock_guard lock(m_mx);

		auto it = std::find(m_owners.begin(), m_owners.end(), owner);
		if (it != m_owners.end())
		{
			m_owners.erase(it);
		}
	}

	bool RouteAdd(const std::string& routePath) override
	{
		m_broker->SubscribeTopic(GetRequestRouteTopic(routePath));
		return true;
	}

	bool RouteRemove(const std::string& routePath) override
	{
		m_broker->UnsubscribeTopic(GetRequestRouteTopic(routePath));
		return true;
	}

	bool Start() override
	{
		return m_broker->Start();
	}

	void Stop() override
	{
		m_broker->Stop();
	}

private:
	std::string GetRequestRouteTopic(const std::string& routePath) const
	{
		return routePath + "/" + m_broker->ClientId() + "/req";
	}

private:
	::broker::Ptr m_broker;

	std::mutex m_mx;
	std::vector<ServerEvents::Ptr> m_owners;
	std::map<std::string, size_t> m_subs;
};


net_server::Ptr net_server::broker::CreateServer(::broker::Ptr&& broker)
{
	return std::make_unique<BrokerServerImpl>(std::move(broker));
}
