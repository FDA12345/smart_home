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

			virtual bool ReadHoldingRegisters() = 0; //0x03

			virtual bool WriteSingleRegister() = 0; //0x06
			virtual bool WriteMultipleRegisters() = 0; //0x10
		};

		using Ptr = std::unique_ptr<Modbus>;

		Ptr Create(const Params& params);
	};
};