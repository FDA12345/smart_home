#include "stdafx.h"
#include "dongle.h"

#include <thread>
#include <mutex>
#include <list>

#include "noolite/dongle_packet.h"

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

			std::thread
			{ [this]()
			{
				while (true)
				{
					DonglePacket p;

					size_t n = m_serial->ReadUntil(reinterpret_cast<char*>(&p), sizeof(p), static_cast<char>(Footer::SP_FROM_ADAPTER));
					if (n == 0)
					{
						break;
					}
					if ((n == sizeof(p)) && (p.crc == CalcCrc(p)))
					{
						std::lock_guard lock(m_mx);
						m_incomePackets.emplace_back(std::move(p));
						m_cv.notify_one();
					}
				}
			} }.detach();

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
		p.output.ctr = Control::SEND;
		p.footer = Footer::SP_TO_ADAPTER;

		p.crc = CalcCrc(p);

		if (m_serial->Write(reinterpret_cast<char*>(&p), sizeof(p)) != sizeof(p))
		{
			return false;
		}

		WaitAnswer(p.mode, p);
		return true;
	}


	bool SwitchOn(const DongleDeviceConnection& conn) override
	{
		DonglePacket p;

		p.header = Header::ST_TO_ADAPTER;
		p.cmd = Cmd::ON;
		p.output.ctr = conn.useID ? Control::SEND_TO_NOOLITE_F_ADDRESS : Control::SEND;
		p.footer = Footer::SP_TO_ADAPTER;

		PackDongleConnection(p, conn);
		p.crc = CalcCrc(p);

		if (m_serial->Write(reinterpret_cast<char*>(&p), sizeof(p)) != sizeof(p))
		{
			return false;
		}

		while (true)
		{
			WaitAnswer(p.mode, p);

			if (conn.mode != DongleMode::F_TX)
			{
				break;
			}

			if (p.input.mode.F_TX.morePackets == 0)
			{
				break;
			}
		}
		return true;
	}

	bool SwitchOff(const DongleDeviceConnection& conn) override
	{
		DonglePacket p;

		p.header = Header::ST_TO_ADAPTER;
		p.cmd = Cmd::OFF;
		p.output.ctr = conn.useID ? Control::SEND_TO_NOOLITE_F_ADDRESS : Control::SEND;
		p.footer = Footer::SP_TO_ADAPTER;

		PackDongleConnection(p, conn);
		p.crc = CalcCrc(p);

		return m_serial->Write(reinterpret_cast<char*>(&p), sizeof(p)) == sizeof(p);
	}

private:
	std::list<DonglePacket> m_incomePackets;
	std::mutex m_mx;
	std::condition_variable m_cv;
	void WaitAnswer(const DongleMode& mode, DonglePacket& p)
	{
		std::unique_lock lock(m_mx);
		m_cv.wait(lock, [this, &mode, &p]()
		{
			for (auto it = m_incomePackets.begin(); it != m_incomePackets.end(); ++it)
			{
				if (it->mode != mode)
				{
					continue;
				}

				p = std::move(*it);
				m_incomePackets.erase(it);
				return true;
			}

			return false;
		});
	}

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

