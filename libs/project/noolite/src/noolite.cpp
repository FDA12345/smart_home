#include "noolite.h"

#include "noolite/cmd.h"
#include "noolite/control.h"
#include "noolite/answer.h"
#include "dongle_mode.h"

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