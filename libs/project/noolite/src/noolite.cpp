#include "noolite.h"

#include "noolite/cmd.h"
#include "noolite/control.h"
#include "noolite/answer.h"
#include "noolite/packet.h"
#include "noolite/header.h"
#include "noolite/footer.h"

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