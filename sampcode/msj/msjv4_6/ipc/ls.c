/* ls.c RHS 4/6/85 rev. 10/10/85, rev. again 7/10/88, rev. again 7/89
 * this module contains ls(), like the UNIX ls
 *
 * modified for OS/2 7/10/88
 * modified for OS/2 directory server 7/15/89
 *
 *

	LS is a directory utility patterned after the UNIX counterpart by the
	same name. Run it with:

   LS [-flscpternda<rhsad>?] [<list of files>]

	The options are explained by running:

	LS -?

	The list of files are any filespecs (with paths if not in default dir).

 */



#define INCL_SUB
#define INCL_DOS
#include<os2.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<stddef.h>
#include<malloc.h>
#include<conio.h>
#include"di.h"

#define A_NORMAL    	0x0000
#define A_READONLY  	0x0001
#define A_HIDDEN    	0x0002
#define A_SYSTEM    	0x0004
#define A_VOLLABEL  	0x0008

#define A_DIRECTORY 	0x0010
#define A_ARCHIVE   	0x0020
#define A_UNUSED0   	0x0040
#define A_UNUSED1   	0x0080

#define A_UNUSED2   	0x0100
#define A_UNUSED3   	0x0200
#define A_UNUSED4   	0x0400
#define A_UNUSED5   	0x0800

#define A_UNUSED6   	0x1000
#define A_UNUSED7   	0x2000
#define A_UNUSED8   	0x4000
#define A_UNUSED9   	0x8000


#define MAXENTRIES	2000
#define MAXPATH		66
#define MAXDIRS		60
#define PAGELEN		23
#define NUMCOLS		5
#define EXTSIZE		4


#define ERROR -1

#define newline()			printf("\n")
#define opposite(val)   (Options.reverse_order ? -val : val)

unsigned Pagelen;
unsigned entries = 0;									/* offset into entryptrs	*/
PFILEFINDBUF entryptrs[MAXENTRIES];					/* ptrs to entries			*/
unsigned dirdrive = 0;

char *wild_arg = "*.*";
char *blanks = "   ";

struct _options
    {
    USHORT ext_sort;
    USHORT long_format;
    USHORT time_sort;
    USHORT reverse_order;
    USHORT dir_format;
    USHORT column_format;
    USHORT file_format;
    USHORT paging;
    USHORT single_column;
    USHORT no_sort;
    USHORT attrval;
    } Options;

long totalspace = 0L;
long availspace = 0L;

void		main(int argc,char **argv);
void		ls(int argc, char **argv);
void		get_freespace(unsigned drive, long *totalspace, long *availspace);
void		print_entries(void);
int		qscmp(PFILEFINDBUF *p1, PFILEFINDBUF *p2);
int		set_options(char *opt);
void		dopage(USHORT c);
void		usage(void);
int		haswild(char *str);
void		makedirname(char *to,char *from);
void		set_attribute_values(char *str, unsigned attribute);
unsigned	get_attrval(char *c);
int		column_lines(int num);
BOOL		read_options(int *argc, char ***argv);

void main(int argc,char **argv)
    {
    ls(argc,argv);
    }

void ls(int argc, char **argv)
   {
	VIOMODEINFO v;
	PVOID	header = NULL, resulthdl;
	USHORT i, j, numresults, numrequests;
	PCH	curdir;

	if(!read_options(&argc, &argv))					/* read/set the options		*/
		return;

	for( i = 1; i < argc; i++)							/* get each filespec 		*/
		DiMakeRequest(&header, argv[i], Options.attrval);

	DiSendRequest(header);								/* send the request			*/

	DiGetNumResults(header, &numresults, &numrequests);

	for( i = 0; i < numrequests; i++)				/* for each request			*/
		{
		DiGetResultHdl(header, i, &numresults, &resulthdl);

		if(!i)
			{
			curdir = DiGetResultDir(resulthdl);
			get_freespace((*curdir-'A'+1), &totalspace, &availspace);
			}

		for( j = 0; j < numresults; j++)				/* get each filename in req.*/
			if(!j)
				DiGetFirstResultPtr(resulthdl,&entryptrs[entries++]);
			else
				DiGetNextResultPtr(resulthdl,&entryptrs[entries++]);
		}

	if(!Options.no_sort)									/* if sort required, do it	*/
		qsort(entryptrs, entries, sizeof(PFILEFINDBUF), qscmp);

   if(!Options.column_format && !Options.single_column)
      Options.long_format = TRUE;

   if(Options.paging)									/* if paging required		*/
      {
      v.cb = sizeof(v);
      VioGetMode(&v,0);
      Pagelen = v.row-2;								/* set page length			*/
      }

   print_entries();										/* print the results			*/

	DiDestroyRequest(&header);							/* throw out results			*/

	DosExit(EXIT_PROCESS,0);							/* I'm outta here				*/
	}


BOOL read_options(int *argc, char ***argv)
	{
	int nargc;
	char **nargv;
	static char *wargv[3];

   memset(&Options,FALSE,sizeof(Options));	/* init options to FALSE 		*/

	nargv = *argv;
	nargc = *argc;

	if(nargc > 1 && *nargv[1] == '-')			/* if options present			*/
		{
		if(!set_options(nargv[1]))					/* if only usage message		*/
			return FALSE;
															/* shift pointers down one		*/
		memmove(&nargv[1],&nargv[2],(nargc-2)*sizeof(char *));
		nargc--;											/* adjust nargc					*/
		nargv[nargc] = NULL;
		}

	if(nargc == 1)										/* if only the programname		*/
		{
		wargv[0] = *argv[0];							/* set up new argv array		*/
		wargv[1] = wild_arg;
		wargv[2] = NULL;
		nargv = wargv;									/* set nargv and narc			*/
		nargc = 2;
		}
	*argc = nargc;										/* now set the originals		*/
	*argv = nargv;
	}

int set_options(char *opt)
    {
    for( ; *opt ; opt++)
        switch(tolower(*opt))
            {
            case 'f':
                Options.file_format = TRUE;
                break;
            case 'l':
                Options.long_format = TRUE;
                break;
            case 't':
                Options.time_sort = TRUE;
                break;
            case 'r':
                Options.reverse_order = TRUE;
                break;
            case 's':
                Options.long_format = FALSE;
                Options.single_column = TRUE;
                break;
            case 'c':
                Options.column_format = TRUE;
                break;
            case 'd':
                Options.dir_format = TRUE;
                break;
            case 'p':
                Options.paging = TRUE;
                break;
            case 'e':
                Options.ext_sort = TRUE;
                break;
            case 'n':
                Options.no_sort = TRUE;
                break;
            case 'a':
                Options.attrval = get_attrval(++opt);
                Options.dir_format = Options.file_format = FALSE;
                return TRUE;
                break;
            case '?':
                usage();
                return FALSE;
                break;
            }
    return TRUE;
    }

void usage(void)
    {
    printf("Usage: ls [-flscpternda<rhsad>?] [<list of files>]\n");
    printf("<list of files> := <file> | <file> <list of files>\n");
    printf("     -f: files (non-directories) only\n");
    printf("     -l: long listing (default)\n");
    printf("     -s: single column listing\n");
    printf("     -c: columnar format\n");
    printf("     -p: pause at the end of every screen page\n");
    printf("     -t: time sort\n");
    printf("     -e: extension sort\n");
    printf("     -r: reverse sort order\n");
    printf("     -n: no sort listing\n");
    printf("     -d: directory entries only\n");
    printf("     -a: attributes: only files with <attr>\n");
    printf("     -?: this message\n");
    printf("     [by R.H.Shaw 07/29/89]\n");
    }

void get_freespace(unsigned drive, long *totalspace, long *availspace)
    {
    int flag = FALSE;
    FSALLOCATE fs;

    DosQFSInfo(drive, 1, (PBYTE)&fs, sizeof(fs));
    *totalspace = (fs.cSectorUnit * fs.cUnit * fs.cbSector);
    *availspace = (fs.cSectorUnit * fs.cUnitAvail * fs.cbSector);
    }

int qscmp(PFILEFINDBUF *p1, PFILEFINDBUF *p2)
    {
    PFILEFINDBUF f1 = *p1;
    PFILEFINDBUF f2 = *p2;
    int cmp,val = 0,c2,c1,len,len1,len2;
    char *ext1, *ext2, *name1, *name2;
    register USHORT d, t;

    if(Options.time_sort)								/* if time sort				*/
        {
        d = MAKETYPE(f1->fdateLastWrite,USHORT);/* get date1					*/
        t = MAKETYPE(f2->fdateLastWrite,USHORT);/* get date2					*/
        if(d > t)											/* compare						*/
            val = -1;
        else if(d < t)
            val = 1;
        else												/* dates equal, check time	*/
            {
            d = MAKETYPE(f1->ftimeLastWrite,USHORT);
            t = MAKETYPE(f2->ftimeLastWrite,USHORT);
            if(d > t)
               val = -1;
	         else if(d < t)
               val = 1;
				else
					val = 0;
            }
        return opposite(val);
        }
    else                                        /* name or extension sort   */
        {
        c2 = f2->attrFile & A_DIRECTORY;        /* see either is a directory*/
        c1 = f1->attrFile & A_DIRECTORY;
        if(cmp = (c2-c1))                       /* one is, but not the other*/
            return opposite(cmp);               /* directories are lesser   */

        name1 = f1->achName;
        name2 = f2->achName;
        /* either both are directories or both are files to get here        */
        if(Options.ext_sort)                    /* if extension sort        */
            {
            ext1 = strchr(name1,'.');           /* get the extension        */
            ext2 = strchr(name2,'.');
            if(ext1 && ext2)                    /* if both have extensions  */
                {
                    /* get length of both: lesser length used in strncmp()  */
                len = ( ((len1 = strlen(ext1)) > (len2 = strlen(ext2))) ? len2 : len1);
                return opposite(strncmp(ext1, ext2, len));
                }
            else                     /* at least one doesn't have extension */
                return (opposite((ext1 != NULL) ? -1 : 1));
            }
        else                                    /* boring old name sort     */
            {
                    /* get length of both: lesser length used               */
            len = ( ((len1 = strlen(name1)) > (len2 = strlen(name2))) ? len2 : len1);
            return opposite(strncmp(name1, name2, len));
            }
        }
    }

void print_entries(void)
    {
    int reps, lines, max_lines, col_line, col, item, bal, pagelines;
    char e_buf[100],short_format = FALSE;
    long total_used = 0L, total_alloced = 0L;
    char attrstr[9], ap;
    FILEFINDBUF *p;
    unsigned writehours, i;

    pagelines = max_lines = col_line = lines = reps = col = 0;

    if(Options.column_format)                  /* for column formatting */
        {
            /* if Options.paging and more than a pageful set for a pageful */
        if(Options.paging && (entries>(Pagelen*NUMCOLS)) )
            max_lines = Pagelen;
        else        /* if not Options.paging, or if Options.paging and less than a pageful */
            max_lines = column_lines(entries);
        }
    /*  printf("\nmax_lines=%d,entries/NUMCOLS=%d,entries%NUMCOLS=%d",max_lines,entries/NUMCOLS,entries%NUMCOLS); */
                /* main loop to print file info */
    for(i = 0; i < entries; i++)
        {
        p = entryptrs[i];
            /* if directory, put name in [] */
        if(p->attrFile & A_DIRECTORY)
            makedirname(e_buf,p->achName);
        else
            strcpy(e_buf,p->achName);

        if(Options.long_format)        /* vertical column, with full information */
            {
                /* set attribute chars */
            set_attribute_values(attrstr,p->attrFile);

            writehours = (p->ftimeLastWrite.hours > 12 ? p->ftimeLastWrite.hours - 12 : p->ftimeLastWrite.hours);
            ap = (char)(p->ftimeLastWrite.hours > 12 ? 'p' : 'a');

            printf("\n"
                "%8s   "
                "%12ld   "
                /*         Create                   Last Access      */
/*              "%02d-%02d-%02d %02d:%02d %02d-%02d-%02d %02d:%02d " */
                /*       Last Update          */
                "%2d-%02d-%02d  %2d:%02d%c  "

                /* size name */
                "%s",

                attrstr,
                p->cbFile,
/*
                p->fdateCreation.month,p->fdateCreation.day,p->fdateCreation.year+80,
                p->ftimeCreation.hours,p->ftimeCreation.minutes,
                p->fdateLastAccess.month,p->fdateLastAccess.day,p->fdateLastAccess.year+80,
                p->ftimeLastAccess.hours,p->ftimeLastAccess.minutes,
 */
                p->fdateLastWrite.month,p->fdateLastWrite.day,p->fdateLastWrite.year+80,
                writehours,p->ftimeLastWrite.minutes,ap,
                strlwr(e_buf));
            }
        else
            {                      /* if !Options.long_format */
            if(!Options.column_format)
                {         /* vertical format, filenames only */
                newline();
                printf("%-30.30s", strlwr(e_buf));
                }
            else
                {                      /* column format */
                if(!i)                  /* initial newline */
                    newline();
    /* item is offset into array, col_line is actual line down screen in cols */
    /* col is current screen column, max_lines is number of column lines to do */
                item = col_line + (max_lines * col);    /* item is offset */
                    /*  printf("\nitem=%d,col_line=%d,col=%d ",item,col_line,col); */
                if(item < entries)
                    {
                    /* printf("%d    ",item); */
                    if(entryptrs[item]->attrFile & A_DIRECTORY)
                        makedirname(e_buf,entryptrs[item]->achName);
                    else
                        strcpy(e_buf,entryptrs[item]->achName);

                    printf("%-14.14s", strlwr(e_buf));
                    }
                else
                    i--;
                col++;
                if(col >= NUMCOLS)
                    { /* if we're at last column on this line */
                    newline();          /* write newline() */
                    col = 0;            /* set for 1st column */
                    pagelines++;
                    col_line++;         /* go to next column line */
                    }
                }
            }
            lines++;
        if(Options.paging)
            if(Options.column_format && pagelines == Pagelen)
                {
                pagelines = 0;
                col_line = item+1;
                bal = (entries-i);
                max_lines = column_lines( (bal > (Pagelen*NUMCOLS)) ? (Pagelen*NUMCOLS) : bal );
                /* printf("bal=%d,i=%d,max_lines=%d",bal,i,max_lines); */
                dopage(!Options.column_format);
                }
            else if(lines == Pagelen && !Options.column_format)
                {
                dopage(!Options.column_format);
                lines = 0;
                }
        total_alloced += p->cbFileAlloc;
        total_used += p->cbFile;
        }

    if(reps != 0)       /* only executed on short format (lc) */
        newline();
    if(!short_format)
        newline();
    printf("%u Files occupy %ld bytes(%ld Allocated), ", entries, total_used, total_alloced);
    printf("%ld of %ld free on %c:", availspace, totalspace, dirdrive+'A'-1);
    newline();
    }


void dopage(USHORT c)
    {
    if(c)
        newline();
    printf("\nPress any key to continue...");
    getch();
    newline();
    }

    /* places from into to with [] if a directory */
void makedirname(char *to,char *from)
    {
    strcpy(to,"[");
    strcat(to,from);
    strcat(to, "]");
    }


const char *attrs = "--advshr";
unsigned attrvals[8] =
    {
    A_UNUSED1,  A_UNUSED0, A_ARCHIVE, A_DIRECTORY,
    A_VOLLABEL, A_SYSTEM,  A_HIDDEN, A_READONLY
    };

    /* sets the string to "-----" or "drhsa", etc. */
void set_attribute_values(char *str, unsigned attribute)
    {
    unsigned i, attrvals = A_UNUSED1, len = strlen(attrs);

    memset(str,'-',len);      /* initialize attribute value string */
    str[len] = 0;
    for( i = 0; i < len; i++, attrvals >>= 1)
        if(attrvals & attribute)
            str[i] = attrs[i];
    }

unsigned get_attrval(char *attrstr)
    {
	unsigned *i,newattr = 0;
   const char *p;
	char *q;

    for( q = attrstr; *q ; q++)
        for( p = attrs, i = attrvals; *p ; p++, i++)
            if(*q == *p)
                newattr |= *i;
    return newattr;
    }

int column_lines(int num)
    {
    int max_lines;

    max_lines = num/NUMCOLS;    					/* set the maximum lines 		*/
    if(num%NUMCOLS)             					/* increment for leftovers		*/
        max_lines++;

    return max_lines;
    }

    /************ end of ls.c *********************/
