#include "broker_server.h"

using namespace ::broker;
using namespace net_server;


class RequestImpl : public Request
{
public:
	RequestImpl(const Msg& msg)
		: m_topic(msg.Topic())
		, m_payload(msg.Payload())
	{
	}

	const std::string& Route() const override
	{
		return m_topic;
	}

	const std::string_view& Payload() const override
	{
		return m_payload;
	}

private:
	std::string m_topic;
	const std::string_view& m_payload;
};



class ResponseImpl : public Response
{
public:
	const std::string& Route() const override
	{
		return m_route;
	}

	const std::string_view& Payload() const override
	{
		return m_payloadView;
	}

	void Payload(const std::string_view& payload) override
	{
		m_payload = std::vector<char>(payload[0], payload[payload.size()]);
		m_payloadView = std::string_view(&m_payload[0], m_payload.size());
	}

	ResultCodes Result() const override { return m_result; }
	void Result(ResultCodes code) override { m_result = code; }

	const std::string& ResultMsg() const override
	{
		return m_resultMsg;
	}

	void ResultMsg(const std::string& msg) override
	{
		m_resultMsg = msg;
	}


	std::vector<char>&& TransferPayload()
	{
		m_payloadView = std::string_view();
		return std::move(m_payload);
	}
private:
	std::string m_route;

	std::string_view m_payloadView;
	std::vector<char> m_payload;

	ResultCodes m_result = ResultCodes::CODE_OK;
	std::string m_resultMsg;
};



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
		const std::string& resource = std::string(msg.Topic());

		RouteInfo routeInfo;
		{
			std::lock_guard lock(m_mx);

			auto it = m_routes.find(resource);
			if (it == m_routes.end())
			{
				return;
			}

			routeInfo = it->second;
		}

		RequestImpl req{ msg };
		ResponseImpl rsp;

		if (routeInfo.routeFn(req, rsp))
		{
			m_broker->Publish(routeInfo.responseTopic, rsp.TransferPayload());
		}
	}

	void OnMsgSent(BaseBroker& broker, const Msg& msg) override
	{
	}


	bool RouteAdd(const std::string& routePath, RouteFn routeFn) override
	{
		const std::string& req = GetRequestRouteTopic(routePath);
		const std::string& rsp = GetResponseRouteTopic(routePath);

		std::lock_guard lock(m_mx);

		auto it = m_routes.find(req);
		if (it == m_routes.end())
		{
			m_routes[req] = RouteInfo{ rsp, routeFn };
			m_broker->SubscribeTopic(req);
			return true;
		}

		return false;
	}

	bool RouteRemove(const std::string& routePath) override
	{
		const std::string& resource = GetRequestRouteTopic(routePath);

		std::lock_guard lock(m_mx);

		auto it = m_routes.find(resource);
		if (it != m_routes.end())
		{
			m_broker->UnsubscribeTopic(resource);
			m_routes.erase(it);
			return true;
		}

		return false;
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
	static std::string GetRequestRouteTopic(const std::string& routePath)
	{
		return routePath;// +"/req";
	}

	static std::string GetResponseRouteTopic(const std::string& routePath)
	{
		return routePath;// +"/rsp";
	}

private:
	struct RouteInfo
	{
		std::string responseTopic;
		RouteFn routeFn;
	};

private:
	::broker::Ptr m_broker;

	std::mutex m_mx;
	std::map<std::string, RouteInfo> m_routes;
};


net_server::Ptr net_server::broker::CreateServer(::broker::Ptr&& broker)
{
	return std::make_unique<BrokerServerImpl>(std::move(broker));
}
