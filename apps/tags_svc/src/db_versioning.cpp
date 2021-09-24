#pragma once

#include "db_versioning.h"

namespace db
{
namespace versioning
{

class DbVersioningImpl : public DbVersioning
{
public:
	DbVersioningImpl(db::Ptr&& db)
		: m_db(std::move(db))
	{
		LoadVersion();
	}

	bool RegisterVersion(DbVersion::Ptr&& dbVersion) override
	{
		auto it = m_versions.find(dbVersion->Version());
		if (it != std::end(m_versions))
		{
			return false;
		}

		m_versions.emplace(std::make_pair(dbVersion->Version(), std::move(dbVersion)));
		return true;
	}

	uint64_t Version() const override
	{
		return 0;
	}

	std::string Description() const override
	{
		return "";
	}

	bool Upgrade() override
	{
		return false;
	}

	bool Upgrade(uint64_t version) override
	{
		return false;
	}

	bool Downgrade() override
	{
		return false;
	}

	bool Downgrade(uint64_t version) override
	{
		return false;
	}

private:
	void LoadVersion()
	{
	}

private:
	db::Ptr m_db;
	std::map<uint64_t, DbVersion::Ptr> m_versions;
};

DbVersioning::Ptr Create(db::Ptr&& db)
{
	return std::make_unique<DbVersioningImpl>(std::move(db));
}

}
};