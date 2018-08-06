/*	    Mouse Function Calls and Microsoft C

	    To Run :	MSC MSCEXAMP;
			LINK MSCEXAMP,,,MOUSE;
			    or
			MAKE MSCEXAMP.MAK

*/

#include <stdio.h>
#include <dos.h>

void chkdrv();
void graf();

main()
{
	int m1, m2, m3, m4;

	chkdrv();		/* check for mouse driver   */

	m1 = 0; 		/* initialize mouse	    */
	cmouses( &m1, &m2, &m3, &m4);

	if ( m1 = 0 ) {
	    printf("Microsoft Mouse NOT found");
	    exit (-1);		/* exit, if mouse not found */
	    }

	m1 = 4; 		/* function call 4	    */
	m3 = 200;		/* set mouse pisition at    */
	m4 = 100;		/* center of the screen     */
	cmouses( &m1, &m2, &m3, &m4);

	m1 = 7; 		/* function call 7	    */
	m3 = 150;		/* minimum horizontal value */
	m4 = 450;		/* maximum horizontal value */
	cmouses( &m1, &m2, &m3, &m4);

	m1 = 8; 		/* function call 8	    */
	m3 = 50;		/* minimum vertical value   */
	m4 = 150;		/* maximum vertical value   */
	cmouses( &m1, &m2, &m3, &m4);

	graf();

	printf("Graphics cursor limited to center of the screen.\n");
	printf("Press the left button to EXIT.");

	m1 = 1; 		/* function 1, show cursor  */
	cmouses( &m1, &m2, &m3, &m4);

	m2 = 0; 		/* loop until left mouse    */
	while ( m2 != 1 ) {	/* button is pressed	    */
	    m1 = 3;
	    cmouses( &m1, &m2, &m3, &m4 );
	    }

	m1 = 2; 		/* function 2, show cursor  */
	cmouses( &m1, &m2, &m3, &m4);

}


void chkdrv()
{
	union REGS inregs, outregs;
	struct SREGS segregs;
	long address;
	unsigned char first_byte;

	inregs.x.ax = 0x3533;
	intdosx ( &inregs, &outregs, &segregs );
    
	address = (((long) segregs.es) << 16) + (long) outregs.x.bx;
	first_byte = * (long far *) address;

	if ((address == 0) || (first_byte == 0xcf)) {
	    printf("Mouse driver NOT installed");
	    exit();
	    }

}


void graf()
{
	union REGS cpuregs;

	cpuregs.x.ax = 0x0006;
	int86 ( 0x10, &cpuregs, &cpuregs );
}
