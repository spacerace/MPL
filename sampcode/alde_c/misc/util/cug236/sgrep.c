/*
	HEADER:		CUG236;
	TITLE:		SGREP Text Pattern Replacer (McKe86);
	DATE:		05/17/1987;
	DESCRIPTION:	"Is a modified version of the CUG 152 grep program,
			with the added features of string substitution,
			multiple pattern search, case sensitivity, and scanning
			option.";
	VERSION:	1.0;
	KEYWORDS:	Text filter;
	FILENAME:	SGREP.C;
	SEE-ALSO:	SGREP.DOC;
	COMPILERS:	vanilla;
	AUTHORS:	J. McKeon;
*/

/*
 *
 * The	information  in	 this  document	 is  subject  to  change
 * without  notice  and	 should not be construed as a commitment
 * by Digital Equipment Corporation or by DECUS.
 *
 * Neither Digital Equipment Corporation, DECUS, nor the authors
 * assume any responsibility for the use or reliability of  this
 * document or the described software.
 *
 *	Copyright (C) 1980, DECUS
 *
 * General permission to copy or modify, but not for profit,  is
 * hereby  granted,  provided that the above copyright notice is
 * included and reference made to  the	fact  that  reproduction
 * privileges were granted by DECUS.
 *
 */

#include "stdio.h"

/*
 * grep.
 *
 * Runs on the Decus compiler or on vms.
 * Converted for BDS compiler (under CP/M-80), 20-Jan-83, by Chris Kern.
 * Converted to IBM PC with CI-C86 C Compiler June 1983 by David N. Smith
 * On vms, define as:
 *	grep :== "$disk:[account]grep"	   (native)
 *	grep :== "$disk:[account]grep grep"	(Decus)
 *
 * sgrep.
 *
 * Addition of string substitution capability,
 * multiple pattern search, upper-lower case option, scanning options,
 * and name changed to sgrep, April 1986 by James J. McKeon.
 * Runs on IBM PC or compatibles using ECO C88 compiler.
 *
 * For help type "sgrep ?".  See below for more information.
 *
 * May 1987, William C. Colley, III -- Fixed bug in usage of function fgets()
 * in function compat().  Added several notes about portability to various
 * compilers.  Fixed a glitch in case STAR of function pmatch() that crashed
 * the program when compiled under MSDOS large model.  The glitch occurred
 * when a pointer was (deliberately) run backward past the beginning of an
 * array of char.  The array was exactly on a paragraph boundary, so the
 * pointer decremented from 0x0e5d0000 to 0x0e5dffff instead of to 0x0e5cffff
 * as expected.  A pointer comparison then turned out bogus data and the
 * program went into an infinite loop.  The moral of this story is:  Don't
 * play fast and loose with pointers if you want portability.
 */

/*
 * Portability Note:  The AZTEC C compilers handle the binary/text file
 * dichotomy differently from most other compilers.  Uncomment the following
 * pair of #defines if you are running AZTEC C:
 */

/*
#define getc(f)		agetc(f)
#define putc(c,f)	aputc(c,f)
*/

/*
 * Portability Note:  8-bit systems often don't have header file ctype.h.
 * If your system doesn't have it, uncomment the #define NO_CTYPE_H.
 */

/*
#define NO_CTYPE_H
*/

#ifdef	NO_CTYPE_H
int isalpha(), isalnum(), isdigit(), tolower();
#else
#include <ctype.h>
#endif

char	*documentation[] = {
"Sgrep searches a file for a given pattern and substitutes a pattern.",
"If no substitution is required, the command sgrep -myn corresponds",
"to grep -n and will match only. Output is to the screen and may be",
"re-directed using \">\" at the DOS level. Execute by",
"  sgrep [flags] pattern-file input-file",
"",
"Flags are single characters preceeded by '-':",
"   -c      Only a count of matching lines is printed",
"   -m      Match patterns only, no substitutions.",
"   -n      Each line is preceeded by its line number",
"   -v      Only print non-matching lines",
"   -y      Upper and lower case match.",
"",
0 };

char	*patdoc[] = {
"Each match pattern is on a separate line followed by its substitute",
"pattern, if any, also on a separate line.",

" MATCH PATTERNS",
"The regular_expression defines the pattern to search for.  Upper and",
"lower-case are regarded as different unless -y option is used.",
"x      An ordinary character (not mentioned below) matches that character.",
"'\\'    The backslash quotes any character.  \"\\$\" matches a dollar-sign.",
"'$'    Matches beginning or end of line.",
"'.'    A period matches any character except \"new-line\".",
"':a'   A colon matches a class of characters described by the following",
"':d'     character.  \":a\" matches any alphabetic, \":d\" matches digits,",
"':n'     \":n\" matches alphanumerics, \": \" matches spaces, tabs, and",
"': '     other control characters except new-line.",
"'*'    An expression followed by an asterisk matches zero or more",
"       occurrances of that expression: \"fo*\" matches \"f\", \"fo\"",
"       \"foo\", etc.",
"'+'    An expression followed by a plus sign matches one or more",
"       occurrances of that expression: \"fo+\" matches \"fo\", etc.",
"'-'    An expression followed by a minus sign optionally matches",
"       the expression.",
"'[]'   A string enclosed in square brackets matches any character in",
"       that string, but no others.  If the first character in the",
"       string is a circumflex, the expression matches any character",
"       except \"new-line\" and the characters in the string.  For",
"       example, \"[xyz]\" matches \"xx\" and \"zyx\", while \"[^xyz]\"",
"       matches \"abc\" but not \"axb\".  A range of characters may be",
"       specified by two characters separated by \"-\".  Note that,",
"       [a-z] matches alphabetics, while [z-a] never matches.",
"The concatenation of regular expressions is a regular expression.",

" Scanning options:",
"The default scanning option is match all occurences of the pattern.",
"'@'    at the beginning of the pattern, is equivalent to",
"       \"$: *\", meaning match first non-whitespace pattern.",
"'@e'   at the end of a pattern means that if a match is",
"       found (and a substitution made), end all pattern search",
"       on current line.",
"'@r'   at the end of a pattern means that after a match",
"       (and substitution), rescan line until no match occurs.",

" SUBSTITUTE PATTERNS ",
"The only control character for substitute patterns is \"?\".",
"Any other character represents itself. Only the characters ? and",
" \\ itself need to be quoted.",

"'?n'   where n is a non-zero digit, indicates the position where",
"       the nth wildcard string is to be placed.",
"A wildcard string is any string which is not completely fixed,",
"both as to the characters and number of characters in the string.",
0};

#define LMAX	100
#define PMAX	2000
#define NPATMAX 100

#define CHR	1
#define SCOP	2
#define BEOL	3
#define ANY	4
#define CLASS	5
#define NCLASS	6
#define STAR	7
#define PLUS	8
#define MINUS	9
#define ALPHA	10
#define DIGIT	11
#define NALPHA	12
#define PUNCT	13
#define RANGE	14
#define ENDPAT	15
#define lowopt(x) yflag?tolower(x):x

int	cflag;
int	mflag;
int	nflag;
int	vflag;
int	yflag;

int	debug	=	0;	   /* Set for debug code      */

char	*pp, *psp;

#ifndef vms
char	file_name[81];
#endif

char	lbuf[LMAX], slbuf[LMAX];
char	pbuf[PMAX], spbuf[PMAX];
char	* wldstr[19][2];  /* Location of wildcard strings */
int	nws, npat, rescan, nxtln;
char   *mpp[NPATMAX], *mpsp[NPATMAX];  /* Location of patterns */

void exit();
/*******************************************************/

int main(argc, argv)
int argc;
char *argv[];
{
   register char   *p;
   register int	   c;
   FILE		   *f;
   void cant(), compat(), grep(), help(), usage();

   if (argc <= 1)
      usage("No arguments");
   if (argc == 2 && argv[1][0] == '?' && argv[1][1] == 0) {
      help(documentation);
      help(patdoc);
      return !0;
      }
      p=argv[argc-1];
      if ((f=fopen(p, "r")) == NULL) cant(p);
      p = argv[1];
      if (*p == '-') {
	 ++p;
	 while (c = *p++) {
	    switch(tolower(c)) {

	    case 'c':
	       ++cflag;
	       break;

	    case 'd':
	       ++debug;
	       break;

	    case 'm':
	       ++mflag;
	       break;

	    case 'n':
	       ++nflag;
	       break;

	    case 'v':
	       ++vflag;
	       break;

	    case 'y':
	       ++yflag;
	       break;

	    default:
	       usage("Unknown flag");
	    }
	 }
	p=argv[2];
      }
      compat(p);
      grep(f);
      fclose(f);
      return 0;
}

/*******************************************************/

void cant(s)
char *s;
{
   fprintf(stderr, "%s: cannot open\n", s);
}


/*******************************************************/

void help(hp)
char **hp;  /* dns added extra '*'  */
/*
 * Give good help
 */
{
   register char   **dp;

   for (dp = hp; *dp; dp++)
      printf("%s\n", *dp);
}

/*******************************************************/

void usage(s)
char *s;
{
   fprintf(stderr, "?SGREP-%s\n", s);
   fprintf(stderr,
  "Usage: sgrep [-mycnv] pattern-file input-file. sgrep ? for help\n");
   exit(1);
}

/*******************************************************/


void compile(source)
char *source;		/* Pattern to compile	      */
/*
 * Compile the pattern into global pbuf[]
 */
{
   register char  *s;	      /* Source string pointer	   */
   register char  *lp;	      /* Last pattern pointer	   */
   register int	  c;	      /* Current character	   */
   int		  o;	      /* Temp			   */
   char		  *spp;	      /* Save beginning of pattern */
   char		  *cclass();  /* Compile class routine	   */
   void badpat(), store();

   s = source;
   if (debug)
      printf("Pattern = %s\n", s);
   while (c = *s++) {
      /*
       * STAR, PLUS and MINUS are special.
       */
      if (c == '*' || c == '+' || c == '-') {
	 if (pp == pbuf ||
	      (o=pp[-1]) == SCOP || o == '\0' ||
	      o == BEOL ||
	      o == STAR ||
	      o == PLUS ||
	      o == MINUS)
	    badpat("Illegal occurrance op.", source, s);
	 store(ENDPAT);
	 store(ENDPAT);
	 spp = pp;		 /* Save pattern end	 */
	 while (--pp > lp)	 /* Move pattern down	 */
	    *pp = pp[-1];	 /* one byte		 */
	 *pp =	 (c == '*') ? STAR :
	    (c == '-') ? MINUS : PLUS;
	 pp = spp;		 /* Restore pattern end	 */
	 continue;
      }
      /*
       * All the rest.
       */
      lp = pp;	       /* Remember start       */
      switch(c) {

      case '@':
	 if(s-1 == source) {
	   store(BEOL);
	   store(STAR);
	   store(PUNCT);
	   store(ENDPAT);
	   break;
	 } else {
	 store(SCOP);
	 store(*s++);
	 break;
	 }
      case '$':
	 store(BEOL);
	 break;

      case '.':
	 store(ANY);
	 break;

      case '[':
	 s = cclass(source, s);
	 break;

      case ':':
	 if (*s) {
	    switch(c = *s++) {

	    case 'a':
	    case 'A':
	       store(ALPHA);
	       break;

	    case 'd':
	    case 'D':
	       store(DIGIT);
	       break;

	    case 'n':
	    case 'N':
	       store(NALPHA);
	       break;

	    case ' ':
	       store(PUNCT);
	       break;

	    default:
	       badpat("Unknown : type", source, s);

	    }
	    break;
	 }
	 else	 badpat("No : type", source, s);

      case '\\':
	 if (*s)
	    c = *s++;

      default:
	 store(CHR);
	 store(lowopt(c));
      }
   }
   pp -= 2;		   /* delete linefeed */
   store(ENDPAT);
   store(0);		    /* Terminate string	    */
   mpp[npat+1] = pp;
   if (debug) {
      for (lp = mpp[npat]; lp < pp;) {
	 if ((c = (*lp++ & 0377)) < ' ')
	    printf("\\%x ", c);
	 else	 printf("%c ", c);
	}
	printf("\n");
   }
}

/*******************************************************/

char *cclass(source, src)
char *source;	     /* Pattern start -- for error msg.	     */
char *src;	     /* Class start	      */
/*
 * Compile a class (within [])
 */
{
   register char   *s;	      /* Source pointer	   */
   register char   *cp;	      /* Pattern start	   */
   register int	   c;	      /* Current character */
   int		   o;	      /* Temp		   */
   void badpat(), store();

   s = src;
   o = CLASS;
   if (*s == '^') {
      ++s;
      o = NCLASS;
   }
   store(o);
   cp = pp;
   store(0);			      /* Byte count	 */
   while ((c = *s++) && c!=']') {
      if (c == '\\') {		      /* Store quoted char    */
	 if ((c = *s++) == '\0')      /* Gotta get something  */
	    badpat("Class terminates badly", source, s);
	 else store(lowopt(c));
      }
      else if (c == '-' &&
	    (pp - cp) > 1 && *s != ']' && *s != '\0') {
	 c = pp[-1];		 /* Range start	    */
	 pp[-1] = RANGE;	 /* Range signal    */
	 store(c);		 /* Re-store start  */
	 c = *s++;		 /* Get end char and*/
	 store(lowopt(c));	/* Store it	   */
      }
      else {
	 store(lowopt(c));	/* Store normal char */
      }
   }
   if (c != ']')
      badpat("Unterminated class", source, s);
   if ((c = (pp - cp)) >= 256)
      badpat("Class too large", source, s);
   if (c == 0)
      badpat("Empty class", source, s);
   *cp = c;
   return(s);
}

/*******************************************************/

void store(op)
int op;
{
   void error();

   if (pp >= &pbuf[PMAX])
      error("Pattern too complex\n");
   *pp++ = op;
}

/*******************************************************/

void badpat(message, source, stop)
char  *message;	      /* Error message */
char  *source;	      /* Pattern start */
char  *stop;	      /* Pattern end   */
{
   void error();

   fprintf(stderr, "-GREP-E-%s, pattern is\"%s\"\n", message, source);
   fprintf(stderr, "-GREP-E-Stopped at byte %d, '%c'\n",
	 stop-source, stop[-1]);
   error("?GREP-E-Bad pattern\n");
}

/*******************************************************/

void grep(fp)
FILE	   *fp;	      /* File to process	    */
/*
 * Scan the file for the pattern in pbuf[]
 */
{
   register int lno, count, m;
   int	jpat, mm, match();

   lno = 0;
   count = 0;
   while (fgets(lbuf, LMAX, fp)) {
    ++lno;
    mm = 0;
    for(jpat=1;jpat<=npat;jpat++){
     m = nxtln = 0;
     do {
       rescan = 0;
       if(match(jpat)) m = 1;
     } while(rescan == 1);
     if(m) mm = 1;
      if(nxtln)
	break;
    }/*jpat*/
    if (!mflag)
      printf("%s", lbuf);
    else if(mm && !vflag || !mm && vflag) {
      if(cflag)
	count++;
      else {
	if(nflag)
	  printf("%d\t", lno);
	printf("%s\n", lbuf);
      }
    }
   }/*wh*/
   if (cflag)
      printf("%d\n", count);
}

/*******************************************************/

int match(jp)
/*
 * Match the current line (in lbuf[]), return 1 if it does.
 */
  int  jp;
{
   register char   *l;	      /* Line pointer	    */
   char	 *s, *pat, *lst;
   int km;
   char *pmatch(), *movs();
   void substit();

   km=0;
   pat = mpp[jp];
   l = lst = lbuf;
   while(*l) {
      nws = 0;
      s=pmatch(l,pat);
      if (s){
	if(s < l) break;    /* BOL impossible */
	km = 1;
	if(mflag) break;
	wldstr[0][0]=l;
	wldstr[0][1]=s;
	substit(lst, jp);
	lst = s;
	l = s - 1;
      }
      if(nxtln) break;
      l++;
   }
   if(km && !mflag) {
     substit(lst, 0);
     l = movs(slbuf,slbuf+LMAX,lbuf);
     *l = 0;
   }
   return(km);
}

/*******************************************************/

char *pmatch(line, pattern)
char		   *line;     /* (partial) line to match      */
char		   *pattern;  /* (partial) pattern to match   */
{
   register char   *l;	      /* Current line pointer	      */
   register char   *p;	      /* Current pattern pointer      */
   register char   c;	      /* Current character	      */
   char		   *e;	      /* End for STAR and PLUS match  */
   int		   op;	      /* Pattern operation	      */
   int		   n;	      /* Class counter		      */
   char		   *are;      /* Start of STAR match	      */
   int	ows;
   static int wss;	      /* Wild string switch */
   void error();

   l = line;
   if (debug > 1)
      printf("pmatch(\"%s\")\n", line);
   p = pattern;
   while ((op = *p++) != ENDPAT) {
      if (debug > 1)
	 printf("byte[%d] = 0%o, '%c', op = 0%o\n",
	       l-line, *l, *l, op);
      switch(op) {

      case CHR:
	 c = *l++;
	 if ((yflag ? tolower(c) : c) != *p++)
	    return NULL;
	 break;

      case SCOP:
	 if(*p++ == 'r')
	   rescan = 1;
	 else
	   nxtln = 1;
	 break;

      case BEOL:
	 if((p-1) == pattern && l != lbuf) return(line-1);
	 if((p-1) != pattern && *l != '\n') return NULL;
	 break;

      case ANY:
	 if (*l++ == '\n')
	    return NULL;
	 break;

      case DIGIT:
	 c = *l++;
	 if(!isdigit(c)) return NULL;
	 break;

      case ALPHA:
	 c = *l++;
	 if (!isalpha(c)) return NULL;
	 break;

      case NALPHA:
	 c = *l++;
	 if (!isalnum(c)) return NULL;
	 break;

      case PUNCT:
	 if ((c = *l++) == '\n' || c > ' ')
	    return NULL;
	 break;

      case CLASS:
      case NCLASS:
	 c = *l++;
	 if (yflag) c = tolower(c);
	 n = *p++ & 0377;
	 do {
	    if (*p == RANGE) {
	       p += 3;
	       n -= 2;
	       if (c >= p[-2] && c <= p[-1])
		  break;
	    }
	    else {
	      if(c == '\n')
		return NULL;
	      if (c == *p++) break;
	    }
	 } while (--n > 1);
	  if(op == CLASS && n <= 1) return NULL;
	  if(op != CLASS && n > 1) return NULL;
	 if (op == CLASS)
	    p += n - 2;
	 break;

      case MINUS:
	 e = pmatch(l, p);	 /* Look for a match	*/
	 while (*p++ != ENDPAT); /* Skip over pattern	*/
	 if (e)			 /* Got a match?	*/
	    l = e;		 /* Yes, update string	*/
	 break;			 /* Always succeeds	*/

      case PLUS:		 /* One or more ...	*/
	 wss=1;			 /* wldstr switch on */
	 if (!(l = pmatch(l, p))){
	    wss=0;
	    return NULL;	 /* Gotta have a match	*/
	 }
      case STAR:		 /* Zero or more ...	*/
	 wss = 1;		 /* wldstr switch on */
	 for (are = l; *l && (e = pmatch(l, p)); l = e);
				 /* Get longest match	*/
	 wss = 0;
	 ows = ++nws;		 /* Save wildstring number */
	 while (*p++ != ENDPAT); /* Skip over pattern	*/
	 do {			 /* Try to match rest	*/
	    if (e = pmatch(l, p)){
	     wldstr[ows][0]=are - op + 7; /* Subtract 1 if plus */
	     wldstr[ows][1]=l;
	      return(e);
	     }
	    nws = ows;
	 } while (l-- > are);
	 return NULL;		 /* Nothing else worked */

      default:
	 printf("Bad op code %d\n", op);
	 error("Cannot happen -- match\n");
      }
      if((wss == 0) && (op >= 4) && (op <= 13)){
	++nws;
	wldstr[nws][0] = l-1;  /* single character */
	wldstr[nws][1] = l;
	if(op == 9 && e == 0)
	  wldstr[nws][0] = l;  /* null string */
      }
  }
   return(l);
}

/*******************************************************/

void error(s)
char *s;
{
   fprintf(stderr, "%s", s);
   exit(1);
}

void compat(patfile)	/* Read in pattern file and compile patterns */
char  *patfile;
{
    FILE  *fp;
    char  *l;
    void cant(), compile(), compsub(), error();

    if((fp=fopen(patfile,"r")) == NULL) cant(patfile);
    npat = 0;  mpp[1] = pp = pbuf;  mpsp[1] = psp = spbuf;
    while((l = fgets(slbuf,LMAX,fp)) && *l){
	npat += 1;  compile(slbuf);
	if(!mflag) {
	    if (l = fgets(slbuf,PMAX,fp)) compsub();
	    else
		error("?GREP-E-Odd number of lines in substitution pattern\n");
	}
    }
    if(debug) printf(" npat = %d \n",npat);
    fclose(fp);
}

void compsub()
/* Compile substitute pattern into spbuf */
{
char *l, *p, c;
  if(debug) printf(" Subspat = %s \n",slbuf);
  p = psp;
  for(l=slbuf;*l;l++){
    c = *l;
    switch(c){
    case '?':
      *p++ = 7;
      l++;
      *p++ = (*l - 48);
      break;
    case '\\':
      c = *(++l);
    default:
      *p++ = c;
    }
  }
    p--;	 /* delete linefeed */
    *p = 0;
    psp = ++p;
   mpsp[npat+1] = psp;
   if(debug){
    for(l=mpsp[npat];*l;l++){
      if((c = (*l & 0377)) < ' ') printf("\\%x ",c);
      else printf("%c ",c);
    }
    printf("\n");
   }
}

char  *movs(l1,l2,p)
/* Move string from l1 to p */
char  *l1, *l2, *p;
{
char  *l;
  for(l = l1; l < l2 && *l; l++)
    *p++ = *l;
  return(p);
}

void substit(lb, jpat)	/* construct output record */
char  *lb;
int  jpat;
{
char  *movs(), *p;
static char *sl = slbuf;
int k;
 if(jpat == 0){
  sl =	movs(wldstr[0][1],lbuf+LMAX,sl);
  *sl = 0;
  sl = slbuf;
  return;
 }
  psp = mpsp[jpat];
  sl = movs(lb,wldstr[0][0],sl);
  for(p=psp; *p; p++){
    switch(*p) {
    case 7:
      p++;
      k = *p;
      sl =   movs(wldstr[k][0],wldstr[k][1],sl);
      break;
    default:
      *sl++ = *p;
    }/*sw*/
  }
}
