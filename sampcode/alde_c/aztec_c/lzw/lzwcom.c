/* LZWCOM - FILE COMPRESSOR UTILITY                                     */
#include "stdio.h"
#include "debug.h"
#define FALSE    0
#define TRUE     !FALSE
#define TABSIZE  4096
#define NO_PRED  0xFFFF
#define EMPTY   0xFFFF 
#define NOT_FND 0xFFFF 
#define UEOF ((unsigned)EOF)
struct entry {
  char used;
  unsigned int next;      /* hi bit is 'used' flag                        */
  unsigned int predecessor;               /* 12 bit code                  */
  unsigned char follower;
} string_tab[TABSIZE];

/*   routines common to compress and decompress, contained in CommLZW.c */
unsigned hash();
unsigned unhash();
unsigned getcode();
putcode();
init_tab();
upd_tab();

FILE *op;
main(argc,argv)
int argc; char *argv[];
{
  register unsigned int c, code, localcode;
  int code_count = TABSIZE - 256;
  FILE *infd, *outfd;
  if (3 != argc) {
    fprintf(stderr,"Usage : lzwcom oldfilename squeezefilename\n");
    exit(0);
  }
  if ( -1 == (infd = fopen( *++argv, "r")) ) {
    fprintf(stderr,"Cant open %s\n", *argv);
    exit(0);
  }
  if ( -1 == (outfd = fopen(*++argv,"w")) ) {
    fprintf(stderr,"Cant create %s\n",*argv);

    exit(0);
  }
  init_tab();                           /* initialize code table        */
  c = getc(infd);
  code = unhash(NO_PRED,c);             /* initial code for table       */
  DEBUGGER (\
	if (c >= ' ' || c <= '~' || c == '\n' || c == '\r')\
  		putchar(c);\
	else\
		printf("[%2x]",c);\
  )
  while ( UEOF != (c = getc(infd)) ) 
  {
	  DEBUGGER (\
		if (c >= ' ' || c <= '~' || c == '\n' || c == '\r')\
	  		putchar(c);\
		else\
			printf("[%2x]",c);\
	  )
	  if ( NOT_FND != (localcode = unhash(code,c)) ) 
	  {
	      code = localcode;
	      continue;
	  }
/* when the above clause comes false, you have found the last known code */
      putcode(outfd,code);      /* only update table if table isn't full */
	  DEBUGGER(printf( "\n%x\n",code);)
      if ( code_count ) 
	  {
      	upd_tab(code,c);
	  	DEBUGGER(printf("\nadding %x %c = %x\n",code,c,unhash(code,c));)
      --code_count;
      }
/* start loop again with the char that didn't fit into last string      */
      code = unhash(NO_PRED,c);
  }
  putcode(outfd,code);                  /* once EOF reached, always     */
                                        /* one code left unsent         */
  DEBUGGER(fprintf(stderr,"\n%x\n",code);)
  flushout(outfd);                      /* make sure everything's written */
  exit(0);                              /* make sure everything gets closed */
}

