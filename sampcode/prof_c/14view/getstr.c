/*
 *	getstr -- get a string from the keyboard
 */

#include <stdio.h>
#include <string.h>
#include <local\std.h>
#include <local\video.h>
#include <local\keydefs.h>

char *
getstr(buf, width)
char *buf;
int width;
{
	int row, col;
	char *cp;

	/* function prototypes */
	extern int putcur(int, int, int);
	extern int readcur(int *, int *, int);
	extern int writec(char, int, int);
	extern int getkey();

	/* gather keyboard input into a string buffer */
	cp = buf;
	while ((*cp = getkey()) != K_RETURN && cp - buf < width) {
		switch (*cp) {
		case K_CTRLH:
			/* destructive backspace */
			if (cp > buf) {
				readcur(&row, &col, Vpage);
				putcur(row, col - 1, Vpage);
				writec(' ', 1, Vpage);
				--cp;
			}
			continue;
		case K_ESC:
			/* cancel string input operation */
			return (char *)NULL;
		}
		put_ch(*cp, Vpage);
		++cp;
	}
	*cp = '\0';
	return (buf);
}
