#pragma once

#include "serial.h"

namespace serial
{
	namespace modbus
	{
		template <typename T>
		T RevertBytesOrder(const T value)
		{
			constexpr size_t typeSize = sizeof(value);

			T result = value;

			uint8_t* b1 = reinterpret_cast<uint8_t*>(&result);
			uint8_t* b2 = b1 + typeSize - 1;

			for (size_t j = 0; j < typeSize / 2; ++j)
			{
				std::swap(*b1++, *b2--);
			}

			return result;
		}

		class Modbus
		{
		public:
			static const uint8_t broadcastAddress = 0xFF;

		public:
			virtual ~Modbus() = default;

			virtual bool Open() = 0;
			virtual void Close() = 0;

			virtual bool ReadHoldingRegisters(uint8_t deviceAddress, uint16_t address, uint16_t total, uint16_t* dst) = 0; //0x03
			virtual bool WriteSingleRegister(uint8_t deviceAddress, uint16_t address, uint16_t value) = 0; //0x06
			//virtual bool WriteMultipleRegisters() = 0; //0x10
		};

		using Ptr = std::unique_ptr<Modbus>;

		Ptr Create(const Params& params);
	};
};