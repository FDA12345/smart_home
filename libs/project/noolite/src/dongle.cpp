#include "stdafx.h"
#include "dongle.h"

#include "noolite/header.h"
#include "noolite/footer.h"
#include "noolite/control.h"
#include "noolite/answer.h"
#include "noolite/cmd.h"
#include "noolite/packet.h"
#pragma pack(push)
#pragma pack(1)
struct DonglePacket
{
	Header header = Header::ST_FROM_ADAPTER;
	DongleMode mode = DongleMode::TX;
	Control ctr = Control::SEND;
	Answer answer = Answer::OK;
	uint8_t reserv = 0;
	uint8_t togl = 0;
	uint8_t channel = 0;
	Cmd cmd = Cmd::OFF;
	uint8_t fmt = 0;
	uint8_t d[4] = { 0 };
	uint32_t id = 0;
	uint8_t crc = 0;
	Footer footer = Footer::SP_TO_ADAPTER;
};
#pragma pack(pop)


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

	bool ForceInit() override
	{
		DonglePacket p;

		p.header = Header::ST_TO_ADAPTER;
		p.mode = DongleMode::F_SERVICE_RX;
		p.ctr = Control::SEND;
		p.footer = Footer::SP_TO_ADAPTER;

		p.crc = CalcCrc(p);

		return m_serial->Write(reinterpret_cast<char*>(&p), sizeof(p)) == sizeof(p);
	}


	bool SwitchOn(const DongleDeviceConnection& conn) override
	{
		DonglePacket p;

		p.header = Header::ST_TO_ADAPTER;
		p.cmd = Cmd::ON;
		p.ctr = conn.useID ? Control::SEND_TO_NOOLITE_F_ADDRESS : Control::SEND;
		p.footer = Footer::SP_TO_ADAPTER;

		PackDongleConnection(p, conn);
		p.crc = CalcCrc(p);

		return m_serial->Write(reinterpret_cast<char*>(&p), sizeof(p)) == sizeof(p);
	}

	bool SwitchOff(const DongleDeviceConnection& conn) override
	{
		DonglePacket p;

		p.header = Header::ST_TO_ADAPTER;
		p.cmd = Cmd::OFF;
		p.ctr = conn.useID ? Control::SEND_TO_NOOLITE_F_ADDRESS : Control::SEND;
		p.footer = Footer::SP_TO_ADAPTER;

		PackDongleConnection(p, conn);
		p.crc = CalcCrc(p);

		return m_serial->Write(reinterpret_cast<char*>(&p), sizeof(p)) == sizeof(p);
	}

private:
	void PackDongleConnection(DonglePacket& p, const DongleDeviceConnection& conn)
	{
		p.mode = conn.mode;
		p.channel = conn.channel;
		p.id = conn.id;
	}

	uint8_t CalcCrc(const DonglePacket& p)
	{
		uint32_t sum = 0;
		const char* buffer = reinterpret_cast<const char*>(&p);

		for (size_t i = 0; i < (size_t(Packet::CRC_POS) - size_t(Packet::ST_POS)); ++i)
		{
			sum += 0xFFul & buffer[i];
		}

		return uint8_t(sum);
	}

private:
	serial::Ptr m_serial;
};

Ptr CreateDongle()
{
	return std::make_unique<DongleImpl>();
}

};//namespace noolite

