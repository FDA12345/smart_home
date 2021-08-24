#pragma once

#include <functional>

namespace serial
{
	enum class StopBits
	{
		_1_0,
		_1_5,
		_2_0,
	};

	enum class FlowControl
	{
		NONE,
		SOFTWARE,
		HARDWARE,
	};

	enum class Parity
	{
		NONE,
		ODD,
		EVEN,
	};


	class Serial
	{
	public:
		using OnWriteFn = std::function<void(bool error, size_t transferred)>;
		using OnReadFn = std::function<void(bool error, size_t transferred)>;

	public:
		virtual ~Serial() = default;

		virtual bool Open() = 0;
		virtual void Close() = 0;

		virtual size_t Write(const char* data, size_t offset, size_t count) = 0;
		virtual void WriteAsync(const char* data, size_t offset, size_t count, OnWriteFn onWriteFn) = 0;

		virtual size_t Read(char* data, size_t offset, size_t count) = 0;
		virtual void ReadAsync(char* data, size_t offset, size_t count, OnReadFn onReadFn) = 0;

		virtual size_t ReadUntil(char* data, size_t offset, size_t count, const char delim) = 0;

	public:
		size_t Write(const char* data, size_t count);
		void WriteAsync(const char* data, size_t count, OnWriteFn onWriteFn);

		size_t Read(char* data, size_t count);
		void ReadAsync(char* data, size_t count, OnReadFn onReadFn);
		size_t ReadUntil(char* data, size_t count, const char delim);
	};

	struct Params
	{
		std::string serialName;

		size_t baudRate = 9600;
		StopBits stopBits = StopBits::_1_0;
		Parity parity = Parity::NONE;
		FlowControl flowControl = FlowControl::NONE;
		size_t characterSize = 8;

#ifdef WIN32
		size_t readTimeoutMs = 100;
		size_t writeTimeoutMs = 100;
#endif
	};

	using Ptr = std::unique_ptr<Serial>;
	Ptr Create(const Params& params);
};//namespace serial
