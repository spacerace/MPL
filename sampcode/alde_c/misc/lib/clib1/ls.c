/*
NAME
	dirc	-   UNIX-"ls" like directory listing utility written in 'C'.
		    Tested under PC-DOS using Lattice C rel 2.11.
		    This version allows limited recursive directory searches.

AUTHOR
	Marty Ross,	Computing Resource Center,	Santa Monica, Ca.

	For updates and more utilities of the like, call CRC-PC, or CRCBBS
	bulletin board system(s).  Currently at:

	(213) 829-1487	(300 baud, IBM-PC)
	(213) 828-1331  (300 baud, VM/370) <-- type "LOG DEMO DEMO" for access

	NOTE: We are moving around late OCT., 1984 so above numbers will be
	      invalid after OCT, 1984.

SYNOPSIS 
   [pc/ms-dos]
	A>dirc	[-options] [d:][/path/]filename[.ext] 
	
   [as C function]
	ret =   dirc(s,op);
		char *s;	/* Pointer to [d:][/path/]filename[.ext] */
		char *op;	/* Pointer to option string */
				/* Will 'ret' urn 0 normally, or 2 if no find */
where:	
	options	 
		  Is a string of lower case letters selecting
		how many columns to generate, which files, according to
		file attribute, should be displayed, and the selection
		of file size/date of last access, volume header display,
		subdirectory/time of creation, and file attribute bit
		display.  The option string "shrvdzt1" for instance,
		will generate one column (1), will display system files
		(s), hidden files (h), read-only files (r), volume-header
		entry [files] (v), subdirectories (d), siZe (z), and
		date-Time (t) information when it outputs its list.
		An added feature in this version is the 'e' option, which
		will also enable the 'd' option to show subdirectories
		(which match the filename - usually '*.*'), and invoke
		itself recursively for that subdirectory.  Thus, an entire
		hard-disk or other tree-structured file system can be
		completely scanned with one command.

		Note that because of DOS interfacing conventions, only
		directories which match the file specification you typed
		on the command line (if none, '*.*') will be shown.  This 
		disables a potentially very useful feature: ability to 
		selectively scan across directories for particular files.  
		I will most likely include that feature in the next release
		of DIRC, when I invent my own subroutines to handle wildcard
		symbols myself - i'll always ask DOS for *.* and do the
		filtering myself!


	option letter		meaning
	-------------
		
		1		Generate one column/line output.
		2		Generate two columns/line output.
		3		Generate three columns/line output.
		4		Generate four colums/line output.

		a		Archive bit.  Set on when file written	
				to since last FDISK backup.  Allows DIRC to
				select only files changed since last backup.
		
		b		Bits.  Causes file access (mode) bits to be 
				displayed before each filename.

		d		Directories.  Includes Subdirectories 
				in file searches.
		
		e		Expand.  Expand sub-directories which match
				given filename wildcard.  Should use "*.*"
				to get all subdirectories. 
	
		h		Hidden files. List files with 'hidden' status.
	
		l		Long.  Include '.' and '..' subdir files.

		q		Quiet.  Don't say "[/path/]filesp:" before dir.

		r		Read-Only bit. Files which cannot be written to.

		s		System file. Don't erase these files.

		t		Time & Date.  Include for each file.
		
		u		Un-archive bit.  See files which have not been
				changed since last FDISK BACKUP.

		v		Volume header(s?)

		x		Exclusive.  Don't list "normal" files.

		z		siZe.  Include for each file.

		]		Right Justify filenames in filename field.

		[		Left Justfy filenames.

	----------------


DESCRIPTION

	DIRC provides a flexible directory interface to the PC-DOS
file system when using C under the Lattice compiler.  Output goes
to STDOUT, and for this test version written as a PC-DOS command,
input is via the first two 'argv[]' tokens.  Since it is a demonstration
of the capabilities of the DIRC function only, a simple option passing scheme
(using the second token), was chosen rather than the more standard
UNIX option scheme, where the option comes (optionally) first.


	DIRC was written for PC-DOS using the Lattice C compiler.  The
author claims to be no UNIX expert, but realizes that most of the features
of the program are 'boarrowed' from that system.  The desired goal was a
flexible way to provide directories from within C programs.  The first
result of that effort lies herein, in a test 'shell' which calls DIRC
using 'argv[1]' and 'argv[2]'.

	From PC-DOS, you can specify both file specification and option
string as 'tokens', separating with a space.  Because DIRC options are
lower case only (in the UNIX tradition), YOU MUST SPECIFY ALL OPTIONS 
IN LOWER CASE!  

	Note that stack size is set large in case of deeply nested directories
when recursive option 'e' is used.  If '*** STACK OVERFLOW ***' error STILL
occurs, then you may override the stack size, using the Lattice Compiler, by
typing an equal sign, followed by the desired stack size, in decimal.  

I.E.:

A>dirc =30000 *.*
*/

int	_stack	=	25000;		/* large stack in case of recursion! */

/* Following imitations of key routines in SEDIT.H */
int newline()
{
printf("\n");
return 0;
}

spout(line,arg1,arg2)
char *line;
int arg1, arg2;
{
printf("%s",line);
}

/* Start of DIRC.C */
#include	<dos.h>		/* Note: Tested under Lattice Rel 2.11 */
#define		BLANK		" "	/* For SPOUT */
#define		DEF_OPTS	""	/* Use internal defaults now */
#define		DEF_FN		"*.*"

main(argc,argv)
int argc;
char *argv[];
{
int i, rc;
char *opts, *fn;

if (argc<2) {
	opts = DEF_OPTS;
	fn = DEF_FN;
	}
else if (argv[1][0]=='-') {
	if (argc<3) fn = DEF_FN;
	else fn = argv[2];
	opts = argv[1];	/* If options */
	}
else {
	opts = DEF_OPTS;
	fn = argv[1];
	}
rc=dirc(fn,opts);
if (rc && rc!=2) printf("R(%d);\n",rc);
}


#define		FIND_FILE	0x4E		/* DOS find file command */
#define		NEXT_FILE	0x4F		/* DOS next file command */
#define		NORMAL_ATTR	0x20		/* "NORMAL" file attr */
#define		DTA_SIZE	128		/* Size of DOS DTA */
#define		B_F_WIDTH	13		/* 'vanilla' field width */
#define		B_DIR_TYP	0		/* Default 'type' for DIR */
#define		B_DIR_OPT	0x0B		/* File and size only dflt */
#define		K		1024		/* What is a 'K' ? */
#define		NA		0		/* No Argument - don't care */
#define		SET_DTA		0x1A		/* Set DTA address DOS SVC */

char *filedate(), *filetime();			/* Forward references */

		/* Our attribute byte fields */
#define		F_RONLY		1		/* Read only bit */
#define 	F_HIDDEN	2		/* File hidden */
#define		F_SYSTEM	4		/* System file */
#define		F_VOLUME	8		/* Volume */
#define		F_SUBDIR	16		/* Subdirectory */
#define		F_ARCHIVE	32		/* Archived */
#define		F_B6		64
#define		F_B7		128

char	fatbits[]	= "rhsvda!?";		/* Meaning of attribute bits */

		/* Our option bit flags image */
#define		O_WIDTH		3		/* Two bit width field */
#define		O_NONORMAL	4		/* Don't show normal files */
#define		O_ONLYARCH	8		/* Only show archived files */
#define		O_NOARCH	16		/* Only show non archived fls */
#define		O_DATE		32		/* Include date/time */
#define		O_SIZE		64		/* Show size of file */
#define		O_ATTRIBS	128		/* Show file attribute bits */
#define		O_QUIET		256		/* Quiet flag */
#define		O_EXPAND	512		/* Expand sub-dirs */
#define		O_RJUST		1024		/* Right- justify */
#define		O_LONG		2048		/* Include '.' and '..' */
	
#define		COLON		':'
#define		ESCSYM		'\\'
#define		ESCSTR		"\\"


	/* k l u d g e macro (sorry) */
				/* on if attrib selected */
#define	  L_Field	(((options&O_ATTRIBS)?9:0)+f_width)   

int mdirfile(fn,type,options) 
char *fn;
char type;
unsigned options;
{
int nff;
int i, cp, rc;
int r_col, f_width;
char entr[81];					/* maximum length output tok */
char spbuf[10];
char temp[40];
char extbfr[128];			/* For rcrsv subdir expansion */
unsigned ds,dx, es, bx;
int dat,siz,attrs,rjust;
union {					/* DTA and DIR field image */
	char	disk_buffer[DTA_SIZE];
	struct	{
			char 	 sys_data[21];
			char 	 file_attribute;
			unsigned file_time ;
			unsigned file_date ;
			long	 file_size ;
			char	 fn_ext[13] ;
			}  file_table;
	} my_dta;

static int lvl;  	/* Level of recursion - will be zero when prog begins */

lvl++;						/* First level */
rc = 1001;					/* RC for no string given */
if (*fn != '\0') {
	nff = 0;				/* Zero files found so far */
	f_width = B_F_WIDTH;			/* Fieldsize for plain dir */
	dat = (options & O_DATE);
	siz = (options & O_SIZE);		/* Size flag selected */
	attrs = (options & O_ATTRIBS);		/* Attributes required ? */

	if (siz) f_width += 7;	/* adjust 'per field' size */
	if (dat) f_width += 16;	/* etc...*/

	r_col = ((options & O_WIDTH)+1)*L_Field;
	rjust = (options & O_RJUST);
	getdta(&ds,&dx);		/* Get and save current DTA */
	setdta(0,&my_dta);		/* Set up our own DTA */
	rc = bdos2(FIND_FILE,NA,NA,type,fn);
	if (rc==0) {		/* Look for ALL files */
		while (rc==0) {
			for (cp=0;cp<r_col&&rc==0;rc=bdos2(NEXT_FILE)) {
				if (!(options&O_LONG)) {
					if (!strcmp(my_dta.file_table.fn_ext,".") ||
					    !strcmp(my_dta.file_table.fn_ext,".."))
						continue;
					}
				if (options&O_NONORMAL) {   /* -x */
					if (my_dta.file_table.file_attribute==NORMAL_ATTR)
						continue;
					}
				if (options&O_ONLYARCH) {   /* -a */
					if (!(my_dta.file_table.file_attribute&F_ARCHIVE)) 
						continue;
					}
				if (options&O_NOARCH) {     /* -u */
					if (my_dta.file_table.file_attribute&F_ARCHIVE) 
						continue;
					}
				if (nff++==0 && (options&O_QUIET)==0) {
					sprintf(temp,"%s;\n",fn);
					spout(temp,2,1);
					}
				if (attrs) {    /* -b */
					for (i=7;i>=0;i--) {
						if (my_dta.file_table.file_attribute&(1<<i)) {
							stccpy(spbuf,fatbits+i,2);
							spout(spbuf,2,1);
							}
						else spout("-",2,1);
						}
					spout(BLANK,2,1);
					}
				if (rjust) sprintf(entr,"%12s",my_dta.file_table.fn_ext);
				else sprintf(entr,"%-12s",my_dta.file_table.fn_ext);
				if (siz) {	    /* -z */
					if (my_dta.file_table.file_attribute &
						F_VOLUME) sprintf(temp,"  <vol>");
					else if (my_dta.file_table.file_attribute &
						F_SUBDIR) sprintf(temp,"  <dir>");
					else sprintf(temp,"%6.1fK", 
					   (((float)my_dta.file_table.file_size / (float)K) + (float)0.05) );
					strcat(entr,temp);
					}
				if (dat) {     /* -t */
					sprintf(temp,"  %s %s",
						filedate(my_dta.file_table.file_date),
						filetime(my_dta.file_table.file_time));
					strcat(entr,temp);
					}
				i = f_width - strlen(entr) - 1;
				while(i-->0) spout(BLANK,2,1);
				spout(entr,2,1);
				if ((my_dta.file_table.file_attribute&F_SUBDIR) && (options&O_EXPAND) 
				     && strcmp(my_dta.file_table.fn_ext,"..")
				     && strcmp(my_dta.file_table.fn_ext,".")) {
					spout("\n",2,1);
					mknwdir(extbfr,fn,my_dta.file_table.fn_ext);
					rc = mdirfile(extbfr,type,options);
					cp = 0;
					if (rc==1) break;
					}
				else {
					cp+=L_Field;	/* Account for horiz motion */
					if (cp<r_col) {
						spout(BLANK,2,1);
						if (attrs || dat) spout(BLANK,2,1);
						}
					else if (newline()) {
						rc=1;
						break;
						}
					}
				}	/* End FOR */
			if (rc==1) break;
			} 	/* End WHILE */
		}	/* End IF */
	setdta(ds,dx);			/* Restore callers' DTA */
	} /* endif */

if (nff==0) {
	if (lvl==1) spout("No file(s) found.\n",2,1); /* 1st level only */
	else {
		if (!(options&O_NONORMAL)) {
			sprintf(extbfr,"No files: %s\n",fn);
			spout(extbfr,2,1);
			}
		}
	rc = 2;			/* No files found */
	}
else {
	if (rc==18) rc=0;	/* Get rid of error 18 if files found */
	if (cp && cp<r_col) spout("\n",2,1);
	if (lvl!=1) {
		sprintf(extbfr,"(End of %s)\n",fn);
		spout(extbfr,2,1);
		}
	}
return rc;
}  /* enddir */

int dirc(fn,optlist)
char *fn;
char *optlist;
{
unsigned opts;		/* directory listing option bits */
char type;		/* File attribute */

type = 0;		/* Normal files only */
opts = 0;		/* Default one column */
			/* Default size & date/time */
/* if (!*optlist) opts = opts | O_SIZE | O_DATE;  */  

for (;*optlist;optlist++) {
	switch(*optlist) {
		case '-': ;			/* Freedom of option passing */
	break;	case 'v': type = type | F_VOLUME;	/* Volume headers req'd */
    	break;	case 'r': type = type | F_RONLY;	/* Read-only files */
    	break;	case 'h': type = type | F_HIDDEN;	/* Hidden files */
	break; 	case 'd': type = type | F_SUBDIR;	/* SubDirectories */
	break;	case 's': type = type | F_SYSTEM;	/* System files */
	break;	case '!': type = type | F_B6;		/* B6 ?? */
	break;	case '@': type = type | F_B7;		/* B7 ?? */
	break;	case 'x': opts = opts | O_NONORMAL;	/* Exclusive */
	break;	case 'a': opts = opts | O_ONLYARCH;	/* Archived only */
			  type = type | F_ARCHIVE;	/* Search for it, too */
	break;	case 'u': opts = opts | O_NOARCH;	/* Un archived only */
			  type = ((~F_ARCHIVE)&type);	/* Un-do the archive bit */
	break;	case 't': opts = opts | O_DATE;		/* Show date/time */
	break;	case 'z': opts = opts | O_SIZE;		/* Show size */
	break;	case 'b': opts = opts | O_ATTRIBS;	/* Show attrib bits */
	break;	case 'q': opts = opts | O_QUIET;	/* Shut up */
	break;	case 'e': opts = opts | O_EXPAND; 	/* Expand subdirs */
			  type = type | F_SUBDIR;
	break;	case 'l': opts = opts | O_LONG;		/* Incl. '.' and '..' */
	break;	case ']': opts = opts | O_RJUST;	/* Right justify fn's */
	break;	case '[': opts = opts & ~O_RJUST;	/* Turn off RJUST */
	break;	case '1': ;                           	/* number of columns */
	        case '2': ;
	        case '3': ;
	        case '4': opts = (opts&~(unsigned)3) | ((*optlist)-'1');
	break;  default: {
			spout("Bad internal dir opts\n",2,1);
			return 1;
			}
		}
	}
if (*fn=='\0') fn="*.*";		/* Default for current version */
return (mdirfile(fn,type,opts));
}

mknwdir(buffer,curspec,newdir)
char buffer[], curspec[], newdir[];
{
char mybuf[32];
int i;

split_file(curspec,buffer,mybuf);
i = strlen(buffer);
if (i) {
	if (buffer[--i]!=ESCSYM && buffer[i]!=COLON) strcat(buffer,ESCSTR);
	}	
strcat(buffer,newdir);
strcat(buffer,ESCSTR);
strcat(buffer,mybuf);
}

split_file(curspec,buffer,hisbuf)
char *curspec, *buffer, *hisbuf;
{
int i;

buffer[0] = hisbuf[0] = '\0';
i = strlen(curspec);
if (i) {
	while(curspec[--i]!=ESCSYM && curspec[i]!=COLON && i!=0);
	if (i) stccpy(buffer,curspec,++i+1);
	strcpy(hisbuf,curspec+i);
	}
}

char *filetime(codetime)
unsigned codetime;
{
static char ftime[7];
sprintf(ftime,"%02d:%02d",(codetime&0xF800)>>11,
		      (codetime&0x07E0)>>5);
return(ftime);
}

char *filedate(codedate)
unsigned codedate;
{
static char fdate[11];	

sprintf(fdate,"%02d/%02d/%02d",
	(codedate&0x01E0)>>5,
	(codedate&0x001F),
	((codedate&0xFE00)>>9)+80);
return(fdate);
}

#define		GETDTA	0x2F
#define		SETDTA	0x1A

/* (8)	register = mdos(ah,al,bx,cx,dx)	* Call DOS function (ah), pass parms */
bdos2(ah,al,bx,cx,dx)
char ah,al;
int bx,cx,dx;
{
union 		REGS 	myregs;
myregs.h.ah = ah;
myregs.h.al = al;
myregs.x.bx = bx;
myregs.x.cx = cx;
myregs.x.dx = dx;
intdos(&myregs,&myregs);
return myregs.x.ax;
}

getdta(esptr,bxptr)
unsigned *esptr,*bxptr;
{
union 		REGS 	myregs;
struct 		SREGS	mysregs;
segread(&mysregs);		/* Lattice suggests reading seg regs first */
myregs.h.ah = GETDTA;
intdosx(&myregs,&myregs,&mysregs);
*esptr = mysregs.es;		/* Get DTA outputs to ES:BX */
*bxptr = myregs.x.bx;
}

setdta(es,bx)
unsigned es,bx;
{
union 		REGS 	myregs;
struct 		SREGS	mysregs;
segread(&mysregs);		/* Lattice suggests reading seg regs first */
myregs.h.ah = SETDTA;		/* Assign new DTA function code */
myregs.x.dx = bx;		/* Assign new offset to DTA */
if (es!=0) mysregs.ds = es;		/* If zero, use our data segment */
intdosx(&myregs,&myregs,&mysregs); 	/* Set DTA req's DS:DX as argument */
}
