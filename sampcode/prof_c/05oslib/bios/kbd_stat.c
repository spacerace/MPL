/*
 *	kbd_stat -- return the keyboard status
 *	word (bit-significant)
 */

#include <dos.h>
#include <local\bioslib.h>
#include <local\keybdlib.h>

unsigned char
kbd_stat()
{
	union REGS inregs, outregs;

	inregs.h.ah = KBD_STATUS;
	int86(KEYBD_IO, &inregs, &outregs);

	return ((unsigned char)(outregs.h.al));
}
