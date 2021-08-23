#include "stdafx.h"
#include "serial.h"

#include <boost/asio.hpp>

namespace serial
{

size_t Serial::Write(const char* data, size_t count)
{
	return Write(data, 0, count);
}

size_t Serial::Read(char* data, size_t count)
{
	return Read(data, 0, count);
}

void Serial::WriteAsync(const char* data, size_t count, OnWriteFn onWriteFn)
{
	WriteAsync(data, 0, count, onWriteFn);
}

void Serial::ReadAsync(char* data, size_t count, OnReadFn onReadFn)
{
	ReadAsync(data, 0, count, onReadFn);
}




class SerialImpl : public Serial
{
public:
	SerialImpl(const Params& params)
		: m_params(params)
		, m_port(m_io)
	{
	}

	bool Open() override
	{
		Close();

		while (true)
		{
			boost::system::error_code ec;

			m_port.open(NormalizeSerialName(m_params.serialName), ec);
			if (ec)
			{
				break;
			}

			if (!Setup())
			{
				break;
			}

			return true;
		}

		Close();
		return false;
	}

	void Close()
	{
		if (m_port.is_open())
		{
			boost::system::error_code ec;
			m_port.close(ec);
		}
	}

	size_t Write(const char* data, size_t offset, size_t count) override
	{
		boost::system::error_code ec;
		return boost::asio::write(m_port, boost::asio::const_buffer(data + offset, count), ec);
	}

	size_t Read(char* data, size_t offset, size_t count) override
	{
		boost::system::error_code ec;
		return boost::asio::read(m_port, boost::asio::mutable_buffer(data + offset, count), ec);
	}

	void WriteAsync(const char* data, size_t offset, size_t count, OnWriteFn onWriteFn) override
	{
		boost::asio::async_write(m_port, boost::asio::const_buffer(data + offset, count),
		[onWriteFn](const boost::system::error_code& ec, std::size_t transferred)
		{
			onWriteFn(ec ? true: false, transferred);
		});
	}

	void ReadAsync(char* data, size_t offset, size_t count, OnReadFn onReadFn) override
	{
		boost::asio::async_read(m_port, boost::asio::mutable_buffer(data + offset, count),
			[onReadFn](const boost::system::error_code& ec, std::size_t transferred)
		{
			onReadFn(ec ? true : false, transferred);
		});
	}

private:
	static std::string NormalizeSerialName(const std::string& serialName)
	{
#ifdef WIN32
		return Win32SerialName(serialName);
#else
		return serialName;
#endif
	}

#ifdef WIN32
	static std::string Win32SerialName(const std::string serialName)
	{
		static std::string baseName = "COM";

		if (serialName.substr(0, baseName.length()) != baseName)
		{
			return serialName;
		}

		const std::string numStr = serialName.substr(baseName.length());
		size_t number = std::stoul(numStr);

		if (std::to_string(number) != numStr)
		{
			return serialName;
		}

		if (number < 10)
		{
			return serialName;
		}

		return "\\\\.\\" + serialName;
	}
#endif

	bool Setup()
	{
		using base = boost::asio::serial_port_base;
		boost::system::error_code ec;

		m_port.set_option(base::baud_rate(m_params.baudRate), ec);
		if (ec)
		{
			return false;
		}

		m_port.set_option(base::character_size(m_params.characterSize), ec);
		if (ec)
		{
			return false;
		}

		switch (m_params.flowControl)
		{
		case FLOW_CONTROL_NONE: m_port.set_option(base::flow_control(base::flow_control::none), ec); break;
		case FLOW_CONTROL_SOFTWARE: m_port.set_option(base::flow_control(base::flow_control::software), ec); break;
		case FLOW_CONTROL_HARDWARE: m_port.set_option(base::flow_control(base::flow_control::hardware), ec); break;
		default:
			return false;
		}
		if (ec)
		{
			return false;
		}

		switch (m_params.parity)
		{
		case PARITY_NONE: m_port.set_option(base::parity(base::parity::none), ec); break;
		case PARITY_ODD: m_port.set_option(base::parity(base::parity::odd), ec); break;
		case PARITY_EVEN: m_port.set_option(base::parity(base::parity::even), ec); break;
		default:
			return false;
		}

		switch (m_params.stopBits)
		{
		case STOPBITS_1_0: m_port.set_option(base::stop_bits(base::stop_bits::one), ec); break;
		case STOPBITS_1_5: m_port.set_option(base::stop_bits(base::stop_bits::onepointfive), ec); break;
		case STOPBITS_2_0: m_port.set_option(base::stop_bits(base::stop_bits::two), ec); break;
		default:
			return false;
		}

#ifdef WIN32
		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadTotalTimeoutConstant = m_params.readTimeoutMs;
		timeouts.WriteTotalTimeoutConstant = m_params.writeTimeoutMs;

		if (!SetCommTimeouts(m_port.native_handle(), &timeouts))
		{
			return false;
		}
#endif
		return true;
	}

private:
	const Params m_params;

	boost::asio::io_service m_io;
	boost::asio::serial_port m_port;
};

Ptr Create(const Params& params)
{
	return std::make_unique<SerialImpl>(params);
}

};//namespace serial