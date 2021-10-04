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
	params.db = "smarthome_tags";
	params.user = "root";
	params.password = "";

	auto tagsDb = db::tags::Create(db::mysql::Create(params), db::versioning::Create(db::mysql::Create(params)), {});
	if (!tagsDb->Upgrade())
	{
		return -1;
	}
}
