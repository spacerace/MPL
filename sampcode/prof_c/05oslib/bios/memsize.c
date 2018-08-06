/*
 *	memsize -- get memory size
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>

int
memsize()
{
	union REGS inregs, outregs;

	return (int86(MEM_SIZE, &inregs, &outregs));
}
