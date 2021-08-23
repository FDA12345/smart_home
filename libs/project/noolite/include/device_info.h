#pragma once

enum StateType : uint8_t
{
	STATE_OFF,
	STATE_ON,
	STATE_TEMPORARY_ON,
};

enum BindType : uint8_t
{
	BIND_DISABLED,
	BIND_ENABLED,
};

enum RelayType : uint8_t
{
	RELAY_OPENED,
	RELAY_CLOSED,
};

struct DeviceInfo0 {
	uint8_t deviceType = 0;
	uint8_t firmware = 0;

	StateType state = STATE_OFF;
	BindType bindMode = BIND_DISABLED;
	uint8_t lightLevel = 0;
};

struct DeviceInfo1 {
	uint8_t deviceType = 0;
	uint8_t firmware = 0;

	RelayType relayMode = RELAY_OPENED;
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

enum DongleModes : uint8_t
{
	MODE_TX,
	MODE_RX,
	MODE_F_TX,
	MODE_F_RX,
	MODE_F_SERVICE_RX,
	MODE_F_BOOT,
};

//DongleDeviceConnection - device addressing in dongle
struct DongleDeviceConnection {
	//Mode
	DongleModes mode = MODE_TX;

	//Lite addressing info
	uint8_t Channel = 0;

	//LiteF additional addressing info
	bool useID = false;
	uint32_t id = 0;
};
