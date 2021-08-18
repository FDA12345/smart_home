#pragma once

namespace broker
{

class TopicMsg
{
public:
	virtual ~TopicMsg() = default;

	virtual const std::string_view& Topic() const = 0;
	virtual const std::string_view& Payload() const = 0;
};


class Broker;
class BrokerEvents
{
public:
	virtual ~BrokerEvents() = default;

	virtual void OnConnected(Broker& broker) = 0;
	virtual void OnDisconnected(Broker& broker) = 0;

	virtual void OnMsgRecv(Broker& broker, const TopicMsg& topicMsg) = 0;
	virtual void OnMsgSent(Broker& broker, const TopicMsg& topicMsg) = 0;
};


class Broker
{
public:
	using Ptr = std::unique_ptr<Broker>;

public:
	virtual ~Broker() = default;

	virtual bool Start() = 0;
	virtual void Stop() = 0;

	virtual void SubscribeEvents(BrokerEvents& brokerEvents) = 0;
	virtual void UnsubscribeEvents(BrokerEvents& brokerEvents) = 0;

	virtual void SubscribeTopic(const std::string& topicName) = 0;
	virtual void UnsubscribeTopic(const std::string& topicName) = 0;
};

}; //namespace broker
