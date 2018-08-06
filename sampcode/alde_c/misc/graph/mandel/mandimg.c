/****************************************************************************/
/* MODULE NAME: 							    */
/*    MANDMAKE - Nmemonic "compute MANDelbrot set and MAKE an image file"   */
/*									    */
/* MODULE DESCRIPTION:							    */
/*    Computes an image file for a given area with a specified magnification*/
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
/* OUTPUT:								    */
/*   An image file.							    */
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

/*---> Define Structures <---*/
struct RHEADER begin	    /**********************************************/
   char 	 NAME[30] ; /* Image name.				  */
   unsigned int  PIX_HT   ; /* Image height in pixles			  */
   unsigned int  PIX_WDT  ; /* Image width in pixles.			  */
   unsigned int  DAT_WDT  ; /* Data width in bytes.			  */
   unsigned int  PIX_SIZ  ; /* Size of pixle in bits.			  */
   unsigned int  PIX_FRM  ; /* Pixle format ( ie. b&w, gray, color, etc ) */
   unsigned int  COL_SET  ; /* Color Set ( ie. none, RBG, CMW, etc. )	  */
   unsigned int  CMP_FMT  ; /* Compression Format  ( none, blk supress,..)*/
   end ; /*rheader*/	    /**********************************************/

main( ARGC, ARGV )

   /*---> Define Parameters <---*/
   int		     ARGC	     ; /*				   */
   char 	    *ARGV[]	     ; /*				   */

begin	/*MANDEL*/

   /*---> Define Local Variables <---*/
   int		     ITTER     = 1000; /* Maximum number of itterations/pel*/

   FILE 	    *fp        =  0  ; /* Output image file		   */
   struct RHEADER    HEADER	     ; /* Header record for new image file */
   int		     INDX      =  0  ; /* General purpose counter.	   */
   int		     INDY      =  0  ; /* General purpose counter.	   */
   int		     INDZ      =  0  ; /* General purpose counter.	   */
   int		     XDOTS     =  0  ; /* Number of display pixles in X    */
   int		     YDOTS     =  0  ; /* Number of display pixles in Y    */
   int		     RET_CODE  =  0  ; /* General purpose return code.	   */
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

   /*---> Process runtime arguments <---*/
   if( ARGC < 7 ) then begin
      printf( "INVALID ARGUMENT COUNT of %d . . . . . .\n", ARGC ) ;
      printf( "Use :\n" ) ;
      printf( "mandimg X_real Y_imag  Xdots  Ydots  Range    Out_file\n\n");

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

   /*---> Open output file <---*/
   fp = fopen( ARGV[6], "w" ) ;
   if( fp == 0 ) then begin
      printf("Open ERROR on output file  : %s \n", ARGV[6] ) ;
      return ;
      end   /*if*/

   /*---> Setup image header <---*/
   strcpy( HEADER.NAME, "                             " ) ;
   sprintf( HEADER.NAME, "MB (%8.4f,%8.4f) %7.5f", B_BEGIN, A_BEGIN, RANGE ) ;
   HEADER.PIX_HT   = YDOTS ;
   HEADER.PIX_WDT  = XDOTS ;
   HEADER.DAT_WDT  = XDOTS ;
   HEADER.PIX_SIZ  = 8	   ;
   HEADER.PIX_FRM  = 0	   ;
   HEADER.COL_SET  = 0	   ;
   HEADER.CMP_FMT  = 0	   ;

   /*---> Output image header <---*/
   fwrite( &HEADER, 1, 44, fp ) ;

   /*---> Report new header information <---*/
   printf( "The header is as follows : \n" ) ;
   printf( "   Name       : %s\n", HEADER.NAME     ) ;
   printf( "   Pixle Ht.  : %u\n", HEADER.PIX_HT   ) ;
   printf( "   Pixle Wdt. : %u\n", HEADER.PIX_WDT  ) ;
   printf( "   Data  Wdt. : %u\n", HEADER.DAT_WDT  ) ;
   printf( "   Pixle size : %u\n", HEADER.PIX_SIZ  ) ;
   printf( "   Pixle Fmt. : %u\n", HEADER.PIX_FRM  ) ;
   printf( "   Color Set  : %u\n", HEADER.COL_SET  ) ;
   printf( "   Comp. Fmt. : %u\n", HEADER.CMP_FMT  ) ;
   printf( "Beginning creation of image file . . . . \n" ) ;

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

	 /*---> Output results <---*/
	 putc( (char)(INDZ / 4), fp ) ;

	 end   /*for INDX*/

      /*---> Check for user requested exit. <---*/
      RET_CODE = kbhit() ;
      if( RET_CODE ) then begin
	 INDX = XDOTS ;
	 INDY = YDOTS ;
	 end   /*if*/

      /*---> Report progress to user. <---*/
      printf("Processing for line %d is complete\n", INDY ) ;

      end   /*for INDY*/

   /*---> Exit <---*/
   return ;
   end	 /* MANDEL */
