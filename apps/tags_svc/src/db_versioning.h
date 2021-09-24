#pragma once

#include "db.h"

namespace db
{
	namespace tags
	{
		class DbVersion
		{
		public:
			virtual ~DbVersion() = default;

			virtual bool Upgrade() = 0;
		};
	}
};