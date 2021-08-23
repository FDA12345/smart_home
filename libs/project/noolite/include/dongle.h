#pragma once

#include "serial.h"

namespace noolite
{

/*
	USB DONGLE MTRF64-USB
*/
class Dongle
{
public:
	virtual ~Dongle() = default;

	virtual bool Start(const serial::Params& serialParams) = 0;
	virtual void Stop() = 0;

	//send init command to dongle
	virtual bool Init() = 0;
	//send reboot command to dongle
	virtual bool Reboot() = 0;

	//switch relay on
	virtual bool SwitchOn() = 0;
	//switch relay off
	virtual bool SwitchOff() = 0;
};

using Ptr = std::unique_ptr<Dongle>;
Ptr CreateDongle();

};//namespace noolite
