#pragma once

enum class Control : uint8_t
{
	SEND,
	SEND_BROADCAST,
	READ,
	BIND,
	UNBIND,
	CLEAR_CHANNEL,
	CLEAR_ALL_CHANNELS,
	UNBIND_ADDRESS_FROM_CHANNEL,
	SEND_TO_NOOLITE_F_ADDRESS,

	B_CMD_RESET_OK = 14,
};