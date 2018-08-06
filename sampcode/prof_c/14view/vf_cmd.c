/*
 *	vf_cmd -- ViewFile command processor
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <local\std.h>
#include <local\keydefs.h>
#include <local\video.h>
#include "vf.h"
#include "message.h"

extern unsigned char Attr;

int
vf_cmd(fp, fname, numbers)
FILE *fp;
char *fname;
BOOLEAN numbers;
{
	register int i;		/* general index */
	unsigned int offset;	/* horizontal scroll offset */
	unsigned int n;		/* relative line number */
	int memerr;		/* flag for memory allocation errors */
	char *s, lbuf[MAXLINE];	/* input line buffer and pointer */
	int k;			/* key code (see keydefs.h) */
	int radix = 10;		/* base for number-to-character conversions */
	char number[17];	/* buffer for conversions */
	int errcount = 0;	/* error counter */
	DNODE *tmp;		/* pointer to buffer control nodes */
	DIRECTION srchdir;	/* search direction */
	char *ss;		/* pointer to search string */
	static char srchstr[MAXSTR] = { "" };	/* search string buffer */
	DNODE *head;		/* pointer to starting node of text buffer list */
	DNODE *current;		/* pointer to the current node (text line) */
	static DNODE *freelist;	/* pointer to starting node of "free" list */
				/* initialized to 0 at runtime; retains value */

	/* function prototypes */
	static void prtshift(int, int);
	extern DNODE *vf_mklst();
	extern DNODE *vf_alloc(int);
	extern DNODE *vf_ins(DNODE *, DNODE *);
	extern DNODE *vf_del(DNODE *, DNODE *);
	extern DNODE *search(DNODE *, DIRECTION, char *);
	extern DNODE *gotoln(DNODE *);
	extern char *getxline(char *, int, FILE *);
	extern char *getsstr(char *);
	extern int clrscrn(unsigned char);
	extern void showhelp(unsigned char);
	extern void clrmsg();
	extern void vf_dspy(DNODE *, DNODE *, int, BOOLEAN);
	extern int putstr(char *, int);
	extern int writec(char, int, int);
	extern char *nlerase(char *);

	/* display the file name */
	offset = 0;
	putcur(HEADROW, 0, Vpage);
	writec(' ', Maxcol[Vmode], Vpage);
	putstr("File: ", Vpage);
	putstr(fname, Vpage);

	/* establish the text buffer */
	memerr = 0;
	if ((head = vf_mklst()) == NULL)
		++memerr;
	if (freelist == NULL && (freelist = vf_alloc(N_NODES)) == NULL)
		++memerr;
	if (memerr) {
		clean();
		fprintf(stderr, "Memory allocation error\n");
		exit(1);
	}

	/* read the file into the buffer */
	current = head;
	n = 0;
	while ((s = getxline(lbuf, MAXLINE, fp)) != NULL) {
		/* add a node to the list */
		if ((freelist = vf_ins(current, freelist)) == NULL)
			++memerr;
		current = current->d_next;

		/* save the received text in a line buffer */
		if ((current->d_line = strdup(nlerase(s))) == NULL)
			++memerr;
		if (memerr) {
			clean();
			fprintf(stderr, "File too big to load\n");
			exit(1);
		}
		current->d_lnum = ++n;
		current->d_flags = 0;
	}

	/* show the file size as a count of lines */
	putstr(" (", Vpage);
	putstr(itoa(current->d_lnum, number, radix), Vpage);
	putstr(" lines)", Vpage);
	prtshift(offset, Vpage);
	current = head->d_next;
	vf_dspy(head, current, offset, numbers);

	/* process user commands */
	while ((k = getkey()) != K_ESC) {
		clrmsg();
		switch (k) {
		case 'b':
		case 'B':
		case K_HOME:
			current = head->d_next;
			break;
		case 'e':
		case 'E':
		case K_END:
			current = head->d_prev;
			i = NROWS - 1;
			while (i-- > 0)
				if (current->d_prev != head->d_next)
					current = current->d_prev;
			break;
		case K_PGUP:
		case 'u':
		case 'U':
			i = NROWS - OVERLAP;
			while (i-- > 0)
				if (current != head->d_next)
					current = current->d_prev;
			break;
		case K_PGDN:
		case 'd':
		case 'D':
			i = NROWS - OVERLAP;
			while (i-- > 0)
				if (current != head->d_prev)
					current = current->d_next;
			break;
		case K_UP:
		case '-':
			if (current == head->d_next)
				continue;
			current = current->d_prev;
			break;
		case K_DOWN:
		case '+':
			if (current == head->d_prev)
				continue;
			current = current->d_next;
			break;
		case K_RIGHT:
		case '>':
		case '.':
			if (offset < MAXLINE - SHIFTWIDTH)
				offset += SHIFTWIDTH;
			prtshift(offset, Vpage);
			break;
		case K_LEFT:
		case '<':
		case ',':
			if ((offset -= SHIFTWIDTH) < 0)
				offset = 0;
			prtshift(offset, Vpage);
			break;
		case K_ALTG:
		case 'g':
		case 'G':
			if ((tmp = gotoln(head)) == NULL)
				continue;
			current = tmp;
			break;
		case K_ALTH:
		case 'h':
		case 'H':
		case '?':
			showhelp(Attr);
			break;
		case K_ALTN:
		case 'n':
		case 'N':
			numbers = (numbers == TRUE) ? FALSE : TRUE;
			break;
		case K_ALTQ:
		case 'q':
		case 'Q':
			clrscrn(Attr);
			putcur(0, 0, Vpage);
			return (-1);
		case 'r':
		case 'R':
		case '\\':
			srchdir = BACKWARD;
			ss = getsstr(srchstr);
			if (ss == NULL)
				/* cancel search */
				break;
			if (strlen(ss) > 0)
				strcpy(srchstr, ss);
			if ((tmp = search(current, srchdir, srchstr)) == NULL)
				continue;
			current = tmp;
			break;
		case 's':
		case 'S':
		case '/':
			srchdir = FORWARD;
			ss = getsstr(srchstr);
			if (ss == NULL)
				/* cancel search */
				break;
			if (strlen(ss) > 0)
				strcpy(srchstr, ss);
			if ((tmp = search(current, srchdir, srchstr)) == NULL)
				continue;
			current = tmp;
			break;
		default:
			/* ignore all other keys */
			continue;
		}
		vf_dspy(head, current, offset, numbers);
	}
	clrmsg();

	/* release the allocated text buffer memory */
	while (head->d_next != head) {
		/* release text buffer */
		free(head->d_next->d_line);

		/* put node back on the freelist */
		freelist = vf_del(head->d_next, freelist);
	}
	/* release the list header node */
	free((char *)head);

	return (errcount);
}

/*
 *	prtshift -- display the number of columns of horizontal shift
 */

#define SHFTDSP	5

static void
prtshift(amt, pg)
int amt, pg;
{
	char number[17];
	int radix = 10;

	/* clear the shift display area */
	putcur(1, Maxcol[Vmode] - 1 - SHFTDSP, pg);
	writec(' ', SHFTDSP, pg);

	/* display the new shift amount, if any */
	if (amt > 0) {
		putstr(itoa(amt, number, radix), pg);
		putstr("->", pg);
	}
}
