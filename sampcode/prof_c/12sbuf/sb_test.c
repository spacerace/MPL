/*
 *	sb_test -- driver for screen-buffer interface functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <local\std.h>
#include <local\keydefs.h>
#include <local\sbuf.h>
#include <local\video.h>
#include <local\box.h>
#include "sb_test.h"

#define BEL 7

extern struct BUFFER Sbuf;

main(argc, argv)
int argc;
char *argv[];
{
	char *s, line[MAXLINE];
	int k;
	short i;
	FILE *fp;
	char fname[MAXPATH];
	struct REGION *cmnd, *stat, *text, *help, *curwin;
	unsigned char cmndattr, statattr, textattr, helpattr, curattr;
	unsigned char ch, userattr;

	/* function prototypes */
	int sb_init();
	int sb_move(struct REGION *, short, short);
	struct REGION *sb_new(short, short, short, short);
	int sb_putc(struct REGION *, unsigned char);
	int sb_puts(struct REGION *, char *);
	int sb_show(short);
	int sb_fill(struct REGION *, unsigned char, unsigned char);
	char *get_fname(struct REGION *, char *, short);

	getstate();
	readca(&ch, &userattr, Vpage);

	/* set up the screen buffer */
	if (sb_init() == SB_ERR) {
		fprintf(stderr, "Bad UPDATEMODE value in environment\n");
		exit(1);
	}

	/* set up windows and scrolling regions */
	cmnd = sb_new(CMND_ROW, CMND_COL, CMND_HT, CMND_WID);
	stat = sb_new(STAT_ROW, STAT_COL, STAT_HT, STAT_WID);
	text = sb_new(TEXT_ROW, TEXT_COL, TEXT_HT, TEXT_WID);
	help = sb_new(HELP_ROW, HELP_COL, HELP_HT, HELP_WID);
	text->wflags |= SB_SCROLL;
	sb_set_scrl(help, 1, 1, HELP_HT - 1, HELP_WID - 1);

	/* display each primary window in its own attribute */
	cmndattr = GRN;
	statattr = (WHT << 4) | BLK;
	textattr = (BLU << 4) | CYAN;
	helpattr = (GRN << 4) | YEL;
	sb_fill(cmnd, ' ', cmndattr);
	if (sb_move(cmnd, 0, 0) == SB_OK)
		sb_puts(cmnd, "SB_TEST (Version 1.0)");
	sb_fill(stat, ' ', statattr);
	if (sb_move(stat, 0, 0) == SB_OK)
		sb_puts(stat, "*** STATUS AREA ***");
	for (i = 0; i <= text->r1 - text->r0; ++i) {
		sb_move(text, i, 0);
		sb_wca(text, i + 'a', textattr,
			text->c1 - text->c0 + 1);
	}
	if (sb_move(text, 10, 25) == SB_OK)
		sb_puts(text, " *** TEXT DISPLAY AREA *** ");
	sb_show(Vpage);
	curwin = text;
	curattr = textattr;

	/* respond to user commands */
	while ((k = getkey()) != K_ESC) {
		switch (k) {
		case K_UP:
			sb_scrl(curwin, 1);
			break;
		case K_DOWN:
			sb_scrl(curwin, -1);
			break;
		case K_PGUP:
			sb_scrl(curwin, curwin->sr1 - curwin->sr0);
			break;
		case K_PGDN:
			sb_scrl(curwin, -(curwin->sr1 - curwin->sr0));
			break;
		case K_ALTC:
			/* clear the current window */
			sb_fill(curwin, ' ', curattr);
			break;
		case K_ALTH:
			/* display help */
			curwin = help;
			curattr = helpattr;
			for (i = 0; i < help->r1 - help->r0; ++i) {
				sb_move(help, i, 0);
				sb_wca(help, i + 'a', helpattr,
					help->c1 - help->c0 + 1);
			}
			sb_box(help, BOXBLK, helpattr);
			break;
		case K_ALTS:
			/* fill the command area with letters */
			curwin = stat;
			curattr = statattr;
			sb_fill(stat, 's', statattr);
			break;
		case K_ALTT:
			/* fill the text area */
			curwin = text;
			curattr = textattr;
			for (i = 0; i <= text->r1 - text->r0; ++i) {
				sb_move(text, i, 0);
				sb_wca(text, i + 'a', textattr,
					text->c1 - text->c0 + 1);
			}
			break;
		case K_ALTR:
			/* read a file into the current window */
			sb_fill(stat, ' ', statattr);
			sb_move(stat, 0, 0);
			sb_puts(stat, "File to read: ");
			sb_show(Vpage);
			(void)get_fname(stat, fname, MAXPATH);
			if ((fp = fopen(fname, "r")) == NULL) {
				sb_fill(stat, ' ', statattr);
				sb_move(stat, 0, 0);
				sb_puts(stat, "Cannot open ");
				sb_puts(stat, fname);
			}
			else {
				sb_fill(stat, ' ', statattr);
				sb_move(stat, 0, 0);
				sb_puts(stat, "File: ");
				sb_puts(stat, fname);
				sb_show(Vpage);
				sb_fill(text, ' ', textattr);
				sb_move(text, 0, 0);
				putcur(text->r0, text->c0, Vpage);
				while ((s = fgets(line, MAXLINE, fp)) != NULL) {
					if (sb_puts(text, s) == SB_ERR) {
						clrscrn(userattr);
						putcur(0, 0, Vpage);
						fprintf(stderr, "puts error\n");
						exit(1);
					}
					sb_show(Vpage);
				}
				if (ferror(fp)) {
					putcur(text->r0, text->c0, Vpage);
					fprintf(stderr, "Error reading file\n");
				}
				fclose(fp);
			}
			break;
		default:
			/* say what? */
			fputc(BEL, stderr);
			continue;
		}
		if ((Sbuf.flags & SB_DELTA) == SB_DELTA)
			sb_show(Vpage);
	}

	clrscrn(userattr);
	putcur(0, 0, Vpage);
	exit(0);
}

/*
 *	get_fname -- get a filename from the user
 */

char *
get_fname(win, path, lim)
struct REGION *win;
char *path;
short lim;
{
	int ch;
	char *s;

	s = path;
	sb_show(Vpage);
	while ((ch = getch()) != K_RETURN) {
		if (ch == '\b')
			--s;
		else {
			sb_putc(win, ch);
			*s++ = ch;
		}
		sb_show(Vpage);
	}
	*s = '\0';
	return (path);
}
