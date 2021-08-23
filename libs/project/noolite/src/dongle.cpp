#include "dongle.h"

namespace noolite
{

class DongleImpl : public Dongle
{
public:
	bool Start() override
	{
		return false;
	}

	void Stop() override
	{
	}

	void Reboot() override
	{
	}

	void Init() override
	{
	}

private:
};

Ptr CreateDongle()
{
	return std::make_unique<DongleImpl>();
}

};//namespace noolite

