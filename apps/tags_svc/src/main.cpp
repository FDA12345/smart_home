#include "stdafx.h"
#include "mysql_db.h"
#include "logger.h"
#include "db_versioning.h"

int main()
{
	auto m_log = logger::Create();

	db::mysql::Params params;
	params.host = "192.168.41.11";
	params.db = "db_terminal";
	params.user = "root";
	params.password = "";

	auto dbVer = db::versioning::Create(db::mysql::Create(params));
	dbVer->Upgrade();
}
