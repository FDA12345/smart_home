#pragma once

namespace noolite
{

/*
	USB DONGLE MTRF64-USB
*/
class Dongle
{
public:
	virtual ~Dongle() = default;

	virtual bool Start() = 0;
	virtual void Stop() = 0;

	//send init command to dongle
	virtual void Init() = 0;
	//send reboot command to dongle
	virtual void Reboot() = 0;
};

using Ptr = std::unique_ptr<Dongle>;
Ptr CreateDongle();

};//namespace noolite
