#pragma once

namespace serial
{
	class Serial
	{
	public:
		virtual ~Serial() = default;

	};

	struct Params
	{

	};

	using Ptr = std::unique_ptr<Serial>;
	Ptr Create(const Params& params);
};//namespace serial
