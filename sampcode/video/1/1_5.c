/* Listing 1-5 */

#include	"dos.h"

main()
{
	struct	BYTEREGS regs;		/* BYTEREGS defined in dos.h */


	regs.ah = 0x0F;			/* AH=0x0F (ROM BIOS function number) */

	int86( 0x10, &regs, &regs );	/* perform interrupt 10h */

	return( (int)regs.al );
}
