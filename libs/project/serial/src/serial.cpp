#include "serial.h"

#include <boost/asio.hpp>

namespace serial
{

class SerialImpl : public Serial
{
public:
	SerialImpl()
		: m_port(m_io)
	{
	}
private:
	boost::asio::io_service m_io;
	boost::asio::serial_port m_port;
};

Ptr Create(const Params& params)
{
	return std::make_unique<SerialImpl>();
}

};//namespace serial