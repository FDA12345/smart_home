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

			virtual bool Function_06() = 0;
			virtual bool Function_03() = 0;
		};

		using Ptr = std::unique_ptr<Modbus>;

		Ptr Create(const Params& params);
	};
};