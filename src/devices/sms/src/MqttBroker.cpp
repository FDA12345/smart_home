#include "MqttBroker.h"
#include "MQTTClient.h"

#include <set>
#include <unordered_set>


class Msg
{
public:
	Msg(Msg&& right)
		: m_topicName(right.m_topicName)
		, m_topicLen(right.m_topicLen)
		, m_message(right.m_message)
		, m_topic(m_topicName)
	{
		right.m_topicName = nullptr;
		right.m_message = nullptr;
	}

	Msg(char* topicName, int topicLen, MQTTClient_message* message)
		: m_topicName(topicName)
		, m_topicLen(topicLen)
		, m_message(message)
		, m_topic(topicName)
	{
	}

	~Msg()
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

	bool operator< (const Msg& right) const
	{
		return strcmp(m_topicName, right.m_topicName) < 0;
	}

private:
	char* m_topicName;
	int m_topicLen;
	MQTTClient_message* m_message;
	std::string_view m_topic;
};


class MqttBrokerImpl : public Broker
{
public:
	bool Start() override
	{
		Stop();

		while (true)
		{
			//if (MQTTClient_create(&m_client, "tcp://192.168.41.11:1883", "ExampleClientPub",
			//if (MQTTClient_create(&m_client, "ws://mqtt.eclipseprojects.io:80", "fda123456789",
			if (MQTTClient_create(&m_client, "tcp://mqtt.eclipseprojects.io:1883", "fda123456789",
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
				std::lock_guard<std::mutex> lock(m_stopMx);
				m_stopped = true;
				m_stopCv.notify_one();
			}

			m_processThread.join();
			m_processThread = std::thread();
		}

		m_clientPtr.reset();
	}

private:
	void DoProcess()
	{
		while (!m_stopped)
		{
			bool connected = false;
			{
				std::lock_guard<std::mutex> lock(m_connectMx);

				m_connected = Connect();
				connected = m_connected;
			}

			if (!connected)
			{
				std::unique_lock<std::mutex> lock(m_stopMx);
				if (m_stopped)
				{
					continue;
				}
				m_stopCv.wait_for(lock, std::chrono::seconds(RECONNECT_INTERVAL), [this]() {return m_stopped; });
				continue;
			}

			std::cout << "connected" << std::endl;

			MQTTClient_subscribe(m_client, "$SYS/#", 0);
			MQTTClient_subscribe(m_client, "#", 0);

			while (!m_stopped && m_connected)
			{
				std::unique_lock<std::mutex> lock(m_stopMx);
				if (m_stopped)
				{
					continue;
				}

				m_stopCv.wait_for(lock, std::chrono::seconds(1), [this]() {return m_stopped; });
				std::cout << "m_messages count = " << m_topicNames.size() << std::endl;
			}

			if (!m_connected)
			{
				std::cout << "disconnected" << std::endl;
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

	static void __onConnLost(void* context, char* cause)
	{
		static_cast<MqttBrokerImpl*>(context)->OnConnLost(cause);
	}
	void OnConnLost(char* cause)
	{
		std::lock_guard<std::mutex> lock(m_connectMx);
		m_connected = false;
		m_connectCv.notify_one();
	}

	static int __onMessageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* message)
	{
		static_cast<MqttBrokerImpl*>(context)->OnMessageArrived({ topicName, topicLen, message });
		return 1;
	}

	std::set<Msg> m_topicNames;
	void OnMessageArrived(Msg&& msg)
	{
		auto it = m_topicNames.find(msg);
		if (it == m_topicNames.end())
		{
			m_topicNames.emplace(std::move(msg));
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

	std::mutex m_stopMx;
	std::condition_variable m_stopCv;
	bool m_stopped = true;

	std::mutex m_connectMx;
	std::condition_variable m_connectCv;
	bool m_connected = false;
};


Broker::Ptr MqttBroker::Create()
{
	return std::make_unique<MqttBrokerImpl>();
}
