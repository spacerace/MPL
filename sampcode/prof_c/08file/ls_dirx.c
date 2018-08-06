/*
 *	ls_dirx -- expand the contents of a directory using
 *	the DOS first/next matching file functions
 */

#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>
#include <direct.h>
#include <signal.h>
#include <search.h>
#include <local\std.h>
#include <local\doslib.h>
#include "ls.h"

#define NFILES	1024

extern int Recursive;
extern int Longlist;
extern int Multicol;
extern int Hidden;

int
ls_dirx(pname, namep)
char *pname;
char *namep;
{
	int status = 0;			/* function return value */
	int n;				/* number of items found */
	int fileattr;			/* attributes of file-matching */
	struct DTA buf;			/* disk transfer area */
	struct OUTBUF *bp, *bq;		/* output buffer pointers */
	char path[MAXPATH + 1];		/* working path string */

	extern void setdta(char *);
	extern int first_fm(char *, int);
	extern int next_fm();
	extern int ls_fcomp(struct OUTBUF *, struct OUTBUF *);
	extern char last_ch(char *);

	/* allocate a buffer */
	bp = bq = (struct OUTBUF *)malloc(NFILES * sizeof(struct OUTBUF));
	if (bp == NULL)
		fatal(pname, "Out of memory");

	/* form name for directory search */
	strcpy(path, namep);
	if (last_ch(path) != '\\')
		strcat(path, "\\");
	strcat(path, "*.*");

	/* list the files found */
	n = 0;
	/* establish a private DTA */
	setdta((char *)&buf);
	/* select file attributes */
	if (Hidden)
		fileattr = SUBDIR | HIDDEN | SYSTEM | READONLY;
	else
		fileattr = SUBDIR;
	if (first_fm(path, fileattr) == 0) {
		/* add file or directory to the buffer */
		do {
			if (!Hidden && buf.d_fname[0] == '.')
				continue;
			bq->o_name = strdup(buf.d_fname);
			bq->o_mode = buf.d_attr;
			bq->o_size = buf.d_fsize;
			bq->o_date = buf.d_mdate;
			bq->o_time = buf.d_mtime;
			++bq;
			++n;
			setdta((char *)&buf);	/* reset to our DTA */
		} while (next_fm() == 0);

		if (n > 0) {
			/* got some -- sort and list them */
			qsort(bp, n, sizeof(struct OUTBUF), ls_fcomp);
			if (Longlist)
				ls_long(bp, n);
			else if (Multicol)
				ls_multi(bp, n);
			else
				ls_single(bp, n);
		}
	}
	else 
		++status;
	free(bp);

	return (status);
}
