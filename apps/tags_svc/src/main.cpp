#include "stdafx.h"
#include "mysql_db.h"
#include "logger.h"
#include "db_versioning.h"
#include "tags_db.h"

int main()
{
	auto m_log = logger::Create();

	db::mysql::Params params;
	params.host = "192.168.41.11";
	params.db = "db_terminal";
	params.user = "root";
	params.password = "";

	auto db = db::mysql::Create(params);
	auto verDb = db::versioning::Create(db);

	auto tagsDb = db::tags::Create(std::move(db), std::move(verDb), {});
	if (!tagsDb->Upgrade())
	{
		return -1;
	}
}
