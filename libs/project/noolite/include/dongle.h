#pragma once

#include "serial.h"
#include "device_info.h"
#include "answer.h"

namespace noolite
{

/*
	USB DONGLE MTRF64-USB
*/

struct ChannelInfo0
{
	DeviceInfo0 info0;
	Answer answer;
};

struct ChannelInfo1
{
	DeviceInfo1 info1;
	Answer answer;
};

struct ChannelInfo2
{
	DeviceInfo2 info2;
	Answer answer;
};

class Dongle
{
public:
	virtual ~Dongle() = default;

	virtual bool Start(const serial::Params& serialParams) = 0;
	virtual void Stop() = 0;

	//send init command to dongle
	virtual bool ForceInit() = 0;
	//send reboot command to dongle
	virtual bool Reboot() = 0;

	//switch relay on
	virtual bool SwitchOn(const DongleDeviceConnection& conn) = 0;
	//switch relay off
	virtual bool SwitchOff(const DongleDeviceConnection& conn) = 0;

	virtual bool ReadChannelInfo0(uint8_t channel, std::vector<ChannelInfo0>& infos0) = 0;
	virtual bool ReadChannelInfo1(uint8_t channel, std::vector<ChannelInfo1>& infos1) = 0;
	virtual bool ReadChannelInfo2(uint8_t channel, std::vector<ChannelInfo2>& infos2) = 0;
};

using Ptr = std::unique_ptr<Dongle>;
Ptr CreateDongle();

};//namespace noolite
