/***********************************************************
 *														   *
 *               PRINT SCREEN UTILITY for Paint 		   *
 *														   *
 *  This was designed for an ALPS 218 seies printer        *
 *   using EPSON emulation.  I do not know how well        *
 *   it will work with other color printers, but it's      *
 *   good for guidelines.							       *
 *														   *
 ***********************************************************/


#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <graphics.h>

#define WIDTH 80L
#define XMAX 638
#define YMAX 349
#define XMIN 5
#define YMIN 0
#define CHARSOUT 800


/* set up buffers for each of the colors.  The leading chars set up the
   color.  'C' just assigns memory for the rest of the array
*/

static unsigned char black_buf[9+CHARSOUT] = {' ',' ',0x1b,'r','0',0x1b,'K',0x5e,0x1};
static unsigned char magenta_buf[9+CHARSOUT] = {' ',' ',0x1b,'r','1',0x1b,'K',0x5e,0x1};
static unsigned char blue_buf[9+CHARSOUT] = {' ',' ',0x1b,'r','2',0x1b,'K',0x5e,0x1};
static unsigned char violet_buf[9+CHARSOUT] = {' ',' ',0x1b,'r','3',0x1b,'K',0x5e,0x1};
static unsigned char yellow_buf[9+CHARSOUT] = {' ',' ',0x1b,'r','4',0x1b,'K',0x5e,0x1};
static unsigned char orange_buf[9+CHARSOUT] = {' ',' ',0x1b,'r','5',0x1b,'K',0x5e,0x1};
static unsigned char green_buf[9+CHARSOUT] = {' ',' ',0x1b,'r','6',0x1b,'K',0x5e,0x1};

/* I nuked off the last line of pins to improve symmetry.  I haven't
   seen any ill effects
*/


/* char tank[] = {' ',' ',0x1b,'A',0x8,'\0'};  */
char tank[] = {' ',' ',0x1b,'A',0x7,'\0'};

char put_out(char character);

void printscr(int reverse)
{
	int x = XMAX, y, i;
	unsigned char mask;
	int c;

	/* set flags to see if any data is on the line for that color */

	char k_flag, m_flag, b_flag, v_flag, y_flag, o_flag, g_flag;

    fputs("\r", stdprn);
	fputs("\n", stdprn);

	fputs(tank,stdprn);

    fputs("\r", stdprn);
	fputs("\n", stdprn);

	while(x > 4) {
		if(kbhit()) {
			fputs("\0x1bA\0x9", stdprn);
			getch();
			break;
		}

		/* clear the buffers and flags */

		for(i = 9; i < CHARSOUT + 9; i++) black_buf[i] = 0;
		for(i = 9; i < CHARSOUT + 9; i++) magenta_buf[i] = 0;
		for(i = 9; i < CHARSOUT + 9; i++) blue_buf[i] = 0;
		for(i = 9; i < CHARSOUT + 9; i++) violet_buf[i] = 0;
		for(i = 9; i < CHARSOUT + 9; i++) yellow_buf[i] = 0;
		for(i = 9; i < CHARSOUT + 9; i++) orange_buf[i] = 0;
		for(i = 9; i < CHARSOUT + 9; i++) green_buf[i] = 0;
		k_flag = m_flag = b_flag = v_flag = y_flag = o_flag = g_flag = 0;

/*		for(i = 0, mask = 0x80; i < 8; i++, mask >>= 1, x--) {  */
		for(i = 0, mask = 0x80; i < 7; i++, mask >>= 1, x--) {
			for(y = 0; y < YMAX; y++) {
				c = getpixel(x,y);     /* fill bit positions for the colors */
				switch(c) {
			 		case DARKGRAY: 	black_buf[y+9] |= mask;
									k_flag = 1;
								   	break;
					case RED: 		magenta_buf[y+9] |= mask;
									m_flag = 1;
									break;
					case LIGHTRED:  if(y & (x & 1) ? 0 : 1) {
										magenta_buf[y+9] |= mask;
										m_flag = 1;
									}
									break;
					case BLUE:	  	blue_buf[y+9] |= mask;
									b_flag = 1;
									break;
					case CYAN:		blue_buf[y+9] |= mask;
									b_flag = 1;
									break;
					case LIGHTBLUE:	if(y & (x & 1) ? 0 : 1) {
										blue_buf[y+9] |= mask;
										b_flag = 1;
									}
									break;
					case LIGHTCYAN:	if(y & (x & 1) ? 0 : 1) {
										blue_buf[y+9] |= mask;
										b_flag = 1;
									}
									break;
					case MAGENTA:	violet_buf[y+9] |= mask;
									v_flag = 1;
									break;
					case LIGHTMAGENTA: if(y & (x & 1) ? 0 : 1) {
											violet_buf[y+9] |= mask;
											v_flag = 1;
									   }
									break;
					case YELLOW:	yellow_buf[y+9] |= mask;
									y_flag = 1;
									break;
					case BROWN:		orange_buf[y+9] |= mask;
									o_flag = 1;
									break;
					case GREEN:		green_buf[y+9] |= mask;
									g_flag = 1;
									break;
					case LIGHTGREEN: if(y & (x & 1) ? 0 : 1) {
										green_buf[y+9] |= mask;
										g_flag = 1;
									 }
									break;
				}
			}
			if(x < XMIN) break;   /* MIN X to keep vestiges from appearing */
		}
		if(k_flag) {           /* print the color only if sometimg is there */
			for(i = 0; i < CHARSOUT+9; i++)
				put_out(black_buf[i]);
        	fputs("\r", stdprn);
		}
        if(m_flag) {
			for(i = 0; i < CHARSOUT+9; i++)
				put_out(magenta_buf[i]);
        	fputs("\r", stdprn);
		}
		if(b_flag) {
			for(i = 0; i < CHARSOUT+9; i++)
				put_out(blue_buf[i]);
        	fputs("\r", stdprn);
		}
        if(v_flag) {
			for(i = 0; i < CHARSOUT+9; i++)
				put_out(violet_buf[i]);
        	fputs("\r", stdprn);
		}
        if(y_flag) {
			for(i = 0; i < CHARSOUT+9; i++)
				put_out(yellow_buf[i]);
        	fputs("\r", stdprn);
		}
        if(o_flag) {
			for(i = 0; i < CHARSOUT+9; i++)
				put_out(orange_buf[i]);
        	fputs("\r", stdprn);
		}
        if(g_flag) {
			for(i = 0; i < CHARSOUT+9; i++)
				put_out(green_buf[i]);
        	fputs("\r", stdprn);
		}


		fputs("\n", stdprn);

	}
	fputs("\x0c", stdprn);
	fflush(stdprn);			/* print the last line */
}

/* not used - but someday ??? */

char status(void)
{
	union REGS regs;

	regs.h.ah = 2;
	regs.x.dx = 0;
	int86(0x17, &regs, &regs);
	return(regs.h.ah & 0x80);
}

/* out put to the printer */

char put_out(char character)
{
	union REGS regs;

	regs.h.ah = 0;
	regs.h.al = character;
	regs.x.dx = 0;
	int86(0x17, &regs, &regs);
	return(regs.h.ah);
}




