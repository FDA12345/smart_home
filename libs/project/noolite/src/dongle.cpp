#include "stdafx.h"
#include "dongle.h"
#include "logger.h"

#include <thread>
#include <mutex>
#include <list>
#include <map>

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

			m_threadRead = std::thread
			{ [this]()
			{
				DoSerialReading();
			} };

			return true;
		}

		Stop();
		return false;
	}

	void Stop() override
	{
		if (m_threadRead.joinable())
		{
			m_serial->Close();
			m_threadRead.join();

			m_threadRead = std::thread();
		}

		m_serial.reset();
	}

	bool Reboot() override
	{
		DonglePacket req;

		req.header = Header::ST_TO_ADAPTER;
		req.mode = DongleMode::F_BOOT;
		req.output.ctr = Control::B_CMD_RESET_OK;
		req.footer = Footer::SP_TO_ADAPTER;
		req.crc = CalcCrc(req);

		bool ret = WaitRequest(req, [](const DonglePacket& rsp)
		{
			return true;
		});

		if (ret)
		{
			std::this_thread::sleep_for(std::chrono::seconds(5));
			return ForceInit();
		}

		return false;
	}

	bool ForceInit() override
	{
		DonglePacket req;

		req.header = Header::ST_TO_ADAPTER;
		req.mode = DongleMode::F_SERVICE_RX;
		req.output.ctr = Control::SEND;
		req.footer = Footer::SP_TO_ADAPTER;
		req.crc = CalcCrc(req);

		return WaitRequest(req, [](const DonglePacket& rsp)
		{
			return true;
		});
	}


	bool SwitchOn(const DongleDeviceConnection& conn) override
	{
		DonglePacket req;

		req.header = Header::ST_TO_ADAPTER;
		req.cmd = Cmd::ON;
		req.output.ctr = conn.useID ? Control::SEND_TO_NOOLITE_F_ADDRESS : Control::SEND;
		req.footer = Footer::SP_TO_ADAPTER;

		PackDongleConnection(req, conn);
		req.crc = CalcCrc(req);

		return WaitRequest(req, [](const DonglePacket& rsp)
		{
			return true;
		});
	}

	bool SwitchOff(const DongleDeviceConnection& conn) override
	{
		DonglePacket req;

		req.header = Header::ST_TO_ADAPTER;
		req.cmd = Cmd::OFF;
		req.output.ctr = conn.useID ? Control::SEND_TO_NOOLITE_F_ADDRESS : Control::SEND;
		req.footer = Footer::SP_TO_ADAPTER;

		PackDongleConnection(req, conn);
		req.crc = CalcCrc(req);

		return WaitRequest(req, [](const DonglePacket& rsp)
		{
			return true;
		});
	}

private:
	bool WaitRequest(const DonglePacket& req, std::function<bool(const DonglePacket& rsp)> rspFn)
	{
		if (m_serial->Write(reinterpret_cast<const char*>(&req), sizeof(req)) != sizeof(req))
		{
			return false;
		}

		bool invalidResponse = false;

		while (true)
		{
			DonglePacket rsp;
			if (!WaitResponse(req.mode, rsp))
			{
				return false;
			}

			if (!invalidResponse && !rspFn(rsp))
			{
				invalidResponse = true;
			}

			if ((rsp.mode != DongleMode::RX) && (rsp.mode != DongleMode::F_RX))
			{
				if (rsp.input.mode.otherMode.morePackets > 0)
				{
					continue;
				}
			}

			break;
		}

		return !invalidResponse;
	}

	void DoSerialReading()
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
				{
					std::unique_lock lock(m_incomeMx);

					//if m_incomeWaiting than wait for packet dispatching
					if (m_incomeWaiting)
					{
						m_incomeWaiting = false;
						m_incomeReceived = false;
						m_incomePacket = p;

						m_incomeCv.notify_one();
						m_incomeCv.wait(lock, [this]()
						{
							return m_incomeReceived;
						});
					}
				}

				FireOnPacketIncome(p);
			}
		}
	}

	void FireOnPacketIncome(const DonglePacket& p)
	{
		std::list<ParsePacketFn> receivers;

		{
			std::lock_guard lock(m_incomeMx);
			receivers.splice(receivers.begin(), m_receivers);
		}

		for (const auto& recvFn : receivers)
		{
			recvFn(p);
		}
	}

	bool WaitResponse(const DongleMode& mode, DonglePacket& p)
	{
		std::unique_lock lock(m_incomeMx);

		m_incomeWaiting = true;
		m_incomeCv.wait(lock, [this, &mode, &p]()
		{
			return !m_incomeWaiting && (m_incomePacket.mode == mode);
		});

		p = m_incomePacket;

		m_incomeReceived = true;
		m_incomeCv.notify_one();

		return true;
	}

	static void PackDongleConnection(DonglePacket& p, const DongleDeviceConnection& conn)
	{
		p.mode = conn.mode;
		p.channel = conn.channel;
		p.id = conn.id;
	}

	static uint8_t CalcCrc(const DonglePacket& p)
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
	logger::Ptr m_log = logger::Create();
	serial::Ptr m_serial;
	std::thread m_threadRead;

	bool m_incomeWaiting = false;
	bool m_incomeReceived = false;
	DonglePacket m_incomePacket;

	std::mutex m_incomeMx;
	std::condition_variable m_incomeCv;

	using ParsePacketFn = std::function<bool(const DonglePacket& p)>;
	std::list<ParsePacketFn> m_receivers;
};

Ptr CreateDongle()
{
	return std::make_unique<DongleImpl>();
}

};//namespace noolite

