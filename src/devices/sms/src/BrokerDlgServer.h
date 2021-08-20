#pragma once

#include "common/DlgServer.h"

class BrokerDlgServer : public DlgServer
{
public:
	using Ptr = std::unique_ptr<DlgServer>;

public:
	static Ptr Create();
};