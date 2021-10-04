#pragma once

#include "db.h"

namespace db
{
	namespace versioning
	{

		class DbVersion
		{
		public:
			using Ptr = std::unique_ptr<DbVersion>;

		public:
			virtual ~DbVersion() = default;

			virtual uint64_t Version() const = 0;
			virtual std::string Description() const = 0;

			virtual bool CanUpgrade(const db::Ptr& db) const { return true; }
			virtual bool CanDowngrade(const db::Ptr& db) const { return false; }

			virtual bool Upgrade(const db::Ptr& db) = 0;
			virtual bool Downgrade(const db::Ptr& db) { return false; };
		};

		class DbVersioning
		{
		public:
			virtual ~DbVersioning() = default;

			virtual bool AddVersion(DbVersion::Ptr&& dbVersion) = 0;

			virtual uint64_t Version() const = 0;//0 -> not versioned yet|db is empty

			virtual bool Upgrade() = 0;//upgrade to last version
			virtual bool Upgrade(uint64_t version) = 0;

			virtual bool Downgrade() = 0;//downgrade to previous version
			virtual bool Downgrade(uint64_t version) = 0;
		};

		using Ptr = std::unique_ptr<DbVersioning>;
		Ptr Create(const db::Ptr& db);
	}
}
