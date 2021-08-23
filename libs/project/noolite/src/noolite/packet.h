#pragma once

enum class Packet : uint8_t
{
	PACKET_SIZE = 17,
	CHANNELS_COUNT = 64,

	ST_POS = 0,
	MODE_POS,

	CTR_POS,
	CTR_ANSWER_POS = 2,

	RES_POS,
	TOGL_POS = 3,

	CH_POS,
	CMD_POS,
	FMT_POS,
	D0_POS,
	D1_POS,
	D2_POS,
	D3_POS,
	ID_0_POS,
	ID_1_POS,
	ID_2_POS,
	ID_3_POS,
	CRC_POS,
	SP_POS,
};
