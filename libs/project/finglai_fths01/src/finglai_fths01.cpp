#include "finglai_fths01.h"
#include "modbus.h"
#include "logger.h"

using namespace serial::fths01;

class Fths01Impl : public Fths01
{
public:
	bool Open(const std::string& serialName, BaudRate baudRate) override
	{
		serial::Params params;

		params.serialName = serialName;
		params.stopBits = serial::StopBits::_1_0;
		params.parity = serial::Parity::NONE;
		params.flowControl = serial::FlowControl::NONE;
		params.characterSize = 8;

		switch (baudRate)
		{
		case BaudRate::_1200:  params.baudRate = 1200; break;
		case BaudRate::_2400:  params.baudRate = 2400; break;
		case BaudRate::_4800:  params.baudRate = 4800; break;
		case BaudRate::_9600:  params.baudRate = 9600; break;
		case BaudRate::_19200: params.baudRate = 19200; break;
		default:
			return false;
		}

		m_modbus = serial::modbus::Create(params);

		if (m_modbus)
		{
			return m_modbus->Open();
		}

		return false;
	}

	void Close() override
	{
		if (m_modbus)
		{
			m_modbus->Close();
			m_modbus.reset();
		}
	}

	bool ReadTelemetry(uint8_t address, Telemetry& telemetry) override
	{
		if (!m_modbus)
		{
			return false;
		}

#pragma pack(push)
#pragma pack(1)
		union
		{
			struct
			{
				int16_t temp;
				int16_t humi;
			};

			uint16_t words[2];
		} data {0};
#pragma pack(pop)

		if (m_modbus->ReadHoldingRegisters(address, 0, 2, data.words))
		{
			telemetry.temperature = DecodeUInt16(data.temp, true) / 10.f;
			telemetry.humidity = DecodeUInt16(data.humi, true) / 10.f;

			return true;
		}

		return false;
	}

	bool WriteSettings(uint8_t address, const Settings& settings) override
	{
		if (!m_modbus)
		{
			return false;
		}

		uint16_t value = *reinterpret_cast<const uint16_t*>(&settings);

		logWARN(__FUNCTION__, "в описании FTHS01 в комангде 06 modbus другая структура. могут быть проблемы");
		return m_modbus->WriteSingleRegister(address, 0, value);
	}

private:
	static uint16_t DecodeUInt16(uint16_t data, bool littleEndian)
	{
		uint16_t s = 0;

		uint8_t* buffer = reinterpret_cast<uint8_t*>(&data);

		if (littleEndian)
		{
			s = (uint16_t)
				(((uint64_t)buffer[1] & 0xFF) |
				(((uint64_t)buffer[0] & 0xFF) << 8));
		}
		else
		{
			s = (uint16_t)
				(((uint64_t)buffer[0] & 0xFF) |
				(((uint64_t)buffer[1] & 0xFF) << 8));
		}

		return s;
	}
private:
	const logger::Ptr m_log = logger::Create();
	serial::modbus::Ptr m_modbus;
};

serial::fths01::Ptr serial::fths01::Create()
{
	return std::make_unique<Fths01Impl>();
}
