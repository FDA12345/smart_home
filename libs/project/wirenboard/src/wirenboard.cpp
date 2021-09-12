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

		queryNormedValuesFromUInt64(
			queryModbusFunc3Int64Values(dev_addr, total_power_addr, true, 4),
			new MutableDouble[]{
					data.p_total,
					data.l1_p,
					data.l2_p,
					data.l3_p,
			},
			new double[] {
			3.125e-05,
				3.125e-05,
				3.125e-05,
				3.125e-05,
		}
		);

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

		return false;
	}

private: 
	template <typename ValueType, typename ScaleType>
	static void NormedValuesFromUInt64(uint64_t* dataFrom, size_t dataTotal, ValueType* dataTo, ScaleType *scales)
	{
		for (size_t i = 0; i < dataTotal; ++i) {
			data_to[i].setValue(queryNormedValueFromUInt64
			(data_from[i], scales[i]));
		}
	}

	static uint64_t DecodeUInt64(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3,
			uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7,	bool little_endian)
	{
		uint64_t l = 0;

		if (little_endian) {
			l =
				((uint64_t)b1 & 0xFF) |
				(((uint64_t)b0 & 0xFF) << 8) |
				(((uint64_t)b3 & 0xFF) << 16) |
				(((uint64_t)b2 & 0xFF) << 24) |
				(((uint64_t)b5 & 0xFF) << 32) |
				(((uint64_t)b4 & 0xFF) << 40) |
				(((uint64_t)b7 & 0xFF) << 48) |
				(((uint64_t)b6 & 0xFF) << 56);
		}
		else {
			l =
				((uint64_t)b7 & 0xFF) |
				(((uint64_t)b6 & 0xFF) << 8) |
				(((uint64_t)b5 & 0xFF) << 16) |
				(((uint64_t)b4 & 0xFF) << 24) |
				(((uint64_t)b3 & 0xFF) << 32) |
				(((uint64_t)b2 & 0xFF) << 40) |
				(((uint64_t)b1 & 0xFF) << 48) |
				(((uint64_t)b0 & 0xFF) << 56);
		};

		return l;
	}

	static uint32_t DecodeInt32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, bool little_endian)
	{
		uint32_t l = 0;

		if (little_endian) {
			l = (uint32_t)
				(((uint64_t)b1 & 0xFF) |
				(((uint64_t)b0 & 0xFF) << 8) |
				(((uint64_t)b3 & 0xFF) << 16) |
				(((uint64_t)b2 & 0xFF) << 24));
		}
		else {
			l = (uint32_t)
				(((uint64_t)b3 & 0xFF) |
				(((uint64_t)b2 & 0xFF) << 8) |
				(((uint64_t)b1 & 0xFF) << 16) |
				(((uint64_t)b0 & 0xFF) << 24));
		}
		return l;
	}

	static uint16_t DecodeShort(uint8_t b0, uint8_t b1, bool little_endian)
	{
		uint16_t s = 0;

		if (little_endian) {
			s = (uint16_t)
				(((uint64_t)b1 & 0xFF) |
				(((uint64_t)b0 & 0xFF) << 8));
		}
		else {
			s = (uint16_t)
				(((uint64_t)b1 & 0xFF) |
				(((uint64_t)b0 & 0xFF) << 8));
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
