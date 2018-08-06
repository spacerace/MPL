/**************************************************************************
 *                                                                        *
 *      Utility to change the line style.  Originally set up to           *
 *      use turbo's internal settings, but I had to modify it when I      *
 *      went to the exclusive OR mode custom lines for the grid           *
 *      function. It still looks real pretty.  All that needs to be done  *
 *      is to set a mask and use it in the line draw routine              *
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
#include <stdarg.h>

extern int bkcolor, cc, maxx, maxy;
extern unsigned scr_size[2];
extern void *scr_buffer[2];
extern int width;
extern int gprintf( int xx, int yy, char *fmt, ... );

int lstyle(void)
{
	union {
		int i;
		unsigned char c[2];
	} key;

	struct {
		int type;
		int pattern;
	} style[25] = {
		{0, 0},
		{0, 0},
		{1, 0},
		{2, 0},
		{3, 0},
		{4, 0xfffe},
		{4, 0xfffc},
		{4, 0xfff8},
		{4, 0xfff0},
		{4, 0xffe0},
		{4, 0xffc0},
		{4, 0xff80},
		{4, 0xff00},
		{4, 0xfe00},
		{4, 0xfc00},
		{4, 0xf800},
		{4, 0xf000},
		{4, 0xe000},
		{4, 0xc000},
		{4, 0x8000},
		{4, 0x1111},
		{4, 0x3333},
		{4, 0x5555},
		{4, 0x7777},
		{4, 0x9999}
	};


	struct textsettingstype oldtext;
	struct linesettingstype oldline;

	register int i = 10;
	int title_x, title_y;
	int hite, wide, max_i;
	int ret_flag = 0;
	void *text_buf;
	int index = 1;

    gettextsettings(&oldtext);
	setusercharsize(1, 1, 7, 8);
	settextstyle(SMALL_FONT, HORIZ_DIR, USER_CHAR_SIZE);
	hite = textheight("H");
	wide = textwidth("H");

	text_buf = malloc(hite*320);

	/* save the screen */

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

	for(index = 1; index < 25; index++) {
		gprintf(5, i, "%d ",index);
		setlinestyle(style[index].type, style[index].pattern, width);
		line(5*wide, i+4, 300, i+4);
		i += hite;
	}

	/* indicate center line */

	outtextxy(310, (hite*2)+ 8, "C");
	outtextxy(313, (hite*2)+12, "L");

	max_i = (i/hite)-2;
	i = 300;
	outtextxy(170, i, "           Point to style with arrow keys");
	i += hite+4;
	outtextxy(170, i, "           Hit <ESC> to select and return");

	i = 0;
	getimage(0, (i*hite)+11, (wide*3)+7, ((i+1)*hite)+11, text_buf);
	putimage(0, (i*hite)+11, text_buf, NOT_PUT);

	while(!ret_flag) {
		if(bioskey(1)) {
			key.i = bioskey(0);
			if(key.c[0] == 0x1b) {
				ret_flag = 1;
				setlinestyle(style[i+1].type, style[i+1].pattern, width);
			} else {

				/* only use the cursor --- not the mouse  */

				if(!key.c[0]) {
                 	if(key.c[1] == 72) {		/* up  */
						getimage(0, (i*hite)+11, (wide*3)+7, ((i+1)*hite)+11, text_buf);
						putimage(0, (i*hite)+11, text_buf, NOT_PUT); /* renew the old */
						i -= 1;
						if(i < 0) i = 0;
						getimage(0, (i*hite)+11, (wide*3)+7, ((i+1)*hite)+11, text_buf);
						putimage(0, (i*hite)+11, text_buf, NOT_PUT);
					}
                 	if(key.c[1] == 80) {		/* down  */
						getimage(0, (i*hite)+11, (wide*3)+7, ((i+1)*hite)+11, text_buf);
						putimage(0, (i*hite)+11, text_buf, NOT_PUT); /* renew th old */
						i += 1;
						if(i >= max_i) i = max_i;
						getimage(0, (i*hite)+11, (wide*3)+7, ((i+1)*hite)+11, text_buf);
						putimage(0, (i*hite)+11, text_buf, NOT_PUT);
					}

				}
			}
		}

	}
	setbkcolor(bkcolor);
	clearviewport();
	setcolor(cc);
	settextjustify(oldtext.horiz, oldtext.vert);
	settextstyle(oldtext.font, oldtext.direction, oldtext.charsize);

	/* retun the original picture, and free the buffers  */

	putimage(0, 0, scr_buffer[0], COPY_PUT);
	putimage(0, (maxy+1)/2, scr_buffer[1], COPY_PUT);
 	free(scr_buffer[0]);
	free(scr_buffer[1]);
 	free(text_buf);

	return FALSE;
}

