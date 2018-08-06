/*
**          SM.LIB  function source code
**  Copyright 1986, S.E. Margison
**
**  FUNCTION: smdefs.h
** extra defines for compiling programs by S. Margison.
** 03-16-87 A
*/

#define MAXFN	32	/* max filename space */
#define EXTMARK	'.'	/* delimiter for extension of filename */
#define MAXLINE	192	/* maximum text line space */
#define TRUE	1
#define YES	TRUE
#define FALSE	0
#define NO	FALSE
#define BELL	7	/* control-G ASCII bellcode */

/* following are modes for open() commands */

#define O_READ 0
#define O_WRITE 1
#define O_RDWR 2

/* following are english language equivalents to logical operators */
#define is ==
#define isnot !=
#define and &&
#define or ||

extern FILE *fopenp(), *fopeng(), *fopend();
extern char *getenv();

#define LP1 0    /* printer numbers */
#define LP2 1
#define LP3 2
#define SER1 0    /* COM1 port */
#define SER2 1    /* COM2 port */
#define SER3 2    /* COM3 port */
#define SER4 3    /* COM4 port */

