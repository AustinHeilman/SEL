#include "sysutil.h"

void RebootSystem()
{
	union _REGS inregs, outregs;
	//_int86(0x19, &inregs, &outregs);
	_outpw(0x64, 0xFE);
}
