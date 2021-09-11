#pragma once

namespace serial
{
	namespace wirenboard
	{
		struct WB_MAP3H
		{
			struct
			{
				float all = 0.f;
				float l1 = 0.f;
				float l2 = 0.f;
				float l3 = 0.f;
			} p_total;

			struct
			{
				float all = 0.f;
				float l1 = 0.f;
				float l2 = 0.f;
				float l3 = 0.f;
			} p_moment;

			struct
			{
				float l1 = 0.f;
				float l2 = 0.f;
				float l3 = 0.f;
			} angle;

			struct
			{
				float l1 = 0.f;
				float l2 = 0.f;
				float l3 = 0.f;
			} phase;

			struct
			{
				float l1 = 0.f;
				float l2 = 0.f;
				float l3 = 0.f;
			} u;
		};

		class Wirenboard
		{
		public:
			virtual ~Wirenboard() = default;

			virtual bool Open(const std::string& serialName, size_t baudRate = 9600) = 0;
			virtual void Close() = 0;

			virtual bool Read(uint8_t address, WB_MAP3H& wbMap3H) = 0;
		};

		using Ptr = std::unique_ptr<Wirenboard>;
		Ptr Create();
	};
};