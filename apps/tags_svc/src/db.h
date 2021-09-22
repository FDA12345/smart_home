#pragma once

namespace db
{

using Field = std::string;
using Fields = std::vector<Field>;

class Db
{
public:
	virtual ~Db() = default;

	virtual bool Open() = 0;
	virtual void Close() = 0;

	virtual std::tuple<bool, Fields> Query(const std::string& sql) = 0;
};

using Ptr = std::unique_ptr<Db>;

};