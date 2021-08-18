#include "MqttBroker.h"
#include "MQTTClient.h"

using namespace broker;


class MqttMsg : public TopicMsg
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
				std::lock_guard<std::mutex> lock(m_mx);
				m_stopped = true;
				m_stopCv.notify_one();
			}

			m_processThread.join();
			m_processThread = std::thread();
		}

		m_clientPtr.reset();
	}

	void SubscribeEvents(const TopicEvents::Ptr& eventsPtr) override
	{
		std::lock_guard lock(m_mx);
		m_owners.emplace_back(eventsPtr);
	}

	void UnsubscribeEvents(const TopicEvents::Ptr& eventsPtr) override
	{
		std::lock_guard lock(m_mx);

		auto it = std::find(m_owners.begin(), m_owners.end(), eventsPtr);
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

			if (m_connected)
			{
				MQTTClient_subscribe(m_client, topicName.c_str(), 0);
			}
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

		MQTTClient_unsubscribe(m_client, topicName.c_str());
	}

private:
	void DoProcess()
	{
		while (!m_stopped)
		{
			bool connected = false;
			{
				std::lock_guard<std::mutex> lock(m_mx);

				m_connected = Connect();
				connected = m_connected;
			}

			if (!connected)
			{
				std::unique_lock<std::mutex> lock(m_mx);
				if (m_stopped)
				{
					continue;
				}
				m_stopCv.wait_for(lock, std::chrono::seconds(RECONNECT_INTERVAL), [this]() {return m_stopped; });
				continue;
			}

			OnConnected();

			//MQTTClient_subscribe(m_client, "$SYS/#", 0);
			//MQTTClient_subscribe(m_client, "#", 0);

			while (!m_stopped && m_connected)
			{
				std::unique_lock<std::mutex> lock(m_mx);
				if (m_stopped)
				{
					continue;
				}

				m_stopCv.wait_for(lock, std::chrono::seconds(1), [this]() {return m_stopped; });
				//std::cout << "m_messages count = " << m_topicNames.size() << std::endl;
			}

			if (!m_connected)
			{
				OnDisconnected();
			}
		}

		if (MQTTClient_isConnected(m_client))
		{
			MQTTClient_disconnect(m_client, DISCONNECT_TIMEOUT);
		}
	}

	bool Connect() const
	{
		MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
		opts.keepAliveInterval = KEEP_ALIVE_INTERVAL;
		opts.cleansession = 1;
		opts.connectTimeout = CONNECT_TIMEOUT;

		return MQTTClient_connect(m_client, &opts) == MQTTCLIENT_SUCCESS;
	}

	void OnConnected()
	{
		std::cout << "connected" << std::endl;

		std::lock_guard lock(m_mx);

		for (const auto& subs : m_subs)
		{
			MQTTClient_subscribe(m_client, subs.first.c_str(), 0);
		}
	}

	void OnDisconnected()
	{
		std::cout << "disconnected" << std::endl;
	}

	static void __onConnLost(void* context, char* cause)
	{
		static_cast<MqttBrokerImpl*>(context)->OnConnLost(cause);
	}
	void OnConnLost(char* cause)
	{
		std::lock_guard<std::mutex> lock(m_mx);
		m_connected = false;
		m_connectCv.notify_one();
	}

	static int __onMessageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* message)
	{
		static_cast<MqttBrokerImpl*>(context)->OnMessageArrived({ topicName, topicLen, message });
		return 1;
	}
	void OnMessageArrived(const MqttMsg& msg)
	{
		std::vector<TopicEvents::Ptr> owners;

		{
			std::lock_guard lock(m_mx);
			for (auto& owner : m_owners)
			{
				owners.push_back(owner);
			}
		}

		for (const auto& owner : owners)
		{
			owner->OnMsgRecv(msg);
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

	std::condition_variable m_stopCv;
	bool m_stopped = true;

	std::condition_variable m_connectCv;
	bool m_connected = false;

	std::vector<TopicEvents::Ptr> m_owners;
	std::map<std::string, size_t> m_subs;
};


Broker::Ptr MqttBroker::Create(const std::string& address, const std::string& clientId)
{
	return std::make_unique<MqttBrokerImpl>(address, clientId);
}
