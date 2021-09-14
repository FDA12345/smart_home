#pragma once

namespace messages
{

	class SensorsMsg
	{
	public:
		using Ptr = std::unique_ptr<SensorsMsg>;
		static Ptr Create();

	public:
		virtual ~SensorsMsg() = default;

		virtual void AddSensor(const std::string& className, const std::string& address, float value) = 0;
		virtual void AddSensor(const std::string& className, const std::string& address, int value) = 0;
		virtual void AddSensor(const std::string& className, const std::string& address, const std::string& value) = 0;

		virtual std::string MakeJson() const = 0;
	};

};
