static char *sccsid = "@(#)usq.c        1.8u (UCF) 83/09/02";
/*
 *      usq.c - CP/M compatible file unsqueezer utility
 *
 *      compile as follows:
 *      cc [-DVAX] -O usq.c -o usq
 *         (define VAX only if running on VAX)
 */

/*      modified by J. Chappell 02-02-84 to check if ferror occurred on write
                                02-07-84 to accept wildcards in filenames
                                09-05-84 to give message for disk full
        modified by M. Sehorne  09-10-84 to correct message output format
*/


#include <stdio.h>
/* #include <signal.h> */
/* #include <ctype.h> */

#define TRUE 1
#define FALSE 0
#define ERROR (-1)
#define PATHLEN 312     /* Number of characters allowed in pathname */
#define OK 0

#define VERSION "1.8u   09-02-83"
#define RECOGNIZE 0xFF76        /* unlikely pattern */
#define DLE 0x90                /* repeat byte flag */
#define SPEOF 256               /* special endfile token */
#define NUMVALS 257             /* 256 data values plus SPEOF*/
#define LARGE 30000

#ifdef VAX   /* then we don't want 32 bit integers */

typedef short INT;
typedef unsigned short UNSIGNED;

#else   /*  16 bit machines  */

typedef int INT;
typedef unsigned UNSIGNED;

#endif

struct _sqleaf {                /* Decoding tree */
        INT _children[2];       /* left, right */
};
struct _sqleaf Dnode[NUMVALS - 1];


INT Bpos;               /* last bit position read */
INT Curin;              /* last byte value read */
INT Repct;              /* Number of times to return value */
INT Value;              /* current byte value or EOF */

INT MakeLCPathname=TRUE;        /* translate pathname to lc if all caps */
INT Nlmode=FALSE;               /* zap cr's if true */
INT Inbackground = FALSE;

INT getcr(), getuhuff(), portgetw();

extern int fflush(),ferror(),fclose();
#define WILDCARD TRUE
#define DOS TRUE

main(argc, argv)
unsigned char *argv[];
{
        register unsigned char *cp;
        register INT npats=0;
        unsigned char **patts;
        INT n, errorstat;

/*
        if (signal(SIGINT, SIG_IGN)==SIG_IGN)
                Inbackground++;
        else
                signal(SIGINT, SIG_DFL);
        signal(SIGHUP, SIG_IGN);
                                        */

        errorstat=0;
        if(argc<2)
                goto usage;
        while (--argc) {
                cp = *++argv;
                if(*cp == '-') {
                        while( *++cp) {
                                switch(*cp) {
                                case 'n':
                                        Nlmode=TRUE; break;
                                case 'u':
                                        MakeLCPathname=FALSE; break;
                                default:
                                        goto usage;
                                }
                        }
                }
                else if( !npats && argc>0) {
                        if(argv[0][0]) {
                                npats=argc;
                                patts=argv;
                        }
                }
        }
        if(npats < 1) {
usage:
                fprintf(stderr,"File squeezer version %s by\n\tRichard Greenlaw\n\t251 Colony Ct.\n\tGahanna, Ohio 43230\n", VERSION);
                fprintf(stderr,"Usage: usq [-nu] file ...\n");
                fprintf(stderr,"\t-n Nlmode: remove carriage returns\n");
                fprintf(stderr,"\t-u preserve Uppercase pathnames\n");
#ifdef WILDCARD
                fprintf(stderr,"\n\tWildcards are acceptable in filenames.\n");
                fprintf(stderr," \tThis means that you can USQ *.?Q?\n");
#endif
                fprintf(stderr,"\n\tUSQ will write the output file to the");
                fprintf(stderr," \n\tcurrently logged drive/directory.\n");
                exit(1);
        }

#ifdef WILDCARD
        for(n=0; n<npats; ++n){
                unsigned char *dir(), *cp, *s;
                if(cp=dir(patts[n],1)){
                   for(s=cp;*s;s+=(strlen(s)+1)) errorstat |= squeeze(s);
                   free(cp);
                }
                else errorstat |= squeeze(patts[n]);
        }
#else
        for(n=0; n<npats; ++n)
                errorstat |= squeeze(patts[n]);
#endif

        exit(errorstat != 0);
}

/*
        The following code is primarily from typesq.c and utr.c.  Typesq
is a modification of USQ by Dick Greenlaw.  Those modifications (usq
to typesq) were made by Bob Mathias, I am responsible for the butchery
done to make it work with cat.

*/

FILE *in, *out;
squeeze(fname)
unsigned char *fname;
{
        register INT i, c;
        register unsigned char *p;
        register INT numnodes;                  /* size of decoding tree */
        register UNSIGNED crc;
        UNSIGNED filecrc;
        unsigned char origname[PATHLEN];        /* Original file name without
                                                  drive */
        long written;   /* number of bytes written */

        init_cr(); init_huff(); crc=0;

        if((in=fopen( fname, "rb"))==NULL) {
                fprintf(stderr, "usq: can't open %s\n", fname);
                return ERROR;
        }
        if(portgetw(in) != (INT) RECOGNIZE) {/* Process header */
                fprintf(stderr, "usq: %s is not a SQueezed file\n", fname);
                return(ERROR);
        }
        filecrc = (UNSIGNED) portgetw(in);      /* checksum */
        p = origname;                           /* Get original file name */
        do {                                    /* send it to array */
                *p = getc(in);
        } while(*p++ != '\0');

        numnodes = portgetw(in);
        if(numnodes < 0 || numnodes >= NUMVALS) {
                fprintf(stderr, "usq: %s has invalid decode tree\n", fname);
                fclose(in);
                return(ERROR);
        }
        /* Initialize for possible empty tree (SPEOF only) */
        Dnode[0]._children[0] = -(SPEOF + 1);
        Dnode[0]._children[1] = -(SPEOF + 1);

        for(i = 0; i < numnodes; ++i) { /* Get decoding tree from file */
                Dnode[i]._children[0] = portgetw(in);
                Dnode[i]._children[1] = portgetw(in);
        }
        /* Get translated output bytes and write file */
        if(MakeLCPathname && !IsAnyLower(origname))
                uncaps(origname);
        for(p=origname; *p; ++p)                /* change / to _ */
                if( *p == '/')
                        *p = '_';
        if (!Inbackground)
                fprintf(stderr,"usq: unsqueezing %s to %s ",fname,origname);
        if((out=fopen(origname, "wb"))==NULL) {
                fprintf(stderr, "usq: can't create %s\n", origname);
        }

        written=0;
        while ((c = getcr()) != EOF) {
                crc += (UNSIGNED) c;
                if ( c == '\r' && Nlmode)
                        continue;
                if(fputc(c,out)!=c){
                        fclose(in);
                        crash(origname);
                }
                else ++written;
        }
        fclose(in);

        if(fflush(out)<0 || ferror(out)<0 || fclose(out)<0)
                crash(origname);

        if( crc != filecrc ) {
                fprintf(stderr, "usq: bad checksum in %s\n", fname);
                fflush(stdout);
                return(ERROR);
        }
        fprintf(stderr,"%8ld bytes written.\n",written);
        return(OK);
}

crash(fn) /* crash due to error writing file */
char *fn;
{

  fprintf(stderr,"\nusq: Error writing file: %s\n",fn);
  fprintf(stderr,"\nThe disk you are writing to is probably full.\n");
#ifdef DOS
  fprintf(stderr,"\nTry running CHKDSK on your destination disk.\n");
#else
  fprintf(stderr,"\nTry running STAT on your destination disk.\n");
#endif
  exit(1);
}

/*** from utr.c - */
/* initialize decoding functions */

init_cr()
{
        Repct = 0;
}

init_huff()
{
        Bpos = 99;      /* force initial read */
}

/* Get bytes with decoding - this decodes repetition,
 * calls getuhuff to decode file stream into byte
 * level code with only repetition encoding.
 *
 * The code is simple passing through of bytes except
 * that DLE is encoded as DLE-zero and other values
 * repeated more than twice are encoded as value-DLE-count.
 */

INT
getcr()
{
        register INT c;

        if(Repct > 0) {
                /* Expanding a repeated char */
                --Repct;
                return(Value);
        } else {
                /* Nothing unusual */
                if((c = getuhuff()) != DLE) {
                        /* It's not the special delimiter */
                        Value = c;
                        if(Value == EOF)
                                Repct = LARGE;
                        return(Value);
                } else {
                        /* Special token */
                        if((Repct = getuhuff()) == 0)
                                /* DLE, zero represents DLE */
                                return(DLE);
                        else {
                                /* Begin expanding repetition */
                                Repct -= 2;     /* 2nd time */
                                return(Value);
                        }
                }
        }
}
/* Decode file stream into a byte level code with only
 * repetition encoding remaining.
 */

INT
getuhuff()
{
        register INT i;

        /* Follow bit stream in tree to a leaf*/
        i = 0;  /* Start at root of tree */
        do {
                if(++Bpos > 7) {
                        if((Curin = getc(in)) == ERROR)
                                return(ERROR);
                        Bpos = 0;
                        /* move a level deeper in tree */
                        i = Dnode[i]._children[1 & Curin];
                } else
                        i = Dnode[i]._children[1 & (Curin >>= 1)];
        } while(i >= 0);

        /* Decode fake node index to original data value */
        i = -(i + 1);
        /* Decode special endfile token to normal EOF */
        i = (i == SPEOF) ? EOF : i;
        return(i);
}
/*
 * Machine independent getw which always gets bytes in the same order
 *  as the CP/M version of SQ wrote them
 */
INT
portgetw(f)
FILE *f;
{
        register INT c;

        c = getc(f) & 0377;
        return(c | (getc(f) << 8));
}


/* make string s lower case */
uncaps(s)
unsigned char *s;
{
        for( ; *s; ++s)
                if(isupper(*s))
                        *s = tolower(*s);
}


/*
 * IsAnyLower returns TRUE if string s has lower case letters.
 */
IsAnyLower(s)
unsigned char *s;
{
        for( ; *s; ++s)
                if (islower(*s))
                        return(TRUE);
        return(FALSE);
}


#ifdef WILDCARD
/*      dir: for DOS ALL

        Entry:
                1. Filename possibly containing wildcards ? and *
                2. Drive specifier flag

        Returns: Pointer to data area containing NULL terminated
                 list of filenames, or NULL if none found.

        Notes: User must free up data allocated by this function
                with free() if memory is to be restored to heap
                after use.

              Drive specifier: if non-zero will drive specifiers
                will be put on file names

              Path names: not supported

*/

unsigned char *dir(filespec,drflag)
unsigned char *filespec;
short drflag;
{
#define BLOCK 100               /* room for fcb,dta,etc.. */
#define FIRST 0x1100            /* search first */
#define NEXT 0x1200             /* search next */
#define CURRENT 0x19            /* get current drive */
#define PARSE 0x2900            /* parse file name */
#define SETDTA 0x1a00           /* set disk transfer address */

unsigned char *fcb,*rets,*cp,*tmp,*dta,*realloc();
unsigned short mode,pos,i;
struct regval { int ax,bx,cx,dx,si,di,ds,es;} r;

  tmp=realloc(0,BLOCK); fcb=realloc(0,BLOCK);
  dta=realloc(0,BLOCK); rets=realloc(0,BLOCK);

  segread(&r.si);                       /* parse file name */
  r.es=r.ds; r.si=filespec; r.di=fcb; r.ax=PARSE; sysint21(&r,&r);

  segread(&r.si);                       /* set dta */
  r.dx=dta; r.ax=SETDTA; sysint21(&r,&r);

  for(pos=0,mode=FIRST;;mode=NEXT){
        segread(&r.si); r.ax=mode; r.dx=fcb; sysint21(&r,&r);   /* search  */
        if(r.ax&0xff==0xff)break;                       /* not found */
        cp=tmp;
        if(drflag){                             /* put on drive specifier */
                if(*dta==0)*dta=(bdos(CURRENT)&0xff)+1;
                *cp++=*dta+'A'-1; *cp++=':';
        }
        for(i=1;i<9;i++)if(*(dta+i)!=' ')*cp++=*(dta+i); *cp++='.';
        for(;i<12;i++)if(*(dta+i)!=' ')*cp++=*(dta+i); *cp='\0';
        rets=realloc(rets,pos+strlen(tmp)+1);   /* store file name */
        strcpy(rets+pos,tmp);
        pos+=(strlen(tmp)+1);
  }

  if(pos)rets=realloc(rets,pos+strlen(tmp)+1);
  else free(rets);
  free(fcb); free(tmp); free(dta);
  return pos ? rets: 0;
}

#endif
