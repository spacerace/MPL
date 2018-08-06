
 /*
  *
  *	Turbo Pascal To C Translator.
  *	Standard Include File.
  *
  * Glockenspiel Ltd. Dublin. Ireland.
  *
  */

typedef char T2C_string_type[256];

#include <malloc.h>


#ifndef FALSE
#define		TRUE	1
#endif
#ifndef FALSE
#define		FALSE	0
#endif
#define     T2C_PI  (3.1415926536)
#define 	NIL 	((char *)0)

#ifndef NULL
#define NULL ((char *)0)
#endif

#include 	<string.h>

#define  T2CA_str_assign(x,y)	strcpy((x),(y))
#define	 T2CA_arr_assign(x,y,s)	memcpy((x),(y),(s))
#define  T2C_ORD(x)	((int)(x))
#define  T2C_SUCC(x)	(1+(x))
#define  T2C_PRED(x)	((x)-1)
#define  T2C_PRED_BOOL(x)	(!(x))
#define  T2C_CHR(x)	((char)(x))
#define	 T2C_LENGTH(x)	((int)strlen(x))
#define  T2C_LONG_PTR(x,y) ((((unsigned long)(x))<<4L) + (unsigned long)(y))
#define  INIT_STR_PASS_BY_VALUE char * Tpbv_ptr
#define  STR_PASS_BY_VALUE(x,y) Tpbv_ptr=alloca(y);x=strcpy(Tpbv_ptr,x)
#define  SET_PASS_BY_VALUE(x)   Tpbv_ptr=alloca(32);x=memcpy(Tpbv_ptr,x,32)


typedef int bool;
typedef unsigned char byte;

typedef struct file_info {
	char file_name[30];
	FILE * file_ptr;
	int blocksize;
	int rec_size;
	} FILE_INFO, *PTR_FILE;

FILE_INFO T2C_KBD = { "KBD",stdin,1,1};
FILE_INFO T2C_CON = { "CON",stdout,1,1};
FILE_INFO T2C_TRM = { "TRM",stdout,1,1};
FILE_INFO T2C_USR = { "USR",stdout,1,1};
FILE_INFO T2C_AUX = { "AUX",stdaux,1,1};
FILE_INFO T2C_LST = { "LST",stdprn,1,1};


/*
 * TURBO PASCAL CONSTANT DEFINITIONS 
 *
 */
#define MAXINT 32767
#define Black	0
#define Blue	1
#define Green	2
#define Cyan 	3
#define Red 	4
#define Magenta	5
#define Brown	6
#define LightGray 7
#define DarkGray	8
#define LightBlue	9
#define LightGreen	10
#define LightCyan	11
#define LightRed	12
#define LightMagenta	13
#define	Yellow		14
#define White		15

#define North		0
#define East		90
#define West		270
#define South		180

/*
 *   Used to emulate { $I+ } and {$C+ } directives.
 */
extern int IFLAG;
extern int CFLAG;

#define CHAR_TYPE	0
#define STRING_TYPE	1

 /*
  * Header info. for Turbo Pascal Emulation ( set handling ( internal ) )
  *
  * Anthony Phillips,
  * Glockenspiel Ltd. Dublin. Ireland.
  *
  */

typedef unsigned char   set[32];
typedef unsigned char * TURBOSET;
typedef int boolean;


#define  SETEND   -1
#define  SETTO    -2

extern  TURBOSET    T2CSOP_tsmake   ();
extern  TURBOSET    T2CSOP_tsunion  ( TURBOSET, TURBOSET );
extern  TURBOSET    T2CSOP_tsdiff   ( TURBOSET, TURBOSET );
extern  TURBOSET    T2CSOP_tsinter  ( TURBOSET, TURBOSET );
extern  TURBOSET    T2CSOP_tsassign ( TURBOSET, TURBOSET );
extern  boolean     T2CSOP_tsincl   ( int,     TURBOSET );
extern  boolean     T2CSOP_tseq     ( TURBOSET, TURBOSET );
extern  boolean     T2CSOP_tsnoteq  ( TURBOSET, TURBOSET );
extern  boolean     T2CSOP_tslesseq ( TURBOSET, TURBOSET );
extern  boolean     T2CSOP_tsgrteq  ( TURBOSET, TURBOSET );

extern  void        set_elem ( TURBOSET, int );
extern  void        del_elem ( TURBOSET , int );

extern  void        setturboerror( char * );

#ifdef DEBUG
extern  void        out_char ( unsigned char );
extern  void        out_set  ( TURBOSET );
#endif

 /*
  * Header info. for Turbo Pascal Emulation ( String Handling routines )
  *
  * Anthony Phillips,
  * Glockenspiel Ltd. Dublin. Ireland.
  *
  */

extern 		void 	T2CST_delete  ( char *, int, int );
extern 		void	T2CST_insert  ( char *, char *, int );
extern 		int  	T2CST_length  ( char * );
extern      void    T2CST_R_val   ( char *, double *, int * );
extern      void    T2CST_I_val   ( char *, int *   , int * );
extern 		char *  T2CST_copy	  ( char *, int, int );
extern      char *  T2CST_concat  ( int, char *, char *, ... );    
extern      int     T2CST_pos     ( char *, char * );

extern      char *  T2CS_ch2str    ( char ); 	  /* Puts character in string */
extern      char    T2CS_str2ch    ( char * );	  /* Puts string into character */

extern      void    strturboerror ( char * );

 /*				
  * Header info. for Turbo Pascal Emulation ( Turtle graphics routines )
  *
  * Anthony Phillips,
  * Glockenspiel Ltd. Dublin. Ireland.
  *
  */

extern  void 	T2CTR_back                ( int );
extern  void	T2CTR_forwd               ( int );
extern  void  	T2CTR_setheading          ( int );
extern  void	T2CTR_turnright           ( int );
extern  void 	T2CTR_turnleft            ( int ); 
extern  void 	T2CTR_home                ();
extern  void 	T2CTR_setposition         ( int, int );
extern  void 	T2CTR_pendown             ();
extern  void 	T2CTR_penup               ();
extern  void 	T2CTR_setpencolor         ( int );
extern  int 	T2CTR_heading             ();
extern  int 	T2CTR_xcor                ();
extern  int 	T2CTR_ycor                ();
extern  void    T2CTR_hideturtle          (); 
extern  void    T2CTR_showturtle          (); 
extern  int     T2CTR_turtlethere         (); 
extern  void    T2CTR_wrap				  ();
extern  void    T2CTR_nowrap			  ();
extern  void    T2CTR_turtlewindows       ( int, int, int, int );
extern  void    T2CTR_turtledelay         ( int );

extern  void    delay               ();
extern  void 	grapturboerror      ( char * );

extern  void    drawturtle          ();
extern  float   tcos                ( int );
extern  float   tsin                ( int );
extern  int   zone                ();
extern  int   klip                ( int *, int * );

extern  struct  VIDEOCONFIG T2CGR_config;
extern  int   Heding;
extern  int   KlipOn;
extern  int   DelayCount;
extern  int   Ton;
extern  int   WrapOn;
extern  int     T2CTR_on; /* Whether turtles being used */

 /*
  * Header info. for Turbo Pascal Emulation ( Graphics routines )
  *
  * Anthony Phillips,
  * Glockenspiel Ltd. Dublin. Ireland.
  *
  */

/* General definitions    */

#ifndef ERRORPOSITION 
#define ERRORPOSITION 0,0
#endif

#ifndef OKRETVAL
#define OKRETVAL      0
#endif


/* Turbo Like definitions. */

#define BW40   (0)
#define BW80   (2) 
#define C40    (1)
#define C80    (3)
#define Blink  (0) /* Blinking not allowed */

/* Functions to allow TurboPascal Screen Mode Control Emulation. */

extern  int     T2CG_textmode            ( int, ... );   /* nothing or int. */
extern  int     T2CG_graphcolormode      ();
extern  int     T2CG_graphics            (); /* Late addition see GRAPH.BIN ( Turbo ) */
extern  int     T2CG_graphmode           ();
extern  int     T2CG_hires               ();
extern  int     T2CG_textcolor           ( int );  
extern  int     T2CG_textbackground      ( int );
extern  int     T2CG_wherex              ();
extern  int     T2CG_wherey              ();
extern  int     T2CG_hirescolor          ( int );
extern  int     T2CG_palette             ( int );
extern  int     T2CG_graphbackground     ( int );
extern  void    T2CG_clearscreen         ();
extern  void 	T2CG_setpencolor         ( int );

/* Functions to implement Turbo Pascal Window handling Emulation. */

extern  void   T2CG_window               ( int, int, int, int );
extern  int  T2CG_graphwindow          ( int, int, int, int );

/* Functions to implement Turbo Basic Graphics Emulation routines. */

extern  int    T2CG_plot                 ( int, int, int );
extern  int    T2CG_draw                 ( int, int, int, int, int );
extern  void   T2CG_colortable           ( int, int, int, int );
extern  void   T2CG_arc                  ( int, int, int, int, int );
extern  int    T2CG_circle               ( int, int, int, int );
extern  int    T2CG_getpic               ( char *, int, int, int, int );
extern  void   T2CG_putpic               ( char *, int, int );
extern  int    T2CG_getdotcolor          ( int, int );
extern  int    T2CG_fillscreen           ( int );
extern  int    T2CG_fillshape            ( int, int, int, int );
extern  int    T2CG_fillpattern          ( int, int, int, int, int );
extern  void   T2CG_pattern              ( char * );

extern  int    setset               ( int );
        
extern  float  tcos                 ( int );
extern  float  tsin                 ( int );
extern  void   grapturboerror       ( char * );

/* for possible Turtling */

extern  int     T2CTR_on;
extern  void  	T2CTR_setheading          ( int );
extern  void    drawturtle          ();

/* Global with configuration information */

extern  struct  VIDEOCONFIG T2CGR_config;

 /*
  * Header info. for Turbo Pascal Emulation ( mathematical routines )
  *
  * Anthony Phillips,
  * Glockenspiel Ltd. Dublin. Ireland.
  *
  */

#define TURBO_INTEGER_VAL   (-99)
#define TURBO_REAL_VAL      (-98)
#define TURBO_CHAR_VAL      (-97)
#define TURBO_PARAM_ERROR   (-96)
#define TOLERANCE_VAL       (0.01) /* Tuning down of this value may be */
                                   /* desirable	in some situations.    */


/* turbo Pascal emulation functions   */
/* Parameter casted to double if nescessary */

extern double T2CM_arctan( double );	     /* in real/int out real */
extern double T2CM_cos   ( double );	     /* in real/int out real */
extern double T2CM_exp   ( double );         /* in real/int out real */
extern double T2CM_frac  ( double );	     /* in real/int out real */
extern double T2CM_int   ( double );	     /* in real/int out real */
extern double T2CM_ln    ( double );         /* in real/int out real */
extern double T2CM_sin   ( double );         /* in real/int out real */
extern double T2CM_sqrt  ( double );	     /* in real/int out real */

/* Two versions of same thing */

extern int    T2CM_I_abs    ( int );         /* in real/int out real/int */
extern double T2CM_R_abs    ( double );      /* in real/int out real/int */

extern int    T2CM_I_sqr    ( int );         /* in real/int out real/int */
extern double T2CM_R_sqr    ( double );      /* in real/int out real/int */

extern int    T2CM_I_random ( int );   	 	/* in int out int */
extern double T2CM_R_random ( ); 		/* in nothing out real  */

/* Straight Through functions */

extern int    T2CM_odd   ( );        
extern int    T2CM_round ( double );     /* in real out int */
extern int    T2CM_trunc ( double );	 /* in real out int */
extern int    T2CM_hi    ( );        /* in int out int  */
extern int    T2CM_lo    ( );        /* in int out int  */
extern void   T2CM_randomize ();         /* in nothing out nothing */	  

extern        mthTurboError ( char * );
 /*
  * extdec.h
  *
  * C Source Code module for Turbo Pascal Emulation ( Graphics routines )
  *
  * Pauline Hayes,
  * Glockenspiel Ltd. Dublin. Ireland.
  *
  */













										/* arguments passed to functions	*/
										/* files.c							*/
										/* file structure is a Glockenspiel	*/
										/* defined structure to hold details*/
										/* of a file, name fp etc			*/										
extern void T2CF_assign();		/* file structure, pointer to string*/
extern void T2CF_rewrite();		/* file structure, int which will be*/
extern void T2CF_reset();		/* 0 or 1 if optional second parm*/
										/* var arg which may or may not be	*/
/* present its an int if it is 		*/

extern void T2CF_seek();		/* file structure, int		*/
extern void T2CF_flush();		/* file structure		*/
extern void T2CF_close();		/* file structure		*/
extern void T2CF_erase();		/* file structure		*/
extern void T2CF_rename();		/* file structure, pointer to string*/
extern int T2CF_eof();			/* file structure		*/
extern int T2CF_filepos();		/* file structure		*/
extern int T2CF_filesize();		/* file structure		*/
extern int T2CF_eoln();			/* file structure		*/
extern int T2CF_seekeoln();		/* file structure		*/
extern int T2CF_seekeof();		/* file structure		*/
extern double T2CF_longfilepos();	/* file structure	*/
extern double T2CF_longfilesize();	/* file structure		*/
extern void T2CF_longseek();		/* file structure, int 		*/
extern void T2CF_truncate();		/* file structure		*/
extern void T2CF_append();		/* file structure		*/
extern void T2CF_blockread();		/* 1. 	file structure		*/
extern void T2CF_blockwrite();		/* 2.   char *var; could be any var */
										/*      transfer starts with first  */
/*      byte must be big enough		*/
/* 3.   int no of 128 byte blocks 	*/
/*      to be transfered			*/
/* 4.   short count; 0 or 1 if more */
/* 5.   if one this is * int		*/

extern void T2CF_chain();				/* file structure*/
extern void T2CF_execute();				/* file structure*/
extern void T2CF_read();	/* file structure , ????????		*/
extern void T2CF_write();	/* file structure , ????????		*/
										/* avail.c							*/
extern int T2CR_memavail();	/* nothing passed int returned		*/
extern int T2CR_maxavail();	/* nothing passed int returned		*/

										/* dir.c							*/
extern void T2CD_mkdir();	/* pointer to string passed no ret	*/
extern void T2CD_chdir();	/* pointer to string passed no ret	*/
extern void T2CD_rmdir();	/* pointer to string passed no ret	*/
extern void T2CD_getdir();	/* int, pointer to str passed no ret*/

										/* kb.c								*/
extern int T2CV_keypressed();	/* nothing passed	*/

										/* misc.c							*/
extern int T2CV_swap();			 /* passed int and  rets int	*/
extern void T2CV_delay();		 /* passed int	*/
extern char T2CV_upcase();		/* passed char and returns char	*/
extern void T2CV_fillchar();		/* variable of any type, int, char*/
extern void T2CV_move();		/* two variables of any type, int*/

										/* ofs_seg.c						*/
extern 	int 	T2CH_ofs();	/* any variable, function etc..		*/
extern 	int 	T2CH_seg();	/* just pass addr or ptr to it		*/
extern 	int 	T2CH_cseg();	/* nothing passed int returned		*/
extern 	int 	T2CH_dseg();	/* nothing passed int returned		*/
extern 	int	T2CH_sseg();	/* nothing passed int returned		*/
extern 	char far *T2CH_ptr();	/* 2 ints passed 32 bit ptr returned*/
extern 	void	T2CH_intr();	/* int and PAS_REGS * passed		*/
extern 	void	T2CH_msdos();	/* PAS_REGS * passed				*/
										/* PAS_REGS defined in ofs_seg.h	*/

										/* screen.c							*/
extern 	void	T2CSCR_clreol();		/* nothing passed	*/
extern 	void	T2CSCR_clrscr();		/* nothing passed 	*/
extern 	void	T2CSCR_clrinit();		/* nothing passed 	*/
extern 	void	T2CSCR_clrexit();		/* nothing passed 	*/
extern 	void	T2CSCR_gotoxy();		/* nothing passed 	*/
extern 	void	T2CSCR_highvideo();		/* nothing passed 	*/
extern 	void	T2CSCR_normvideo();		/* nothing passed 	*/
extern 	void	T2CSCR_lowvideo();		/* nothing passed 	*/
extern 	void	T2CSCR_insline();		/* nothing passed 	*/
extern 	void	T2CSCR_delline();		/* nothing passed 	*/

										/* sound.c							*/
extern 	void	T2CV_sound();			/* int passed		*/
extern 	void	T2CV_nosound();			/* nothing passed 	*/

										/* hndl_io.c						*/
extern	int		T2CF_ioresult();		/* nothing passed*/

										/* mem.c							*/
extern void T2CR_getmem();			/* ptr to ptr , int	*/
extern void T2CR_freemem();			/* ptr to ptr , int	*/

										/* always.c							*/
extern void T2C_setup_delay();			

    
/* Global variables available to TURBO */
int conoutptr;				

extern T2C_change_strlen();
extern unsigned long T2C_ptol();
