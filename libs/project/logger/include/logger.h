#pragma once

#include <string>
#include <sstream>
#include <memory>

namespace logger
{
	enum class LogLevel
	{
		Info,
		Warn,
		Error,
		Debug,
		Trace,
	};
};//namespace logger


#define logOUT(level, name, msg) {\
	if (m_log->Level() >= level) \
	{\
		std::stringstream ss; \
		ss << msg; \
		m_log->Out(level, name, ss.str()); \
	}\
}

#define logINFO(name, msg)  logOUT(logger::LogLevel::Info, name, msg)
#define logWARN(name, msg)  logOUT(logger::LogLevel::Warn, name, msg)
#define logERROR(name, msg) logOUT(logger::LogLevel::Error, name, msg)
#define logDEBUG(name, msg) logOUT(logger::LogLevel::Debug, name, msg)
#define logTRACE(name, msg) logOUT(logger::LogLevel::Trace, name, msg)


namespace logger
{

class Logger
{
public:
	virtual ~Logger() = default;

	virtual LogLevel Level() const = 0;

	virtual void Info(const std::string& name, const std::string& msg) = 0;
	virtual void Warn(const std::string& name, const std::string& msg) = 0;
	virtual void Error(const std::string& name, const std::string& msg) = 0;
	virtual void Debug(const std::string& name, const std::string& msg) = 0;
	virtual void Trace(const std::string& name, const std::string& msg) = 0;

	virtual void Out(const LogLevel level, const std::string& name, const std::string& msg) = 0;
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
