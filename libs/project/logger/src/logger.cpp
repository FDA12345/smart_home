#include "logger.h"

#include <sstream>
#include <map>
#include <mutex>

namespace logger
{

std::mutex g_mx;
std::map<std::string, Ptr> g_loggers;



void Logger::Info(const std::string& name, const std::string& msg)
{
	Out("INFO", name, msg);
}

void Logger::Warn(const std::string& name, const std::string& msg)
{
	Out("WARN", name, msg);
}

void Logger::Debug(const std::string& name, const std::string& msg)
{
	Out("DEBUG", name, msg);
}

void Logger::Error(const std::string& name, const std::string& msg)
{
	Out("ERROR", name, msg);
}

void Logger::Trace(const std::string& name, const std::string& msg)
{
	Out("TRACE", name, msg);
}



class LoggerImpl : public Logger
{
public:
	LoggerImpl(bool console, const std::string& fileName)
		: m_console(console)
		, m_fileName(fileName)
	{
	}

private:
	void Out(const std::string& level, const std::string& name, const std::string& msg) override
	{
		FILE *f = fopen(m_fileName.c_str(), "a+");
		if (!f)
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

		std::stringstream ss;
		ss << "[" << buffer << "." << micros_txt << "] - " << level << " - " << name << " - " << msg << std::endl;

		fprintf(f, "%s\n", ss.str().c_str());
		fclose(f);
	}

private:
	const bool m_console;
	const std::string m_fileName;
};



Ptr Create(const std::string& fileName)
{
	std::lock_guard lock(g_mx);

	auto it = g_loggers.find(fileName);
	if (it != g_loggers.end())
	{
		return it->second;
	}

	return g_loggers.emplace(fileName, std::make_shared<LoggerImpl>(true, fileName)).first->second;
}

Ptr Create()
{
	return Create("system.log");
}

};//namespace logger
