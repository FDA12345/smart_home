#include "stdafx.h"
#include "mysql_db.h"

#include "mysql.h"

namespace db
{
namespace mysql
{

class MysqlDb : public Db
{
public:
	MysqlDb()
	{
		MYSQL mysql;
		mysql_init(&mysql);
	}

	bool Open() override
	{
		return false;
	}

	void Close()  override
	{
	}

	std::tuple<bool, Fields> Query(const std::string& sql)  override
	{
		std::tuple<bool, Fields> result;
		return std::move(result);
	}
};

Ptr Create()
{
	return std::make_unique<MysqlDb>();
}

}//mysql
}//db
