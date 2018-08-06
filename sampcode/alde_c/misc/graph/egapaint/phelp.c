/**************************************************************************
 *                                                                        *
 *                      P A I N T ---- H E L P                            *
 *                                                                        *
 *    To keep the main file alittle smaller, and for a little practice    *
 *     in external stuff, we'll keep this a separate file.  Someday I'll  *
 *     have sub levels of help.                                           *
 *                                                                        *
 **************************************************************************/

#define FALSE 0
#define TRUE  1

#include <dos.h>
#include <bios.h>
#include <conio.h>
#include <graphics.h>
#include <alloc.h>
#include <stdlib.h>

extern int bkcolor, cc, maxx, maxy;
extern int *first, *second;
unsigned scr_size[2];
void *scr_buffer[2];

int paint_help(void)
{
	union {
		int i;
		unsigned char c[2];
	} key;

	struct textsettingstype oldtext;

	register int i = 10;
	int title_x, title_y, title_color = YELLOW;
	int hite, wide, max_i;
	int ret_flag = 0;
    int timer = 999;
    void *text_buf;

	/* we are going to change the text style */


    gettextsettings(&oldtext);
	setusercharsize(1, 1, 7, 8);
	settextstyle(SMALL_FONT, HORIZ_DIR, USER_CHAR_SIZE);
	hite = textheight("H");
	wide = textwidth("H");

    text_buf = malloc(640 * (unsigned)hite);

	/* save the image  */

	scr_size[0] = imagesize(0, 0, maxx, ((maxy+1)/2)-1);
    scr_buffer[0] = malloc(scr_size[0]);
    if(scr_buffer[0] == NULL) {
		free(scr_buffer[0]);
    	return TRUE;
	}
	getimage(0, 0, maxx, ((maxy+1)/2)-1, scr_buffer[0]);

	scr_size[1] = imagesize(0, (maxy+1)/2, maxx, maxy);
    scr_buffer[1] = malloc(scr_size[1]);
    if(scr_buffer[1] == NULL) {
		free(scr_buffer[0]);
		free(scr_buffer[1]);
		return TRUE;
	}
	getimage(0, (maxy+1)/2, maxx, maxy, scr_buffer[1]);

	setbkcolor(BLACK);
	setcolor(YELLOW);
	clearviewport();

	outtextxy((maxx/2)+30, 30+(1 *hite), "*******************************");
	outtextxy((maxx/2)+30, 30+(2 *hite), "*                             *");
	outtextxy((maxx/2)+30, 30+(3 *hite), "*                             *");
	title_x = (maxx/2)+29+(7*wide);
	title_y = 30+(3*hite);
	outtextxy((maxx/2)+30, 30+(4 *hite), "*                             *");
	outtextxy((maxx/2)+30, 30+(5 *hite), "*                             *");
	outtextxy((maxx/2)+30, 30+(6 *hite), "*                             *");
	outtextxy((maxx/2)+30, 30+(7 *hite), "*        by J. Hudson         *");
	outtextxy((maxx/2)+30, 30+(8 *hite), "*                             *");
	outtextxy((maxx/2)+30, 30+(9 *hite), "*            1988             *");
	outtextxy((maxx/2)+30, 30+(10*hite), "*                             *");
	outtextxy((maxx/2)+30, 30+(11*hite), "*******************************");


	outtextxy(5, i, "F1, F2, F3, F4 - Set step increment to 1, 5, 10, 20");
	i += hite;
	outtextxy(5, i, "F5  - Change background color");
	i += hite;
	outtextxy(5, i, "F6  - Save the previously saved icon to a file");
	i += hite;
	outtextxy(5, i, "F7  - Read the icon file (use 'K' to display it)");
	i += hite;
	outtextxy(5, i, "F8  - Reset first and second markers");
	i += hite;
	outtextxy(5, i, "F9  - Turn header area on/off");
	i += hite;
	outtextxy(5, i, "F10 - Turn mouse ON/OFF");
	i += hite;
	outtextxy(5, i, "F11 - Change fill pattern");
	i += hite;
	outtextxy(5, i, "F12 - Change line width");
	i += hite;
	outtextxy(5, i, "1   - Set color number to 1  - ");
	setcolor(1);
	outtextxy((29 * wide) + 6, i, "BLUE");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "2   - Set color number to 2  - ");
	setcolor(2);
	outtextxy((29 * wide) + 6, i, "GREEN");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "3   - Set color number to 3  - ");
	setcolor(3);
	outtextxy((29 * wide) + 6, i, "CYAN");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "4   - Set color number to 4  - ");
	setcolor(4);
	outtextxy((29 * wide) + 6, i, "RED");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "5   - Set color number to 5  - ");
	setcolor(5);
	outtextxy((29 * wide) + 6, i, "MAGENTA");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "6   - Set color number to 6  - ");
	setcolor(6);
	outtextxy((29 * wide) + 6, i, "BROWN");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "7   - Set color number to 7  - ");
	setcolor(7);
	outtextxy((29 * wide) + 6, i, "LIGHT GRAY");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "8   - Set color number to 8  - ");
	setcolor(8);
	outtextxy((29 * wide) + 6, i, "DARK GRAY");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "9   - Set color number to 9  - ");
	setcolor(9);
	outtextxy((29 * wide) + 6, i, "LIGHT BLUE");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "0   - Set color number to 10 - ");
	setcolor(10);
	outtextxy((29 * wide) + 6, i, "LIGHT GREEN");
	setcolor(YELLOW);
	i += hite;
	outtextxy(8, i, "!   - Set color number to 11 - ");
	setcolor(11);
	outtextxy((29 * wide) + 6, i, "LIGHT CYAN");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "@   - Set color number to 12 - ");
	setcolor(12);
	outtextxy((29 * wide) + 6, i, "LIGHT RED");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "#   - Set color number to 13 - ");
	setcolor(13);
	outtextxy((29 * wide) + 6, i, "LIGHT MAGENTA");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "$   - Set color number to 14 - ");
	setcolor(14);
	outtextxy((29 * wide) + 6, i, "YELLOW");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "%   - Set color number to 15 - ");
	setcolor(15);
	outtextxy((29 * wide) + 6, i, "WHITE");
	setcolor(YELLOW);
	i += hite;
	outtextxy(5, i, "B   - Draw box around marked points");
	i += hite;
	outtextxy(5, i, "C   - Draw a circle, first point is the center, second point is the radius");
	i += hite;
	outtextxy(5, i, "D   - Delete a marked area");
	i += hite;
	outtextxy(5, i, "E   - Draw ellipse");
	i += hite;
/*	outtextxy(5, i, "F   - ");
	i += hite;
*/	outtextxy(5, i, "G   - Turn grid ON/OFF");
	i += hite;
/*	outtextxy(5, i, "H   - ");
	i += hite;
*/	outtextxy(5, i, "J   - Save an image to memory");
	i += hite;
	outtextxy(5, i, "K   - Restore the saved image from memory");
	i += hite;
	outtextxy(5, i, "L   - Draw a line between points");
	i += hite;
	outtextxy(5, i, "M   - Move an area");
	i += hite;
	outtextxy(5, i, "N   - New color for an area");
	i += hite;
	outtextxy(5, i, "O   - Turn DRAW on/off (Pen up - Pen down)");
	i += hite;
	outtextxy(5, i, "P   - Swap pages");
	i += hite;
	outtextxy(0, i, "^P  - Print Screen");
	i += hite;
	outtextxy(5, i, "Q   - Quit");
	i += hite;
	outtextxy(5, i, "R   - Restore image from a file ");
	i += hite;
	outtextxy(5, i, "S   - Save image to a file");
	i += hite;
	outtextxy(0, i, "^S   - Change the screen scale");
	i += hite;
	outtextxy(5, i, "T   - Enter text");
	i += hite;
	outtextxy(5, i, "V   - Jump to the nearest pixel that matches the drawing color");
	i += hite;
	outtextxy(5, i, "W   - Fill a bounded area (boundry must be same color)");
	i += hite;
	outtextxy(5, i, "X   - Copy an area");
	i += hite;
	outtextxy(5, i, "Z   - Clear the screen");
	i += hite;
	outtextxy(5, i, "?   - Print this help screen");
	i += hite;
	outtextxy(0, i, "<CR> - Mark first   ,then second   points");
	putimage(5 + (17*wide), i-2, first, COPY_PUT);
	putimage(5 + (32*wide), i+2, second, COPY_PUT);

	max_i = (i/hite) - 1;

	outtextxy(400, 349-hite, "Hit <ESC> to return");

	settextstyle(TRIPLEX_FONT, HORIZ_DIR, 2);

	i = 0;
	getimage(0, (i*hite)+11, (wide*15)+7, ((i+1)*hite)+11, text_buf);
	putimage(0, (i*hite)+11, text_buf, NOT_PUT);

	while(!ret_flag) {
		if(bioskey(1)) {
			key.i = bioskey(0);
			if(key.c[0] == 0x1b) ret_flag = 1;
			else {

                /* only use the keypad --- alais not the mouse  */


				if(!key.c[0]) {
                 	if(key.c[1] == 72) {		/* up  */
						if(i == 0) getimage(0, (i*hite)+11, (wide*15)+7, ((i+1)*hite)+11, text_buf);
						else getimage(0, (i*hite)+11, (wide*3)+7, ((i+1)*hite)+11, text_buf);
						putimage(0, (i*hite)+11, text_buf, NOT_PUT); /* renew the old */
						i -= 1;
						if(i < 0) i = 0;
						if(i == 0) getimage(0, (i*hite)+11, (wide*15)+7, ((i+1)*hite)+11, text_buf);
						else getimage(0, (i*hite)+11, (wide*3)+7, ((i+1)*hite)+11, text_buf);
						putimage(0, (i*hite)+11, text_buf, NOT_PUT);
					}
                 	if(key.c[1] == 80) {		/* down  */
						if(i == 0) getimage(0, (i*hite)+11, (wide*15)+7, ((i+1)*hite)+11, text_buf);
						else getimage(0, (i*hite)+11, (wide*3)+7, ((i+1)*hite)+11, text_buf);
						putimage(0, (i*hite)+11, text_buf, NOT_PUT); /* renew th old */
						i += 1;
						if(i >= max_i) i = max_i;
						getimage(0, (i*hite)+11, (wide*3)+7, ((i+1)*hite)+11, text_buf);
						putimage(0, (i*hite)+11, text_buf, NOT_PUT);
					}

				}
			}
		} else {
			if(timer != 12000) timer++;
			else {
				if(title_color++ >= MAXCOLORS) title_color = BLUE;
				setcolor(title_color);
				outtextxy(title_x, title_y, "P A I N T");
				timer = 0;
			}
		}
	}
	setbkcolor(bkcolor);
	clearviewport();
	setcolor(cc);
	settextjustify(oldtext.horiz, oldtext.vert);
	settextstyle(oldtext.font, oldtext.direction, oldtext.charsize);

	putimage(0, 0, scr_buffer[0], COPY_PUT);
	putimage(0, (maxy+1)/2, scr_buffer[1], COPY_PUT);
 	free(scr_buffer[0]);
	free(scr_buffer[1]);

	return FALSE;
}

