
/***********************************************************************/
/* CHATT.C - This program was typed from the April 1988 issue of Micro */
/* Systems Journal from an article by Mark Zeiger titled "Using MS-DOS */
/* Functions in C". CHATT changes file mode to or from READ/ONLY,      */
/* SYSTEM, or HIDDEN interactively by first displaying current mode    */
/* and then asking if you wish a change to be made. Wildcards may be   */
/* used. CHATT.C and its functions was written in Eco-88 by the author */
/* and modified by the typist for Quick-C. This file will not work     */
/* with Eco-88. I (the typist) couldn't get the program to work with   */
/* some of Mr. Zeigler's code and made some changes, most notably in   */
/* the format of the way the prompt to change is handled and in the    */
/* method of calling INT 21h. If you have the article you can compare  */
/* the code, if you don't it won't matter anyhow; the program works    */
/* and will compile as is with Quick C in the small memory model. Note */
/* that getzf.asm must be assembled and the two object modules must    */
/* be linked with chatt.obj linked first. Any changes I made are re-   */
/* flected in the remarks and code below.                              */
/***********************************************************************/

/*
Command line parameters

C>chatt [control] [path\]filename [[path\]filename ...]

"Control" parameter specifies whether attribute should be turned on or off
and whether you should be asked if change is to be done.

R specifies READ/ONLY, H specifies hidden, and S specifies SYSTEM. If path is 
not specified, default directouy is used. Filename may include wildcard 
characters. A directory alone is expanded to all files in directory (i.e.
test = test\*.* if test is a directory).

A "+N" in control parameter will allow changes to be made without asking. If 
no control parameter is specified then attributes will be displayed only.
A "-N" in control paremeter is equivilent to omitting this paremeter, you will 
be asked to confirm each change.
The control parameter takes the form of the string +/-R | +/-H | +/-S | +/-N in 
any order, and the control string must start with a + or -)

   examples:

1. chatt +r-h+s+n abc\*.com   Changes all .COM files in directory "abc" to 
                              READ/ONLY and SYSTEM andtakes away HIDDEN 
                              attribute. No prompt is issued for each file

2. chatt -s+h abc\*.* d:*.asm Changes all files in directory "abc" to HIDDEN 
                              and all .ASM files in current directory of drive 
                              "D" to HIDDEN. Also takes away SYSTEM attribute 
                              of those files. You will be prompted if change 
                              is to be done.

3. chatt -s+h abc d:*.asm     Same as above

4. chatt \abc\*.com           Lists attributes of all .COM files in dir \abc.
*/

#include <dos.h>           /* declares REGS */
#include <string.h>
#include <ctype.h>         /* for toupper(), etc. */

#define TRUE 1
#define FALSE 0
#define NULL 0

/* DOS call parameters */

#define  MSDOSS   0x0021   /* MS-DOS interrupt */
#define  SETMODE  0x4301   /* subfunction number in AL */
#define  GETMODE  0x4300   /* subfunction number in AL */
#define  SEARCHF  0x4e00   /* search for first occurance */
#define  SEARCHN  0x4f00   /* search for next occurance */
#define  SETDMA   0x1a00   /* set DMA address */
#define  CONNIO   0x06     /* raw console I/O */
#define  CARRYF   0x0001   /* position of carry flag */
#define  ZEROF    0x0040   /* position of zero flag */

/* file attribute bits for function 43H */

#define  RO       0x0001
#define  HIDDEN   0x0002
#define  SYSTEM   0x0004
#define  VOLUME   0x0008
#define  SUBDIR   0x0010

/* errors from setmode and search commands */

#define  NOFILE   0x0002   /* file not found */
#define  NOPATH   0x0003   /* path not found */
#define  NOACCESS 0x0005   /* access denied */
#define  NOFILES  0x0012   /* no more files on SEARCHN */

/* BIOS interrupt 10h constants */

#define  CURR_VIDEO  0x0f   /* */
#define  READ_CURSOR 0x03   /* */
#define  SET_CURSOR  0x02   /* */
#define  VIDEO_INT   0x0010   /* */

/* structure filled in by SEARCHF command of MS-DOS */

struct DMA {
   char reserved[21];   /* reserved by MS-DOS */
   char attribute;      /* attribute of file */
   unsigned time;
   unsigned date;
   unsigned size_L;     /* file size - low word */
   unsigned size_h;     /* file size - high word */
   char fname[13];      /* parsed name of file */
};

union REGS inregs, outregs;
struct SREGS segregs;

/*---------------------------- MAIN -----------------------------------*/

main(argc, argv)
int argc;
char *argv[];
{
   char atton_off;      /* is it a + or - */
   char path[64];       /* holds path name up to 64 bytes */
   char new_arg[64];    /* might hold subdir concat with \*.*    */
   int bslpos;          /* position of last backslash in path name */
   struct DMA dmabuf;   /* used for MS-DOS search function */
   int i;               /* used to count parameters */
   int carryf;          /* carry flag for MS-DOS calls */

   void fpe(), bad_syntax(), pfname();
   void set_att_masks(), change_att();
   void concat();
   void display_type();
   char lastchar();
   int getpath();

   unsigned att_on_mask, att_off_mask; /* masks to set/reset attributes */
   unsigned ask;                       /* prompt for each change ???    */
   int start_file;               /* which argv[] contains 1st. path\file */
   int display_att_only;         /* flag to display attributes only */

   if (argc < 2)
      bad_syntax();
   atton_off = argv[1][0];
   if (atton_off != '+' && atton_off != '-') {
      start_file = 1;                  /* argv[1] is a path */
      display_att_only = TRUE;
   }
   else {
      start_file = 2;               /* argv[1] is control parameter */
      display_att_only = FALSE;     /* and argv[2] starts files */
      if (argc < 3)                 /* must have at least one file */
         bad_syntax();
      set_att_masks(argv[1], &att_on_mask, &att_off_mask, &ask);
   }

/* set the MS-DOS DMA x'fer address to dmabuf */

   segread(&segregs);      /* put seg regs in structure (need DS) */
   inregs.x.ax = SETDMA;              /* AH = funct 1Ah */
   inregs.x.dx = (unsigned) &dmabuf;
   int86x(MSDOSS, &inregs, &outregs, &segregs);

   for(i = start_file; i < argc; i++) {
      /* changes a subdirectory name to subdir\*.* or changes root to
         full search (\ -> \*.*) or changes to drivespec:\*.* (c: -> c:\*.*)
      */
      if (lastchar(argv[i]) == ':') {
         concat(new_arg, argv[i], "\\*.*");
         argv[i] = new_arg;
      }
      else if(lastchar(argv[i]) == '\\') {
         concat(new_arg, argv[i], "*.*");
         argv[i] = new_arg;
      }
      else {      /* Subdirectory ? or file ?   */
         segread(&segregs);      /* put seg regs in structure (need DS) */
         inregs.x.ax = GETMODE;  /* AX = 4300 (Int 43h, 00 = 'get' mode */
         inregs.x.dx = (unsigned) argv[i];
         int86x(MSDOSS, &inregs, &outregs, &segregs);
         carryf = outregs.x.cflag;
         if(outregs.x.cx == SUBDIR) {
            concat(new_arg, argv[i], "\\*.*");
            argv[i] = new_arg;
         }
      }

      bslpos = getpath(argv[i], path);

      fpe("\n====================> ");
      fpe(" >====================\n\n");

      segread(&segregs);      /* put seg regs in structure (need DS) */
      inregs.x.ax = SEARCHF;             /* AH = funct 4Eh */
      inregs.x.dx = (unsigned) argv[i];
      inregs.x.cx = 0x001f;              /* search for file with any attri- */
      int86x(MSDOSS, &inregs, &outregs, &segregs);  /* bute except archive */
      carryf = outregs.x.cflag;

      if ((carryf & CARRYF) == CARRYF) {
         if ((outregs.x.ax == NOFILES) || (outregs.x.ax == NOPATH )) {
            fpe("\n");
            fpe(argv[i]);
            fpe(" not found\n\n\r");
         }
         else {
            fpe("\nUndefined error\n\007");
            exit(1);
         }
      }

      while((carryf & CARRYF) != CARRYF) {
         pfname(path, &(dmabuf.fname[0]), bslpos);
         fpe(path);     /* echo path and/or file */
         display_type(dmabuf.attribute);        /* echo current attribs */
         if ((display_att_only != TRUE) && (dmabuf.attribute != SUBDIR))
            change_att(path, dmabuf.attribute, &att_on_mask,&att_off_mask,&ask);
         else
            fpe("\n");

         segread(&segregs);      /* put seg regs in structure (need DS) */
         inregs.x.ax = SEARCHN;  /* AX = 4F00 (4Fh= 'search for next match' */
         int86x(MSDOSS, &inregs, &outregs, &segregs);
         carryf = outregs.x.cflag;
      }
   }        /* end of for loop */
   exit(0);
}           /* end of main */

/*-----------Funct set_att_masks---------------------------------------*/
/*
   When it comes time to set or reset attributes, directory attribute byte 
   will be "or"ed with "att_on_mask" to set attribute bits we want on and then 
   "and"ed with "att_off_mask" to reset attributes we want off. This is done 
   in function "change_att()".
*/

void set_att_masks(att_string, att_on_mask, att_off_mask, ask)
char *att_string;
unsigned *att_on_mask, *att_off_mask, *ask;
{

int sub_str();

   *att_on_mask = 0;
   *att_off_mask = 0xffff;

   if (sub_str(att_string, "+R"))  *att_on_mask |= RO;
   else if (sub_str(att_string, "-R"))  *att_off_mask ^= RO;

   if (sub_str(att_string, "+H"))  *att_on_mask |= HIDDEN;
   else if (sub_str(att_string, "-H"))  *att_off_mask ^= HIDDEN;

   if (sub_str(att_string, "+S"))  *att_on_mask |= SYSTEM;
   else if (sub_str(att_string, "-S"))  *att_off_mask ^= SYSTEM;

   if (sub_str(att_string, "+N"))  *ask = FALSE;
   else *ask = TRUE;
}

/*----------------------function sub_str()-----------------------------*/

/*
   Returns TRUE if substring (+R,-H, etc.) is contained in first string or
   FALSE if not. Case insensitive.
*/

int sub_str(main_string, sub_string)
char *main_string, *sub_string;
{

static int *pos, temp;
int len_main, len_sub;
int xstrcomp();

   pos = &temp;         /* point to any int to initialize */
   len_main = strlen(main_string);
   len_sub = strlen(sub_string);
   if (len_main < len_sub)
      return FALSE;
   for (*pos = 0; *pos < len_main - len_sub + 1; (*pos)++)
      if (xstrcomp(main_string, sub_string, pos, len_sub))
         return TRUE;
   return FALSE;
}

/*------------------------function xstrcomp()--------------------------*/

/*
   Compares "sub_string" substring with "main_string" starting at "pos" 
   with a length of "length". Case insensitive.
*/

int xstrcomp(main_string, sub_string, pos, length)
char *main_string, *sub_string;
int *pos, length;
{

int j;

   for( j = 0; j < length; (*pos)++, j++) {
      if(toupper(*(main_string + *pos)) != toupper(*(sub_string + j))) {
         if(!(isalpha(*(main_string + *pos))))     /* if not alphabetic */
            (*pos)++;
         return FALSE;
      }
      continue;
   }
   return TRUE;
}

/*------------------------function concat()----------------------------*/

/*
   Concatenates "first" string followed by "second" and puts result in "new" 
   string.
*/

void concat(new, first, second)
char *new, *first, *second;
{

   while(*first != (char) NULL) {
      *new = *first;
      first++;
      new++;
   }
   while(*second != (char) NULL) {
      *new = *second;
      second++;
      new++;
   }
   *new = (char) NULL;
}

/*--------------------------function lastchar()------------------------*/

/*
   returns the last (non-null) character of a string
*/

char lastchar(string)
char *string;
{
   return string[(strlen(string) - 1)];
}

/*-------------------------function getpath()--------------------------*/

/*
   Puts path name (without file name) in array "path" and returns position of 
   last backslash. If no path name (default dir) then puts NULL string in 
   path[] and sets bslpos to -1. A path is also considered to be a drive 
   specification without a subdirectory, hence the test for(:).
*/

int getpath(argvs, path)
char *argvs, *path;
{

static char ch;
register int i;
int bslpos = -1;

   for(i = 0; (ch = argvs[i]) != (char) NULL; i++) {
      if (ch == '\\' || ch == ':')
         bslpos = i;
   }
   if (bslpos != -1)
      for(i = 0; i <= bslpos; i++)
         path[i] = argvs[i];
   path[i] = NULL;
   return bslpos;
}

/*--------------------------function pfname()--------------------------*/

/*
   Puts file name (from dmabuf.fname) at end of path (which contains 
   pathname).  bslpos is position of last backslash.
*/

void pfname(path, fname, bslpos)
char *path, *fname;
int bslpos;
{

register i, j;
static char ch;

   j = bslpos != -1 ? bslpos + 1 : 0;
   for (i = 0; (ch = fname[i]) != (char) NULL; i++, j++)
      path[j] = fname[i];
   path[j] = (char) NULL;
}

/*------------------function change_att()------------------------------*/

/*
   Changes attribute of file after query (unless "ask" is FALSE). No change if 
   subdirectory.
*/

void change_att(fullname, att, att_on_mask, att_off_mask, ask)
char *fullname;
unsigned att, *att_on_mask, *att_off_mask, *ask;
{

static unsigned char answer;
static int carryf;
char input();
void echo(), state_err(), fpe();

   if (*ask == TRUE) {
      fpe("     Change? ");
      do {
         answer = input();
         if (answer == 3)        /* abort if ^C */
            exit(2);
      }
      while ((toupper(answer) != 'Y') && (toupper(answer) != 'N'));
      echo(answer);
   }
   else answer = 'Y';         /* force answer without asking */
   if(toupper(answer) == 'Y') {
      segread(&segregs);      /* put seg regs in structure (need DS) */
      inregs.x.ax = SETMODE;  /* AX = 4301, (Int 43h, 01 = 'set' mode  */
      inregs.x.cx = att | *att_on_mask;
      inregs.x.cx = (inregs.x.cx & *att_off_mask) & 0x00ff;
      inregs.x.dx = (unsigned) fullname;
      int86x(MSDOSS, &inregs, &outregs, &segregs);
      carryf = outregs.x.cflag;
      if ((carryf & CARRYF) == CARRYF)
         state_err(outregs.x.ax);
   }
   fpe("\n");
}

/*----------------------function state_err-----------------------------*/
/*
   States error returned in AX register after change attribute call. Will 
   abort if no file or incorrect path since this indicates program can not 
   start. Will not abort on "Access Denied" since this can happen with 
   subdirectories.
*/

void state_err(rax)
unsigned rax;
{

void fpe();

   rax = rax & 0x00ff;        /* isolate AL register */
   if (rax == NOFILE)
      fpe("\nFile not found.\007");
   if (rax == NOPATH)
      fpe("\nPath not found.\007");
   if (rax == NOACCESS)
      fpe("    Access denied.\007");

   if ( !((rax == NOFILE) || (rax == NOPATH) || (rax == NOACCESS )))
      fpe("\n\007Undefined error.");
   if (rax == NOACCESS) {
      fpe("\n");
      exit(1);
   }
}

/*---------------------function bad_syntax()---------------------------*/
/*
   Indicates command line error and shows correct syntax.
*/

void bad_syntax()
{

void fpe();

   fpe("\nIncorrect syntax\007");      /* 361 is octal for +/-  */
   fpe("\n\nUsage: chatt [\361R | \361S | \361H | \361N] file file ...\007");
   fpe("\n\n\361R = R/O on/off.");
   fpe("\n\361S = SYSTEM on/off.");
   fpe("\n\361H = HIDDEN on/off.");
   fpe("\n+N = Change without asking.");
   fpe("\nControls may be strung together.");
   fpe("\n   i.e. +R-H+N+S or -S+R+N");
   fpe("\n\nIf no control, then file attributes are displayed only.\n");
   exit(1);
}

/*------------------function display_type()----------------------------*/
/*
   Displays attribute of file. Called from pfname() which gets attribute from 
   dmabuf.attribute.
*/

void display_type(att)
char att;
{

void fpe(), cursor_col();

   cursor_col(30);
   if ((att & 0x1f) == 0)
      fpe(" NORMAL");
   else {
      if ((att & RO) == RO)
         fpe(" READ/ONLY");
      if ((att & HIDDEN) == HIDDEN)
         fpe(" HIDDEN");
      if ((att & SYSTEM) == SYSTEM)
         fpe(" SYSTEM");
      if ((att & VOLUME) == VOLUME)
         fpe(" VOLUME");
      if ((att & SUBDIR) == SUBDIR)
         fpe(" SUB-DIRECTORY");
   }
}

/*------------------------function fpe()-------------------------------*/
/*
   Prints string using MS-DOS console I/O function. This is because fputs() 
   library function is incredibly slow. Also tests for console input and 
   pauses if any key is pressed. Will resume when any key is pressed again.
*/

void fpe(string)
char *string;
{

static unsigned char ch, ch1;
void echo();
char input();

   while (( ch = *string++) != (unsigned char) NULL)
      if (ch != '\n')
         echo(ch);
      else {
         echo('\015');        /* ASCII CR */
         echo(ch);            /* now do linefeed */
      }
      if ((ch1 = input()) != (unsigned char) NULL)
         if (ch1 == 3)
            exit(1);
         else while ((ch1 = input()) == (unsigned char) NULL)
            if (ch1 == 3)
               exit(1);
}


/*---------------function cursor_col()---------------------------------*/
/*
   Places cursor at column "col" of cursor line using BIOS int 10h
*/

void cursor_col(col)
int col;
{

static unsigned char current_page;

   inregs.h.ah = CURR_VIDEO;     /* Get current page funct */
   int86(VIDEO_INT, &inregs, &outregs);
   current_page = outregs.h.bh;

   inregs.h.ah = READ_CURSOR;    /* Read cur pos funct call */
   inregs.h.bh = current_page;   /* Page number */
   int86(VIDEO_INT, &inregs, &outregs);

   inregs.h.ah = SET_CURSOR;     /* Set cur pos funct call */
   inregs.h.bh = current_page;   /* Page number */
   inregs.h.dh = outregs.h.dh;   /* row (y) from fumct 3 above */
   inregs.h.dl = outregs.h.dl;   /* col (x) from funct 3 above */
   int86(VIDEO_INT, &inregs, &outregs);
}

/*-----------------------function input()------------------------------*/
/*
   Will return NULL if not character ready, or character. Does not wait for 
   input. Uses MS-DOS function AH = 6 for raw console I/O. Can not be used for 
   function key input.
*/

char input()
{

static unsigned int zerof;    /* used for return of zero flag */
extern int getzf();

   inregs.h.ah = CONNIO;              /* AH = funct 06h */
   inregs.h.dl = 0xff;           /* input subfunction */
   int86x(MSDOSS, &inregs, &outregs, &segregs);
   zerof = getzf();        /* ASM routine to get zero flag */

   if ((zerof & ZEROF) == ZEROF)
      return (char) NULL;
   else
      return outregs.h.al;
}

/*-----------------------function echo()-------------------------------*/
/*
   Echos character to console using MS-DOS function AH = 6.
*/

void echo(ch)
unsigned char ch;
{

   inregs.h.ah = CONNIO;
   inregs.h.dl = ch;
   int86(MSDOSS, &inregs, &outregs);
}

