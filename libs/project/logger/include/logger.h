#pragma once

#include <string>
#include <sstream>
#include <memory>

#define logOUT(level, name, msg) {\
	std::stringstream ss; \
	ss << msg; \
	m_log->Out(level, name, ss.str()); \
}

#define logINFO(name, msg)  logOUT("INFO", name, msg)
#define logWARN(name, msg)  logOUT("WARN", name, msg)
#define logERROR(name, msg) logOUT("ERROR", name, msg)
#define logDEBUG(name, msg) logOUT("DEBUG", name, msg)
#define logTRACE(name, msg) logOUT("TRACE", name, msg)

namespace logger
{

class Logger
{
public:
	virtual ~Logger() = default;

	virtual void Info(const std::string& name, const std::string& msg) = 0;
	virtual void Warn(const std::string& name, const std::string& msg) = 0;
	virtual void Error(const std::string& name, const std::string& msg) = 0;
	virtual void Debug(const std::string& name, const std::string& msg) = 0;
	virtual void Trace(const std::string& name, const std::string& msg) = 0;

	virtual void Out(const std::string& level, const std::string& name, const std::string& msg) = 0;
};


enum LogLevel
{
	INFO,
	WARN,
	ERROR,
	DEBUG,
	TRACE,
};
void SetLogLevel(const LogLevel& logLevel);
LogLevel GetLogLevel();


using Ptr = std::shared_ptr<Logger>;

//create logger with custom log level, it cannot be modified at runtime
Ptr Create(const LogLevel& logLevel, const std::string& fileName);
Ptr Create(const LogLevel& logLevel);

//create logger with common log level, it can be modified threw SetLogLevel at runtime
Ptr Create(const std::string& fileName);
Ptr Create();

}; //namespace logger
