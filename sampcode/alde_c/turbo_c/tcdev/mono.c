/* 78,63 */
/* 07-14-87 00:06:06 clear the 25th line after scrolling. */
/* 07-13-87 23:58:36 handle output status. */
/* 07-13-87 23:53:31 position the cursor after writing. */
/* 07-13-87 23:44:11 store the correct attributes. */
#include <dos.h>
#include <string.h>
#include <mem.h>

struct srh {
	char len;
	char unit;
	char cmd;
	int status;
	char res[8];
	union {
		struct { /* init */
			char units;
			char far *kaddr;
			char far *baddr;
		} cin;
		struct { /* media check */
			char media, stat;
		} cmc;
		struct { /* build parameter block. */
			char media;
			char far *taddr;
			char far *baddr;
		} cbpb;
		struct { /* reading and writing */
			char media;
			char far *taddr;
			unsigned count;
			unsigned start;
		} crw;
		struct { /* nondestructive read no wait */
			char ch;
		} cic;
	} c;
};
extern char dev_name[9];
extern int dev_attr;


#define WHITE 0x700
int	row = 0, col = 0;

device_driver(struct srh far *request)
{
	char far *tptr;  /* pointer to the string of chars */
	int far *sptr;   /* pointer to the screen */
	unsigned count;
	extern char enddata;

	switch(request->cmd) {
	case  0: /* initialize */
		strcpy(&dev_name, "MON     ");
		dev_attr = 0x8000;
		request->c.cin.kaddr = (char far *) &enddata;
		request->status = 0x0100;  /* ready */
		break;
	case  8: /* output */
	case  9: /* output with verify */
		tptr = request->c.crw.taddr;
		sptr = (int far *) MK_FP(0xb000, (row*80 + col) <<1);
		count = request->c.crw.count;
		while (count--) switch(*tptr) {
			case 13:
				col = 0;
				sptr = (int far *) MK_FP(0xb000, (row*80 + col) <<1);
				break;
			case 10:
				if (++row > 24) {
					--row;
					movedata(0xb000, 160, 0xb000, 0, 24*80*2);
					sptr = (int far *) MK_FP(0xb000, (row*80 + col) <<1);
					{	/* clear the bottom line. */
						int count = 80;

						while (count--) *sptr++ = WHITE | ' ';
					}
				}
				sptr = (int far *) MK_FP(0xb000, (row*80 + col) <<1);
				break;
			case 8:
				if (col) {
					col--;
					sptr--;
					}
				break;
			case 9:
				if (col >= 80) break;
				while (++col & 7) *sptr++ = WHITE | ' ';
				break;
			default:
				if (col++ >= 80) break;
				*sptr++ = WHITE | *tptr++;
				break;
		}
		{
			int cursor_pos = FP_OFF(sptr) >> 1;
			disable();
			outportb(0x3b4, 14);
			outportb(0x3b5, cursor_pos >> 8);
			outportb(0x3b4, 15);
			outportb(0x3b5, cursor_pos & 0xff);
			enable();
		}
		request->status = 0x0100;  /* ready */
		break;

	case 10: /* output status */
	case 11: /* output buffer flush */
		request->status = 0x0100;  /* ready */
		break;
	/* these are cases that we don't handle ourselves */
	case  1: /* media check */
	case  2: /* build bpb */
	case  3: /* input ioctl */
	case  4: /* input */
	case  5: /* input check (buffer status) */
	case  6: /* input status */
	case  7: /* input flush */
	case 12: /* output ioctl */
	default:
		request->status = 0x8003;  /* unknown command */
		break;
	}
}
