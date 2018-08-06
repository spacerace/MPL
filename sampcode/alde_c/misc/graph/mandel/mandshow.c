/****************************************************************************/
/* MODULE NAME: 							    */
/*    MANDSHOW - Nmemonic "SHOW  MANDelbrot set"                            */
/*									    */
/* MODULE DESCRIPTION:							    */
/*    Colors, and displays a MANDelbrot image file for CGA.		    */
/*									    */
/* USAGE:								    */
/*    mandel  mandelbrot_image_file_name				    */
/*									    */
/* INPUT:								    */
/*    FILE    fp	The mandelbrot image file name. 		    */
/*									    */
/* PROCESS:  (Pseudo code)						    */
/*	 Initialize global constants.					    */
/*	 Define parameters.						    */
/*	 Begin MANDEL.							    */
/*	    Define local variables.					    */
/*	    --- Insert pseudo code here ---				    */
/*	 End MANDEL.							    */
/*									    */
/* OUTPUT:								    */
/*									    */
/* EQUATIONS/ALGORITHMS:						    */
/*									    */
/* REFERENCES:								    */
/*									    */
/****************************************************************************/

/*---> Standard definitions and declares <---*/
#include <stdio.h>
#include <dos.h>

/*---> Initialize global constants <---*/
#define then
#define begin {
#define end }
#define TRUE 1
#define FALSE 0
extern double atof() ;

main( ARGC, ARGV )

   /*---> Define Parameters <---*/
   int		     ARGC	     ; /*				   */
   char 	    *ARGV[]	     ; /*				   */

begin	/*MANDEL*/

   /*---> Define Local Variables <---*/
   FILE 	    *fp 	     ;
   int		     INDX      =  0  ; /* General purpose counter.	   */
   int		     INDY      =  0  ; /* General purpose counter.	   */
   int		     Z	       =  0  ; /* Itteration value		   */
   int		     XDOTS     =  0  ; /* Number of display pixles in X    */
   int		     YDOTS     =  0  ; /* Number of display pixles in Y    */
   int		     INTNO     =  0  ;
   int		     COLOR     =  0  ;
   int		     RET_CODE  =  0  ;
   char 	    *IN_BUF    =  0  ;
   double	     N	       = 0.0 ; /* Running sum of PELS computed	   */
   union REGS INREGS		 ;
   union REGS OUTREGS		 ;

   /*---> Process runtime arguments <---*/
   if( ARGC < 2 ) then begin
      printf( "INVALID ARGUMENT COUNT of %d . . . . . .\n", ARGC-1 ) ;
      printf( "Use :   mandshow  mandelbrot_image_file_name   \n" ) ;
      return ;
      end   /*if*/

   /*---> Open input file <---*/
   fp = fopen( ARGV[1], "r" ) ;
   if( fp == 0 ) then begin
      printf("Open ERROR on input file  : %s \n", ARGV[1] ) ;
      return ;
      end   /*if*/

   /*---> Read X,Y dimensions <---*/
   fgets( IN_BUF, 80, fp ) ;
   XDOTS = atoi( IN_BUF ) ;
   fgets( IN_BUF, 80, fp ) ;
   YDOTS = atoi( IN_BUF ) ;

   /*---> Generate interrupt to set 320x200 color mode on CRT <---*/
   INTNO       = 0x10 ;
   INREGS.h.ah = 0x00 ;
   INREGS.h.al = 0x04 ;
   int86( INTNO, &INREGS, &OUTREGS ) ;

   /*---> Generate interrupt to set background color (0-Black) <---*/
   INTNO       = 0x10 ;
   INREGS.h.ah = 0x0b ;
   INREGS.h.bh = 0x00 ;
   INREGS.h.bl = 0x00 ;
   int86( INTNO, &INREGS, &OUTREGS ) ;

   /*---> Generate interrupt to set color palette (1,2,3-RGY) <---*/
   INTNO       = 0x10 ;
   INREGS.h.ah = 0x0b ;
   INREGS.h.bh = 0x01 ;
   INREGS.h.bl = 0x00 ;
   int86( INTNO, &INREGS, &OUTREGS ) ;

   /*---> Generate interrupt to set test pattern <---*/
   INTNO       = 0x10		     ;
   INREGS.h.ah = 0x0c		     ;
   INREGS.h.al = 0		     ;
   INREGS.x.dx = 5		     ;
   INREGS.x.cx = 5		     ;
   int86( INTNO, &INREGS, &OUTREGS ) ;
   INREGS.h.al = 1		     ;
   INREGS.x.dx = 7		     ;
   INREGS.x.cx = 7		     ;
   int86( INTNO, &INREGS, &OUTREGS ) ;
   INREGS.h.al = 2		     ;
   INREGS.x.dx = 9		     ;
   INREGS.x.cx = 9		     ;
   int86( INTNO, &INREGS, &OUTREGS ) ;
   INREGS.h.al = 3		     ;
   INREGS.x.dx = 11		     ;
   INREGS.x.cx = 11		     ;
   int86( INTNO, &INREGS, &OUTREGS ) ;

   /*---> Loop to compute each pixle column value on 2-D Complex grid <---*/
   for( INDY=0; INDY < YDOTS; INDY++ ) begin
      for( INDX=0; INDX < XDOTS; INDX++ ) begin

	 /*---> Read value of pixle from input file <---*/
	 fgets( IN_BUF, 80, fp ) ;
	 COLOR = atoi( IN_BUF ) ;

	 /*---> Generate interrupt to set current pixle <---*/
	 INTNO = 0x10 ;
	 INREGS.h.ah = 0x0c ;
	 INREGS.x.dx = INDY ;
	 INREGS.h.al = (char)COLOR	   ;
	 INREGS.x.cx = INDX		   ;
	 int86( INTNO, &INREGS, &OUTREGS ) ;

	 end   /*for INDX*/
      end   /*for INDY*/

   /*---> Pause for user response <---*/
   getch() ;

   /*---> Loop to draw vertical grid <---*/
   for( INDY=0; INDY < YDOTS; INDY++ ) begin
      for( INDX=0; INDX < XDOTS; INDX += (XDOTS / 10) ) begin
	 INTNO = 0x10 ;
	 INREGS.h.ah = 0x0c ;
	 INREGS.x.dx = INDY ;
	 INREGS.h.al = 0x00 ;
	 INREGS.x.cx = INDX ;
	 int86( INTNO, &INREGS, &OUTREGS ) ;
	 end   /*for INDX*/
      end   /*for INDY*/

   /*---> Loop to draw horizontal grid <---*/
   for( INDY=0; INDY < YDOTS; INDY += (YDOTS / 10) ) begin
      for( INDX=0; INDX < XDOTS; INDX++ ) begin
	 INTNO = 0x10 ;
	 INREGS.h.ah = 0x0c ;
	 INREGS.x.dx = INDY ;
	 INREGS.h.al = 0x00 ;
	 INREGS.x.cx = INDX ;
	 int86( INTNO, &INREGS, &OUTREGS ) ;
	 end   /*for INDX*/
      end   /*for INDY*/

   /*---> Pause for user response <---*/
   getch() ;

   /*---> Generate interrupt to set color palette (1,2,3-CMW) <---*/
   INTNO       = 0x10 ;
   INREGS.h.ah = 0x0b ;
   INREGS.h.bh = 0x01 ;
   INREGS.h.bl = 0x01 ;
   int86( INTNO, &INREGS, &OUTREGS ) ;

   /*---> Pause for user response <---*/
   getch() ;

   /*---> Generate interrupt to set 320x200 color mode on CRT <---*/
   INTNO       = 0x10 ;
   INREGS.h.ah = 0x00 ;
   INREGS.h.al = 0x03 ;
   int86( INTNO, &INREGS, &OUTREGS ) ;

   /*---> Exit <---*/
   return ;
   end	 /* MANDSHOW */
