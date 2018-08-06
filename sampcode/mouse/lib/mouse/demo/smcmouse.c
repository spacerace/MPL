/*----------------------------------------------------------------------*/
/* REFERENCE -- Small model mouse drive interface test   IN  smcmouse.c	*/
/*									*/
/* To create smcmouse.exe:						*/
/*				MSC SMCMOUSE;				*/
/*				LINK SMCMOUSE+SUBS,,,..\MOUSE		*/
/*			or:						*/
/*				MAKE SMCMOUSE.MAK			*/
/*									*/
/*----------------------------------------------------------------------*/
#include <stdio.h>
#include <dos.h>

#define mouse(a, b, c, d)	cmouses(a, b, c, d)
    
#define	M_RESET		0
#define M_SHOW_CURS	1
#define M_HIDE_CURS	2
#define M_GET_STATUS	3
#define M_SET_CURS	4
#define M_GET_PRESS	5
#define M_GET_REL	6
#define M_SET_MAX_POS	7
#define M_SET_MIN_POS	8
#define M_SET_G_CURS	9
#define M_SET_T_CURS	10
#define M_READ_COUNT	11
#define M_USER_SUB	12
#define M_LPEN_ON	13
#define M_LPEN_OFF	14
#define M_MICK_2_PIX	15
#define M_COND_OFF	16
#define M_D_SPEED	19
    
#define XMAX		640
#define YMAX		200


main()
{
    int m1, m2, m3, m4, m5;
    int t, b, l, r;
    int bounds[4];
    int *bptr=bounds;

    cls();
    chkdrv();
    
    m1 = M_RESET;
    mouse(&m1, &m2, &m3, &m4);
    if (m1)
    {
	printf("\nMouse driver installed, %d button mouse", m2);
    }
    else
    {
	printf("\nMouse driver not installed, exiting.");
	return;
    }

    test_graphics_cursor();

    m1 = M_SET_CURS;
    m3 = XMAX;
    m4 = YMAX/2;
    mouse(&m1, &m2, &m3, &m4);
    
    m1 = M_SHOW_CURS;
    mouse(&m1, &m2, &m3, &m4);
    
    printf("\nCursor should appear at end of middle line (press a key)...");
    getch();
	
    t = 0;    b = YMAX;    l = 0;    r = XMAX;
    do
    {
	circle(t, b, l, r);
	t += 8;
    	b -= 8;
    	l += 8;
    	r -= 8;
    } 
    while (t <= b);

    m1 = M_HIDE_CURS;
    mouse(&m1, &m2, &m3, &m4);
    cls();
    m1 = M_SHOW_CURS;
    mouse(&m1, &m2, &m3, &m4);
   
    printf("+----------------------------------------------------+\n");
    printf("|  Test of conditional off function call.  Move the  |\n");
    printf("|  cursor around, and verify that it disappears in   |\n");
    printf("|  the area bounded by the dashed line.              |\n");
    printf("|                                                    |\n");
    printf("|  Press any key to redisplay the cursor.            |\n");
    printf("|                                                    |\n");
    printf("|  Press ESCape when verified.                       |\n");
    printf("+----------------------------------------------------+\n");
    
    do
    {
	m1 = M_SET_CURS;
	m3 = XMAX;
	m4 = YMAX/2;
	mouse(&m1, &m2, &m3, &m4);
	
	m1 = M_SHOW_CURS;
	mouse(&m1, &m2, &m3, &m4);
	
	m1 = M_COND_OFF;
	bounds[0] = 0;
	bounds[1] = 0;
	bounds[2] = 424;
	bounds[3] = 64;
	mouse(&m1, &m2, &m3, &bptr);
    } while (getch() != 0x1B);

    m1 = M_SHOW_CURS;
    mouse(&m1, &m2, &m3, &m4);
    
    read_pos();

    printf("\n\n\nPress a key to terminate this program");
    getch();
    m1 = M_RESET;
    mouse(&m1, &m2, &m3, &m4);
}

   
chkdrv()
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

    
cls()
{
    union REGS inregs, outregs;
    
    inregs.x.ax = 0x0600;
    inregs.h.bh = 7;
    inregs.x.cx = 0;
    inregs.x.dx = 0x184f;
    int86(0x10, &inregs, &outregs);
    inregs.x.ax = 0x0200;
    inregs.h.bh = 0;
    inregs.x.dx = 0;
    int86(0x10, &inregs, &outregs);
}


#define VWAIT 100
#define HWAIT 50

circle (t, b, l, r)
int t, b, l, r;
{
    int m1, m2, m3, m4, i;

    m1 = M_SET_CURS;
    m4 = t;

    for (m3 = l; m3 < r; m3++)
    {
	for (i=0; i < HWAIT; i++);
	mouse(&m1, &m2, &m3, &m4);
    }

    for (m4 = t; m4 < b; m4++)
    {
	for (i=0; i < VWAIT; i++);
	mouse(&m1, &m2, &m3, &m4);
    }

    for (; m3 >= l; m3--)
    {
	for (i=0; i < HWAIT; i++);
	mouse(&m1, &m2, &m3, &m4);
    }

    for (; m4 >= t; m4--)
    {
	for (i=0; i < VWAIT; i++);
	mouse(&m1, &m2, &m3, &m4);
    }
}


read_pos()
{
    int m1, m2, m3, m4;

    m1 = M_HIDE_CURS;
    mouse(&m1, &m2, &m3, &m4);
    cls();
    m1 = M_SHOW_CURS;
    mouse(&m1, &m2, &m3, &m4);
    
    printf("\nMove mouse to the UPPER LEFT corner, RELEASE both buttons");
    printf("\nand then press Enter: ");
    while(getch() != 0x0d)
	;
    m1 = M_GET_STATUS;
    mouse(&m1, &m2, &m3, &m4);
    if (m2 != 0)	printf("\nBUTTON INFO WRONG.");
    if (m3 != 0)	printf("\nX VALUE WRONG, EXPECT 0, READ %d", m3);
    if (m4 != 0)	printf("\nY VALUE WRONG, EXPECT 0, READ %d", m4);

    printf("\nMove mouse to the BOTTOM RIGHT corner, PRESS both buttons");
    printf("\nand then press Enter: ");
    while(getch() != 0x0d)
	;
    m1 = M_GET_STATUS;
    mouse(&m1, &m2, &m3, &m4);
    if (m2 != 3)	printf("\nBUTTON INFO WRONG.");
    if (m3 != 632)	printf("\nX VALUE WRONG, EXPECT 632, READ %d", m3);
    if (m4 != 192)	printf("\nY VALUE WRONG, EXPECT 192, READ %d", m4);
}


test_graphics_cursor()
{
    static int cursor[32] =
    {
	/* SCREEN MASK FOR POINTING HAND */
	
	0xE1FF, 0xE1FF, 0xE1FF, 0xE1FF, 0xE1FF, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	
	/* CURSOR MASK FOR POINTING HAND */
	
	0x1E00, 0x1200, 0x1200, 0x1200, 0x1200, 0x13FF, 0x1249, 0x1249,
	0xF249, 0x9001, 0x9001, 0x9001, 0x8001, 0x8001, 0x8001, 0xFFFF
    };

    int *cptr=cursor;
    
    int m1, m2, m3, m4;
    
    video_mode(6);			/* 640 X 200 B&W		*/

    m1 = 9;
    m2 = 0;
    m3 = 0;
    mouse(&m1, &m2, &m3, &cptr);
    m1 = 1;
    mouse(&m1, &m2, &m3, &m4);

    printf("\ncursor should now be a pointing hand.");
    printf("\nMove around, press Enter when done...");
    getch();
    video_mode(3);
}


video_mode(mode)
int mode;
{
    union REGS inregs, outregs;
    
    inregs.h.ah = 0;
    inregs.h.al = mode;
    int86(0x10, &inregs, &outregs);
}
