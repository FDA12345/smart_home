#pragma once

#include "db.h"

namespace db
{
	namespace mysql
	{
		struct Params
		{
			std::string host;
			uint16_t port = 3306;

			std::string db;

			std::string user;
			std::string password;
		};

		Ptr Create(const Params& params);
	}
};