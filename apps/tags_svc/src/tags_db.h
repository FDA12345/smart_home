#pragma once

#include "db_versioning.h"

namespace db
{
	namespace tags
	{
		struct Params
		{
		};

		class TagsDb :
			public db::Db,
			public db::versioning::DbVersioning
		{
		public:
			virtual ~TagsDb() = default;

		};

		using Ptr = std::unique_ptr<TagsDb>;

		Ptr Create(db::Ptr&& db, db::versioning::Ptr&& verDb, const Params& params);
	}
};