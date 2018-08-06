/*
 *	getreply -- display a message and wait for a reply
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <local\std.h>
#include <local\keydefs.h>
#include "linebuf.h"

char *
getreply(row, col, width, mesg, lp, size, attr, pg)
short row, col, width;	/* window location and width */
char *mesg;		/* message text */
LINEBUF *lp;		/* line pointer */
short size;		/* size of line buffer */
short attr;		/* video attribute for response field */
short pg;		/* active display page */
{
	int n, k, len;
	short mfw;	/* message field width */
	short rfw;	/* response field width */
	short ccol;	/* visible cursor column */
	int msgflag;	/* non-zero after a message is displayed */
	char *cp;	/* character pointer */
	char *wp;	/* pointer to window start */
	char *tmp;	/* temporary char pointer */

	extern int writemsg(short, short, short, char *, char *, short);

	/* display the prompt string and calculate response field width */
	putcur(row, col, pg);
	mfw = writemsg(row, col, width, mesg, NULL, pg);
	rfw = width - mfw;
	writea(attr, rfw, pg);

	/* collect the user's response */
	memset(lp->l_buf, '\0', size);
	wp = cp = lp->l_buf;
	putcur(row, col + mfw, pg);
	msgflag = 0;
	while ((k = getkey()) != K_RETURN) {
		if (msgflag) {
			/* clear old messages */
			errmsg("");
			putcur(row, ccol, pg);
			msgflag = 0;
		}
		if (isascii(k) && isprint(k)) {
			len = strlen(cp);
			if (cp + len - lp->l_buf < size - 1) {
				memcpy(cp + 1, cp, len);
				*cp = k;
				++cp;
			}
			else {
				errmsg("input buffer full");
				++msgflag;
			}
		}
		else
			switch (k) {
			case K_LEFT:
				/* move left one character */
				if (cp > lp->l_buf)
					--cp;
				break;
			case K_RIGHT:
				/* move right one character */
				if (*cp != '\0')
					++cp;
				break;
			case K_UP:
				/* pop a line off the stack */
				if (lp->l_prev != NULL) {
					lp = lp->l_prev;
					wp = cp = lp->l_buf;
				}
				break;
			case K_DOWN:
				/* push a line onto the stack */
				if (lp->l_next != NULL) {
					lp = lp->l_next;
					wp = cp = lp->l_buf;
				}
				break;
			case K_HOME:
				/* beginning of buffer */
				cp = lp->l_buf;
				break;
			case K_END:
				/* end of buffer */
				while (*cp != '\0')
					++cp;
				break;
			case K_CTRLH:
				if (cp > lp->l_buf) {
					tmp = cp - 1;
					memcpy(tmp, cp, strlen(tmp));
					--cp;
				}
				break;
			case K_DEL:
				/* delete character at cursor */
				memcpy(cp, cp + 1, strlen(cp));
				break;
			case K_ESC:
				/* cancel current input */
				lp->l_buf[0] = '\0';
				putcur(row, col, pg);
				writec(' ', width, pg);
				return (NULL);
			default:
				errmsg("unknown command");
				++msgflag;
				break;
			}

		/* adjust the window pointer if necessary */
		if (cp < wp)
			wp = cp;
		else if (cp >= wp + rfw)
			wp = cp + 1 - rfw;

		/* display the reply window */
		ccol = col + mfw;
		writemsg(row, ccol, rfw, wp, NULL, pg);

		/* reposition the cursor */
		ccol = col + mfw + (cp - wp);
		putcur(row, ccol, pg);
	}
	putcur(row, col, pg);
	writec(' ', width, pg);	/* blank message area */
	return (lp->l_buf);
}
