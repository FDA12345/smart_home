#pragma once

namespace broker
{

class Msg
{
public:
	virtual ~Msg() = default;

	virtual const std::string_view& Topic() const = 0;
	virtual const std::string_view& Payload() const = 0;
};


class BaseBroker;
class BrokerEvents
{
public:
	virtual ~BrokerEvents() = default;

	virtual void OnConnected(BaseBroker& broker) = 0;
	virtual void OnDisconnected(BaseBroker& broker) = 0;

	virtual void OnMsgRecv(BaseBroker& broker, const Msg& msg) = 0;
	virtual void OnMsgSent(BaseBroker& broker, const Msg& msg) = 0;
};


class BaseBroker
{
public:
	virtual ~BaseBroker() = default;

	virtual void SubscribeTopic(const std::string& topicName) = 0;
	virtual void UnsubscribeTopic(const std::string& topicName) = 0;
};


class Broker : public BaseBroker
{
public:
	using Ptr = std::unique_ptr<Broker>;

public:
	virtual bool Start() = 0;
	virtual void Stop() = 0;

	virtual void SubscribeEvents(BrokerEvents& brokerEvents) = 0;
	virtual void UnsubscribeEvents(BrokerEvents& brokerEvents) = 0;
};

}; //namespace broker
