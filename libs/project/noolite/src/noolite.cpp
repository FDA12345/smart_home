#include "noolite.h"

#include "include/cmd.h"
#include "include/control.h"

/*
	USB DONGLE MTRF64-USB
*/

class DongleReader
{

};

class Dongle
{
public:
	void Reboot();
	void Init();
	void Start();
	void Stop();
private:
};