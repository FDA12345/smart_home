#include "stdafx.h"
#include "serial.h"
#include "logger.h"

#include <iomanip>
#include <boost/asio.hpp>
#include <thread>

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

size_t Serial::ReadUntil(char* data, size_t count, const char delim)
{
	return ReadUntil(data, 0, count, delim);
}




class SerialImpl : public Serial
{
public:
	SerialImpl(const Params& params)
		: m_params(params)
		, m_work(std::make_unique<boost::asio::io_service::work>(m_io))
		, m_port(m_io)
	{
		m_threadIo = std::thread
		{ [this]()
		{
			m_io.run();
		} };
	}

	~SerialImpl()
	{
		m_work.reset();
		m_threadIo.join();
	}

	bool Open() override
	{
		logINFO(__FUNCTION__, "open " << m_params.serialName << " port");
		Close();

		while (true)
		{
			boost::system::error_code ec;

			const std::string& normalSerialName = NormalizeSerialName(m_params.serialName);
			if (normalSerialName != m_params.serialName)
			{
				logWARN(__FUNCTION__, "port name modified from '" << m_params.serialName  << "' to '" << normalSerialName << "'");
			}

			m_port.open(normalSerialName, ec);
			if (ec)
			{
				logERROR(__FUNCTION__, "port " << m_params.serialName << " open error " << ec.message());
				break;
			}

			if (!Setup())
			{
				logERROR(__FUNCTION__, "port " << m_params.serialName << " setup failed");
				break;
			}

			logINFO(__FUNCTION__, "port " << m_params.serialName << " opened");
			return true;
		}

		Close();
		return false;
	}

	void Close()
	{
		if (m_port.is_open())
		{
			logINFO(__FUNCTION__, "close port");
			boost::system::error_code ec;
			m_port.close(ec);
			logINFO(__FUNCTION__, "port closed");
		}
	}

	size_t Write(const char* data, size_t offset, size_t count) override
	{
		logTRACE("WRITE", BytesToHex(data, offset, count));

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

	size_t ReadUntil(char* data, size_t offset, size_t count, const char delim)
	{
		for (size_t i = 0; i < count; ++i)
		{
			size_t n = Read(data, offset + i, 1);
			if (n == 0)
			{
				return 0;
			}

			if (data[offset + i] == delim)
			{
				logTRACE("READ", BytesToHex(data, offset, i + 1));
				return i + 1;
			}
		}

		logTRACE("READ", BytesToHex(data, offset, count));
		return count;
	}

private:
	static std::string BytesToHex(const char* data, size_t offset, size_t count)
	{
		std::stringstream ss;

		for (size_t i = 0; i < count; ++i)
		{
			ss << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (0xFF & *(data + offset + i)) << " ";
		}

		return ss.str();
	}

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
		case FlowControl::NONE: m_port.set_option(base::flow_control(base::flow_control::none), ec); break;
		case FlowControl::SOFTWARE: m_port.set_option(base::flow_control(base::flow_control::software), ec); break;
		case FlowControl::HARDWARE: m_port.set_option(base::flow_control(base::flow_control::hardware), ec); break;
		default:
			return false;
		}
		if (ec)
		{
			return false;
		}

		switch (m_params.parity)
		{
		case Parity::NONE: m_port.set_option(base::parity(base::parity::none), ec); break;
		case Parity::ODD: m_port.set_option(base::parity(base::parity::odd), ec); break;
		case Parity::EVEN: m_port.set_option(base::parity(base::parity::even), ec); break;
		default:
			return false;
		}

		switch (m_params.stopBits)
		{
		case StopBits::_1_0: m_port.set_option(base::stop_bits(base::stop_bits::one), ec); break;
		case StopBits::_1_5: m_port.set_option(base::stop_bits(base::stop_bits::onepointfive), ec); break;
		case StopBits::_2_0: m_port.set_option(base::stop_bits(base::stop_bits::two), ec); break;
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
	const logger::Ptr m_log = logger::Create();

	boost::asio::io_service m_io;
	std::thread m_threadIo;
	std::unique_ptr<boost::asio::io_service::work> m_work;
	boost::asio::serial_port m_port;
};

Ptr Create(const Params& params)
{
	return std::make_unique<SerialImpl>(params);
}

};//namespace serial