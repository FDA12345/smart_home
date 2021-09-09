#include "modbus.h"

using namespace serial;

class ModbusImpl : public modbus::Modbus
{
public:
	ModbusImpl(const Params& params)
		: m_serial(serial::Create(params))
	{
	}

	bool ReadHoldingRegisters() override
	{
		return false;
	}

	bool WriteSingleRegister() override
	{
		return false;
	}

	bool WriteMultipleRegisters() override
	{
		return false;
	}

private:
	serial::Ptr m_serial;
};

modbus::Ptr modbus::Create(const Params& params)
{
	return std::make_unique<ModbusImpl>(params);
}