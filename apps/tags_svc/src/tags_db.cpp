#include "stdafx.h"
#include "tags_db.h"

namespace db
{
namespace tags
{

class TagsDbImpl : public TagsDb
{
public:
	TagsDbImpl(const db::Ptr& db, db::versioning::Ptr&& verDb, const Params& params)
		: m_db(db)
		, m_verDb(std::move(m_verDb))
	{
		#include "tags_db_ver_1.h"
	}

	//db::versioning::DbVersioning
	bool AddVersion(db::versioning::DbVersion::Ptr&& dbVersion) override
	{
		return m_verDb->AddVersion(std::move(dbVersion));
	}

	uint64_t Version() const override
	{
		return m_verDb->Version();
	}

	bool Upgrade() override
	{
		return m_verDb->Upgrade();
	}

	bool Upgrade(uint64_t version) override
	{
		return m_verDb->Upgrade(version);
	}

	bool Downgrade() override
	{
		return m_verDb->Downgrade();
	}

	bool Downgrade(uint64_t version) override
	{
		return m_verDb->Downgrade(version);
	}

private:
	db::Ptr m_db;
	db::versioning::Ptr m_verDb;
};

Ptr Create(const db::Ptr& db, db::versioning::Ptr&& verDb, const Params& params)
{
	return std::make_unique<TagsDbImpl>(db, std::move(verDb), params);
}

}//tags
}//db
