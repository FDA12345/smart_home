#include "stdafx.h"
#include "dongle.h"

namespace noolite
{

class DongleImpl : public Dongle
{
public:
	bool Start(const serial::Params& serialParams) override
	{
		Stop();

		while (true)
		{
			m_serial = serial::Create(serialParams);
			if (!m_serial)
			{
				break;
			}

			if (!m_serial->Open())
			{
				break;
			}


			return true;
		}

		Stop();
		return false;
	}

	void Stop() override
	{
		if (m_serial)
		{
			m_serial->Close();
			m_serial.reset();
		}
	}

	bool Reboot() override
	{
		return false;
	}

	bool Init() override
	{
		return false;
	}

	bool SwitchOn() override
	{
		return false;
	}

	bool SwitchOff() override
	{
		return false;
	}

private:

private:
	serial::Ptr m_serial;
};

Ptr CreateDongle()
{
	return std::make_unique<DongleImpl>();
}

};//namespace noolite

