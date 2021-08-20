#include "BrokerDlgServer.h"

class BrokerDlgServerImpl : public DlgServer
{
public:
	bool AddRoute(const std::string& path, std::function<void(const std::string_view& payload)> routeFn) override
	{
		return false;
	}

	bool RemoveRoute(const std::string& path) override
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
};


BrokerDlgServer::Ptr BrokerDlgServer::Create()
{
	return nullptr;
}
