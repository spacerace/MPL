/**************************************************************************
 *                                                                        *
 *                               P A I N T                                *
 *                                 V?.??                                  *
 *                                                                        *
 *                        By: Jim Hudson                                  *
 *                            4600 Hillsboro Ave No                       *
 *                            New Hope, MN    55428                       *
 *                            612-533-4450                                *
 *                                                                        *
 *   All contributions, comments, improvements, praise, or whatever       *
 *    will not be reported to the IRS and will, be greatly appreciated    *
 *                                                                        *
 **************************************************************************/


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define ON 		  1
#define OFF		  0

#define NO_MOUSE  0x80
#define NOT_MOVED 0
#define RIGHT     1
#define LEFT      2
#define UP	      3
#define DOWN      4

#define LEFTB	  1
#define RIGHTB	  2
#define CENTERB   4

#define XHOME	  320
#define YHOME	  170

#define TIMED_OUT  300
#define DELAY_TIME 1500		/* may need to be adjusted for processor speed */
#define STR_LENGTH 80
#define sqr(X) ((X) * (X))
#define round(x) (int)((x)<0?-floor(-(x)+.5):floor((x)+.5))

#define len(A,B) ((A > B) ? (A - B) : (B - A))

#define F11		0x2300		/* machine dependent  */
#define F12		0x2400

#define ENABLE 0x0f
#define INDEXREG 0x3ce
#define VALREG 0x3cf
#define OUTINDEX(index, val) {outport(INDEXREG, index); outport(VALREG, val);}
#define EGABASE 0xa0000000L

#include <dos.h>
#include <stdio.h>
#include <math.h>
#include <bios.h>
#include <conio.h>
#include <ctype.h>
#include <graphics.h>
#include <alloc.h>
#include <string.h>
#include <process.h>
#include <stdlib.h>
#include <dir.h>

extern int paint_help(void);
extern void lstyle(void);
void get_keyboard(void), text_mode(void), print_text(void), v_print_text(void);
void swap_page(void), save_pik(void), read_pik(void), get_scale(void);
void vector(void), set_fill(void), delete_area(void);
void xhair(int state), grid(int state);
void help(void), capture(void), uncapture(void), check_flags(void);
void p_message(char *str), xcircle(int state);
void fill_box(void), fill_circle(void);
void copy(void), move(void), save_pic(void), load_pic(void);
void new_color(void), xrectangle(void), xellipse(void);
void scr_restore(char *text), line_style(void);
int scr_save(void), get_string(char *text, char name[]);
void gprintf(int, int, char *fmt, ... );
void wcomp(unsigned size, char *ptr, FILE *fd);
void rcomp(unsigned size, char *ptr, FILE *fs);
void pointf(int x, int y);
void xline(int startx, int starty, int endx, int endy, int state);
void xline1(int startx, int starty, int endx, int endy, int state);
void xarc(int state);
extern void printscr(int reverse);
void center(double x[], double y[], double *xc, double *yc, double *r);

/* mouse stuff */

extern void cmouses(int *function, int *arg2, int *arg3, int *arg4);

void swap_mouse(void), set_limits(void);
void set_mouse_position(void), get_mouse_position(void);
void mouse_motion(char *dx, char *dy), wait_on(int button);
int r_button_pressed(void), l_button_pressed(void), mouse_reset(void);
int c_button_pressed(void);

char *color_num[] = {
		"Black",
		"Blue",
		"Green",
		"Cyan",
		"Red",
		"Magenta",
		"Brown",
		"Light Gray",
		"Dark Gray",
        "Light Blue",
		"Light Green",
		"Light Cyan",
		"Light Red",
		"Light Magenta",
		"Yellow",
		"White"
};
char *fill_num[] = {
		"Empty ",
		"Solid ",
		"Line ",
		"Light Slash ",
		"Heavy Slash ",
		"Back Slash ",
		"Lt Back Slash ",
		"Hatch ",
		"Heavy Hatch ",
		"Interleave ",
		"Wide Dot ",
		"Close Dot "
};
char *line_width[] = {
		"Narrow",
		"      ",
		"      ",
		"wide"
};

int bkcolor = LIGHTGRAY, cc = BLUE, fill_style = SOLID_FILL, msg_color;
int on_flag = FALSE, cap_flag = FALSE, first_point = TRUE, time_out = TIMED_OUT;
int first_flag = FALSE, second_flag = FALSE, kb_flag = TRUE, xhair_flag = FALSE;
int maxx = 635, maxy = 349, minx = 5, miny = 20, inc = 10, x, y;
int header = TRUE, page = 0, height, text_ok = 0, text_size = 5;
int startx = 0, starty = 0, endx = 0, endy = 0, gf = FALSE;
void *xhair_buf, *first, *second, *arc1;
unsigned size, c_size;
unsigned char *c_buffer;
struct textsettingstype oldtext;
char text_line[80], oldc;
int cb_flag = FALSE, width = 0;
double scale = 1.000;
int grid_flag = OFF, arc_mode = 0, arc_count = 0;

/* more mouse stuff */

int mouse_flag = FALSE;		/* assume on */
int function = 0, arg2 = 0, arg3 = 0, arg4 = 0;
char deltax, deltay;		/* make sure this is a signed value  */

/* This aspect ratio is preset because that's the way my monitor works   */
/* properly, 'getaspectratio(&xasp, &yasp)' is called to return 7750 and */
/* 10000 respectivly.  Either use the system call or adjust the xasp for */
/* your needs.														     */

int xasp = 7180, yasp = 10000;

union k {
	char c[2];
	int  i;
} key;

struct linesettingstype oldlinestyle;

void main()
{
	char info_line[120], temp[10], *info_ptr;
	register int i, oldx, oldy;
	int Gdriver = DETECT, Gmode;

	startx = starty = endx = endy = 0;

	if(registerfarbgidriver(EGAVGA_driver_far) < 0) exit(1);
	if(registerfarbgifont(small_font_far) < 0) exit(1);
	if(registerfarbgifont(triplex_font_far) < 0) exit(1);

	initgraph(&Gdriver, &Gmode, "");
	if(getgraphmode() != EGAHI) {
		restorecrtmode();
		printf("\n\nI'm sorry, but this program ONLY works with an EGA system\n\n");
		exit(0);
	}
    gettextsettings(&oldtext);
/*	getaspectratio(&xasp, &yasp);	*/
	height = textheight("H");
	oldx = x = XHOME;
	oldy = y = YHOME;
	setbkcolor(bkcolor);

	if(mouse_reset()) mouse_flag = NO_MOUSE;
	else {
		set_mouse_position();
        set_limits();
	}
	setcolor(LIGHTGREEN);	/* first point marker  */
	line(x-2, y, x, y-2);
	line(x, y-2, x+2, y);
	line(x+2, y, x, y+2);
	line(x, y+2, x-2, y);
	line(x-7, y, x-4, y);
	line(x+4, y, x+7, y);
	line(x, y-7, x, y-4);
	line(x, y+4, x, y+7);
	size = imagesize(x-7, y-7, x+7, y+7);
	first = malloc(size);
	getimage(x-7, y-7, x+7, y+7, first);
	putimage(x-7, y-7, first, XOR_PUT);

	line(x-3, y-3, x+3, y-3); 	/* second point marker */
	line(x+3, y-3, x+3, y+3);
	line(x+3, y+3, x-3, y+3);
	line(x-3, y+3, x-3, y-3);
	size = imagesize(x-3, y-3, x+3, y+3);
	second = malloc(size);
	getimage(x-3, y-3, x+3, y+3, second);
    putimage(x-3, y-3, second, XOR_PUT);

	line(x-4, y-3, x+4, y+3);
	line(x+4, y-3, x-4, y+3);
	size = imagesize(x-4, y-3, x+4, y+3);
	arc1 = malloc(size);
	getimage(x-4, y-3, x+4, y+3, arc1);
	putimage(x-4, y-3, arc1, XOR_PUT);

	setcolor(DARKGRAY);
	line(x - 5, y, x + 4, y);	/* cross hair marker  */
	line(x, y + 5, x, y - 4);
	size = imagesize(x-5, y-4, x+4, y+5);
	xhair_buf = malloc(size);
	getimage(x-5, y-4, x+4, y+5, xhair_buf);
    putimage(x-5, y-4, xhair_buf, XOR_PUT);

	while(TRUE){		  /* we exit this loop when we quit         */
		if(kb_flag) {     /* only paint the header if a key was hit */
			if(bkcolor == cc) cc = 0;
			if(header) {
				setcolor(bkcolor);
				setviewport(0, 0, maxx, 9, 1);
				clearviewport();
				if (bkcolor == BLACK        || bkcolor == BLUE     ||
					bkcolor == RED          || bkcolor == MAGENTA  ||
					bkcolor == BROWN        || bkcolor == DARKGRAY ||
					bkcolor == LIGHTBLUE    || bkcolor == LIGHTRED ||
					bkcolor == LIGHTMAGENTA || bkcolor == GREEN)
					msg_color = WHITE;
				else msg_color = BROWN;
				setcolor(msg_color);
				setviewport(0, 0, maxx, maxy, 1);

                /* build the information line, this is slower than just
				   addessing unique X coordinates but with all the variables
				   I think it comes out alittle better.  Notice the use
				   of 'stpcpy' - this updates the pointer where 'strcpy'
				   does not.
				*/

				info_ptr = info_line;
				info_ptr = stpcpy(info_ptr, "X: ");
				itoa(x, temp, 10);
				info_ptr = stpcpy(info_ptr, temp);
				info_ptr = stpcpy(info_ptr, " Y: ");
				itoa(y, temp, 10);
				info_ptr = stpcpy(info_ptr, temp);
				info_ptr = stpcpy(info_ptr, on_flag ? " Pen Down " : " Pen Up ");
				if(second_flag) info_ptr = stpcpy(info_ptr, "Second Point ");
				else if(first_flag) info_ptr = stpcpy(info_ptr, "First Point ");
				info_ptr = stpcpy(info_ptr, "Inc = ");
				itoa(inc, temp, 10);
				info_ptr = stpcpy(info_ptr, temp);
				if(mouse_flag != NO_MOUSE) {
					info_ptr = stpcpy(info_ptr, mouse_flag ? " Mouse Off" : " Mouse On");
				}
                strcpy(info_ptr, " Color: ");
				gprintf(5, 1, "%s", info_line);
				gprintf(580, 1, "Page %d", page+1);
				setcolor(cc);
				gprintf(textwidth(info_line), 1, "%s", color_num[cc]);
				if(!(arc_mode || text_ok)) p_message("  ");
				miny = 20;
				setcolor(msg_color);
				if(!(first_point || text_ok)) {
					gprintf(275, height+2,
						"Delta X %+6.3f Delta Y %+6.3f Delta XY %+6.3f",
						((x-startx)/66.000) * scale, ((y-starty)/47.000) * scale,
						hypot(((x-startx)/66.000) * scale, ((y-starty)/47.000) * scale));
				}
				setcolor(cc);
			} else miny = 10;
			setviewport(0, 0, maxx, maxy, 1);
			setcolor(cc);
			xhair(ON);
			kb_flag = FALSE;
			set_mouse_position();
        }
		if(bioskey(1)) get_keyboard();
		if(!mouse_flag) {				/* process the mouse movements */
        	/* if center button -- do last function  */
			if(c_button_pressed()) {
				wait_on(CENTERB);
				if(key.c[0] >= 'a' && key.c[0] <= 'z') {
					cb_flag = TRUE;
                    get_keyboard();
				}
			}
			/* right button defines endpoints  */
			if(r_button_pressed()) {
				if(first_point) {
					if(first_flag) check_flags();
					startx = x;
					starty = y;
					putimage(x-7, y-7, first, XOR_PUT);
					first_flag = TRUE;
				}
				else {
					if(second_flag) check_flags();
					endx = x;
					endy = y;
					putimage(x-3, y-3, second, XOR_PUT);
					second_flag = TRUE;
				}
				first_point = !first_point;
    			wait_on(RIGHTB);
			}
			oldx = x; oldy = y;
			mouse_motion(&deltax, &deltay);
			if(deltax || deltay) {
				xhair(OFF);
				get_mouse_position();
				if(l_button_pressed()) xline(oldx, oldy, x, y, 0);
				time_out = 0;
			} else time_out++;
			if(time_out == TIMED_OUT) {
				kb_flag = TRUE;
			}
			xhair(ON);
		}
	}
}

void get_keyboard(void)
{
		kb_flag = TRUE;				     /* make sure the header gets painted     */
		if(!cb_flag) key.i = bioskey(0); /* cb_flag says the center button on the */
		cb_flag = FALSE;                 /* mouse was hit - repeat last command   */
		xhair(OFF);
		if (!key.c[0]) switch(key.c[1]) {
			case 75:						/* left */
				if((x - inc) < minx) break;
				if (on_flag) xline(x, y, x-inc, y, 0);
				x -= inc;
				break;
			case 77:                        /* right */
				if((x + inc) > maxx) break;
				if (on_flag) xline(x, y, x+inc, y, 0);
				x += inc;
				break;
			case 72:                        /* up */
				if((y - inc) < miny) break;
				if (on_flag) xline(x, y, x, y-inc, 0);
				y -= inc;
				break;
			case 80:                        /* down */
				if((y + inc) > maxy) break;
				if (on_flag) xline(x, y, x, y+inc, 0);
				y += inc;
				break;
			case 71:                        /* up left */
				if((x - inc) < minx || (y - inc) < miny) break;
				if (on_flag) xline(x, y, x-inc, y-inc, 0);
				x -= inc; y -= inc;
				break;
			case 73:                        /* up right */
				if((x + inc) > maxx || (y - inc) < miny) break;
				if (on_flag) xline(x, y, x+inc, y-inc, 0);
				x += inc; y -= inc;
				break;
			case 79:                        /* down left */
				if((x - inc) < minx || (y + inc) > maxy) break;
				if (on_flag) xline(x, y, x-inc, y+inc, 0);
				x -= inc; y += inc;
				break;
			case 81:                        /* down right */
				if((x + inc) > maxx || (y + inc) > maxy) break;
				if (on_flag) xline(x, y, x+inc, y+inc, 0);
				x += inc; y += inc;
				break;
			case 59:				/* F1 */
				if(text_ok) {
					print_text();
					break;
				}
				inc = 1;
				break;
			case 60:				/* F2 */
				if(text_ok) {
					v_print_text();
					break;
				}
				inc = 5;
				break;
			case 61: inc = 10;      /* F3 */
				break;
			case 62: inc = 20;      /* F4 */
				break;
			case 63:                /* F5 - Change background color */
				bkcolor++;
				if(bkcolor >= MAXCOLORS) bkcolor = 0;
				setbkcolor(bkcolor);
				break;
			case 64:				/* F6 - Save an icon to a file */
				save_pik();
				break;
			case 65:  				/* F7 - Restore the icon from a file */
				read_pik();
				break;
			case 66:  				/* F8 - Clear the first and second flags */
				check_flags();
				break;
			case 67:                /* F9 - Turn info lines on/off */
				if(header) {
					p_message("WARNING --- ALL messages will be turned off");
					delay(DELAY_TIME);
					setcolor(bkcolor);
					setviewport(0, 0, maxx, 19, 1);
					clearviewport();
					setviewport(0, 0, maxx, maxy, 1);
					setcolor(cc);
				}
				header ^= 1;
				set_limits();
				break;
			case 68:                /* F10 - Turn mouse ON/OFF    */
				swap_mouse(); break;
			case (F11 >> 8):		/* F11 - Change FILL pattern  */
				set_fill(); break;
			case (F12 >> 8):		/* F12 - Change line style    */
				line_style(); break;
			case 83:  				/* DELETE key homes cursor    */
				x = XHOME; y = YHOME; break;
			case 119:
				x = 10; y = miny + 10; break;
			case -124:
				x = 630; y = miny + 10; break;
			case 115:
				x = 10; break;
			case 116:
				x = 630; break;
			case 117:
				x = 10; y = 340; break;
			case 118:
				x = 630; y = 340; break;
            }
		else if(!text_ok) switch(tolower(key.c[0])) {
			case '1': cc = 1; break;
			case '2': cc = 2; break;
			case '3': cc = 3; break;
			case '4': cc = 4; break;
			case '5': cc = 5; break;
			case '6': cc = 6; break;
			case '7': cc = 7; break;
			case '8': cc = 8; break;
			case '9': cc = 9; break;
			case '0': cc = 10; break;
			case '!': cc = 11; break;
			case '@': cc = 12; break;
			case '#': cc = 13; break;
			case '$': cc = 14; break;
			case '%': cc = 15; break;
			case '?': help(); break;
            case 'a': xarc(FALSE); break;
			case 'b': xrectangle(); break;
			case 'c': xcircle(TRUE); break;
            case 'd': delete_area(); break;
			case 'e': xellipse(); break;
			case 'f': break;
			case 'g': grid_flag ? grid(OFF) : grid(ON); break;
			case 'h': break;
			case 'j': capture(); break;
			case 'k': uncapture(); break;
			case 'l': xline(startx, starty, endx, endy, 1); break;
            case 'm': move(); break;
			case 'n': new_color(); break;
			case 'o': on_flag = !on_flag; break;
			case 'p': swap_page(); break;
			case 'p' - 0x60: printscr(0); break;
			case 'q':
            	p_message("Do you really want to quit [Y/N]");
				key.i = bioskey(0);
				if(tolower(key.c[0]) == 'y') {
					closegraph();
					restorecrtmode();
					/* free any allocated buffers */
					free(first); free(second); free(xhair_buf);
					free(c_buffer);
                	exit(0);
				}
				break;
			case 'r': load_pic(); break;
			case 's': check_flags(); save_pic(); break;
			case 's' - 0x60: get_scale(); break;
			case 't': text_mode(); break;
			case 'w':
				setfillstyle(fill_style, cc);
				floodfill(x, y, cc);
				break;
			case 'v': vector(); break;
            case 'x': copy(); break;
			case 'z':
				p_message("Attempt to clear display, Hit \"z\" again");
				key.i = bioskey(0);
				if(tolower(key.c[0]) == 'z') {
					if(grid_flag) {
						grid(OFF);
						gf = TRUE;
					} else gf = FALSE;
					check_flags();
					clearviewport();
					x = XHOME;
					y = YHOME;
					if(gf) grid(ON);
				}
				break;
			case '\r':
				if(arc_mode) {
					xarc(TRUE);
					break;
				}
				if(first_point) {
					if(first_flag) check_flags();
					startx = x;
					starty = y;
					putimage(x-7, y-7, first, XOR_PUT);
					first_flag = TRUE;
				}
				else {
					if(second_flag) check_flags();
					endx = x;
					endy = y;
					putimage(x-3, y-3, second, XOR_PUT);
					second_flag = TRUE;
				}
				first_point = !first_point;
				break;
            }
}

/* change the screen scale for the Delta X, Delta Y  display */
void get_scale(void)
{
	char temp[10], sc_string[80], *st_ptr;
    int gf = FALSE;

	p_message("                                                ");
	st_ptr = sc_string;
	st_ptr = stpcpy(st_ptr, "Enter new screen scale <CR> to abort [");
	sprintf(temp, "%2.4lf", scale);
	st_ptr = stpcpy(st_ptr, temp);
	strcpy(st_ptr, "] ");

	if(!get_string(sc_string, temp)) return;

	if(grid_flag) {
		grid(OFF);
		gf = TRUE;
	}
	sscanf(temp, "%lf", &scale);
	if(gf) grid(ON);
}

/* draw an ellipse after checking two endpoints were marked */
void xellipse(void)
{
	if(!(first_flag & second_flag)) {
		p_message("Mark first AND second points");
		delay(DELAY_TIME);
		return;
	}
	check_flags();
	ellipse(x, y, 0, 360, len(startx,endx) , len(starty,endy));

}
/* draw a retangle after checking two endpoints were marked */
void xrectangle(void)
{
	if(!(first_flag & second_flag)) {
		p_message("Mark first AND second points");
		delay(DELAY_TIME);
		return;
	}
	check_flags();

	xline(startx, starty, endx,   starty, 0);
	xline(endx,   starty, endx,   endy,   0);
	xline(endx,   endy,   startx, endy,   0);
	xline(startx, endy,   startx, starty, 0);
}

/* draw the normalized circle */

void xcircle(int state)
{
	long dx, dy;

	if(state) {
		if(!(first_flag & second_flag)) {
			p_message("Mark first AND second points");
			delay(DELAY_TIME);
			return;
		}
		check_flags();
	}
	dx = len(startx,endx);
    dy = len(starty,endy);

	if((!dx && !dy) || (dx && dy)) {
		p_message("Indicate only one axis");
		delay(DELAY_TIME);
		return;
	}
    if(dx) dy = (dx * (long)xasp) / (long)yasp;
	if(dy) dx = (dy * (long)yasp) / (long)xasp;
	ellipse(startx, starty, 0, 360, (int)dx, (int)dy);
}

/* save the display  */
void save_pic()
{
	char fname[STR_LENGTH];
	char ftemp[STR_LENGTH];
	char fback[STR_LENGTH];
	char dr[MAXDRIVE];
	char dir[MAXDIR];
	char fil[MAXFILE];
	char ext[MAXEXT];
	FILE *fp;
    void *buffer_ul, *buffer_ll;
	char *file_ptr;
	int ok_flag = TRUE, gf = FALSE;
	unsigned size_ul, size_ll;

	while(ok_flag) {
		if(!get_string("Enter filename: ",fname)) return; /* return if no keys were entered */

		fnsplit(fname, dr, dir, fil, ext);
		strcpy(ext, ".BAK");
		fnmerge(fback, dr, dir, fil, ext);
		strcpy(ext, ".$$$");
		fnmerge(ftemp, dr, dir, fil, ext);
		strcpy(ext, ".PIC");
		fnmerge(fname, dr, dir, fil, ext);

		if(!(fp = fopen(ftemp, "wb"))) {
			if(header) gprintf(320, height+2, "cannot open file %s", fname);
			delay(DELAY_TIME);
		}
		else ok_flag = FALSE;
	}
    p_message("Saving the screen to the file");
	if(grid_flag) {
		grid(OFF);
		gf = TRUE;
	}
	size_ul = imagesize(0, 0, maxx, ((maxy+1)/2)-1);
	size_ll = imagesize(0, (maxy+1)/2, maxx, maxy);

    buffer_ul = malloc(size_ul);
    buffer_ll = malloc(size_ll);

	if(buffer_ul == NULL || buffer_ll == NULL) {
		p_message("Out of memory");
		delay(DELAY_TIME);
		free(buffer_ul);
		free(buffer_ll);
		if(gf) grid(ON);
		return;
	}

	p_message("   ");
	getimage(0, 0, maxx, ((maxy+1)/2)-1, buffer_ul);
    p_message("Saving the screen to the file");
	getimage(0, (maxy+1)/2, maxx, maxy, buffer_ll);

	file_ptr = buffer_ul;
	wcomp(size_ul, file_ptr, fp);

	free(buffer_ul);

	file_ptr = buffer_ll;
	wcomp(size_ll, file_ptr, fp);

	free(buffer_ll);

	fputc(EOF, fp);
	fcloseall();
	unlink(fback);
	rename(fname, fback);
	rename(ftemp, fname);
	p_message("  ");
	if(gf) grid(ON);
}
/* read a picture file  */
void load_pic()
{
	char fname[STR_LENGTH], *fname_ptr;
	int ok_flag = TRUE;
	FILE *fp;
	char *buffer, *buf_ptr;
    unsigned size_buf, size_buf1;

	size_buf = imagesize(0, 0, maxx, ((maxy+1)/2)-1);
    buffer = malloc(size_buf);

	if(buffer == NULL) {
		p_message("Out of memory");
		delay(DELAY_TIME);
		free(buffer);
		return;
	}

	while(ok_flag) {
		if(!get_string("Enter filename: ",fname)) {
			free(buffer);
			return; /* return if no keys were entered */
		}
		fname_ptr = fname;

		while(*fname_ptr != '\0' && *fname_ptr != '.') fname_ptr++;
		strcpy(fname_ptr, ".PIC");

		if(!(fp = fopen(fname, "rb"))) {
			if(header) gprintf(320, height+2, "cannot open file %s", fname);
			delay(DELAY_TIME);
		}
		else ok_flag = FALSE;
	}
	setcolor(bkcolor);
	setviewport(0,0,639,20,1);
	clearviewport();
	setviewport(0,0,639,349,1);
	setcolor(cc);

    p_message("Reading the file");

	size_buf1 = size_buf;
	rcomp(size_buf1, buffer, fp);
	putimage(0, 0, buffer, XOR_PUT);
	size_buf1 = size_buf;
	rcomp(size_buf1, buffer, fp);
	putimage(0, (maxy+1)/2, buffer, XOR_PUT);

	free(buffer);
	fclose(fp);
}

/* move one region to another   */


/* be careful of start end points with this -- you can get a fold */


void move(void)
{
	register int i, j;
	int oldx, oldy, xx, yy, cx, cy, xdir, ydir, gf = FALSE;
	unsigned char c;

	if(!(first_flag & second_flag)) {
		p_message("Mark first AND second points");
		delay(DELAY_TIME);
		return;
	}
	check_flags();
    if(grid_flag) {
		grid(OFF);
		gf = TRUE;
	}

	oldx = x; oldy = y;

	xdir = x > max(startx, endx) ? 0 : 1;
	ydir = y > max(starty, endy) ? 0 : 1;

	cx = len(startx, endx) + 1;
	cy = len(starty, endy) + 1;


	for(i = 0, (xx = xdir ? min(startx,endx) : max(startx, endx));
		i < cx;
		i++, xdir ? xx++ : xx--, xdir ? x++ : x--) {

		for(j = 0, y = oldy, (yy = ydir ? min(starty,endy) : max(starty, endy));
			j < cy;
			j++, ydir ? yy++ : yy--, ydir ? y++ : y--) {

			c = getpixel(xx, yy);
			putpixel(xx, yy, 0);
			if(c) putpixel(x, y, c);
		}
	}
	x = oldx; y = oldy;
	if(gf) grid(ON);
}

/* copy one region to another - it's the same as move except it does not
	clear the original pixel */
void copy(void)
{
	register int i, j;
	int oldx, oldy, xx, yy, cx, cy, xdir, ydir, gf = FALSE;
	unsigned char c;

	if(!(first_flag & second_flag)) {
		p_message("Mark first AND second points");
		delay(DELAY_TIME);
		return;
	}
	check_flags();
    if(grid_flag) {
		grid(OFF);
		gf = TRUE;
	}

	oldx = x; oldy = y;

	xdir = x > max(startx, endx) ? 0 : 1;
	ydir = y > max(starty, endy) ? 0 : 1;

	cx = len(startx, endx) + 1;
	cy = len(starty, endy) + 1;

	for(i = 0, (xx = xdir ? min(startx,endx) : max(startx, endx));
		i < cx;
		i++, xdir ? xx++ : xx--, xdir ? x++ : x--) {

		for(j = 0, y = oldy, (yy = ydir ? min(starty,endy) : max(starty, endy));
			j < cy;
			j++, ydir ? yy++ : yy--, ydir ? y++ : y--) {

			c = getpixel(xx, yy);
			if(c) putpixel(x, y, c);
		}
	}
	x = oldx; y = oldy;
	if(gf) grid(ON);
}

void delete_area(void)
{
	register int i, j, cx, cy, xx, yy, gf = FALSE;

	if(!(first_flag & second_flag)) {
		p_message("Mark first AND second points");
		delay(DELAY_TIME);
		return;
	}
	check_flags();
    if(grid_flag) {
		grid(OFF);
		gf = TRUE;
	}

	for(i = 0, xx = min(startx, endx); i <= len(startx, endx); i++, xx++)
		for(j = 0, yy = min(starty, endy); j <= len(starty, endy) ; j++, yy++)
			putpixel(xx, yy, 0);
	if(gf) grid(ON);
}

/*
	GPRINTF: Used like PRINTF except the output is sent to the
	screen in graphics mode at the specified co-ordinate.
	NOTE: a string HAS to be defined as gprintf(x, y, "%s", "text")
*/

void gprintf( int xx, int yy, char *fmt, ... )
{
  va_list  argptr;			        /* Argument list pointer	    */
  char str[140];			        /* Buffer to build sting into	*/

  va_start( argptr, format );		/* Initialize va_ functions	    */

  vsprintf( str, fmt, argptr );	/* prints string to buffer	*/
  outtextxy( xx, yy, str );	        /* Send string in graphics mode */
  va_end( argptr );			        /* Close va_ functions		    */
}

/* print an informational message on the second line
   but only if HEADER is turned on
*/
void p_message(char *str)
{
	if(!header) return;

	setcolor(bkcolor);
	setviewport(0, height + 2, maxx, 19, 1);
	clearviewport();

	setcolor(msg_color);
	setviewport(0, 0, maxx, maxy, 1);
	gprintf(5, height + 2, "%s", str);
	setcolor(cc);
}

/* turn the first and second markers off.  Since the icon is XOR
   we have to first test if they are displayed
*/
void check_flags(void)
{
	if(first_flag) {
		first_flag = FALSE;
		putimage(startx-7, starty-7, first, XOR_PUT);
	}
	if(second_flag) {
		second_flag = FALSE;
		putimage(endx-3, endy-3, second, XOR_PUT);
	}
	first_point = TRUE;		/* reset to first point only */
}

/* save an area (icon) to memory */
void capture(void)
{
	register int i, j, c;
	int xx, yy, gf = FALSE;
	unsigned char *buf_ptr;
	unsigned long temp_size;
	union {
		int i;
		unsigned char c[2];
	} cx, cy;

	if(!(first_flag & second_flag)) {
		p_message("Mark first AND second points");
		delay(DELAY_TIME);
		return;
	}

	if(cap_flag) {
		p_message("There is an icon in memory.  Do you wish to overwrite it? [Y/N] ");
		key.i = bioskey(0);
		c = tolower(key.c[0]);
		if(c != 'y') return;
	}
	check_flags();
    if(grid_flag) {
		grid(OFF);
		gf = TRUE;
	}

	p_message("  ");

	cx.i = len(startx, endx) + 1;
	cy.i = len(starty, endy) + 1;

	temp_size = (long)cx.i * (long)cy.i;
	if(temp_size > 64000L) {
		p_message("Icon is too big");
		delay(DELAY_TIME);
		return;
	}
	c_size = cx.i * cy.i;
	if(cap_flag) free(c_buffer);
    c_buffer = malloc(c_size + 4);

	if(c_buffer == NULL) {
		p_message("Out of memory");
		delay(DELAY_TIME);
		free(c_buffer);
		return;
	}

	buf_ptr = c_buffer;
	*buf_ptr++ = cx.c[0];
	*buf_ptr++ = cx.c[1];
	*buf_ptr++ = cy.c[0];
	*buf_ptr++ = cy.c[1];

	for(i = 0, xx = min(startx, endx); i < cx.i; i++, xx++) {
		for(j = 0, yy = min(starty, endy); j < cy.i; j++, yy++) {
			c = getpixel(xx, yy);
			*buf_ptr++ = c;
		}
	}
	cap_flag = TRUE;
	if(gf) grid(ON);
}

/* display the saved icon */
void uncapture(void)
{
	register int i, j, c;
	int xx, yy;
	unsigned char *buf_ptr;
	union {
		int i;
		unsigned char c[2];
	} cx, cy;

	if(!cap_flag) {
		p_message("There is NO captured picture - use the \"J\" command first");
		delay(DELAY_TIME);
        return;
	}
	buf_ptr = c_buffer;
	cx.c[0] = *buf_ptr++;
	cx.c[1] = *buf_ptr++;
	cy.c[0] = *buf_ptr++;
	cy.c[1] = *buf_ptr++;

	for(i = 0, xx = x; i < cx.i; i++, xx++) {
		for(j = 0, yy=y; j < cy.i; j++, yy++) {
			c = *buf_ptr++;
			if(c != 0 && c != bkcolor) putpixel(xx, yy, c);
		}
	}
}

/* why are you reading this comment */
void help(void)
{
	if(paint_help()) {
		settextjustify(oldtext.horiz, oldtext.vert);
		settextstyle(oldtext.font, oldtext.direction, oldtext.charsize);
		p_message("Out of memory");
		delay(DELAY_TIME);
		return;
	}
}

/* change all occurences in a specified area to a new color  */
void new_color(void)
{
	unsigned char from, to, cx, cy;
	register int i, j, c, xx, yy;

	if(!(first_flag & second_flag)) {
		p_message("Mark first AND second points");
		delay(DELAY_TIME);
		return;
	}
	check_flags();

	p_message("Enter the FROM color number");
	key.i = bioskey(0);
	from = key.c[0];
	switch(from) {
		case '!': from = 11;
			      break;
		case '@': from = 12;
			      break;
		case '#': from = 13;
			      break;
		case '$': from = 14;
			      break;
		case '%': from = 15;
			      break;
		case '0': from = 10;
			      break;
		default:  from -= '0';
			      break;
	}
	p_message("Enter the TO color number");
	key.i = bioskey(0);
	to = key.c[0];
	switch(to) {
		case '!': to = 11;
			      break;
		case '@': to = 12;
			      break;
		case '#': to = 13;
			      break;
		case '$': to = 14;
			      break;
		case '%': to = 15;
			      break;
		case '0': to = 10;
			      break;
		default:  to -= '0';
			      break;
	}
	if( to == bkcolor) to = 0;
	p_message("Changing colors");

	for(i = 0, xx = min(startx, endx); i <= len(startx, endx); i++, xx++) {
		for(j = 0, yy = min(starty, endy); j <= len(starty, endy) ; j++, yy++) {
			c = getpixel(xx, yy);
			if( c == from) putpixel(xx, yy, to);
		}
	}
}

/* jump to the nearest pixel of thesame color as the drawing color  */
void vector(void)
{
	register int i, xx, yy, z;
	int f0 = 0;

	xx = x;
	yy = y;

	p_message("Searching - please wait (Hit any key to abort)");
	if(x > 0 && x < maxx && y > miny && y < maxy)
		if(getpixel(x, y) == cc) return;

	i = inc;

	while(f0 != 15) {
		f0 = 0;
		z = 2*i;
		while(z--) {
			x++;
			if(x == maxx) f0 &= 1;
			if(x < maxx) if(getpixel(x, y) == cc) return;
		}
		z = 2*i;
		while(z--) {
			y++;
			if(y == maxy) f0 &= 2;
			if(y < maxy) if(getpixel(x, y) == cc) return;
		}
		z = 2*i;
		while(z--) {
			x--;
			if(x == 0) f0 &= 4;
			if(x > minx) if(getpixel(x, y) == cc) return;
		}
		z = 2*i;
		while(z--) {
			y--;
			if(y == miny) f0 &= 8;
			if(y > miny) if(getpixel(x, y) == cc) return;
		}
		x -= inc; y -= inc; i += inc;
		if(bioskey(1)) {
			bioskey(0);				/* get the character  */
			x = xx; y = yy;
			return;
		}
	}
	x = xx; y = yy;
}

/* change visual and active pages  */
void swap_page(void)
{
	check_flags();
	page ^= 1;
	setvisualpage(page);
	setactivepage(page);

}

/* turn cross hairs on, but only if they were off */
/* turn cross hairs off, but only if they were on */
void xhair(int state)
{
	if((state && !xhair_flag) || (!state && xhair_flag)) {
    	putimage(x-5, y-4, xhair_buf, XOR_PUT);
		xhair_flag ^= 1;
	}
}

/* step through the 12 fill patterns   */
void set_fill(void)
{
	int length;
    static char *fill_text = "Current fill pattern: ";

	if(!header) return;						/* only if we are looking at it */
	setlinestyle(0, 0, 0);
	key.i = 0;
	setviewport(0, 0, maxx, 2 * height, 1);
	do {
		clearviewport();
        gprintf(0, (height/2)-1, "%s%s", fill_text, fill_num[fill_style]);
        length = textwidth(fill_text) + textwidth(fill_num[fill_style]);
		setfillstyle(fill_style, cc);
		rectangle(length, 0, length+50, height*2);
		floodfill(length+1, 1, cc);
		gprintf(length + 58, (height/2)-1,"%s", "Hit <CR> for next -- F11 to return");
		key.i = bioskey(0);
		if(key.c[0] == 13) {
			fill_style++;
			if(fill_style == 12) fill_style = 0;
		}
	} while(key.i != F11); /* 68 = F10 */
	setlinestyle(0, 0, width);
	setviewport(0, 0, maxx, maxy, 1);
	setcolor(cc);
	setbkcolor(bkcolor);
}

void line_style(void)
{
	int length;
	static char *line_type = "Current line type: ";

	if(!header) return;
	setviewport(0, 0, maxx, 2 * height, 1);
	getlinesettings(&oldlinestyle);
	do {
		clearviewport();
        gprintf(0, (height/2)-1, "%s%s", line_type, line_width[width]);
		setlinestyle(oldlinestyle.linestyle, oldlinestyle.upattern, width);
		length = textwidth(line_type) + textwidth(line_width[width]);
		line(length + 8, height, length+58, height);
		gprintf(length + 66, (height/2)-1,"%s%s%s", "Hit <CR> for ",(width==0) ? "wide" : "narrow"," - F12 for style, or <ESC>");
		key.i = bioskey(0);
		if(key.c[0] == 0x1b) break;		/* abort */
		if(key.c[0] == 13) width ^= 3;
	} while (key.i != F12);
	setviewport(0, 0, maxx, maxy, 1);
	setcolor(cc);
	setbkcolor(bkcolor);

	if(key.c[0] != 0x1b) lstyle	();


}

/* save the previously defined icon to a file  */
void save_pik(void)
{
	char fname[STR_LENGTH], *fname_ptr;
	FILE *fp;
	unsigned char *file_ptr;
	unsigned pik_size;
	int ok_flag = TRUE;

	if(!cap_flag) {
		p_message("There is NO captured picture - use the \"J\" command first");
		delay(DELAY_TIME);
        return;
	}

	while(ok_flag) {
		if(!get_string("Enter filename: ",fname)) return; /* return if no keys were entered */
		fname_ptr = fname;

		while(*fname_ptr != '\0' && *fname_ptr != '.') fname_ptr++;
		strcpy(fname_ptr, ".PIK");

		if(!(fp = fopen(fname, "wb"))) printf("cannot open file %s\n", fname);
		else ok_flag = FALSE;
	}

	p_message("Saving the icon");

	file_ptr = c_buffer;
	pik_size = c_size + 4;	/* size of the saved image plus the x,y size */
	putw(pik_size, fp);		/* write the size of the picture first 		 */

	while(pik_size--) fputc(*file_ptr++, fp);

	putc(EOF, fp);			/* just in case  					   */
	fclose(fp);
}

/* read the icon file that has been saved, use 'K' to display it  */
void read_pik(void)
{
	char fname[STR_LENGTH], *fname_ptr,c;
	FILE *fp;
	unsigned char *file_ptr;
	unsigned pik_size;
	int ok_flag = TRUE;

	if(cap_flag) {
		p_message("There is an icon in memory.  Do you wish to overwrite it? [Y/N] ");
		key.i = bioskey(0);
		c = tolower(key.c[0]);
		if(c != 'y') return;
	}
    check_flags();
	p_message("  ");
	while(ok_flag) {
		fname_ptr = fname;
		if(!get_string("Enter filename: ",fname)) return; /* return if no keys were entered */
		while(*fname_ptr != '\0' && *fname_ptr != '.') fname_ptr++;
		strcpy(fname_ptr, ".PIK");

		if(!(fp = fopen(fname, "rb"))) printf("cannot open file %s\n", fname);
		else ok_flag = FALSE;
	}
	if(cap_flag) free(c_buffer);
	p_message("Reading icon");

	pik_size = getw(fp);	/* get the size of the picture first */

	c_size = pik_size-4;	/* global for 'what's there' 		   */
	c_buffer = malloc(pik_size);

	if(c_buffer == NULL) {
		p_message("Out of memory");
		delay(DELAY_TIME);
		free(c_buffer);
		return;
	}
	file_ptr = c_buffer;	/* preserve the pointer                */
	while(pik_size--) *file_ptr++ = fgetc(fp);
	cap_flag = TRUE;
	fclose(fp);
}


/* get a text string, displayed on line 2, return the character count
   or zero if <ESC> was hit
*/
int get_string(char *text, char name[])
{
	char *name_ptr;
	int j, count = 0;
	if(header) {
		setcolor(msg_color);
		gprintf(5, height+2, "%s", text);
    }
	name_ptr = name;
	j = textwidth(text);

	while(TRUE) {
		if(header) gprintf(j, height+2, "%c", '_');
		key.i = bioskey(0);
		if(key.c[0] == 13) break;
		if(key.c[0] == 27) {		/* provision for an abort */
			count = 0;
			break;
		}
		if(key.c[0] == 0x08) {	   /* backspace */
			setcolor(bkcolor);
			if(header) gprintf(j, height+2, "%c", 0xdb);
			j -=textwidth("H");
			if(header) {
				gprintf(j, height+2, "%c", 0xdb);
				setcolor(msg_color);
			}
			name_ptr--;
			count--;
			continue;
		}
		*name_ptr++ = key.c[0];
		if(header) {
			setcolor(bkcolor);
			gprintf(j, height+2, "%c", 0xdb);
			setcolor(msg_color);
			gprintf(j, height+2, "%c", key.c[0]);
		}
		j += textwidth("H");
		count++;
	}
	*name_ptr = '\0';
	setcolor(cc);
	return count;
}

void text_mode(void)
{

	char *text_ptr;
	int j, count = 0, ok = 0;
	char *msg1 = "Enter text size(1-9) [";
	char *msg2 = "Enter text: ";
	char *msg3 = "Locate cursor and hit <F1> for horizontal or <F2> for vertical";

	if(header) {
		setcolor(msg_color);
		gprintf(5, height+2, "%s%d] ", msg1, text_size);
    }
	while(!ok) {
		if(header) {
			setcolor(bkcolor);
			gprintf(textwidth(msg1)+textwidth("5] "), height+2, "%c", 0xdb);
			setcolor(msg_color);
			gprintf(textwidth(msg1)+textwidth("5] "), height+2, "%c", '_');
		}
        key.i = bioskey(0);
		if(key.c[0] == 13) break;
		if(key.c[0] == 27) {
			setcolor(cc);
			return;
		}
		text_size = key.c[0] - '0';
		if(text_size > 0 && text_size < 10) ok = 1;
	}
	if(header) {
		setcolor(bkcolor);
		gprintf(textwidth(msg1)+textwidth("5] "), height+2, "%c", 0xdb);
		setcolor(msg_color);
		gprintf(textwidth(msg1)+textwidth("5] "), height+2, "%d", text_size);
		delay(200);
		setcolor(bkcolor);
		setviewport(0, height+2, maxx, 20, 1);
		clearviewport();
		setviewport(0, 0, maxx, maxy, 1);
		setcolor(msg_color);
		outtextxy(5, height+2, msg2);
	}
	text_ptr = text_line;
	j = textwidth(msg2);

	while(TRUE) {
		if(header) gprintf(j, height+2, "%c", '_');
		key.i = bioskey(0);
		if(key.c[0] == 13) break;
		if(key.c[0] == 27) {		/* provision for an abort */
			count = 0;
			break;
		}
		if(key.c[0] == 0x08) {	   /* backspace */
			setcolor(bkcolor);
			if(header) gprintf(j, height+2, "%c", 0xdb);
			j -=textwidth("H");
			if(header) {
				gprintf(j, height+2, "%c", 0xdb);
				setcolor(msg_color);
			}
			text_ptr--;
			count--;
			continue;
		}
		*text_ptr++ = key.c[0];
		if(header) {
			setcolor(bkcolor);
			gprintf(j, height+2, "%c", 0xdb);
			setcolor(msg_color);
			gprintf(j, height+2, "%c", key.c[0]);
		}
		j += textwidth("H");
		count++;
	}
	*text_ptr = '\0';
    setcolor(cc);

	if(!count) return;
	text_ok = TRUE;
	p_message(msg3);
/*	delay(DELAY_TIME); */
	on_flag = FALSE;				/* make sure pen is up */
}

void print_text(void)
{
	text_ok = 0;
	settextstyle(SMALL_FONT, HORIZ_DIR, text_size);
	gprintf(x, y - textheight("H"), text_line);
	settextstyle(oldtext.font, oldtext.direction, oldtext.charsize);
}

void v_print_text(void)
{
	text_ok = 0;
	settextstyle(SMALL_FONT, VERT_DIR, text_size);
	gprintf(x, y - textwidth(text_line), text_line);
	settextstyle(oldtext.font, oldtext.direction, oldtext.charsize);
}


/********************************/
/*				    		    */
/*         MOUSE STUFF          */
/*				    		    */
/********************************/

/* turn mouse on/off, but only if it is present. Mouse reset
   determined if installed. NO_MOUSE indicates not present
*/
void swap_mouse(void)
{
	if(mouse_flag != NO_MOUSE) mouse_flag ^= 1;
}

int mouse_reset(void)
{
	function = 0;
	cmouses(&function, &arg2, &arg3, &arg4);

	if(function == 0) return TRUE;
	return FALSE;
}

/*return true if button is pressed, else zero  */
int r_button_pressed(void)
{
	function = 3;
	cmouses(&function, &arg2, &arg3, &arg4);
	return (arg2 & 2);
}
/*return true if button is pressed, else zero  */
int l_button_pressed(void)
{
	function = 3;
	cmouses(&function, &arg2, &arg3, &arg4);
	return (arg2 & 1);
}


/*return true if button is pressed, else zero  */
int c_button_pressed(void)
{
	function = 3;
	cmouses(&function, &arg2, &arg3, &arg4);
	return (arg2 & 4);
}

/* set mouse cursor coordinates, from global x,y (current pos)  */
void set_mouse_position(void)
{
	function = 4;
	cmouses(&function, &arg2, &x, &y);
}

/* return mouse cursor coordinates, from global x,y (current pos)  */
void get_mouse_position(void)
{
	function = 3;
	cmouses(&function, &arg2, &x, &y);
}

/* return direction of travel  */
void mouse_motion(char *dx, char *dy)
{
	int a3, a4;
	function = 11;
	cmouses(&function, &arg2, &a3, &a4);

	if(a3 > 0) *dy = DOWN;
	else if(a3 < 0) *dy = UP;
	else *dy = NOT_MOVED;

	if(a4 > 0) *dx = RIGHT;
	else if(a4 < 0) *dx = LEFT;
	else *dx = NOT_MOVED;
}

/* return TRUE when specified button is released  */
void wait_on(int button)
{
	switch(button) {
		case LEFTB: while(l_button_pressed());
					break;
		case RIGHTB: while(r_button_pressed());
					break;
		default: while(c_button_pressed());
	}
}

void set_limits(void)
{
	/* set X limit */

	function = 7;
	arg3 = 5;
	arg4 = 635;
	cmouses(&function, &arg2, &arg3, &arg4);

	/* set Y limit */

	function = 8;
	arg3 = header ? 20 : 5;
	arg4 = 345;
	cmouses(&function, &arg2, &arg3, &arg4);
}

#define INDICATE 0xd9

void wcomp(unsigned size, char *ptr, FILE *fd)
{
	unsigned char c, last, chr_cnt = 0;
	int flag = 0;

	c = *ptr++;
	fputc(c, fd);
	size--;

	while(size--) {
		last = c;
		c = *ptr++;

		if(c == INDICATE) {
			if(flag) fputc(chr_cnt, fd);
			chr_cnt = 0;
			flag = 0;
			fputc(c, fd);
			fputc(0, fd);
			continue;
		}

		if(c == last) {
			if(!flag) fputc(INDICATE, fd);
			flag = 1;
			chr_cnt++;
			if(chr_cnt == 0xff) {
				fputc(chr_cnt, fd);
				fputc(c, fd);
				c = !c;
				chr_cnt = 0;
				flag = 0;
			}
			continue;
		}
		if(flag) {
			fputc(chr_cnt, fd);
			chr_cnt = 0;
			flag = 0;
		}
		fputc(c, fd);
	}
	if(chr_cnt) fputc(chr_cnt, fd);

	return;

}

void rcomp(unsigned size, char *ptr, FILE *fs)
{
	unsigned char c, count, last = 0, count1 = 0;

	c = fgetc(fs);
	*ptr++ = c;
	size--;

	while(size > 0) {

		last = c;

		c = fgetc(fs);

		if(c == INDICATE) {
			count = fgetc(fs);
			count1 = count;
			if(count == 0) {
				*ptr++ = last;
				size--;
			}
			else while(count--) {
				*ptr++ = last;
				size--;
			}
			if(count1 == 0xff) {
				c =fgetc(fs);
				if(c != last) {
					*ptr++ = c;
					size--;
				}
			}
		} else {
			*ptr++ = c;
			size--;
		}
	}
	return;
}

void grid(int state)
{
	int oldcolor;
	double lpos, hpos, inc, sc_factor;
	register int i;

	if(!(state ^ grid_flag)) return;
	
	oldcolor = getcolor();
	sc_factor = scale;
	while((sc_factor < 1.000) || (sc_factor >= 10.000)) {
		if(sc_factor >= 10.000L) sc_factor /= 10.000L;
		else if(sc_factor < 1.000L)  sc_factor *= 10.000L;
	}
    if(sc_factor > 5.000L) sc_factor /= 2.000L;

	inc = 66.000L/sc_factor;

	OUTINDEX(0, WHITE);
	OUTINDEX(1, ENABLE);
	OUTINDEX(3, 0x18);

	for(hpos = 5.0;  hpos <= 635.0; hpos += inc)
		for(i = 20; i <= 345; i++) pointf((int)hpos, i);

	for(lpos = 20.0; lpos <= 345.0; lpos += (inc * xasp / yasp))
		for(i = 5; i <= 635; i++) pointf(i, (int)lpos);

	OUTINDEX(0, 0);
	OUTINDEX(1, 0);
	OUTINDEX(3, 0);
	OUTINDEX(8, 0xff);

	setcolor(oldcolor);
	grid_flag ^= 1;
}

void pointf(int x, int y)
{
	unsigned char mask = 0x80, exist_color;
	char far *base;

	base = (char *)(EGABASE + ((long)y * 80L + ((long)x/8L)));
	if(page) base += 0x8000L;
	mask >>= x % 8;
	exist_color = *base;
	OUTINDEX(8, mask);
	*base &= 0xff;
}
void xline(int startx, int starty, int endx, int endy, int state)
{
	xline1(startx, starty, endx, endy, state);
	if(width == 3) {
        if(!(endy - starty)) {
			xline1(startx, starty-1, endx, endy-1, 0);
			xline1(startx, starty+1, endx, endy+1, 0);
		} else if(!(endx - startx)) {
			xline1(startx-1, starty, endx-1, endy, 0);
			xline1(startx+1, starty, endx+1, endy, 0);
		} else if((endy - starty) <= (endx-startx)) {
			xline1(startx, starty-1, endx, endy-1, 0);
			xline1(startx, starty+1, endx, endy+1, 0);
		} else {
			xline1(startx, starty-1, endx, endy-1, 0);
			xline1(startx, starty+1, endx, endy+1, 0);
		}
    }
}
/* draw a line after checking two endpoints were marked -- if state is 1 */
void xline1(int startx, int starty, int endx, int endy, int state)
{
	register int t, distance;
	int xerr = 0, yerr = 0, delta_x, delta_y;
	int incx, incy, ccc;

    if(state) {
		if(!(first_flag & second_flag)) {
			p_message("Mark first AND second points");
			delay(DELAY_TIME);
			return;
		}
		check_flags();
	}

	delta_x = endx - startx;
	delta_y = endy - starty;

	if(delta_x > 0) incx = 1;
	else if(delta_x == 0) incx = 0;
	else incx = -1;

	if(delta_y > 0) incy = 1;
	else if(delta_y == 0) incy = 0;
	else incy = -1;

	delta_x = abs(delta_x);
	delta_y = abs(delta_y);
	distance = delta_x > delta_y ? delta_x : delta_y;

	for(t = 0; t <= distance+1; t++) {

/*        ccc = getpixel(startx, starty);
		if(ccc != cc) putpixel(startx, starty,(ccc ^ cc)); 
*/
		putpixel(startx, starty,cc);

		xerr += delta_x;
		yerr += delta_y;
		if(xerr > distance) {
			xerr -= distance;
			startx += incx;
		}
		if(yerr > distance) {
			yerr -= distance;
			starty += incy;
		}
	}
}

double xx[3], yy[3];



/* undocumented "A" function -- I am still experimenting to get curve fitting
   to work.   Screen aspect ratio is a b**ch.  This routine will paint a circle
   given three points on the circumference.  I can't get the sizing very good.
   From here one would arc from the angle of the first point to the angle of
   the third point.  The method used to get three points when we are used to
   inputting start,end stuff could be very useful for the ellipse and maybe
   some other things.
*/


void xarc(int state)
{
	double xc, yc, rad;
	int tempx1, tempy1;

    if(!state) {
		if(header) p_message("Use <CR> to mark FIRST POINT");
		arc_count = 1;
		arc_mode = TRUE;
	} else {
		switch(arc_count) {
		case 1:
			xx[0] = x;
			yy[0] = y;

			putimage(x-4, y-3, arc1, XOR_PUT);
			if(header) p_message("Use <CR> to mark SECOND POINT");
			break;
		case 2:
			xx[1] = x;
			yy[1] = y;
			putimage(x-3, y-3, second, XOR_PUT);
			if(header) p_message("Use <CR> to mark END point");
			break;
		case 3:
			xx[2] = x;
			yy[2] = y;

			arc_mode = FALSE;
			putimage(xx[1]-3, yy[1]-3, second, XOR_PUT);
			putimage(xx[0]-4, yy[0]-3, arc1, XOR_PUT);
/*
gprintf(10, 60, "xx = %d, %d, %d", xx[0], xx[1], xx[2]);
gprintf(10, 80, "yy = %d, %d, %d", yy[0], yy[1], yy[2]);
*/
			center(xx, yy, &xc, &yc, &rad);
		
			startx = xc;
			starty = yc;
			endx = xc + rad;
			endy = yc;
			xcircle(FALSE);

/*			circle(round(xc), round(yc), round(rad));


gprintf(10, 120, "xx = %d, %d, %d", xx[0], xx[1], xx[2]);
gprintf(10, 140, "yy = %d, %d, %d", yy[0], yy[1], yy[2]);
gprintf(10, 160, "xc = %d, yc = %d, rad = %d", round(xc), round(yc), round(rad));

*/
		}
		key.i = 0;
		arc_count += 1;
	}
}

void center(double x[], double y[], double *xc, double *yc, double *r)
{
	double x2[2], xh[2], y2[2], yk[2];
	double x2y2[2], xnyn, ykn;

	x2[0] = sqr(x[1]) - sqr(x[0]);
	xh[0] = 2.0  * x[1] - 2.0  * x[0];
	y2[0] = sqr(y[1]) - sqr(y[0]);
	yk[0] = 2.0  * y[1] - 2.0  * y[0];

	x2[1] = sqr(x[2]) - sqr(x[1]);
	xh[1] = 2.0  * x[2] - 2.0  * x[1];
	y2[1] = sqr(y[2]) - sqr(y[1]);
	yk[1] = 2.0  * y[2] - 2.0  * y[1];

	x2y2[0] = x2[0] + y2[0];
	x2y2[1] = x2[1] + y2[1];

	xnyn = x2y2[0];
	ykn = yk[0];

	x2y2[0] *= xh[1];
	yk[0] *= xh[1];

	x2y2[1] *= xh[0];
	yk[1] *= xh[0];

	*yc = (x2y2[0] - x2y2[1]) / (yk[0] - yk[1]);
	*xc = (xnyn - (*yc * ykn)) / xh[0];

	x[0] -= *xc;
	y[0] -= *yc;

	*r = hypot(x[0], y[0]);
}
