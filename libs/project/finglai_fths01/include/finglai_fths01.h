#pragma once

#include "serial.h"

namespace serial
{
	namespace fths01
	{
		class Fths01
		{
		public:
			virtual ~Fths01() = default;

		};

		using Ptr = std::unique_ptr<Fths01>;
		Ptr Create(const Params& params);
	};
};
