/*
 *  ansi.h -- header file for ANSI driver information and
 *	      macro versions of the ANSI control sequences
 */

/*****************/
/** ANSI macros **/
/*****************/
 
/* cursor position */
#define	ANSI_CUP(r, c)	printf("\x1B[%d;%dH", r, c)
#define	ANSI_HVP(r, c)	printf("\x1B[%d;%df", r, c)

/* cursor up, down, forward, back */
#define	ANSI_CUU(n)	printf("\x1B[%dA", n)
#define	ANSI_CUD(n)	printf("\x1B[%dB", n)
#define	ANSI_CUF(n)	printf("\x1B[%dC", n)
#define	ANSI_CUB(n)	printf("\x1B[%dD", n)

/* device status report (dumps position data into keyboard buffer) */
#define	ANSI_DSR	printf("\x1B[6n")	

/* save and restore cursor position */
#define	ANSI_SCP	fputs("\x1B[s", stdout)
#define	ANSI_RCP	fputs("\x1B[u", stdout)

/* erase display and line */
#define	ANSI_ED		fputs("\x1B[2J", stdout);
#define	ANSI_EL		fputs("\x1B[K", stdout)

/* set graphic rendition */
#define	ANSI_SGR(a)	printf("\x1B[%dm", a)	

/* set and reset modes */
#define	ANSI_SM(m)	printf("\x1B[=%dh", m)
#define	ANSI_RM(m)	printf("\x1B[=%dl", m)


/**********************/
/** ANSI color codes **/
/**********************/

/* special settings */
#define ANSI_NORMAL	0
#define ANSI_BOLD	1
#define ANSI_BLINK	5
#define ANSI_REVERSE	7
#define ANSI_INVISIBLE	8

/* shift values */
#define ANSI_FOREGROUND	30
#define ANSI_BACKGROUND	40

/* basic colors */
#define ANSI_BLACK	0
#define ANSI_RED	1
#define ANSI_GREEN	2
#define ANSI_BROWN	3
#define ANSI_BLUE	4
#define ANSI_MAGENTA	5
#define ANSI_CYAN	6
#define ANSI_WHITE	7


/*****************************/
/** modes for set and reset **/
/*****************************/

#define ANSI_M40	0
#define ANSI_C40	1
#define ANSI_M80	2
#define ANSI_C80	3
#define ANSI_C320	4
#define ANSI_M320	5
#define ANSI_M640	6
#define ANSI_WRAP	7

/* attribute "position" type */
typedef enum {
	FGND, BKGND, BDR
} POSITION;
