/*
 *	ega_info -- gather information about an EGA;
 *	return a non-zero value if one is found
 */

#include <dos.h>
#include <local\bioslib.h>

#define EGA_INFO 0x10
#define NMODES	2
#define NMEMSIZ	4

int
ega_info(memsize, mode, features, switches)
int *memsize;		/* EGA memory size indicator: 0 = 64K */
			/* 1 = 128K; 2 = 192K; 3 = 256K */
int *mode;		/* 0 = color mode; 1 = mono mode */
			/* use getstate function to find out which mode */
unsigned int
	*features,	/* feature bit settings */
	*switches;	/* EGA switch settings */
{
	int result = 0;
	union REGS inregs, outregs;

	/* request EGA information */
	inregs.h.ah = ALT_FUNCTION;
	inregs.h.bl = EGA_INFO;
	int86(VIDEO_IO, &inregs, &outregs);

	*memsize = outregs.h.bl;
	*mode = outregs.h.bh;
	*features = outregs.h.ch;
	*switches = outregs.h.cl;

	/* return non-zero if EGA installed */
	if (*memsize >= 0 && *memsize < NMEMSIZ && *mode >= 0 && *mode < NMODES)
		result = 1;
	return (result);
}
