#include "stdafx.h"
#include "mysql_db.h"

#include "mysql.h"

namespace db
{
namespace mysql
{

class MysqlResult : public QueryResult
{
public:
	MysqlResult(MYSQL_RES* result)
		: m_result(result)
		, m_rowTotal(!m_result ? 0 : mysql_num_rows(m_result))
		, m_empty(m_rowTotal == 0)
	{
	}

	~MysqlResult() override
	{
		if (m_result)
		{
			mysql_free_result(m_result);
		}
	}

	bool Empty() const override
	{
		return m_empty;
	}

	bool Bof() const override
	{
		return m_rowOffset == 0;
	}

	bool Eof() const override
	{
		return m_rowOffset == m_rowTotal;
	}

	bool Next() override
	{
		m_row = mysql_fetch_row(m_result);

		if (m_row)
		{
			++m_rowOffset;
		}

		return m_row;
	}

	uint32_t FieldsCount() const override
	{
		return mysql_num_fields(m_result);
	}

	std::string ValueAsString(size_t fieldIndex) const override
	{
		return m_row ? (m_row[fieldIndex] ? m_row[fieldIndex] : "") : "";
	}

private:
	MYSQL_RES* const m_result;
	const my_ulonglong m_rowTotal;
	const bool m_empty;
	my_ulonglong m_rowOffset = 0;
	MYSQL_ROW m_row = nullptr;
};


class MysqlHandle
{
public:
	MysqlHandle(const MysqlHandle&) = delete;
	MysqlHandle& operator= (MysqlHandle&) = delete;

	MysqlHandle()
		: m_handle(mysql_init(nullptr))
	{
	}

	~MysqlHandle()
	{
		if (m_handle)
		{
			mysql_close(m_handle);
		}
	}

	MYSQL* Handle() const
	{
		return m_handle;
	}

private:
	MYSQL* const m_handle;
};

class MysqlDb : public Db
{
public:
	MysqlDb(const Params& params)
		: m_params(params)
	{
	}

	bool Open() override
	{
		Close();

		while (true)
		{
			m_mysql = std::make_unique<MysqlHandle>();
			if (!m_mysql)
			{
				break;
			}

			if (!mysql_real_connect(m_mysql->Handle(), m_params.host.c_str(), m_params.user.c_str(),
					m_params.password.c_str(), m_params.db.c_str(), m_params.port, nullptr, 0))//CLIENT_COMPRESS))
			{
				break;
			}

			if (!Query("SET NAMES 'cp1251'").get())
			{
				break;
			}

			if (!Query("SET CHARACTER SET 'cp1251'").get())
			{
				break;
			}

			return true;
		}

		Close();
		return false;
	}

	void Close()  override
	{
		if (m_mysql)
		{
			m_mysql.reset();
		}
	}

	QueryResult::Ptr Query(const std::string& sql) override
	{
		if (!m_mysql)
		{
			return nullptr;
		}

		if (mysql_real_query(m_mysql->Handle(), sql.c_str(), sql.length()) != 0)
		{
			return nullptr;
		}

		return std::make_unique<MysqlResult>(mysql_store_result(m_mysql->Handle()));
	}

	bool Ping() override
	{
		return m_mysql && (mysql_ping(m_mysql->Handle()) == 0);
	}

	std::string LastError() const override
	{
		if (m_mysql)
		{
			return mysql_error(m_mysql->Handle());
		}

		return "";
	}

	bool BeginTransaction() override
	{
		return Query("START TRANSACTION").get();
	}

	bool CommitTransaction() override
	{
		return Query("COMMIT").get();
	}

	bool RollbackTransaction() override
	{
		return Query("ROLLBACK").get();
	}

	bool EscapeString(const std::string& str, std::string& escapedStr) override
	{
		std::vector<char> buffer(str.length() * 2 + 1);//https://dev.mysql.com/doc/c-api/8.0/en/mysql-real-escape-string.html
		auto result = mysql_real_escape_string(m_mysql->Handle(), &buffer[0], str.c_str(), str.length());

		if (result == -1)
		{
			return false;
		}

		escapedStr.assign(&buffer[0], result);
		return true;
	}

private:
	const Params m_params;
	std::unique_ptr<MysqlHandle> m_mysql;
};

Ptr Create(const Params& params)
{
	return std::make_unique<MysqlDb>(params);
}

}//mysql
}//db
