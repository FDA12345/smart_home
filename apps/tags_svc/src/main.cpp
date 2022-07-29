#include "stdafx.h"
#include "logger.h"

#include "http_server.h"
#include <thread>

/*
#include "mysql_db.h"
#include "db_versioning.h"
#include "tags_db.h"

#include "mqtt_broker.h"

#include "http_server.h"

#include <chrono>
#include <thread>

const std::string g_brokerClientId = "tags_db_client";
const std::string g_msgTopicName = g_brokerClientId + "/updateSensor";


auto m_log = logger::Create();


class OnBrokerEvents : public broker::BrokerEvents
{
public:
	void OnConnected(broker::BaseBroker& broker) override
	{
		logINFO(__FUNCTION__, "broker connected");
	}

	void OnDisconnected(broker::BaseBroker& broker) override
	{
		logINFO(__FUNCTION__, "broker disconnected");
	}

	void OnMsgRecv(broker::BaseBroker& broker, const broker::Msg& msg) override
	{
		logDEBUG(__FUNCTION__, "broker msg received: " << std::string(msg.Topic()));

		if (g_msgTopicName != msg.Topic())
		{
			return;
		}
	}

	void OnMsgSent(broker::BaseBroker& broker, const broker::Msg& msg) override	{}
};

int main()
{
	db::mysql::Params params;
	params.host = "192.168.41.11";
	params.db = "smarthome_tags";
	params.user = "root";
	params.password = "";

	auto tagsDb = db::tags::Create(db::mysql::Create(params), db::versioning::Create(db::mysql::Create(params)), {});

	if (tagsDb->NeedUpgrade())
	{
		logINFO(__FUNCTION__, "Upgrading database ...");

		if (!tagsDb->Upgrade())
		{
			logERROR(__FUNCTION__, "Upgrading database failed");
			return -1;
		}

		logINFO(__FUNCTION__, "Upgrading database done");
	}

	while (true)
	{
		if (!tagsDb->Open())
		{
			logERROR(__FUNCTION__, "open database error");
			std::this_thread::sleep_for(std::chrono::seconds(10));
			continue;
		}


		auto broker = broker::mqtt::Create("192.168.41.11", g_brokerClientId);
		if (!broker)
		{
			logERROR(__FUNCTION__, "broker create failed");
			return -1;
		}

		OnBrokerEvents brokerEvents;
		broker->SubscribeEvents(brokerEvents);
		broker->SubscribeTopic(g_msgTopicName);

		if (!broker->Start())
		{
			logERROR(__FUNCTION__, "broker not started");
			return -1;
		}

		while (true)
		{
			if (!tagsDb->Ping())
			{
				tagsDb->Close();
				break;
			}

			std::this_thread::sleep_for(std::chrono::seconds(10));
		}

		broker->Stop();
		broker.reset();
	}

	tagsDb->Close();
	tagsDb.reset();
}
*/

void SplitParam(std::map<std::string, std::string>& params, const std::string& param)
{
	const auto delimPos = param.find('=');
	if (delimPos == std::string::npos)
	{
		return;
	}

	params[param.substr(0, delimPos)] = param.substr(delimPos + 1);
}

std::map<std::string, std::string> SplitQueryParams(const std::string& query)
{
	std::map<std::string, std::string> params;

	size_t offset = 0;
	while (offset < query.length())
	{
		const auto delimPos = query.find('&', offset);

		SplitParam(params, query.substr(offset, delimPos - offset));

		if (delimPos != std::string::npos)
		{
			offset = delimPos + 1;
		}
		else
		{
			break;
		}
	}

	return std::move(params);
}

int main()
{
	auto m_log = logger::Create();

	net_server::http::Params httpParams;
	httpParams.port = 16801;
	httpParams.serverName = "Sensor Filtering Svc 1.0";

	auto server = net_server::http::CreateServer(httpParams);

	//http://127.0.0.1:16801/filter?value=11&alg=aka_noolite&alg_param_1=15&alg_param_2=60
	server->RouteAdd("/filter", [](const net_server::Request& req, net_server::Response& rsp) -> bool
	{
		const auto queryPos = req.Route().find('?');
		const auto query = (queryPos == std::string::npos) ? "" : req.Route().substr(queryPos + 1);
		const auto params = SplitQueryParams(query);

		auto valueIt = params.find("value");
		if (valueIt == std::end(params))
		{
			return false;
		}

		rsp.Payload(valueIt->second);
		return true;
	});

	if (!server->Start())
	{
		logERROR(__FUNCTION__, "connot create http server");
		return -1;
	}

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	server->Stop();
	server.reset();
}
