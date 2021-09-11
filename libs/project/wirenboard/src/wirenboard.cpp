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

		m_modbus = serial::modbus::Create(params);
		if (!m_modbus)
		{
			return false;
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

		if (!QueryNormedFloatValues(address, total_power_addr, 4, &wbMap3H.p_total.all,
				&std::vector<float>{ 3.125e-05f, 3.125e-05f, 3.125e-05f, 3.125e-05f }[0]))
		{
			return false;
		}

		return false;
	}

private:
	bool QueryNormedFloatValues(uint8_t deviceAddress, uint16_t address, uint16_t total, float* dst, const float* norms)
	{
		return false;
	}

private:
	const logger::Ptr m_log = logger::Create();
	serial::modbus::Ptr m_modbus;
};

serial::wirenboard::Ptr serial::wirenboard::Create()
{
	return std::make_unique<WirenboardImpl>();
}
