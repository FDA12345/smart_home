#pragma once

#include <string>

namespace db
{

class QueryResult
{
public:
	using Ptr = std::unique_ptr<QueryResult>;

public:
	virtual ~QueryResult() = default;

	virtual bool Empty() const = 0;

	virtual bool Bof() const = 0;
	virtual bool Eof() const = 0;

	virtual bool Next() = 0;

	virtual uint32_t FieldsCount() const = 0;
	virtual std::string ValueAsString(size_t fieldIndex) const = 0;
};

class Db
{
public:
	virtual ~Db() = default;

	virtual bool Open() = 0;
	virtual void Close() = 0;

	virtual QueryResult::Ptr Query(const std::string& sql) = 0;
	virtual bool Ping() = 0;

	virtual std::string LastError() const = 0;

	virtual bool BeginTransaction() = 0;
	virtual bool CommitTransaction() = 0;
	virtual bool RollbackTransaction() = 0;
};

using Ptr = std::unique_ptr<Db>;

};