 /*** SETEGA.C	OS/2 demo program to switch EGA between 25- and 43- line modes
  *
  *   SETEGA accepts one parameter, which must be either "25" or "43".
  *   Too few or too many parameters, or a parameter other than the two
  *   specified above, will result in an error message and termination
  *   with no action taken.
  *
  *   This program will work only on machines equipped with a standard EGA.
  *
  *   SETEGA functions as follows:
  *
  *	 1) Check parameters
  *
  *	 2) Clear the screen, using the VIOSCROLLUP function as described
  *	    in the CLRSCR example elsewhere in these bulletin boards
  *
  *	 3) Get current mode information using VIOGETMODE
  *
  *		 extern unsigned far pascal VIOGETMODE (
  *			struct ModeData far *,
  *			unsigned );
  *
  *	    This call fills a data structure of type
  *
  *		 struct ModeData {
  *			unsigned length;
  *			unsigned char type;
  *			unsigned char color;
  *			unsigned col;
  *			unsigned row;
  *			unsigned hres;
  *			unsigned vres;
  *			};
  *
  *	    whose address we pass in the first parameter. The second
  *	    parameter is the VIO handle, a reserved word of zeros.
  *
  *	 4) We leave all these fields as they are except "row", the number
  *	    of alphanumeric rows for this mode. This is changed to the
  *	    value specified in our parameter.
  *
  *	 5) VIOSETMODE is now used to set the mode.
  *
  *		 extern unsigned far pascal VIOSETMODE (
  *			struct ModeData far *,
  *			unsigned );
  *
  *	    This call has the same parameters as VIOGETMODE.
  *	    We are now set to the desired mode.
  *
  *	 6) We then set the cursor to an appropriate size and shape. This is
  *	    done by filling in a structure of the type
  *
  *		 struct CursorData {
  *			unsigned cur_start;
  *			unsigned cur_end;
  *			unsigned cur_width;
  *			unsigned cur_attribute;
  *			};
  *
  *	    with appropriate values, then calling VIOSETCURTYPE
  *
  *		 extern unsigned far pascal VIOSETCURTYPE (
  *			struct CursorData far *,
  *			unsigned );
  *
  *	    where the second parameter is again the VIO handle, 0.
  *
  *	 7) Last, we set the cursor to the upper left hand corner of the
  *	    screen using VIOSETCURPOS.
  *
  *
  *   Compile using the -Lp option; this program may be bound for use under
  *   real mode.
  */

#include <subcalls.h>
#include <doscalls.h>
#include <stdio.h>

void usage();

void main( argc, argv )
int  argc;
char *argv[];
{

    struct  ModeData	modedata;

    struct  CursorData	cursordata;

    static char  buffer[2] = { 0x20, 0x07 };	 /* scrolling fill character */
						 /* for clearing the screen  */
    if( argc != 2 )
	usage(argv[0]);

    switch(atoi(argv[1])) {
	case 43:
	    VIOSCROLLUP( 0, 0, -1, -1, -1, (char far *)buffer, 0 );
	    modedata.length = sizeof( modedata );
	    VIOGETMODE( &modedata, 0 );
	    modedata.row = 43;
	    VIOSETMODE( &modedata, 0 );
	    cursordata.cur_start = 7;
	    cursordata.cur_end = 7;
	    cursordata.cur_width = 1;
	    cursordata.cur_attribute = 0;
	    VIOSETCURTYPE( &cursordata, 0 );
	    VIOSETCURPOS( 0, 0, 0 );
	break;
	
	case 25:
	    VIOSCROLLUP( 0, 0, -1, -1, -1, (char far *)buffer, 0 );
	    modedata.length = sizeof( modedata );
	    VIOGETMODE( &modedata, 0 );
	    modedata.row = 25;
	    VIOSETMODE( &modedata, 0 );
	    cursordata.cur_start = 12;
	    cursordata.cur_end = 13;
	    cursordata.cur_width = 1;
	    cursordata.cur_attribute = 0;
	    VIOSETCURTYPE( &cursordata, 0 );
	    VIOSETCURPOS( 0, 0, 0 );
	break;

	default:
	    usage(argv[0]);
	}
	exit(0);
}

void
usage(p)
char *p;
{
    printf( "usage: %s 25|43\n", p);
    exit(1);
}
