#include "serial.h"

namespace serial
{

class SerialImpl : public Serial
{
};

Ptr Create(const Params& params)
{
	return std::make_unique<SerialImpl>();
}

};//namespace serial