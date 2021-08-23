#pragma once

namespace logger
{

class Logger
{
public:
	virtual ~Logger() = default;

	void Info(const std::string& name, const std::string& msg);
	void Warn(const std::string& name, const std::string& msg);
	void Debug(const std::string& name, const std::string& msg);
	void Error(const std::string& name, const std::string& msg);
	void Trace(const std::string& name, const std::string& msg);

private:
	virtual void Out(const std::string& level, const std::string& name, const std::string& msg) = 0;
};

using Ptr = std::shared_ptr<Logger>;

Ptr Create(const std::string& fileName);
Ptr Create();

}; //namespace logger
