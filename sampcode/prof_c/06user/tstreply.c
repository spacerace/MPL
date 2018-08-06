/*
 *	tstreply -- test the getreply function
 */

#include <stdio.h>
#include <string.h>
#include <local\std.h>
#include <local\video.h>
#include "linebuf.h"

#define INPUT_ROW	0
#define INPUT_COL	40
#define WIDTH		40

int Apage = 0;
BOOLEAN Silent = FALSE;

main(argc, argv)
int argc;
char *argv[];
{
	unsigned int r, c, ch, attr, revattr;
	char reply[MAXPATH + 1];
	LINEBUF buf;

	extern char *getreply(short, short, short,
		char *, LINEBUF *, short, short, short);

	/* process command line */
	if (argc == 2 && strcmp(argv[1], "-s") == 0)
		Silent = TRUE;
	else if (argc > 2) {
		fprintf(stderr, "Usage: tstreply [-s]\n");
		exit(1);
	}

	/* initial setup */
	getstate();
	readca(&ch, &attr, Apage);
	revattr = ((attr << 4) | (attr >> 4)) & 0x77;
	clrscrn(attr);
	putcur(0, 0, Apage);
	writestr("TSTREPLY", Apage);
	putcur(1, 0, Apage);
	writec(HLINE, Maxcol[Vmode] - 1, Apage);
	buf.l_buf = reply;
	buf.l_next = buf.l_prev = (LINEBUF *)NULL;

	/* demo getreply() */
	
	if (getreply(INPUT_ROW, INPUT_COL, WIDTH, "File: ", &buf,
		MAXPATH, revattr, 0) == NULL) {
		putcur(INPUT_ROW, INPUT_COL, Apage);
		writeca(' ', attr, WIDTH, Apage);
		putcur(2, 0, Apage);
		fprintf(stderr, "input aborted\n");
		exit(1);
	}
	putcur(INPUT_ROW, INPUT_COL, Apage);
	writeca(' ', attr, WIDTH, Apage);
	putcur(2, 0, Apage);
	fprintf(stderr, "reply = %s\n", reply);
	exit(0);
}

#define MSG_ROW	24
#define MSG_COL	0

int
errmsg(mesg)
char *mesg;
{
	int n;
	extern void sound(unsigned int, float);

	putcur(MSG_ROW, MSG_COL, Apage);
	if ((n = strlen(mesg)) > 0) {
		writestr(mesg, Apage);
		if (Silent == FALSE)
			sound(100, 0.2);
	}
	else
		writec(' ', Maxcol[Vmode] - 1 - MSG_COL, Apage);
	return (n);
}
