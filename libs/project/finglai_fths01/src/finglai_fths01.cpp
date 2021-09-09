#include "finglai_fths01.h"
#include "modbus.h"

serial::fths01::Ptr serial::fths01::Create(const Params& params)
{
	auto modbus = serial::modbus::Create(params);
	return nullptr;
}
