#pragma once

#include <functional>

namespace serial
{
	enum StopBits
	{
		STOPBITS_1_0,
		STOPBITS_1_5,
		STOPBITS_2_0,
	};

	enum FlowControl
	{
		FLOW_CONTROL_NONE,
		FLOW_CONTROL_SOFTWARE,
		FLOW_CONTROL_HARDWARE,
	};

	enum Parity
	{
		PARITY_NONE,
		PARITY_ODD,
		PARITY_EVEN,
	};


	class Serial
	{
	public:
		virtual ~Serial() = default;

		virtual bool Open() = 0;
		virtual void Close() = 0;

		virtual size_t Write(const char* data, size_t offset, size_t count) = 0;
		size_t Write(const char* data, size_t count);

		using OnWriteFn = std::function<void(bool error, size_t transferred)>;
		virtual void WriteAsync(const char* data, size_t offset, size_t count, OnWriteFn onWriteFn) = 0;
		void WriteAsync(const char* data, size_t count, OnWriteFn onWriteFn);

		virtual size_t Read(char* data, size_t offset, size_t count) = 0;
		size_t Read(char* data, size_t count);

		using OnReadFn = std::function<void(bool error, size_t transferred)>;
		virtual void ReadAsync(char* data, size_t offset, size_t count, OnReadFn onReadFn) = 0;
		void ReadAsync(char* data, size_t count, OnReadFn onReadFn);
	};

	struct Params
	{
		std::string serialName;

		size_t baudRate = 9600;
		StopBits stopBits = STOPBITS_1_0;
		Parity parity = PARITY_NONE;
		FlowControl flowControl = FLOW_CONTROL_NONE;
		size_t characterSize = 8;

#ifdef WIN32
		size_t readTimeoutMs = 500;
		size_t writeTimeoutMs = 100;
#endif
	};

	using Ptr = std::unique_ptr<Serial>;
	Ptr Create(const Params& params);
};//namespace serial
