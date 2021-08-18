#include "MqttBroker.h"
#include "MQTTClient.h"

using namespace broker;


class MqttMsg : public Msg
{
public:
	MqttMsg(MqttMsg&& right)
		: m_topicName(right.m_topicName)
		, m_topicLen(right.m_topicLen)
		, m_message(right.m_message)
		, m_topic(m_topicName)
		, m_payload(static_cast<const char*>(m_message->payload), m_message->payloadlen)
	{
		right.m_topicName = nullptr;
		right.m_message = nullptr;
	}

	MqttMsg(char* topicName, int topicLen, MQTTClient_message* message)
		: m_topicName(topicName)
		, m_topicLen(topicLen)
		, m_message(message)
		, m_topic(topicName)
		, m_payload(static_cast<const char*>(m_message->payload), m_message->payloadlen)
	{
	}

	~MqttMsg()
	{
		if (m_topicName != nullptr)
		{
			MQTTClient_free(m_topicName);
		}

		if (m_message != nullptr)
		{
			MQTTClient_freeMessage(&m_message);
		}
	}

	const std::string_view& Topic() const override
	{
		return m_topic;
	}

	const std::string_view& Payload() const override
	{
		return m_payload;
	}

private:
	char* m_topicName;
	int m_topicLen;
	MQTTClient_message* m_message;

	std::string_view m_topic;
	std::string_view m_payload;
};




class MqttBrokerImpl : public Broker
{
public:
	MqttBrokerImpl(const std::string& address, const std::string& clientId)
		: m_address(address)
		, m_clientId(clientId)
	{
	}

	bool Start() override
	{
		Stop();

		while (true)
		{
			if (MQTTClient_create(&m_client, m_address.c_str(), m_clientId.c_str(),
					MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS)
			{
				break;
			}
			m_clientPtr.reset(&m_client);

			if (MQTTClient_setCallbacks(m_client, this, __onConnLost, __onMessageArrived, __onDeliveryComplete) != MQTTCLIENT_SUCCESS)
			{
				break;
			}

			m_stopped = false;
			m_processThread = std::thread(std::bind(&MqttBrokerImpl::DoProcess, this));
			return true;
		}

		Stop();
		return false;
	}

	void Stop() override
	{
		if (m_processThread.joinable())
		{
			{
				std::lock_guard lock(m_mx);

				m_stopped = true;
				m_cv.notify_one();
			}

			m_processThread.join();
			m_processThread = std::thread();
		}

		m_clientPtr.reset();
	}

	void SubscribeEvents(BrokerEvents& brokerEvents) override
	{
		std::lock_guard lock(m_mx);
		m_owners.emplace_back(&brokerEvents);
	}

	void UnsubscribeEvents(BrokerEvents& brokerEvents) override
	{
		std::lock_guard lock(m_mx);

		auto it = std::find(m_owners.begin(), m_owners.end(), &brokerEvents);
		if (it != m_owners.end())
		{
			m_owners.erase(it);
		}
	}

	void SubscribeTopic(const std::string& topicName) override
	{
		std::lock_guard lock(m_mx);

		auto it = m_subs.find(topicName);
		if (it == m_subs.end())
		{
			it = m_subs.emplace(topicName, 0).first;

			m_subscribes.push_back(topicName);
			m_subscribed = true;
			m_cv.notify_one();
		}
		++it->second;
	}

	void UnsubscribeTopic(const std::string& topicName) override
	{
		std::lock_guard lock(m_mx);

		auto it = m_subs.find(topicName);
		if (it != m_subs.end())
		{
			if ((--it->second) > 0)
			{
				return;
			}
		}

		m_unsubscribes.push_back(topicName);
		m_unsubscribed = true;
		m_cv.notify_one();
	}


private:
	void DoProcess()
	{
		m_state = STATE_CONNECT;
		m_disconnected = true;
		m_subscribed = false;
		m_unsubscribed = false;

		while (!m_stopped)
		{
			switch (m_state)
			{
			case STATE_CONNECT:
				{
					m_disconnected = !Connect();
					m_state =!m_disconnected ? STATE_JUST_CONNECTED : STATE_RECONNECT;
				}
				break;

			case STATE_RECONNECT:
				{
					WaitReconnect();
					m_state = STATE_CONNECT;
				}
				break;

			case STATE_JUST_CONNECTED:
				{
					FireConnected();
					PrepareSubscribes();
					m_state = STATE_SUBSCRIBE;
				}
				break;

			case STATE_SUBSCRIBE:
				{
					PerformSubscribe();
					PerformUnsubscribe();
					m_state = STATE_READY;
				}
				break;

			case STATE_READY:
				{
					PerformReadyState();
				}
				break;

			case STATE_DISCONNECTED:
				{
					FireDisconnected();
					m_state = STATE_RECONNECT;
				}
				break;
			};
		}

		Disconnect();
	}

	bool Connect() const
	{
		MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
		opts.keepAliveInterval = KEEP_ALIVE_INTERVAL;
		opts.cleansession = 1;
		opts.connectTimeout = CONNECT_TIMEOUT;

		return MQTTClient_connect(m_client, &opts) == MQTTCLIENT_SUCCESS;
	}

	void WaitReconnect()
	{
		std::unique_lock<std::mutex> lock(m_mx);
		if (m_stopped)
		{
			return;
		}
		m_cv.wait_for(lock, std::chrono::seconds(RECONNECT_INTERVAL), [this]() {return m_stopped; });
	}

	void FireConnected()
	{
		std::vector<BrokerEvents*> owners;

		{
			std::lock_guard lock(m_mx);

			for (auto& owner : m_owners)
			{
				owners.push_back(owner);
			}
		}

		for (const auto& owner : owners)
		{
			owner->OnConnected(*this);
		}
	}

	void PerformSubscribe()
	{
		std::vector<std::string> subs;

		{
			std::lock_guard lock(m_mx);

			for (const auto& s : m_subscribes)
			{
				subs.push_back(s);
			}

			m_subscribes.clear();
		}

		for (const auto& s : subs)
		{
			if (!MQTTClient_isConnected(m_client))
			{
				break;
			}

			if (MQTTClient_subscribe(m_client, s.c_str(), 0) != MQTTCLIENT_SUCCESS)
			{
				break;
			}
		}
	}

	void PerformUnsubscribe()
	{
		std::vector<std::string> unsubs;

		{
			std::lock_guard lock(m_mx);

			for (const auto& s : m_unsubscribes)
			{
				unsubs.push_back(s);
			}

			m_unsubscribes.clear();
		}

		for (const auto& s : unsubs)
		{
			if (!MQTTClient_isConnected(m_client))
			{
				break;
			}

			if (MQTTClient_unsubscribe(m_client, s.c_str()) != MQTTCLIENT_SUCCESS)
			{
				break;
			}
		}
	}

	void PerformReadyState()
	{
		std::unique_lock lock(m_mx);

		while (true)
		{
			if (m_stopped)
			{
				m_state = STATE_STOPPED;
			}
			else if (m_disconnected)
			{
				m_state = STATE_DISCONNECTED;
			}
			else if (m_subscribed || m_unsubscribed)
			{
				m_state = STATE_SUBSCRIBE;
			}
			else
			{
				m_cv.wait(lock, [this]
				{
					return m_stopped || m_disconnected || m_subscribed || m_unsubscribed;
				});
				continue;
			}

			break;
		}
	}

	void FireDisconnected()
	{
		std::vector<BrokerEvents*> owners;

		{
			std::lock_guard lock(m_mx);
			for (auto& owner : m_owners)
			{
				owners.push_back(owner);
			}
		}

		for (const auto& owner : owners)
		{
			owner->OnDisconnected(*this);
		}
	}

	void PrepareSubscribes()
	{
		std::lock_guard lock(m_mx);

		m_unsubscribes.clear();
		m_unsubscribed = false;

		m_subscribed = false;
		m_subscribes.clear();

		for (const auto& s : m_subs)
		{
			m_subscribes.push_back(s.first);
		}
	}

	void Disconnect()
	{
		if (MQTTClient_isConnected(m_client))
		{
			MQTTClient_disconnect(m_client, DISCONNECT_TIMEOUT);
		}
	}

	static void __onConnLost(void* context, char* cause)
	{
		static_cast<MqttBrokerImpl*>(context)->OnConnLost(cause);
	}
	void OnConnLost(char* cause)
	{
		std::lock_guard lock(m_mx);

		m_disconnected = true;
		m_cv.notify_one();
	}

	static int __onMessageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* message)
	{
		static_cast<MqttBrokerImpl*>(context)->OnMessageArrived({ topicName, topicLen, message });
		return 1;
	}
	void OnMessageArrived(const MqttMsg& msg)
	{
		std::vector<BrokerEvents*> owners;

		{
			std::lock_guard lock(m_mx);
			for (auto& owner : m_owners)
			{
				owners.push_back(owner);
			}
		}

		for (const auto& owner : owners)
		{
			owner->OnMsgRecv(*this, msg);
		}
	}

	static void __onDeliveryComplete(void* context, MQTTClient_deliveryToken dt)
	{
		static_cast<MqttBrokerImpl*>(context)->OnDeliveryComplete(dt);
	}
	void OnDeliveryComplete(MQTTClient_deliveryToken dt)
	{
	}

private:
	const std::string m_address;
	const std::string m_clientId;

	enum State
	{
		STATE_CONNECT,
		STATE_JUST_CONNECTED,
		STATE_RECONNECT,
		STATE_READY,
		STATE_DISCONNECTED,
		STATE_STOPPED,
		STATE_SUBSCRIBE,
	};
	State m_state = STATE_CONNECT;

	enum
	{
		CONNECT_TIMEOUT = 5,
		DISCONNECT_TIMEOUT = 3,
		RECONNECT_INTERVAL = 10,
		KEEP_ALIVE_INTERVAL = 30,
	};

	class MQTTClientDestroyer
	{
	public:
		void operator() (MQTTClient* client) const
		{
			MQTTClient_destroy(client);
			(*client) = nullptr;
		}
	};
	MQTTClient m_client = nullptr;
	std::unique_ptr<MQTTClient, MQTTClientDestroyer> m_clientPtr;

	std::thread m_processThread;

	std::mutex m_mx;
	std::condition_variable m_cv;

	bool m_stopped = true;
	bool m_disconnected = false;
	bool m_subscribed = false;
	bool m_unsubscribed = false;

	std::vector<BrokerEvents*> m_owners;

	std::map<std::string, size_t> m_subs;
	std::list<std::string> m_subscribes;
	std::list<std::string> m_unsubscribes;
};


Broker::Ptr MqttBroker::Create(const std::string& address, const std::string& clientId)
{
	return std::make_unique<MqttBrokerImpl>(address, clientId);
}
