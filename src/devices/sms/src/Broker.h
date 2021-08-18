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


class TopicEvents
{
public:
	using Ptr = std::shared_ptr<TopicEvents>;

public:
	virtual ~TopicEvents() = default;

	virtual void OnMsgRecv(const TopicMsg& topicMsg) = 0;
	virtual void OnMsgSent(const TopicMsg& topicMsg) = 0;
};


class Broker
{
public:
	using Ptr = std::unique_ptr<Broker>;

public:
	virtual ~Broker() = default;

	virtual bool Start() = 0;
	virtual void Stop() = 0;

	virtual void SubscribeEvents(const TopicEvents::Ptr& eventsPtr) = 0;
	virtual void UnsubscribeEvents(const TopicEvents::Ptr& eventsPtr) = 0;

	virtual void SubscribeTopic(const std::string& topicName) = 0;
	virtual void UnsubscribeTopic(const std::string& topicName) = 0;
};

}; //namespace broker
