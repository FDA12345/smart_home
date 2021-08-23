#pragma once

#include "dongle_mode.h"

enum class State : uint8_t
{
	OFF,
	ON,
	TEMPORARY_ON,
};

enum class BindMode : uint8_t
{
	DISABLED,
	ENABLED,
};

enum class RelayMode : uint8_t
{
	OPENED,
	CLOSED,
};

struct DeviceInfo0 {
	uint8_t deviceType = 0;
	uint8_t firmware = 0;

	State state = State::OFF;
	BindMode bindMode = BindMode::DISABLED;
	uint8_t lightLevel = 0;
};

struct DeviceInfo1 {
	uint8_t deviceType = 0;
	uint8_t firmware = 0;

	RelayMode relayMode = RelayMode::OPENED;
	bool DisabledTillReboot_Lite = false;
	bool Disabled_Lite = false;
};

struct DeviceInfo2 {
	uint8_t deviceType = 0;
	uint8_t firmware = 0;

	uint8_t FreeBindCells_Lite = 0;
	uint8_t FreeBindCells_Lite_F = 0;
};

struct DeviceInfo {
	bool Info0_Loaded = false;
	DeviceInfo0 Info0;

	bool Info1_Loaded = false;
	DeviceInfo1 Info1;

	bool Info2_Loaded = false;
	DeviceInfo2 Info2;
};


//DongleDeviceConnection - device addressing in dongle
struct DongleDeviceConnection {
	//Mode
	DongleMode mode = DongleMode::TX;

	//Lite addressing info
	uint8_t Channel = 0;

	//LiteF additional addressing info
	bool useID = false;
	uint32_t id = 0;
};
