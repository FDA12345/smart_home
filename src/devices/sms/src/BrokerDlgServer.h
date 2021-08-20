#pragma once

#include "common/DlgServer.h"

class BrokerDlgServer : public dlg_server::Server
{
public:
	using Ptr = std::unique_ptr<dlg_server::Server>;

public:
	static Ptr Create();
};