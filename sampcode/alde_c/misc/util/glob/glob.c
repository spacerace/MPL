
/*
 *	Copyright 1985, 1986  by John Plocher  Non-commercial use approved
 *
 *	glob	expand a given pathname with wildcards into
 *		an argv[] type array of 'strings' which are
 *		the files (directories) which result from the
 *		expansion of the pathname.  (All characters in
 *		the returned list of filenames are in lower case)
 *
 *	usage:	char **glob(pathname) char *pathname;
 *
 *		pathname is an absolute or relative pathname which may
 *		contain DOS type wildcards wherever desired:
 *
 *			names = glob("\\usr\\pl*\\*.c");
 *
 *	It requires an externally declared int in the main function
 *	named globerror:
 *
 *		extern int globerror;
 *
 *	to be used to return an error indication if glob() fails.
 *	If glob() fails, it returns NULL with
 *
 *	globerror set to GE_BADPATH	invalid path - directory not found
 *	globerror set to GE_AMBIGUOUS	ambiguous directory spec
 *	globerror set to GE_MAXFILES	too many files found (MAXFILES)
 *	globerror set to GE_NOMEM	out of memory
 *
 *	else it returns a pointer to a NULL terminated array of pointers
 *	to 'strings':
 *
 *	+-[ MAIN.C ]----------------------------------------------------+
 *	|								|
 *	| #include "glob.h"    						|
 *	| main() {							|
 *	| 	char **glob();						|
 *	| 	char **names;						|
 *	| 	names = glob("/u??/p*");				|
 *	| 	if (names)						|
 *	| 	    printf("the first name returned is %s\n",names[0]);	|
 *	| 	else							|
 *	| 	    printf("glob() error number %d\n",globerror);	|
 *	|	recover(names);						|
 *	| }								|
 *	|								|
 *	+---------------------------------------------------------------+
 *
 *	ALL strings and the array *names[] are made with calls to
 *	malloc().  Thus, be sure to free() *names[x] AND names when done!
 *
 *	+-[sample routines]---------------------------------------------+
 *	|								|
 *	|	(* free() all the space used up by **names; *)		|
 *	| recover(names)						|
 *	| char **names;							|
 *	| {								|
 *	|     int i = 0;						|
 *	|								|
 *	|     while (names[i] != (char *)0) {				|
 *	| 	free(names[i]);						|
 *	| 	i++;							|
 *	|     }								|
 *	|     free(names);						|
 *	| }								|
 *	| 								|
 *	| 								|
 *	|	(* print out all the filenames returned by glob() *)	|
 *	| print(names)							|
 *	| char **names;							|
 *	| {								|
 *	|     int i;							|
 *	|     i = 0;							|
 *	|     while (names[i] != (char *)0) {				|
 *	| 	printf("%s\n",names[i]);				|
 *	| 	i++;							|
 *	|     }								|
 *	| }								|
 *	+---------------------------------------------------------------+
 *
 *	Compile as follows:	(Structures MUST be packed in order for 
 *				 the program to interface correctly with DOS)
 *		cl -c -Zp glob.c	** Microsoft C v3 or 4 **
 *
 *	Using the example given above, compile and link main program:
 *		cl main.c glob
 *
 *	Written December, 1985 by John Plocher with MicroSoft C 3.0
 *	Change log:
 *
 *	version	date		who	comments
 *	------- ----		---	--------
 *	1.0	10-Dec-85	jmp	Orig. coding
 *	1.1	16-Jan-86	jmp	added globerror
 *	1.2	05-Feb-86	jmp	added comments and doccumentation
 *
 */

static char *scssid = "5-Feb-86 (Plocher) @(#)glob.c	1.2";
static char *copyright =
	"Copyright 1985, 1986 John Plocher.  Non-commercial use approved";

#include	<stdio.h>
#include	<dos.h>
#include	<memory.h>
#include	<direct.h>
#include	<string.h>
#include	"glob.h"


/* Change the following if you might have more than this many    */
/* files returned by glob()                                      */

#define MAXFILES	256	/* max files returned by glob() */


static char *list[MAXFILES];	/* MAXFILES entries per sub-dir */


extern char *strlwr();
extern int getdir();
extern char *pos();

int globerror;			/* PUBLIC - global error number */

/*
 *	glob()
 */
char **
glob(proposed)
char *proposed;
{
    int i,j,k,needdir,n;
    char *end, filename[128], newpath[127], tmppath[127];
    char *f, *p1, *p2;
    char **names;
    
    strcpy(filename, proposed);
    
    /* add on current directory if first char is not '/' or '\' */
    if (*filename != '/' && *filename != '\\') {
	getcwd(tmppath,128);
	p2 = strchr(tmppath,'\0');
	p2--;
	if (*p2 != '/' && *p2 != '\\')
	    strcat(tmppath,"/");
	strcat(tmppath,filename);
	strcpy(filename, tmppath);
    }
    for (i=strlen(filename); i; i--)
	if (filename[i] == '\\')
	    filename[i] = '/';
    i = 0; j = 0;
    p2 = strchr(filename,'\0');
    p2--;
    if (*p2 == '.')
	strcat(filename,"/");
    while ((p1 = pos("/./",filename)) != NULL) {
	memccpy(p1+1, p1+3, '\0',strlen(p1));
    }
    while ((p1 = pos("../",filename)) != NULL) {
	char tmp;
	tmp = *(p1 - 1);
	*(p1 - 1) = '\0';
	p2 = strrchr(filename,'/');
	if (p2 == NULL) {
	    globerror = GE_BADFILE;
	    return(NULL);
	}
	*(p1 - 1) = tmp;
	memccpy(p2+1, p1+3, '\0',strlen(p1));
    }
    p2 = strchr(filename,'\0');
    p2--;
    if (*p2 == '/')
	*p2 = '\0';
loop:
    while (filename[i]  != 0 &&		/* copy till hit a wildcard */
	   filename[i]  != '*' &&
	   filename[i]  != '?') {
	tmppath[j++] = filename[i++];
    }
    if (filename[i] != 0)	/* wildcard found */
	while (filename[i]  != 0 &&
	       filename[i]  != '/')
	    tmppath[j++] = filename[i++];
    if (filename[i] != 0)	/* need directory here */
	needdir = 1;
    else			/* any file will do */
	needdir = 0;
    tmppath[j] = 0;
    strcpy(newpath,tmppath);
    end = strrchr(newpath,'/');
    if  (end != NULL)
      *end = '\0';
      
    n = getdir(tmppath,needdir);

    if (n == -2) {	/* out of dynamic memory */
	globerror = GE_NOMEM;
	return(NULL);
    }
    if (n == -1) {	/* exceeded filecount */
	globerror = GE_MAXFILES;
	return(NULL);
    }
    if (n == 0) { /* file not found */
	globerror = GE_BADFILE;
	return(NULL);
    }
    if (needdir && n != 1) { /* ambiguous directory reference */
	for (i = 0; i < n; i++)		/* This is an arbitrary limit */
	    free(list[i]);		/* one could follow all paths */
	globerror = GE_AMBIGUOUS;	/* and not just the first...  */
	return(NULL);
    }
    if (needdir) {
	strcpy(tmppath,newpath);
	strcat(tmppath,"/");
	strcat(tmppath,list[0]);
	free(list[0]);
	j = strlen(tmppath);
	goto loop;
    }

    names = (char **)malloc((n+1) * sizeof(char *));
    for (i = 0; i < n; i++) {
	sprintf(tmppath,"%s/%s", newpath, list[i]);
	names[i] = strdup(tmppath);
	free(list[i]);
    }
    names[n] = (char *)0;

    return(names);
}


/*
 *	pos(target, src)
 *			find target string within src
 */
static char *pos(s1,s2)
char *s1, *s2;
{
    int temploc, dist;
    char *tp;

    if (*s1) {
	for (;;) {
	    tp = strchr(s2, *s1);
	    if (tp == NULL) {		/* couldn't even find first char */
		return(NULL);
	    }
	    if (!strncmp(s1,tp,strlen(s1)))	/* match */ {
		return(tp);
	    }
	    s2 = tp + 1;
	}
    }
}


/*
 *	File attributes
 */

#define AT_RO		0x01	/* Read Only */
#define AT_HIDDEN	0x02	/* hidden file */
#define	AT_SYSTEM	0x04	/* System file */
#define AT_LABEL	0x08	/* volume label */
#define AT_DIR		0x10	/* sub directory */
#define AT_ARCH		0x20	/* archive bit */

static struct REGS rregs;
static struct SREGS sregs;


static struct DMA {			/* For reading directory entry	*/
    char res[21];
    char attr;
    unsigned sec : 5, min : 6, hour : 5;
    unsigned day : 5, mon : 4, year : 7;
    long size;
    char name[13];
} dma;

/*
 *	getdir		gather all the files found in this directory
 */
static int 
getdir(path, needdir)	/* return number of 'files' found in this path */
char  *path;		/* this is where to look */
int needdir;		/* 1 = we want ONLY AT_DIR type entries */
			/* 0 = we will take EVERYTHING */
{
    int i, j, n;
    char *entry;

    bdos(0x1a, &dma,0);

    n = 0;
    rregs.h.ah = 0x4e;
    rregs.x.cx = AT_RO | AT_HIDDEN | AT_SYSTEM | AT_DIR;
    rregs.x.dx = (unsigned int) path;
    j = intdos(&rregs, &rregs);		/* Search for first	*/
    while(j == 0) {
	strlwr(dma.name);
	if (needdir && (dma.attr & AT_DIR != AT_DIR)){ /* skip this entry */
	    rregs.h.ah = 0x4f;		/* wes needs dirs, and this ain't one */
	    j = intdos(&rregs, &rregs);/* Search for next	*/
	    continue;
	}
	entry = strdup(dma.name);
	if (entry == NULL) {		/* out of memory */
	    for (i = 0; i < n; i++)
		free(list[i]);
	    return (-2);
	}
	if (n >= MAXFILES)		/* too many files found */
	    return(-1);
	for (i = n++; i > 0; --i) {	/* alphabetize */
	    if (strcmp(entry, list[i - 1]) >= 0)
		break;
	    list[i] = list[i - 1];
	}
	list[i] = entry;
	rregs.h.ah = 0x4f;
	j = intdos(&rregs, &rregs);		/* Search for next	*/
    }
    return(n);
}

