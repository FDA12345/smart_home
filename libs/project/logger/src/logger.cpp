#include "logger.h"

#include <map>
#include <mutex>
#include <atomic>
#include <iostream>
#include <iomanip>

#ifdef WIN32
	#include <windows.h>
#endif

namespace logger
{

std::string g_defaultLogFileName = "system.log";

std::mutex g_mx;

using LogLevelAtomic = std::atomic<LogLevel>;
using LogLevelPtr = std::shared_ptr<LogLevelAtomic>;
LogLevelPtr g_defaultLogLevel = std::make_shared<LogLevelAtomic>(LogLevel::Error);

std::map<std::string, Ptr> g_loggers;



class LoggerImpl : public Logger
{
public:
	LoggerImpl(const LogLevelPtr& logLevel, bool console, const std::string& fileName)
		: m_logLevelPtr(logLevel)
		, m_logLevelAtomic(*m_logLevelPtr)
		, m_console(console)
		, m_fileName(fileName)
	{
	}

	~LoggerImpl()
	{
		if (m_file)
		{
			fclose(m_file);
			m_file = nullptr;
		}
	}

	LogLevel Level() const override
	{
		return m_logLevelAtomic;
	}

	void Info(const std::string& name, const std::string& msg) override
	{
		Out(LogLevel::Info, name, msg);
	}

	void Warn(const std::string& name, const std::string& msg) override
	{
		if (m_logLevelAtomic >= LogLevel::Warn)
		{
			Out(LogLevel::Warn, name, msg);
		}
	}

	void Error(const std::string& name, const std::string& msg) override
	{
		if (m_logLevelAtomic >= LogLevel::Error)
		{
			Out(LogLevel::Error, name, msg);
		}
	}

	void Debug(const std::string& name, const std::string& msg) override
	{
		if (m_logLevelAtomic >= LogLevel::Debug)
		{
			Out(LogLevel::Debug, name, msg);
		}
	}

	void Trace(const std::string& name, const std::string& msg) override
	{
		if (m_logLevelAtomic >= LogLevel::Trace)
		{
			Out(LogLevel::Trace, name, msg);
		}
	}

	void Out(const LogLevel level, const std::string& name, const std::string& msg) override
	{
		std::lock_guard lock(m_mx);

		if (!m_file)
		{
			m_file = fopen(m_fileName.c_str(), "a+");
		}

		if (!m_file)
		{
			return;
		}

		time_t t = time(nullptr);

		tm _tm = { 0 };
		localtime_s(&_tm, &t);

		char buffer[100];
		strftime(buffer, sizeof(buffer) / sizeof(buffer[0]) - 1, "%Y-%m-%d %H:%M:%S", &_tm);

		using clock = std::chrono::system_clock;
		const auto now = clock::now();
		const auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count() % 1'000'000ull;

		char micros_txt[10];
		sprintf(micros_txt, "%06llu", micros);

		uint64_t pid = 0;
#ifdef WIN32
		pid = GetCurrentProcessId();
#else
		pid = ::getpid();
#endif

		std::stringstream ss;
		ss << buffer << "." << micros_txt << " " << std::hex << std::setw(sizeof(uintptr_t) * 2) << std::setfill('0') << pid << " " <<
			std::setw(8) << std::setfill('0') << std::this_thread::get_id() << " " << LevelToStr(level) << " " << name << " - " << msg;

		const std::string& resultMsg = ss.str();

		fprintf(m_file, "%s\n", resultMsg.c_str());
		fflush(m_file);

		if (m_console)
		{
			std::cout << resultMsg << std::endl;
		}
	}

private:
	static std::string LevelToStr(const LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Info:  return "INFO";
		case LogLevel::Warn:  return "WARN";
		case LogLevel::Error: return "ERROR";
		case LogLevel::Debug: return "DEBUG";
		case LogLevel::Trace: return "TRACE";
		default: return "UNKNOWN(" + std::to_string(uint32_t(level)) + ")";
		}
	}

private:
	const LogLevelPtr m_logLevelPtr;
	const LogLevelAtomic& m_logLevelAtomic;

	const bool m_console;
	const std::string m_fileName;

	std::mutex m_mx;
	FILE* m_file = nullptr;
};



void SetLogLevel(const LogLevel& logLevel)
{
	(*g_defaultLogLevel) = logLevel;
}

LogLevel GetLogLevel()
{
	return *g_defaultLogLevel;
}



Ptr Create(const LogLevelPtr& logLevelPtr, const std::string& fileName)
{
	std::lock_guard lock(g_mx);

	auto it = g_loggers.find(fileName);
	if (it != g_loggers.end())
	{
		return it->second;
	}

	return g_loggers.emplace(fileName, std::make_shared<LoggerImpl>(logLevelPtr, true, fileName)).first->second;
}

Ptr Create(const LogLevelPtr& logLevelPtr)
{
	return Create(logLevelPtr, g_defaultLogFileName);
}



Ptr Create(const LogLevel& logLevel, const std::string& fileName)
{
	return Create(std::make_shared<LogLevelAtomic>(logLevel), fileName);
}

Ptr Create(const LogLevel& logLevel)
{
	return Create(std::make_shared<LogLevelAtomic>(logLevel));
}

Ptr Create(const std::string& fileName)
{
	return Create(g_defaultLogLevel, fileName);
}

Ptr Create()
{
	return Create(g_defaultLogLevel, g_defaultLogFileName);
}

};//namespace logger
