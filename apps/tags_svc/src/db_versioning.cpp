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

	bool AddVersion(DbVersion::Ptr&& dbVersion) override
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
		return m_version;
	}

	bool Upgrade() override
	{
		//perform clean install
		if (m_version == 0)
		{
			return UpgradeFrom(std::begin(m_versions));
		}
		else
		{
			auto it = m_versions.find(m_version);
			if (it == std::end(m_versions))
			{
				return false;
			}

			++it;
			if (it != std::end(m_versions))
			{
				return UpgradeFrom(it);
			}

			//upgrade not needed
			return true;
		}
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
	using VersionsMap = std::map<uint64_t, DbVersion::Ptr>;

	bool UpgradeFrom(const VersionsMap::const_iterator& it)
	{
		for (auto curIt = it; curIt != std::end(m_versions); ++curIt)
		{
			m_db->BeginTransaction();

			const auto& version = curIt->second;

			if (!UpgradeToVersion(version))
			{
				m_db->RollbackTransaction();
				return false;
			}

			m_db->CommitTransaction();
		}

		return true;
	}

	bool UpgradeToVersion(const DbVersion::Ptr& version)
	{
		if (!version->CanUpgrade(m_db) || !version->Upgrade(m_db))
		{
			return false;
		}

		if (!UpgradeVersionsTable(version))
		{
			return false;
		}

		return true;
	}

	bool UpgradeVersionsTable(const DbVersion::Ptr& version)
	{
		return false;
	}

	void LoadVersion()
	{
		if (m_db->Open())
		{
			const auto recs = m_db->Query("SELECT MAX(id) FROM versions");
			if (recs && !recs->Empty())
			{
				m_version = std::atoll(recs->ValueAsString(0).c_str());
			}

			m_db->Close();
		}
	}

private:
	const db::Ptr m_db;

	uint64_t m_version = 0;
	VersionsMap m_versions;
};

Ptr Create(db::Ptr&& db)
{
	return std::make_unique<DbVersioningImpl>(std::move(db));
}

}
};