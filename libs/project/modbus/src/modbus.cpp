#include "modbus.h"

using namespace serial;

class ModbusImpl : public modbus::Modbus
{
public:
	ModbusImpl(const Params& params)
		: m_serial(serial::Create(params))
	{
	}

	bool Open() override
	{
		return m_serial->Open();
	}

	void Close() override
	{
		m_serial->Close();
	}

	bool ReadHoldingRegisters(uint8_t deviceAddress, uint16_t address, uint16_t total, uint16_t* dst) override
	{

//https://modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf
#pragma pack(push)
#pragma pack(1)
		struct Request
		{
			uint8_t deviceAddress = 0;
			uint8_t function = 0;
			uint16_t address = 0;
			uint16_t total = 0;
			uint16_t crc;
		};

		struct ResponseHdr
		{
			uint8_t deviceAddress = 0;
			uint8_t function = 0;
			uint16_t bytesTotal = 0;
		};
#pragma pack(push)

		constexpr uint16_t SERIAL_ADU_SIZE = 256;
		constexpr uint16_t MAX_PDU_SIZE = SERIAL_ADU_SIZE - 1 - 1 - 1;
		constexpr uint16_t MAX_REGS_TOTAL = MAX_PDU_SIZE >> 1;

		if (total > MAX_REGS_TOTAL)
		{
			return false;
		}

		Request req = { 0 };
		req.deviceAddress = deviceAddress;
		req.function = 0x03;
		req.address = address;
		req.total = total;

		return m_serial->Write(reinterpret_cast<const char*>(&req), sizeof(req)) == sizeof(req);
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