#include "stdafx.h"
#include "tags_db.h"

namespace db
{
namespace tags
{

class TagsDbImpl : public TagsDb
{
public:
	TagsDbImpl(db::Ptr&& db, db::versioning::Ptr&& verDb, const Params& params)
		: m_db(std::move(db))
		, m_verDb(std::move(verDb))
	{
		#include "tags_db_ver_1.h"
	}

	//db::Db
	bool Open() override
	{
		return m_db->Open();
	}

	void Close() override
	{
		m_db->Close();
	}

	QueryResult::Ptr Query(const std::string& sql) override
	{
		return m_db->Query(sql);
	}

	bool Ping() override
	{
		return m_db->Ping();
	}

	std::string LastError() const override
	{
		return m_db->LastError();
	}

	bool BeginTransaction() override
	{
		return m_db->BeginTransaction();
	}

	bool CommitTransaction() override
	{
		return CommitTransaction();
	}

	bool RollbackTransaction() override
	{
		return m_db->RollbackTransaction();
	}

	bool EscapeString(const std::string& str, std::string& escapedStr) override
	{
		return m_db->EscapeString(str, escapedStr);
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

	bool NeedUpgrade() const override
	{
		return m_verDb->NeedUpgrade();
	}

private:
	db::Ptr m_db;
	db::versioning::Ptr m_verDb;
};

Ptr Create(db::Ptr&& db, db::versioning::Ptr&& verDb, const Params& params)
{
	return std::make_unique<TagsDbImpl>(std::move(db), std::move(verDb), params);
}

}//tags
}//db
