#pragma once

namespace serial
{
	namespace wirenboard
	{
		class Wirenboard
		{
		public:
			virtual ~Wirenboard() = default;

			virtual bool Open(const std::string& serialName, size_t baudRate = 9600) = 0;
			virtual void Close() = 0;
		};

		using Ptr = std::unique_ptr<Wirenboard>;
		Ptr Create();
	};
};