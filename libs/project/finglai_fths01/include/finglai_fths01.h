#pragma once

namespace serial
{
	namespace fths01
	{
		struct Telemetry
		{
			float temperature = 0.f;
			float humidity = 0.f;
		};

		enum class BaudRate : uint8_t
		{
			_1200 = 3,
			_2400,
			_4800,
			_9600,
			_19200,
		};

#pragma pack(push)
#pragma pack(1)
		struct Settings
		{
			uint8_t address = 0;
			BaudRate baudRate = BaudRate::_9600;
		};
#pragma pack(pop)

		class Fths01
		{
		public:
			virtual ~Fths01() = default;

			virtual bool Open(const std::string& serialName, BaudRate baudRate = BaudRate::_9600) = 0;
			virtual void Close() = 0;

			virtual bool ReadTelemetry(uint8_t address, Telemetry& telemetry) = 0;
			virtual bool WriteSettings(uint8_t address, const Settings& settings) = 0;
		};

		using Ptr = std::unique_ptr<Fths01>;
		Ptr Create();
	};
};
