#include "wirenboard.h"
#include "modbus.h"
#include "logger.h"

using namespace serial::modbus;
using namespace serial::wirenboard;


class WirenboardImpl : public Wirenboard
{
public:
	bool Open(const std::string& serialName, size_t baudRate) override
	{
		serial::Params params;

		params.serialName = serialName;
		params.stopBits = serial::StopBits::_2_0;
		params.parity = serial::Parity::NONE;
		params.flowControl = serial::FlowControl::NONE;
		params.characterSize = 8;
		params.baudRate = baudRate;

		//params.readTimeoutMs = 250;
		//params.writeTimeoutMs = 250;

		m_modbus = serial::modbus::Create(params);
		if (!m_modbus)
		{
			return false;
		}

		return m_modbus->Open();
	}

	void Close() override
	{
		if (m_modbus)
		{
			m_modbus->Close();
			m_modbus.reset();
		}
	}

	bool Read(uint8_t address, WB_MAP3H& wbMap3H) override
	{
		if (!m_modbus)
		{
			return false;
		}

		//modbus register table of WB MAP 3H
		constexpr uint16_t total_power_addr = 0x1200;
		constexpr uint16_t l1_power_addr = 0x1204;
		constexpr uint16_t l2_power_addr = 0x1208;
		constexpr uint16_t l3_power_addr = 0x120c;

		constexpr uint16_t l1_voltage_angle = 0x10fd;
		constexpr uint16_t l2_voltage_angle = 0x10fe;
		constexpr uint16_t l3_voltage_angle = 0x10ff;

		constexpr uint16_t l1_phase_angle = 0x10f9;
		constexpr uint16_t l2_phase_angle = 0x10fa;
		constexpr uint16_t l3_phase_angle = 0x10fb;

		constexpr uint16_t moment_power = 0x1300;
		constexpr uint16_t l1_moment_power = 0x1302;
		constexpr uint16_t l2_moment_power = 0x1304;
		constexpr uint16_t l3_moment_power = 0x1306;

		constexpr uint16_t l1_u_addr = 0x10d9;
		constexpr uint16_t l2_u_addr = 0x10da;
		constexpr uint16_t l3_u_addr = 0x10db;

		if (!QueryNormedValues<uint64_t>(address, total_power_addr, 4, true, &wbMap3H.p_total.all,
				&std::vector<double>{ 3.125e-05, 3.125e-05, 3.125e-05, 3.125e-05 }[0]))
		{
			return false;
		}

		if (!QueryNormedValues<int16_t>(address, l1_voltage_angle, 3, false, &wbMap3H.angle.l1,
			&std::vector<float>{ 0.1f, 0.1f, 0.1f }[0]))
		{
			return false;
		}

		if (!QueryNormedValues<int16_t>(address, l1_phase_angle, 3, false, &wbMap3H.phase.l1,
			&std::vector<float>{ 0.1f, 0.1f, 0.1f }[0]))
		{
			return false;
		}

		if (!QueryNormedValues<int32_t>(address, moment_power, 4, false, &wbMap3H.p_moment.all,
			&std::vector<double>{ 6.10352e-05, 1.52588e-05, 1.52588e-05, 1.52588e-05 }[0]))
		{
			return false;
		}

		if (!QueryNormedValues<uint16_t>(address, l1_u_addr, 3, false, &wbMap3H.u.l1,
			&std::vector<float>{ 0.01f, 0.01f, 0.01f }[0]))
		{
			return false;
		}

		return true;
	}

private:
	template <typename FromType, typename ResultType, typename ScaleType>
	bool QueryNormedValues(uint8_t deviceAddress, uint16_t address, size_t total, bool littleEndian, ResultType* data, ScaleType *scales)
	{
		std::vector<FromType> dataFrom(total);
		if (!QueryModbusValue(deviceAddress, address, total, littleEndian, &dataFrom[0]))
		{
			return false;
		}

		for (size_t i = 0; i < total; ++i)
		{
			data[i] = static_cast<ResultType>(dataFrom[i]) * scales[i];
		}

		return true;
	}

	template <typename ResultType>
	bool QueryModbusValue(uint8_t deviceAddress, uint16_t address, size_t total, bool littleEndian, ResultType* result)
	{
		constexpr size_t bytesStep = sizeof(ResultType);
		constexpr size_t wordsStep = bytesStep / sizeof(uint16_t);

		const size_t bytesTotal = bytesStep * total;
		const size_t wordsTotal = bytesTotal / sizeof(uint16_t);

		std::vector<uint8_t> buffer(bytesTotal);
		size_t dataIndex = 0;

		if (!m_modbus->ReadHoldingRegisters(deviceAddress, address, wordsTotal, reinterpret_cast<uint16_t*>(&buffer[0])))
		{
			return false;
		}

		for (size_t i = 0; i < total; ++i)
		{
			switch (wordsStep)
			{
			case 1:
				result[i] = DecodeUInt16(&buffer[dataIndex], littleEndian);
				break;
			case 2:
				result[i] = DecodeUInt32(&buffer[dataIndex], littleEndian);
				break;
			case 4:
				result[i] = DecodeUInt64(&buffer[dataIndex], littleEndian);
				break;
			default:
				return false;
			}

			dataIndex += bytesStep;
		}

		return true;
	}

	static uint64_t DecodeUInt64(uint8_t buffer[8], bool littleEndian)
	{
		uint64_t ull = 0;

		if (littleEndian)
		{
			ull =
				((uint64_t)buffer[1] & 0xFF) |
				(((uint64_t)buffer[0] & 0xFF) << 8) |
				(((uint64_t)buffer[3] & 0xFF) << 16) |
				(((uint64_t)buffer[2] & 0xFF) << 24) |
				(((uint64_t)buffer[5] & 0xFF) << 32) |
				(((uint64_t)buffer[4] & 0xFF) << 40) |
				(((uint64_t)buffer[7] & 0xFF) << 48) |
				(((uint64_t)buffer[6] & 0xFF) << 56);
		}
		else
		{
			ull =
				((uint64_t)buffer[7] & 0xFF) |
				(((uint64_t)buffer[6] & 0xFF) << 8) |
				(((uint64_t)buffer[5] & 0xFF) << 16) |
				(((uint64_t)buffer[4] & 0xFF) << 24) |
				(((uint64_t)buffer[3] & 0xFF) << 32) |
				(((uint64_t)buffer[2] & 0xFF) << 40) |
				(((uint64_t)buffer[1] & 0xFF) << 48) |
				(((uint64_t)buffer[0] & 0xFF) << 56);
		};

		return ull;
	}

	static uint32_t DecodeUInt32(uint8_t buffer[4], bool littleEndian)
	{
		uint32_t ul = 0;

		if (littleEndian)
		{
			ul = (uint32_t)
				(((uint64_t)buffer[1] & 0xFF) |
				(((uint64_t)buffer[0] & 0xFF) << 8) |
				(((uint64_t)buffer[3] & 0xFF) << 16) |
				(((uint64_t)buffer[2] & 0xFF) << 24));
		}
		else
		{
			ul = (uint32_t)
				(((uint64_t)buffer[3] & 0xFF) |
				(((uint64_t)buffer[2] & 0xFF) << 8) |
				(((uint64_t)buffer[1] & 0xFF) << 16) |
				(((uint64_t)buffer[0] & 0xFF) << 24));
		}

		return ul;
	}

	static uint16_t DecodeUInt16(uint8_t buffer[2], bool littleEndian)
	{
		uint16_t s = 0;

		if (littleEndian)
		{
			s = (uint16_t)
				(((uint64_t)buffer[1] & 0xFF) |
				(((uint64_t)buffer[0] & 0xFF) << 8));
		}
		else
		{
			s = (uint16_t)
				(((uint64_t)buffer[1] & 0xFF) |
				(((uint64_t)buffer[0] & 0xFF) << 8));
		}

		return s;
	}


private:
	const logger::Ptr m_log = logger::Create();
	serial::modbus::Ptr m_modbus;
};


serial::wirenboard::Ptr serial::wirenboard::Create()
{
	return std::make_unique<WirenboardImpl>();
}
