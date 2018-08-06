/*
 *	ls -- display a directory listing
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <dos.h>
#include <direct.h>
#include <signal.h>
#include <search.h>
#include <local\std.h>
#include <local\doslib.h>
#include "ls.h"

/* allocation quantities */
#define N_FILES	256
#define N_DIRS	16

/* global data */
int Multicol = 0;
int Filetype = 0;
int Hidden = 0;
int Longlist = 0;
int Reverse = 0;
int Modtime = 0;

main(argc, argv)
int argc;
char *argv[];
{
	int ch, i;
	int errflag;		/* error flag */
	char *ep;		/* environment pointer */
	int status = 0;		/* return status value */
	int fileattr;		/* file attribute number */
	struct DTA buf;		/* private disk buffer */
	char path[MAXPATH + 1];	/* working pathname */
	struct OUTBUF *fp, *fq;	/* pointers to file array */
	char **dp, **dq;	/* pointer to directory pointer array */
	int fbc = 1;		/* file memory block allocation count */
	int dbc = 1;		/* directory memory block allocation count */
	int nfiles;		/* number of file elements */
	int ndirs;		/* number of directory elements */

	static char pgm[MAXNAME + 1] = { "ls" };

	/* function prototypes */
	void getpname(char *, char *);
	extern int getopt(int, char **, char *);
	extern int optind, opterr;
	extern char *optarg;
	extern char *drvpath(char *);
	extern void fatal(char *, char *, int);
	extern void setdta(char *);
	extern int first_fm(char *, int);
	extern int next_fm();
	extern int ls_fcomp(struct OUTBUF *, struct OUTBUF *);
	extern int ls_dcomp(char *, char *);
	extern int ls_single(struct OUTBUF *, int);
	extern int ls_multi(struct OUTBUF *, int);
	extern int ls_dirx(char *, char *);
	int bailout();

	/* guarantee that needed DOS services are available */
	if (_osmajor < 2)
		fatal(pgm, "ls requires DOS 2.00 or later", 1);

	/* get program name from DOS (version 3.00 and later) */
	if (_osmajor >= 3)
		getpname(*argv, pgm);

	/* useful aliases (DOS version 3.00 and later) */
	if (strcmp(pgm, "lc") == 0)
		++Multicol;
	if (strcmp(pgm, "lf") == 0) {
		++Multicol;
		++Filetype;
	}

	/* prepare for emergencies */
	if (signal(SIGINT, bailout) == (int(*)())-1) {
		perror("Can't set SIGINT");
		exit(2);
	}

	/* process optional arguments first */
	errflag = 0;
	while ((ch = getopt(argc, argv, "aCFlrt")) != EOF)
		switch (ch) {
		case 'a':
			/* all files (hidden, system, etc.) */
			++Hidden;
			break;
		case 'C':
			++Multicol;
			break;
		case 'F':
			/* show file types (/=directory, *=executable) */
			++Filetype;
			break;
		case 'l':
			/* long list (overrides multicolumn) */
			++Longlist;
			break;
		case 'r':
			/* reverse sort */
			++Reverse;
			break;
		case 't':
			/* sort by file modification time */
			++Modtime;
			break;
		case '?':
			errflag = TRUE;
			break;
		}
	argc -= optind;
	argv += optind;

	/* check for command-line errors */
	if (argc < 0 || errflag) {
		fprintf(stderr, "Usage: %s [-aCFlrt] [pathname ...]", pgm);
		exit(3);
	}

	/* allocate initial file and directory storage areas */
	dp = dq = (char **)malloc(N_DIRS * sizeof (char *));
	if (dp == NULL)
		fatal(pgm, "Out of memory", 4);
	fp = fq = (struct OUTBUF *)malloc(N_FILES * sizeof (struct OUTBUF));
	if (fp == NULL)
		fatal(pgm, "Out of memory", 4);
	nfiles = ndirs = 0;

	/* use current directory if no args */
	if (argc == 0) {
		if (getcwd(path, MAXPATH) == NULL)
			fatal(pgm, "Cannot get current directory", 5);
		*dq = path;
		ndirs = 1;
	}
	else {
		/* use arguments as file and directory names */
		for ( ; argc-- > 0; ++argv) {
			strcpy(path, *argv);
			if (path[0] == '\\') {
				/* prepend default drive name */
				memcpy(path + 2, path, strlen(path) + 1);
				path[0] = 'a' + getdrive();
				path[1] = ':';
			}
			if (path[1] == ':' && path[2] == '\0' && drvpath(path) == NULL) {
				fprintf(stderr, "%s: Cannot get drive path", pgm);
				continue;
			}

			/* establish private disk transfer area */
			setdta((char *)&buf);

			/* set file attribute for search */
			if (Hidden)
				fileattr = SUBDIR | HIDDEN | SYSTEM | READONLY;
			else
				fileattr = SUBDIR;
			if (first_fm(path, fileattr) != 0 && path[3] != '\0') {
				fprintf(stderr, "%s -- No such file or directory\n", path);
				continue;
			}
			if ((buf.d_attr & SUBDIR) == SUBDIR || path[3] == '\0') {
				/* path is a (sub)directory */
				*dq = strdup(path);
				if (++ndirs == dbc * N_DIRS) {
					++dbc;	/* increase space requirement */
					dp = (char **)realloc(dp, dbc * N_DIRS * sizeof (char *));
					if (dp == NULL)
						fatal(pgm, "Out of memory", 4);
					dq = dp + dbc * N_DIRS;
				}
				else
					++dq;
			}
			else {
				fq->o_name = strdup(path);
				fq->o_mode = buf.d_attr;
				fq->o_date = buf.d_mdate;
				fq->o_time = buf.d_mtime;
				fq->o_size = buf.d_fsize;
				if (++nfiles == fbc * N_FILES) {
					++fbc;
					fp = (struct OUTBUF *)realloc(fp, fbc * N_FILES * sizeof (struct OUTBUF));
					if (fp == NULL)
						fatal(pgm, "Out of memory", 4);
					fq = fp + fbc * N_FILES;
				}
				else
					++fq;
			}
		}
	}

	/* output file list, if any */
	if (nfiles > 0) {
		qsort(fp, nfiles, sizeof(struct OUTBUF), ls_fcomp);
		if (Longlist)
			ls_long(fp, nfiles);
		else if (Multicol)
			ls_multi(fp, nfiles);
		else
			ls_single(fp, nfiles);
		putchar('\n');
	}
	free(fp);
	 
	/* output directory lists, if any */
	if (ndirs == 1 && nfiles == 0) {
		/* expand directory and output without header */
		if (ls_dirx(pgm, *dp))
			fprintf(stderr, "%s -- empty directory\n", strlwr(*dp));
	}
	else if (ndirs > 0) {
		/* expand each directory and output with headers */
		dq = dp;
		qsort(dp, ndirs, sizeof(char *), ls_dcomp);
		while (ndirs-- > 0) {
			fprintf(stdout, "%s:\n", strlwr(*dq));
			if (ls_dirx(pgm, *dq++))
				fprintf(stderr, "%s -- empty directory\n",
					strlwr(*dq));
			putchar('\n');
		}
	}

	exit(0);
}

/*
 *	bailout -- optionally terminate upon interrupt
 */
int
bailout()
{
	char ch;

	signal(SIGINT, bailout);
	printf("\nTerminate directory listing? ");
	scanf("%1s", &ch);
	if (ch == 'y' || ch == 'Y')
		exit(1);
}
