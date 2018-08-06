/*
 *	ls_list -- list functions (long, single, multi) for ls
 */

#include <stdio.h>
#include <string.h>
#include "ls.h"

#define MAXCOL		80
#define MONTH_SHIFT	5
#define MONTH_MASK	0x0F
#define DAY_MASK	0x1F
#define YEAR_SHIFT	9
#define DOS_EPOCH	80
#define HOUR_SHIFT	11
#define HOUR_MASK	0x1F
#define MINUTE_SHIFT	5
#define MINUTE_MASK	0x3F

extern int Filetype;

/*
 *	ls_long -- list items in "long" format (mode time size name)
 */

int
ls_long(buf, nelem)
struct OUTBUF *buf;
int nelem;
{
	int n = 0;
	char modebuf[5];
	static void modestr(unsigned short, char *);

	while (nelem-- > 0) {
		/* convert mode number to a string */
		modestr(buf->o_mode, modebuf);
		printf("%s ", modebuf);

		/* display file size in bytes */
		if ((buf->o_mode & SUBDIR) == SUBDIR)
			printf("        ");
		else
			printf("%7ld ", buf->o_size);

		/* convert date and time values to formatted presentation */
		printf("%02d-%02d-%02d ", (buf->o_date >> MONTH_SHIFT) & MONTH_MASK,
			buf->o_date & DAY_MASK, (buf->o_date >> YEAR_SHIFT) + DOS_EPOCH);
		printf("%02d:%02d ", (buf->o_time >> HOUR_SHIFT) & HOUR_MASK,
			(buf->o_time >> MINUTE_SHIFT) & MINUTE_MASK);

		/* display filenames as lowercase strings */
		printf("%s\n", strlwr(buf->o_name));

		++buf;
		++n;
	}

	/* tell caller how many entries were printed */
	return (n);
} /* end ls_long() */


/*
 *	ls_single -- list items in a single column
 */

int
ls_single(buf, nelem)
struct OUTBUF *buf;
int nelem;
{
	int n = 0;

	while (nelem-- > 0) {
		printf("%s", strlwr(buf->o_name));
		if (Filetype && (buf->o_mode & SUBDIR) == SUBDIR)
			putchar('\\');
		putchar('\n');
		++buf;
		++n;
	}

	/* tell caller how many entries were printed */
	return (n);
} /* end ls_single() */


/*
 *	ls_multi -- list items in multiple columns that
 *	vary in width and number based on longest item size
 */

int
ls_multi(buf, nelem)
struct OUTBUF *buf;
int nelem;
{
	int i, j;
	int errcount = 0;
	struct OUTBUF *tmp;	/* temporary buffer pointer */
	struct OUTBUF *base;	/* buffer pointer for multi-col output */
	int n;			/* number of items in list */
	int len, maxlen;	/* pathname lengths */
	int ncols;		/* number of columns to output */
	int nlines;		/* number of lines to output */

	/*
	 *  get length of longest pathname and calculate number
	 *  of columns and lines (col width = maxlen + 1)
	 */
	tmp = buf;
	n = 0;
	maxlen = 0;
	for (tmp = buf, n = 0; n < nelem; ++tmp, ++n)
		if ((len = strlen(tmp->o_name)) > maxlen)
			maxlen = len;
	/*
	 *  use width of screen - 1 to allow for newline at end of
	 *  line and leave two spaces between entries (one for optional
	 *  file type flag)
	 */
	ncols = (MAXCOL - 1) / (maxlen + 2);
	nlines = n / ncols;
	if (n % ncols)
		++nlines;

	/* output multi-column list */
	base = buf;
	for (i = 0; i < nlines; ++i) {
		tmp = base;
		for (j = 0; j < ncols; ++j) {
			len = maxlen + 2;
			len -= printf("%s", strlwr(tmp->o_name));
			if (Filetype && (tmp->o_mode & SUBDIR) == SUBDIR) {
				putchar('\\');
				--len;
			}
			while (len-- > 0)
				putchar(' ');
			tmp += nlines;
			if (tmp - buf >= nelem)
				break;
		}
		putchar('\n');
		++base;
	}

	return (errcount);
} /* end ls_multi() */


static void
modestr(mode, s)
unsigned short mode;	/* file mode number */
char s[];		/* mode string buffer */
{

	/* fill in the mode string to show what's set */
	s[0] = (mode & SUBDIR) == SUBDIR ? 'd' : '-';
	s[1] = (mode & HIDDEN) == HIDDEN ? 'h' : '-';
	s[2] = (mode & SYSTEM) == SYSTEM ? 's' : '-';
	s[3] = (mode & READONLY) == READONLY ? 'r' : '-';
	s[4] = '\0';
} /* end modestr() */
