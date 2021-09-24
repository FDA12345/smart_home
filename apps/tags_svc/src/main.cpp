#include "stdafx.h"
#include "mysql_db.h"
#include "logger.h"

int main()
{
	auto m_log = logger::Create();

	db::mysql::Params params;
	params.host = "192.168.41.11";
	params.db = "db_terminal";
	params.user = "root";
	params.password = "";

	auto db = db::mysql::Create(params);
	if (!db)
	{
		logERROR(__FUNCTION__, "db object not created");
		return -1;
	}

	if (!db->Open())
	{
		logERROR(__FUNCTION__, "db not opened");
		return -2;
	}

	const auto result = db->Query("SELECT * FROM classes");
	if (!result)
	{
		logERROR(__FUNCTION__, "query failed, error: " << db->LastError());
		return -3;
	}

	while (result->Next())
	{
		std::ostringstream oss;
		for (size_t idx = 0; idx < result->FieldsCount(); ++idx)
		{
			if (idx != 0)
			{
				oss << "|";
			}

			oss << result->ValueAsString(idx);
		}

		logINFO(__FUNCTION__, oss.str());
	}

	db->Close();
	db.reset();
}
