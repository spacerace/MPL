
/*
 *	chd.c		change directory
 *			John Plocher
 *
 *	usage:	chd [new directory]
 *
 *		[new directory] may contain wildcards; it may not be ambiguous
 *		If no arguments, use $HOME variable in environment
 *
 *		C:\BIN > set HOME=/usr/plocher
 *		C:\BIN > chd \u*\lo*
 *		C:\USR\LOCAL > chd
 *		C:\USR\PLOCHER >  
 *
 *		uses glob() to expand wildcards in pathname
 *
 *	returns (ERRORLEVEL)
 *
 *		0	No error
 *		1	could not find directory
 *		2	ambiguous pathname
 *		3	directoryname expected, you gave a filename
 *		4	internal error
 *
 *	Change log
 *
 *	Version	Date		Who	Comments
 *	-------	----		---	--------
 *	1.00	16-Jan-86	jmp	Initially coded
 *	1.01	21-Jan-86	jmp	Expanded help menu (-?), fixed glob()
 */


#include <stdio.h>
#include "glob.h"

#define VERSION	"1.01 21-Jan-86"
/* #define DEBUG */

char *sccsid	= "21-Jan-86 (Plocher) @(#)chd.c	1.01";

extern char *strchr();
extern char *getenv();
extern char *strdup();
extern void usage();

extern char *filename();
extern char *fixslash();
extern void recover();

main(argc, argv)
int argc;
char *argv[];
{
    char **names;	/* array of pointers to names returned by glob() */
    char *progname;	/* name of program (either argv[0] or hardcoded) */
    char *p;		/* temp string pointer */
    char errstring[150];/* for usage() errors */
    char path[127];	/* workspace for getting starting (un-globbed) path */

    if (argv[0])	/* if there is something here, use it (DOS 3.xx+) */
	    progname = filename(argv[0]);
    else		/* hardcode the program name */
	    progname = "chd";

    if (argc > 2) {	/* must have less than 2 arguments */
	    usage(progname,"Too many arguments");
	    exit(1);
    }
    if (argc == 1) {		/* need to get HOME */
	    p = getenv("HOME");	/* p == NULL if not found ... */
	    if (p != NULL)
		    strcpy(path,p);
	    else {
		    usage(progname,"Environment variable HOME not found\n");
		    exit(1);
	    }
    } else {		/* we have a pathspec from command line */
	    if (argv[1][0] == '-') {	/* if invoked chd -anything, just */
		    usage(progname,NULL);	/* explain ourselves */
		    exit(0);
	    }
	    strcpy(path,argv[1]);
    }

    fixslash(path);	/* convert all '\'s into '/'s */

    names = glob(path);			/* get files which match */
    
    if (names == NULL && globerror == GE_BADPATH) {
	    sprintf(errstring,"Path not found: \"%s\"",path);
	    usage(progname, errstring);
	    recover(names);
	    exit(1);
    }
    if ((names == NULL && globerror == GE_AMBIGUOUS) || names[1] != NULL) {
	    sprintf(errstring,"Ambiguous pathname \"%s\"",path);
	    usage(progname, errstring);
	    recover(names);
	    exit(2);
    }
    if (chdir(names[0])) {	/* do the actual chdir */
	    usage(progname,"You must specify a DIRECTORY path");
	    recover(names);
	    exit(3);
    }
    if (names == NULL && (globerror == GE_MAXFILES || globerror == GE_NOMEM)) {
	    sprintf(errstring,"Internal error: %s",
		globerror == GE_MAXFILES ? "Too many files" : "Out of memory");
	    usage(progname, errstring);
	    recover(names);
	    exit(4);
    }
    recover(names);	/* free memory used by *names[] */

    exit(0);		/* no errors */
}

/*
 *	usage		tell the workd about this program
 */
void usage(progname, error)
char *progname;
{
fprintf(stderr,"%s version %s by John Plocher on FidoNet 121/90\n",
		progname,VERSION);
if (error)
	fprintf(stderr,"\nERROR: %s\n",error);
else {
 fprintf(stderr,"\n%s - a replacement for MS-DOS's chdir & cd commands\n",
		progname);
 fprintf(stderr,"%s is Copyright 1986 by John Plocher\n\n",progname);
 fprintf(stderr,"%s is released to the NON PROFIT, NON COMMERCIAL public.\n",
		progname);
 fprintf(stderr,"Commercial rights to %s reserved by John Plocher\n", progname);
 fprintf(stderr,"%s has some features which the DOS version lacks:\n",progname);
 fprintf(stderr,"\t1) ALL wildcards (* and ?) are expanded, no matter where\n");
 fprintf(stderr,"\t   they are in the path.\n");
 fprintf(stderr,"\t\tC:\\ >%s \\u*\\p*\\w*\\?\n",progname);
 fprintf(stderr,"\t\tC:\\USR\\PLOCHER\\WORK\\C >\n");
 fprintf(stderr,"\t2) If %s is invoked without arguments, it will try to\n",progname);
 fprintf(stderr,"\t   change to the directory found in the environment\n");
 fprintf(stderr,"\t   variable HOME.\n");
 fprintf(stderr,"\t\tC:\\ >SET HOME=\\usr\\plocher    -or-\n");
 fprintf(stderr,"\t\tC:\\ >SET HOME=\\u*\\p*\n");
 fprintf(stderr,"\t3) If %s fails, it leaves an indication in ERRORLEVEL.\n",progname);
 fprintf(stderr,"\t   This exit code can be tested in batch files:\n");
 fprintf(stderr,"\t    ERRORLEVEL  Meaning\n");
 fprintf(stderr,"\t    ----------  -------\n");
 fprintf(stderr,"\t\t0	No error\n");
 fprintf(stderr,"\t\t1	could not find directory\n");
 fprintf(stderr,"\t\t2	ambiguous pathname\n");
 fprintf(stderr,"\t\t3	directoryname expected, you gave a filename\n");
 fprintf(stderr,"\t\t4	internal error (let me know about this!)\n\n");
}
fprintf(stderr,"usage: %s <directory name>     change to directory\n",progname);
fprintf(stderr,"       %s                      change to HOME\n",progname);
fprintf(stderr,"       %s -?                   help screen\n",progname);
}

/*
 *	filename		extract the filename from a pathname
 *				ie C:\usr\plocher\chd.exe results in chd
 */
char *filename(path)
char *path;

{
	register char *p, *pd;

	p = strchr(path,'\0');	/* work from the back... */
	while (--p != path && *p != ':' && *p != '\\' && *p != '/');
	if (*p == '\\' || *p == '/' || *p == ':')
		p++;
	pd = strchr(p,'.');		/* we don't want the .EXE */
	if (pd != NULL)
		*pd = '\0';
	strlwr(p);
	return (p);
}

/*
 *	fixslash	convert all '\'s to '/'s
 */
char *fixslash(s)
char *s;
{
	register int x;

	if (s == NULL) return;
	for (x=0; x < strlen(s); x++)
		if (s[x] == '\\')
			s[x] = '/';
}


/*
 *	recover		free up the space malloc()'d in *names[] by glob()
 */
void recover(names)
char **names;
{
	register int i;

	i = 0;
	while (names[i] != (char *)0) {
		free(names[i]);
		i++;
	}
	free(names);
}

