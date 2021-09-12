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

		if (!QueryNormedValues<uint64_t>(address, total_power_addr, 4, true, &wbMap3H.p_total.all,
				&std::vector<double>{ 3.125e-05, 3.125e-05, 3.125e-05, 3.125e-05 }[0]))
		{
			return false;
		}

		if (!QueryNormedValues<int16_t>(address, l1_voltage_angle, 3, true, &wbMap3H.angle.l1,
			&std::vector<float>{ 0.1f, 0.1f, 0.1f }[0]))
		{
			return false;
		}

		if (!QueryNormedValues<int16_t>(address, l1_phase_angle, 3, true, &wbMap3H.phase.l1,
			&std::vector<float>{ 0.1f, 0.1f, 0.1f }[0]))
		{
			return false;
		}

		if (!QueryNormedValues<int32_t>(address, moment_power, 4, true, &wbMap3H.p_moment.all,
			&std::vector<double>{ 6.10352e-05, 1.52588e-05, 1.52588e-05, 1.52588e-05 }[0]))
		{
			return false;
		}

		if (!QueryNormedValues<uint16_t>(address, l1_u_addr, 3, true, &wbMap3H.u.l1,
			&std::vector<float>{ 0.01f, 0.01f, 0.01f }[0]))
		{
			return false;
		}

		return false;
	}

private:
	template <typename RegType, typename ValueType, typename ScaleType>
	bool QueryNormedValues(uint8_t deviceAddress, uint16_t address, uint16_t total, bool littleEndian, ValueType* dst, const ScaleType* norms)
	{
		constexpr size_t typeSize = sizeof(RegType);
		const uint16_t regsTotal = typeSize * total / 2;

		std::vector<RegType> data(total);
		if (!m_modbus->ReadHoldingRegisters(deviceAddress, address, regsTotal, reinterpret_cast<uint16_t*>(&data[0])))
		{
			return false;
		}

		for (size_t i = 0; i < total; ++i)
		{
			if (!littleEndian)
			{
				uint8_t* b1 = reinterpret_cast<uint8_t*>(&data[i]);
				uint8_t* b2 = b1 + typeSize - 1;

				for (size_t j = 0; j < typeSize / 2; ++j)
				{
					std::swap(*(b1++), *(b2--));
				}
			}

			dst[i] = ValueType(data[i]) * norms[i];
		}

		return true;
	}

private:
	const logger::Ptr m_log = logger::Create();
	serial::modbus::Ptr m_modbus;
};

serial::wirenboard::Ptr serial::wirenboard::Create()
{
	return std::make_unique<WirenboardImpl>();
}
