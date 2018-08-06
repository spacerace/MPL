/*% cc -O lar.c -o lar
From linus!sch Tue Jul 26 08:07:37 1983
Subject: CP/M Lu library maintainer

When transfering files to my personal computer, I often want to transfer
several files at once using the Umodem program.  To do this I wrote
the following small program to combine files for the CP/M LU program.

No special treatment necessary, just:
	cc -O lar.c -o lar
to make it.

-- 
Stephen Hemminger,  Mitre Corp. Bedford MA 
	{allegra,genrad,ihnp4, utzoo}!linus!sch	(UUCP)
	linus!sch@mitre-bedford			(ARPA)
----------------- lar.c ----------------------
*/
/* May 1985  J. P. Worth
	Modifications and enhancements to original UNIX  c source
	to allow operation under CP/M, MS/DOS and compilation with
	the CI C86 compiler. All changes are transparent to the
	original lar.c source through the use of a define variable
	"C86". Compile using as one of the options  "-dC86" or
	include "#define	C86" as first line of source code
	All features checked and tested under CP/M 86 with Version
	2.20H of CI C86 compiler
*/

/*
 * Lar - LU format library file maintainer
 * by Stephen C. Hemminger
 *	linus!sch	or	sch@Mitre-Bedford
 *
 *  Usage: lar key library [files] ...
 *
 *  Key functions are:
 *	u - Update, add files to library
 *	t - Table of contents
 *	e - Extract files from library
 *	p - Print files in library
 *	d - Delete files in library
 *	r - Reorginize library
 *  Other keys:
 *	v - Verbose
 *
 *  This program is public domain software, no warranty intended or
 *  implied.
 *
 *  DESCRPTION
 *     Lar is a Unix program to manipulate CP/M LU format libraries.
 *     The original CP/M library program LU is the product
 *     of Gary P. Novosielski. The primary use of lar is to combine several
 *     files together for upload/download to a personal computer.
 *
 *  PORTABILITY
 *     The code is modeled after the Software tools archive program,
 *     and is setup for Version 7 Unix.  It does not make any assumptions
 *     about byte ordering, explict and's and shift's are used.
 *     If you have a dumber C compiler, you may have to recode new features
 *     like structure assignment, typedef's and enumerated types.
 *
 *  BUGS/MISFEATURES
 *     The biggest problem is text files, the programs tries to detect
 *     text files vs. binaries by checking for non-Ascii (8th bit set) chars.
 *     If the file is text then it will throw away Control-Z chars which
 *     CP/M puts on the end.  All files in library are padded with Control-Z
 *     at the end to the CP/M sector size if necessary.
 *
 *     No effort is made to handle the difference between CP/M and Unix
 *     end of line chars.  CP/M uses Cr/Lf and Unix just uses Lf.
 *     The solution is just to use the Unix command sed when necessary.
 *
 *  * Unix is a trademark of Bell Labs.
 *  ** CP/M is a trademark of Digital Research.
 */

#include <stdio.h>
#include <ctype.h>
#define	C86

#define ACTIVE	00
#define UNUSED	0xff
#define DELETED 0xfe
#define CTRLZ	0x1a

#define MAXFILES 256
#define SECTOR	 128
#define DSIZE	( sizeof(struct ludir) )
#define SLOTS_SEC (SECTOR/DSIZE)
#define equal(s1, s2) ( strcmp(s1,s2) == 0 )
/* if you don't have void type just define as blank */
#ifndef	C86

#define	VOID	(void)

#else

#define VOID	

#endif

/* if no enum's then define false as 0 and true as 1 and bool as int */

#ifndef	C86
typedef enum {false=0, true=1} bool;



#else
#define	false	0
#define	true	1


#define	bool	int

#endif


/* Globals */
char   *fname[MAXFILES];
bool ftouched[MAXFILES];

typedef struct {
    unsigned char   lobyte;
    unsigned char   hibyte;
} word;

/* convert word to int */
#define wtoi(w) ( (w.hibyte<<8) + w.lobyte)
#define itow(dst,src)	dst.hibyte = (src & 0xff00) >> 8;\
				dst.lobyte = src & 0xff;

struct ludir {			/* Internal library ldir structure */
    unsigned char   l_stat;	/*  status of file */
    char    l_name[8];		/*  name */
    char    l_ext[3];		/*  extension */
    word    l_off;		/*  offset in library */
    word    l_len;		/*  lengty of file */
    char    l_fill[16];		/*  pad to 32 bytes */
} ldir[MAXFILES];

int     errcnt, nfiles, nslots;
bool	verbose = false;
char	*cmdname;

char   *getname(), *sprintf();
int	update(), reorg(), table(), extract(), print(), delete();
#ifdef	C86
long	rewind(), ftell(), fseek();
#endif

main (argc, argv)
int	argc;
char  **argv;
{
    register char *flagp;
    char   *aname;			/* name of library file */
    int	   (*function)() = NULL;	/* function to do on library */
/* set the function to be performed, but detect conflicts */
#define setfunc(val)	if(function != NULL) conflict(); else function = val

    cmdname = "lar";	/* can't get command name in CPM with argv[0] */
    if (argc < 3)
	help ();

    aname = argv[2];
    filenames (argc, argv);

    for(flagp = argv[1]; *flagp; flagp++)
	switch (*flagp) {
	case '-':
		break;
	case 'u': 
	    setfunc(update);
	    break;
	case 't': 
	    setfunc(table);
	    break;
	case 'e': 
	    setfunc(extract);
	    break;
	case 'p': 
	    setfunc(print);
	    break;
	case 'd': 
	    setfunc(delete);
	    break;
	case 'r': 
	    setfunc(reorg);
	    break;
	case 'v':
	    verbose = true;
	    break;
	default: 
	    help ();
    }

    if(function == NULL) {
	fprintf(stderr,"No function key letter specified\n");
	help();
    }

    (*function)(aname);
}

/* print error message and exit */
help () {
    fprintf (stderr, "Usage: %s {utepdr}[v] library [files] ...\n", cmdname);
    fprintf (stderr, "Functions are:\n\tu - Update, add files to library\n");
    fprintf (stderr, "\tt - Table of contents\n");
    fprintf (stderr, "\te - Extract files from library\n");
    fprintf (stderr, "\tp - Print files in library\n");
    fprintf (stderr, "\td - Delete files in library\n");
    fprintf (stderr, "\tr - Reorginize library\n");

    fprintf (stderr, "Flags are:\n\tv - Verbose\n");
    exit (1);
}

conflict() {
   fprintf(stderr,"Conficting keys\n");
   help();
}

error (str)
char   *str;
{
    fprintf (stderr, "%s: %s\n", cmdname, str);
    exit (1);
}


/* Get file names, check for dups, and initialize */
filenames (ac, av)
char  **av;
{
    register int    i, j;

    errcnt = 0;
    for (i = 0; i < ac - 3; i++) {
	fname[i] = av[i + 3];
	ftouched[i] = false;
	if (i == MAXFILES)
	    error ("Too many file names.");
    }
    fname[i] = NULL;
    nfiles = i;
    for (i = 0; i < nfiles; i++)
	for (j = i + 1; j < nfiles; j++)
	    if (equal (fname[i], fname[j])) {
		fprintf (stderr, "%s", fname[i]);
		error (": duplicate file name");
	    }
}

table (lib)
char   *lib;
{
    FILE   *lfd;
    register int    i, total;
    int active = 0, unused = 0, deleted = 0;
    char *uname;

#ifndef	C86
    if ((lfd = fopen (lib, "r")) == NULL) {
#else
    if ((lfd = fopen (lib, "rb")) == NULL) {
#endif
	fprintf(stderr, "%s: can't open\n", lib);
	exit(1);
	}

    getdir (lfd);
    total = wtoi(ldir[0].l_len);
    if(verbose) {
 	printf("Name          Index Length\n");
	printf("Directory           %4d\n", total);
    }

    for (i = 1; i < nslots; i++)
	switch(ldir[i].l_stat) {
	case ACTIVE:
		active++;
		uname = getname(ldir[i].l_name, ldir[i].l_ext);
		if (filarg (uname))
		    if(verbose) 
			printf ("%-12s   %4d %4d\n", uname,
			    wtoi (ldir[i].l_off), wtoi (ldir[i].l_len));
		    else
			printf ("%s\n", uname);
		total += wtoi(ldir[i].l_len);
		break;
	case UNUSED:
		unused++;
		break;
	default:
		deleted++;
	}
    if(verbose) {
	printf("--------------------------\n");
	printf("Total sectors       %4d\n", total);
	printf("\nLibrary %s has %d slots, %d deleted %d active, %d unused\n",
		lib, nslots, deleted, active, unused);
    }

    VOID fclose (lfd);
    not_found ();
}

getdir (f)
FILE *f;
{

    rewind(f);

    if (fread ((char *) & ldir[0], DSIZE, 1, f) != 1)
	error ("No directory\n");

    nslots = wtoi (ldir[0].l_len) * SLOTS_SEC;

    if ( fread( (char *) & ldir[1], DSIZE, nslots, f) != nslots)
	error("Can't read directory - is it a library?");
}

putdir (f)
FILE *f;
{

    rewind(f);
    if (fwrite ((char *) ldir, DSIZE, nslots, f) != nslots)
	error ("Can't write directory - library may be botched");
}

initdir (f)
FILE *f;
{
    register int    i, j;
    int     numsecs;
    char    line[80];
    static struct ludir blankentry = {
	UNUSED,
	{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
	{ ' ', ' ', ' ' },
    };

    for (;;) {
	printf ("Number of slots to allocate (multiple of 8):  ");
	if (fgets (line, 80, stdin) == NULL)
	    error ("Eof when reading input");
	nslots = atoi (line);
	if (nslots < 1)
	    printf ("Must have at least one!\n");
	else if (nslots > MAXFILES)
	    printf ("Too many slots\n");
	else
	    break;
    }

    numsecs = nslots / SLOTS_SEC;
    nslots = numsecs * SLOTS_SEC;

    for (i = 0; i < nslots; i++) {

#ifndef	C86
	ldir[i] = blankentry;
#else


	for(j = 0; j < 16; j++) {
		ldir[i].l_ext[j%3] = blankentry.l_ext[j%3];
		ldir[i].l_name[j%8] = blankentry.l_name[j%8];
		ldir[i].l_fill[j] = blankentry.l_fill[j];
	}
	ldir[i].l_stat = blankentry.l_stat;
#endif
    }
    ldir[0].l_stat = ACTIVE;
    itow (ldir[0].l_len, numsecs);

    putdir (f);
}

/* convert nm.ex to a Unix style string */
char   *getname (nm, ex)
char   *nm, *ex;
{
    static char namebuf[14];
    register char  *cp, *dp;

    for (cp = namebuf, dp = nm; *dp != ' ' && dp != &nm[8];) {
	*cp++ = isupper (*dp) ? tolower (*dp) : *dp;
	++dp;
    }
    *cp++ = '.';

    for (dp = ex; *dp != ' ' && dp != &ex[3];) {
	*cp++ = isupper (*dp) ? tolower (*dp) : *dp;
	++dp;
    }

    *cp = '\0';
    return namebuf;
}

putname (cpmname, unixname)
char   *cpmname, *unixname;
{
    register char  *p1, *p2;

    for (p1 = unixname, p2 = cpmname; *p1; p1++, p2++) {
	while (*p1 == '.') {
	    p2 = cpmname + 8;
	    p1++;
	}
	if (p2 - cpmname < 11)
	    *p2 = islower(*p1) ? toupper(*p1) : *p1;
	else {
	    fprintf (stderr, "%s: name truncated\n", unixname);
	    break;
	}
    }
    while (p2 - cpmname < 11)
	*p2++ = ' ';
}

/* filarg - check if name matches argument list */
filarg (name)
char   *name;
{
    register int    i;
    char *strpname;

    if (nfiles <= 0)
	return 1;

    for (i = 0; i < nfiles; i++) {
/* the following is a hack to strip the drive label and colon 
   for use under CPM, MSDOS, etc  may 85 jpw   */
	strpname = fname[i];
	if ( *(fname[i] + 1) == ':')  strpname = fname[i] + 2;
	if (equal (name, strpname)) {
	    ftouched[i] = true;
	    return 1;
	}
    }

    return 0;
}

not_found () {
    register int    i;

    for (i = 0; i < nfiles; i++)
	if (!ftouched[i]) {
	    fprintf (stderr, "%s: not in library.\n", fname[i]);
	    errcnt++;
	}
}


extract(name)
char *name;
{
	getfiles(name, false);
}

print(name)
char *name;
{
	getfiles(name, true);
}

getfiles (name, pflag)
char   *name;
bool	pflag;
{
    FILE *lfd, *ofd;
    register int    i;
    char   *unixname;

#ifndef	C86
    if ((lfd = fopen (name, "r")) == NULL) {
#else
    if ((lfd = fopen (name, "rb"))  == NULL) {
#endif
	fprintf(stderr, "%s: can't open\n", name);
	exit(1);
	}

    ofd = pflag ? stdout : NULL;
    getdir (lfd);

    for (i = 1; i < nslots; i++) {
	if(ldir[i].l_stat != ACTIVE)
		continue;
	unixname = getname (ldir[i].l_name, ldir[i].l_ext);
	if (!filarg (unixname))
	    continue;
	fprintf(stderr,"%s", unixname);
	if (ofd != stdout)
#ifndef	C86
	    ofd = fopen (unixname, "w");
#else
	    ofd = fopen (unixname, "wb");
#endif
	if (ofd == NULL) {
	    fprintf (stderr, "  - can't create");
	    errcnt++;
	}
	else {
	    VOID fseek (lfd, (long) wtoi (ldir[i].l_off) * SECTOR, 0);
	    acopy (lfd, ofd, wtoi (ldir[i].l_len));
	    if (ofd != stdout)
		VOID fclose (ofd);
	}
	putc('\n', stderr);
    }
    VOID fclose (lfd);
    not_found ();
}

acopy (fdi, fdo, nsecs)
FILE *fdi, *fdo;
register unsigned int nsecs;
{
    register int    i, c;
    int	    textfile = 1;

    while( nsecs-- != 0) 
	for(i=0; i<SECTOR; i++) {
		c = getc(fdi);
#ifndef	C86
		if( feof(fdi) )
#else
		if( c == EOF )
#endif
			error("Premature EOF\n");
		if( ferror(fdi) )
		    error ("Can't read");
		if( !isascii(c) )
		    textfile = 0;
		if( nsecs != 0 || !textfile || c != CTRLZ) {
			putc(c, fdo);
			if ( ferror(fdo) )
			    error ("write error");
		}
	 }
}

update (name)
char   *name;
{
    FILE *lfd;
    register int    i;

#ifndef	C86
   if ((lfd = fopen (name, "r+")) == NULL) {
#else
    if ((lfd = fopen (name, "rwb")) == NULL) {
#endif
#ifndef	C86
	if ((lfd = fopen (name, "w+")) == NULL) {
#else
	if ((lfd = fopen (name, "wrb")) == NULL) {
#endif
	fprintf(stderr, "%s: can't open or create for update\n", name);
	exit(1);
	}
	initdir (lfd);
    }
    else
	getdir (lfd);		/* read old directory */

    if(verbose)
	    fprintf (stderr,"Updating files:\n");
    for (i = 0; i < nfiles; i++)
	addfil (fname[i], lfd);
    if (errcnt == 0)
	putdir (lfd);
    else
	fprintf (stderr, "fatal errors - library not changed\n");
    VOID fclose (lfd);
}

addfil (name, lfd)
char   *name;
FILE *lfd;
{
    FILE	*ifd;
    register int secoffs, numsecs;
    register int i;

#ifndef	C86
    if ((ifd = fopen (name, "r")) == NULL) {
#else
    if ((ifd = fopen (name, "rb")) == NULL) {
#endif
	fprintf (stderr, "%s: can't find to add\n",name);
	errcnt++;
	return;
    }
 /* the following line is to remove drive label and colon if present 
    may 85 jpw */
    if ( *(name+1) == ':')   name +=2 ;
    if(verbose)
        fprintf(stderr, "%s\n", name);
    for (i = 0; i < nslots; i++) {
	if (equal( getname (ldir[i].l_name, ldir[i].l_ext), name) ) /* update */
	    break;
	if (ldir[i].l_stat != ACTIVE)
		break;
    }
    if (i >= nslots) {
	fprintf (stderr, "%s: can't add library is full\n",name);
	errcnt++;
	return;
    }

    ldir[i].l_stat = ACTIVE;
    putname (ldir[i].l_name, name);
    VOID fseek(lfd, 0L, 2);		/* append to end */
    secoffs =  (ftell(lfd) / SECTOR);

    itow (ldir[i].l_off, (unsigned) secoffs);
    numsecs = fcopy (ifd, lfd);
    itow (ldir[i].l_len, numsecs);
    VOID fclose (ifd);
}

fcopy (ifd, ofd)
FILE *ifd, *ofd;
{
    register int total = 0;
    register int i, n;
    char sectorbuf[SECTOR];


    while ( (n = fread( sectorbuf, 1, SECTOR, ifd)) != 0) {
	if (n != SECTOR)
	    for (i = n; i < SECTOR; i++)
		sectorbuf[i] = CTRLZ;
	if (fwrite( sectorbuf, 1, SECTOR, ofd ) != SECTOR)
		error("write error");
	++total;
    }
    return total;
}

delete (lname)
char   *lname;
{
    FILE *f;
    register int    i, j;

#ifndef	C86
    if ((f = fopen (lname, "r+")) == NULL) {
#else
    if ((f = fopen (lname, "rwb")) == NULL) {
#endif
	fprintf(stderr, "%s: can't open\n", lname);
	exit(1);
	}

    if (nfiles <= 0)
	error("delete by name only");

    getdir (f);
    for (i = 0; i < nslots; i++) {
	if (!filarg ( getname (ldir[i].l_name, ldir[i].l_ext)))
	    continue;
	ldir[i].l_stat = DELETED;
	for (j = 0; j < 8; j++) {
	    ldir[i].l_ext[j%3] = ' ';
	    ldir[i].l_name[j] = ' ';
	}
    }

    not_found();
    if (errcnt > 0)
	fprintf (stderr, "errors - library not updated\n");
    else
	putdir (f);
    VOID fclose (f);
}

reorg (name)
char  *name;
{
    FILE *olib, *nlib;
    int oldsize;
    register int i, j;
    struct ludir odir[MAXFILES];
    char tmpname[SECTOR];

    VOID sprintf(tmpname,"%-10.10s.TMP", name);

#ifndef	C86
    if( (olib = fopen(name,"r")) == NULL) {
#else
    if( (olib = fopen(name,"rb")) == NULL) {
#endif
	fprintf(stderr, "%s: can't open\n", name);
	exit(1);
	}

#ifndef	C86
    if( (nlib = fopen(tmpname, "w")) == NULL) {
#else
    if( (nlib = fopen(tmpname, "wb")) == NULL) {
#endif
	fprintf(stderr, "%s: can't open\n", tmpname);
	exit(1);
	}

    getdir(olib);
    printf("Old library has %d slots\n", oldsize = nslots);
    for(i = 0; i < nslots ; i++)
	    copymem( (char *) &odir[i], (char *) &ldir[i],
			sizeof(struct ludir));
    initdir(nlib);
    errcnt = 0;

    for (i = j = 1; i < oldsize; i++)
	if( odir[i].l_stat == ACTIVE ) {
	    if(verbose)
		fprintf(stderr, "Copying: %-8.8s.%3.3s\n",
			odir[i].l_name, odir[i].l_ext);
	    copyentry( &odir[i], olib,  &ldir[j], nlib);
	    if (++j >= nslots) {
		errcnt++;
		fprintf(stderr, "Not enough room in new library\n");
		break;
	    }
        }

    VOID fclose(olib);
    putdir(nlib);
    VOID fclose (nlib);

    if(errcnt == 0) {
#ifndef	C86
	if ( unlink(name) < 0 || link(tmpname, name) < 0) {
#else
	if ( unlink(name) < 0 || rename(tmpname, name) < 0) {
#endif
	    VOID unlink(tmpname);
	fprintf(stderr, "%s: can't delete or rename\n", name);
	exit(1);
	}
    }
    else
	fprintf(stderr,"Errors, library not updated\n");
    VOID unlink(tmpname);

}

copyentry( old, of, new, nf )
struct ludir *old, *new;
FILE *of, *nf;
{
    register int secoffs, numsecs;
    char buf[SECTOR];

    new->l_stat = ACTIVE;
    copymem(new->l_name, old->l_name, 8);
    copymem(new->l_ext, old->l_ext, 3);
    VOID fseek(of, (long) wtoi(old->l_off)*SECTOR, 0);
    VOID fseek(nf, 0L, 2);
    secoffs = ftell(nf) / SECTOR;

    itow (new->l_off, secoffs);
    numsecs = wtoi(old->l_len);
    itow (new->l_len, numsecs);

    while(numsecs-- != 0) {
	if( fread( buf, 1, SECTOR, of) != SECTOR)
	    error("read error");
	if( fwrite( buf, 1, SECTOR, nf) != SECTOR)
	    error("write error");
    }
}

copymem(dst, src, n)
register char *dst, *src;
register unsigned int n;
{
	while(n-- != 0)
		*dst++ = *src++;
}
