/****************************************************************************/
/* MODULE NAME: 							    */
/*    MANDMAKE - Nmemonic "compute MANDelbrot set and MAKE an image file"   */
/*									    */
/* MODULE DESCRIPTION:							    */
/*    Computes, and stores the itteration number for each pixle in a given  */
/*    area with a specified magnification.				    */
/*									    */
/* USAGE:								    */
/*    mandel begin_c_real begin_c_imag Xdots Ydots Range Output_file_name   */
/*									    */
/* INPUT:								    */
/*    double  A_begin	The real portion of the beginning complex coordinate*/
/*    double  B_begin	The imaginary portion of the beginning coordinate   */
/*    int     Xdots	The number of pixles of the desplay device in X.    */
/*    int     Ydots	The number of pixles of the desplay device in Y.    */
/*    int     Range	The zoom factor to use for the display. 	    */
/*    FILE    Out_fl_nm The file that will contain the itteration data.     */
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
extern	double atof() ;
extern	double sqrt() ;

/*---> Define non-standard stack size <---*/
int	_stack	=  20000 ;

main( ARGC, ARGV )

   /*---> Define Parameters <---*/
   int		     ARGC	     ; /*				   */
   char 	    *ARGV[]	     ; /*				   */

begin	/*MANDEL*/

   /*---> Define "variable" Local Variables <---*/
   int		     ITTER     = 1000; /* Maximum number of itterations/pel*/
   double	     HIST[1024]      ; /* Histogram			   */

   /*---> Define Local Variables <---*/
   FILE 	    *tp        =  0  ;
   FILE 	    *fp        =  0  ;
   int		     INDX      =  0  ; /* General purpose counter.	   */
   int		     INDY      =  0  ; /* General purpose counter.	   */
   int		     INDZ      =  0  ; /* General purpose counter.	   */
   int		     XDOTS     =  0  ; /* Number of display pixles in X    */
   int		     YDOTS     =  0  ; /* Number of display pixles in Y    */
   int		     RET_CODE  =  0  ;
   int		     COLOR     =  0  ;
   int		     Z	       =  0  ; /*				   */
   double	     XINC      = 0.0 ; /* Distance between pixles	   */
   double	     YINC      = 0.0 ; /* Distance between pixles	   */
   double	     A_BEGIN   = 0.0 ; /* Beginning coordinate. Real.	   */
   double	     B_BEGIN   = 0.0 ; /* Beginning coordinate. Imaginary. */
   double	     RANGE     = 0.0 ; /* Zoom factor			   */
   double	     CA        = 0.0 ; /* Current coordinate. Real.	   */
   double	     CB        = 0.0 ; /* Current coordinate. Imaginary.   */
   double	     ZA        = 0.0 ; /* Current itteration. Real.	   */
   double	     ZB        = 0.0 ; /* Current itteration. Imaginary.   */
   double	     ZSIZE     = 0.0 ; /* Real distance of Z(A,B) from 0,0 */
   double	     ZNEWA     = 0.0 ; /* Next itteration. Real.	   */
   double	     ZNEWB     = 0.0 ; /* Next itteration. Immaginary.	   */
   double	     N	       = 0.0 ; /* Running sum of PELS computed	   */
   double	     ZSUM      = 0.0 ; /* Running sum of INDZ (itterations)*/
   double	     ZSUMSQ    = 0.0 ; /* Running sum of squares of INDZ   */
   double	     BAND[12]	     ; /*				   */
   double	     COUNT	     ; /*				   */
   char 	     IN_BUF[80]      ; /*				   */

   /*---> Process runtime arguments <---*/
   if( ARGC < 7 ) then begin
      printf( "INVALID ARGUMENT COUNT of %d . . . . . .\n", ARGC ) ;
      printf( "Use :\n" ) ;
      printf( "mandmake X_real Y_imag  Xdots  Ydots  Range    Out_file\n\n");

      printf( "  e.g.  -2.000  -1.250   320   200    2.500   mandata.tmp\n" ) ;
      printf( "        -1.500  -0.500   320   200    0.500   mandata.tmp\n" ) ;
      printf( "        -1.250  -0.200   320   200    0.100   mandata.tmp\n" ) ;
      printf( "        -1.250  -0.180   320   200    0.030   mandata.tmp\n" ) ;
      printf( "        -1.235  -0.171   320   200    0.006   mandata.tmp\n" ) ;
      printf( "        -1.233  -0.169   320   200    0.001   mandata.tmp\n" ) ;
      printf( "        -1.233  -0.169   320   200    0.0001  mandata.tmp\n" ) ;
      printf( "        -1.233  -0.169   320   200    0.00001 mandata.tmp\n" ) ;
      return ;
      end   /*if*/
   A_BEGIN =  atof( ARGV[ 2 ] ) ;
   B_BEGIN =  atof( ARGV[ 1 ] ) ;
   XDOTS   =  atoi( ARGV[ 3 ] ) ;
   YDOTS   =  atoi( ARGV[ 4 ] ) ;
   RANGE   =  atof( ARGV[ 5 ] ) ;

   /*---> Initialize <---*/
   for( INDX=0; INDX < 1005; INDX++ ) HIST[ INDX ] = 0.0 ;
   for( INDX=0; INDX <	 12; INDX++ ) BAND[ INDX ] = 0.0 ;

   /*---> Open output file <---*/
   tp = fopen( "mandtemp.dat", "w" ) ;
   if( tp == 0 ) then begin
      printf("Open ERROR on output file  : %s \n", "mandtemp.dat" ) ;
      return ;
      end   /*if*/

   /*---> Compute complex spacing between pixles <---*/
   XINC = RANGE / (float)XDOTS ;
   YINC = RANGE / (float)YDOTS ;

   /*---> Loop to compute each pixle column value on 2-D Complex grid <---*/
   for( INDY=0; INDY < YDOTS; INDY++ ) begin

      /*---> Initialize for new row <---*/
      CA = YINC * INDY + B_BEGIN ;

      for( INDX=0; INDX < XDOTS; INDX++ ) begin

	 /*---> Initialize for new column ( and pixle ) <---*/
	 ZA = 0.0 ;
	 ZB = 0.0 ;
	 CB = XINC * INDX + A_BEGIN ;
	 ZSIZE = sqrt(	CA*CA + CB*CB ) ;

	 /*---> Loop to itterate for each ( Z**2 + C ) <---*/
	 for( INDZ=0; (ZSIZE < 2) && (INDZ <= ITTER); INDZ++ ) begin
	    ZNEWA = ( ZA*ZA ) - ( ZB*ZB ) + CA ;
	    ZNEWB = ( 2*ZA*ZB ) + CB ;
	    ZSIZE = sqrt( ZNEWA*ZNEWA + ZNEWB*ZNEWB ) ;
	    ZA	  = ZNEWA ;
	    ZB	  = ZNEWB ;
	    end   /* for INDZ */

	 /*---> Compute statistics <---*/
	 HIST[ INDZ ]++ ;

	 /*---> Output results <---*/
	 fprintf( tp, "%d\n", INDZ ) ;

	 end   /*for INDX*/

      /*---> Check for user requested exit. <---*/
      RET_CODE = kbhit() ;
      if( RET_CODE ) then begin
	 N    = (float)INDY * (float)INDX  ;
	 INDX = XDOTS ;
	 INDY = YDOTS ;
	 end   /*if*/

      /*---> Report progress to user. <---*/
      printf("Processing for line %d is complete\n", INDY ) ;

      end   /*for INDY*/

   /*---> Display input and computational parameters <---*/
   printf("If you wish the statistical summary to be printed rather than\n" ) ;
   printf("just displayed on the CRT, then : \n" ) ;
   printf("            Turn on the printer,\n" ) ;
   printf("            Then push <ctrl> <PrtSc>\n" ) ;
   printf("            Finally, push <return> to continue :\n" ) ;
   printf("Otherwise just push <return> to continue :\n" ) ;
   gets() ;

   printf( "\n\n\n************ Mnadelbrot Set Computation system ************\n" ) ;
   printf( "\nInput parameters were as follows :\n\n" ) ;
   printf( "     Beginning Coordinate (real) :  %f\n", A_BEGIN  ) ;
   printf( "     Beginning Coordinate (imag) :  %f\n", B_BEGIN  ) ;
   printf( "     Size (X) of screen          :  %d\n", XDOTS    ) ;
   printf( "     Size (Y) of screen          :  %d\n", YDOTS    ) ;
   printf( "     Range                       :  %e\n", RANGE    ) ;
   printf( "     Increment between pels in X :  %e\n", XINC     ) ;
   printf( "     Increment between pels in Y :  %e\n", YINC     ) ;
   printf( "     Itterations per pel         :  %d\n", ITTER    ) ;
   printf( "     Output file name            :  %s\n", ARGV[6] ) ;

   if( N == 0.0 ) then	N = (float)XDOTS * (float)YDOTS ;

   printf("Total Count was actually %f pels.\n", N ) ;
   printf( "Total expected pels was %f \n", (float)XDOTS * (float)YDOTS ) ;

   printf( "\nHISTOGRAM summary : \n" ) ;
   for( INDX=0; INDX < ITTER; INDX += 10 ) begin
      if( (INDX % 100) == 0 ) then printf("\n" ) ;
      printf( "%4d-%4d   %5.0f %5.0f %5.0f %5.0f %5.0f %5.0f %5.0f %5.0f %5.0f %5.0f \n",
	 INDX, INDX+9,
	 HIST[INDX  ],HIST[INDX+1],HIST[INDX+2],HIST[INDX+3],HIST[INDX+4],
	 HIST[INDX+5],HIST[INDX+6],HIST[INDX+7],HIST[INDX+8],HIST[INDX+9] );
      end   /*for*/
   printf("\n1000-       %5.0f\n", HIST[ 1000 ] ) ;

   /*---> (Re)open file(s) <---*/
   fclose( tp ) ;
   tp = fopen( "mandtemp.dat", "r" ) ;
   if( tp == 0 ) then begin
      printf("Open ERROR on input file  : %s \n", "mandtemp.dat" ) ;
      return ;
      end   /*if*/
   fp = fopen( ARGV[6], "w" ) ;
   if( fp == 0 ) then begin
      printf("Open ERROR on output file  : %s \n", ARGV[6] ) ;
      return ;
      end   /*if*/

   /*---> Output size <---*/
   fprintf( fp, "%d\n", XDOTS ) ;
   fprintf( fp, "%d\n", YDOTS ) ;

   /*---> Compute values for color bands <---*/
   COUNT = 0.0 ;
   N =	N / 10.0 ;
   INDZ = 0 ;
   for( INDX=0; INDX <= 1000; INDX++ ) begin
      COUNT += HIST[ INDX ] ;
      if( COUNT < N )		  then BAND[ 0 ] = (float)INDX ;
	 else if( COUNT < 2.0*N ) then BAND[ 1 ] = (float)INDX ;
	 else if( COUNT < 3.0*N ) then BAND[ 2 ] = (float)INDX ;
	 else if( COUNT < 4.0*N ) then BAND[ 3 ] = (float)INDX ;
	 else if( COUNT < 5.0*N ) then BAND[ 4 ] = (float)INDX ;
	 else if( COUNT < 6.0*N ) then BAND[ 5 ] = (float)INDX ;
	 else if( COUNT < 7.0*N ) then BAND[ 6 ] = (float)INDX ;
	 else if( COUNT < 8.0*N ) then BAND[ 7 ] = (float)INDX ;
	 else if( COUNT < 9.0*N ) then BAND[ 8 ] = (float)INDX ;
      end   /*for*/

   printf("Band boundries are : \n" ) ;
   for(INDX=0;INDX<=8;INDX++) printf("   %4.0f", BAND[ INDX ] ) ;
   printf("\n");

   /*---> Loop for each pixle <---*/
   for( INDY=0; INDY < YDOTS; INDY++ ) begin
      for( INDX=0; INDX < XDOTS; INDX++ ) begin

	 /*---> Read value of pixle from input file <---*/
	 fgets( IN_BUF, 80, tp ) ;
	 Z = atoi( IN_BUF ) ;

	 /*---> Compute pixle color value <---*/
	 COLOR = 16 ;
	 if(	     (float)Z < BAND[0] ) then COLOR = 1 ;
	    else if( (float)Z < BAND[1] ) then COLOR = 2 ;
	    else if( (float)Z < BAND[2] ) then COLOR = 3 ;
	    else if( (float)Z < BAND[3] ) then COLOR = 1 ;
	    else if( (float)Z < BAND[4] ) then COLOR = 2 ;
	    else if( (float)Z < BAND[5] ) then COLOR = 3 ;
	    else if( (float)Z < BAND[6] ) then COLOR = 1 ;
	    else if( (float)Z < BAND[7] ) then COLOR = 2 ;
	    else if( (float)Z < BAND[8] ) then COLOR = 3 ;
				   else COLOR = 0 ;

	 /*---> Output results <---*/
	 fprintf( fp, "%d\n", COLOR ) ;

	 end   /*for X*/
      end   /*for Y*/

   /*---> Exit <---*/
   return ;
   end	 /* MANDEL */
