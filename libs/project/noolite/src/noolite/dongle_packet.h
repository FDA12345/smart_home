#pragma once

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
	DonglePacket()
	{
		memset(this, 0, sizeof(*this));
	}

	Header header = Header::ST_FROM_ADAPTER;
	DongleMode mode = DongleMode::TX;

	union
	{
		struct
		{
			Control ctr;
			uint8_t reserv;
		} output;

		struct
		{
			Answer answer;

			union
			{
				struct
				{
					uint8_t morePackets;
				} F_TX;

				struct
				{
					uint8_t packetIndex;
				} RX;

				struct
				{
					uint8_t packetIndex;
				} F_RX;
			} mode;
		} input;
	};

	uint8_t channel = 0;
	Cmd cmd = Cmd::OFF;
	uint8_t fmt = 0;

	union
	{
		uint8_t d[4] = { 0 };

		struct
		{
			uint8_t type;
			uint8_t firmware;

			uint8_t state : 4;
			uint8_t reserv : 3;
			bool binding : 1;

			uint8_t light;
		} fmt_0;

		struct
		{
			uint8_t type;
			uint8_t firmware;

			bool inputClosed;

			bool disabledLiteTillReboot : 1;
			bool disabledLiteInSettings : 1;
		} fmt_1;

		struct
		{
			uint8_t type;
			uint8_t firmware;

			uint8_t freeCellsLite;
			uint8_t freeCellsLiteF;
		} fmt_2;
	};

	uint32_t id = 0;
	uint8_t crc = 0;
	Footer footer = Footer::SP_TO_ADAPTER;
};
#pragma pack(pop)
