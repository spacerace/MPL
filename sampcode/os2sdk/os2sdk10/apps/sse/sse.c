/***	SSE - Simple Screen Editor
 *
 *
 *
 *
 *
 *
 *
 *
 */



#include <ctype.h>
#include <doscalls.h>
#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include <subcalls.h>
#include "ssedefs.h"


unsigned short	 TotalLines;	      /* num of entries in line table */
struct Line far *LineTable[MAXLINES]; /* the line table */

unsigned short	 TotalSegs;	      /* num of entries in seg table */
struct	SegEntry SegTable[MAXSEGS];

char	       fbuffer[FBUFFSIZE];
unsigned short bytesread;

unsigned char ScrBuff[LONGPAGE][LINESIZE];
unsigned char EditBuff[LINESIZE];
unsigned short EditBuffDirty;

unsigned short PageSize;
unsigned short Mode43Set = 0;

unsigned short ForeNorm = 0x07;
unsigned short BackNorm = 0x10;
unsigned short ForeHilite = 0x01;
unsigned short BackHilite = 0x70;
unsigned short Fore25 = 0x07;
unsigned short Back25 = 0x40;

unsigned short CurRow, CurCol;
unsigned short TopRow;

unsigned short LinesMarked, CharsMarked;
unsigned short MarkedLine[MAXLINES], MarkedChar[LINESIZE];

char	       *fname;
unsigned short fhandle;

/***	main
 *
 *
 *
 */

main(argc, argv)
int   argc;
char *argv[];
{
    unsigned short i;

    unsigned short handtype, flagword;

    struct  ModeData	modedata;	   /* from subcalls.h */
    struct  CursorData	cursordata;


/* check if we're in foreground */

    DOSQHANDTYPE( 0, (unsigned far *)&handtype, (unsigned far *)&flagword );
    if( (handtype == 0) || ( !( flagword & 1 )) || ( !( flagword & 2 )) )
      quit(1);


/* disable signals so user can't exit with Ctrl-C or Ctrl-Break */

    DOSHOLDSIGNAL( TRUE );


/* initialize variables */

    EditBuffDirty = 0;
    LinesMarked = 0;
    CharsMarked = 0;
    for (i = 0; i < MAXLINES; i++)
	MarkedLine[i] = 0;
    for (i = 0; i < LINESIZE; i++)
	MarkedChar[i] = 0;

/* Set PageSize to match the mode we're in, 25 or 43.
   This may be changed later by a switch from the command line. */

    modedata.length = sizeof( modedata );
    VIOGETMODE( &modedata, 0 );
    if( modedata.row == 25 )
	PageSize = SHORTPAGE;
    else PageSize = LONGPAGE;

/*  Parse file name */
    if (argc < 2) {
      error25(8);
      quit(1);
      }
    else if (argc < 4) {
	   for( i = 1; i < (argc-1); i++ ) {
	       if( (argv[i][0] == '/') &&
		   ( argv[i][1] == '4' ) &&
		   ( argv[i][2] == '3' ) ) {
		   PageSize = LONGPAGE;
		   modedata.row = ( LONGPAGE + 1 );
		   VIOSETMODE( &modedata, 0 );
		   cursordata.cur_start = 6;
		   cursordata.cur_end = 7;
		   cursordata.cur_width = 1;
		   cursordata.cur_attribute = 0;
		   VIOSETCURTYPE( &cursordata, 0 );
		   Mode43Set = 1;
		   }
	       else if( (argv[i][0] == '/') &&
			( (argv[i][1] == 'B') || (argv[i][1] == 'b') ) &&
			( (argv[i][2] == 'W') || (argv[i][2] == 'w') ) ) {
			   ForeNorm = 0x07;
			   BackNorm = 0;
			   ForeHilite = 0;
			   BackHilite = 0x70;
			   Fore25 = 0;
			   Back25 = 0x70;
			   }
	       else {
		    error25(9);
		    quit(1);
		    }

	   }
       fname = argv[i];
       }
    else {
       error25(10);
       quit(1);
       }

    /* paint the screen */
    clearscr();
    drawscr(0);

/*  read file */
    switch (openfile(fname, &fhandle, FOFLAG)) {
	case  0:
	    if (readfile(fhandle))
		error25(1);
	    if (backupfile(fname, fhandle))
		error25(2);
	    break;

	case  110:
/*	    create the file */
	    error25(3);
	    if (openfile(fname, &fhandle, CFFLAG))
		error25(4);   /* error creating file */
	    break;

	case  32:
/*	    denied write access */
	    error25(5);
	    break;

	default :
/*	    error on opening file */
	    error25(6);
	    break;
    }


/* if open worked, continue */

    drawscr(0);
    name25();
    VIOSETCURPOS(0,0,0);
    getline( 0, &EditBuff[0] );    /* initialize */
    dispatch();
    freesegs();
}




 quit(n)		      /* if we set 43 line mode from command line,  */
    unsigned short n;	      /* set it back to 25 line mode before exiting */
    {
    struct  ModeData	modedata;
    struct  CursorData	cursordata;

    if( Mode43Set ) {
	modedata.length = sizeof( modedata );
	VIOGETMODE( &modedata, 0 );
	modedata.row = 25;
	VIOSETMODE( &modedata, 0 );
	cursordata.cur_start = 12;
	cursordata.cur_end = 13;
	cursordata.cur_width = 1;
	cursordata.cur_attribute = 0;
	VIOSETCURTYPE( &cursordata, 0 );
	BackNorm = 0;
	ForeNorm = 0;
	clearscr();
	}
    DOSEXIT(1,n);		   /* exit with specified completion code */
    }
