#pragma once

#include "serial.h"

namespace serial
{
	namespace modbus
	{
		class Modbus
		{
		public:
			virtual ~Modbus() = default;

			virtual bool Open() = 0;
			virtual void Close() = 0;

			virtual bool ReadHoldingRegisters(uint8_t deviceAddress, uint16_t address, uint16_t total, uint16_t* dst) = 0; //0x03

			virtual bool WriteSingleRegister() = 0; //0x06
			virtual bool WriteMultipleRegisters() = 0; //0x10
		};

		using Ptr = std::unique_ptr<Modbus>;

		Ptr Create(const Params& params);
	};
};