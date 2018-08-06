/*** dispatch() - accepts keyboard input and dispatches control to
 *		  the appropriate function.
 *
 *   Wait in a loop for keyboard input, then dispatch to any of several
 *   functions based on what the character code was. Default is insert()
 *   after validating, otherwise it is one of our control keys. There is
 *   a second dispatch function for special keys - those with zero for a
 *   character value. special_dispatch() works based on the scan code.
 *
 *   EFFECTS:	No global variables are directly effected by this function
 */


#include <ctype.h>
#include <dos.h>
#include <doscalls.h>
#include <subcalls.h>
#include "ssedefs.h"
#include "keydefs.h"


void dispatch()
	{

	struct	KeyData keydata;	  /* data structure from subcalls.h */
	struct	KbdStatus kbdstatus;	  /* data structure from subcalls.h */

	kbdstatus.length = sizeof( kbdstatus );
	KBDGETSTATUS( &kbdstatus, 0 );		       /* set keyboard mode */
	kbdstatus.bit_mask |= 4;
	KBDSETSTATUS( &kbdstatus, 0 );


    /*	the following section waits for keyboard input	  */
    /*	then examines the key received and dispatches us  */
    /*	to the appropriate function for the key pressed.  */


	while(1) {


		KBDCHARIN( &keydata, 0, 0 );   /* get a character from KBD */


		switch( keydata.char_code ) {

		    default:				     /* default is not */
			if ( isprint( keydata.char_code ) )  /* a control key. */
			      insert( keydata.char_code );   /* check validity */
			else  badkey(); 		     /* and if good,   */
			break;				     /* call insert()  */

	 /* "special keys", those with character codes of 00, must */
	 /*  be sent to another dispatcher that checks scan codes. */

		    case SPECIAL_KEYS_CHAR:
			special_dispatch( keydata.scan_code,
					  keydata.shift_state );
			break;

	 /* all others keys we can dispatch from here. some */
	 /* are easy, just dispatch based on char code...   */

		    case CTRL_B_CHAR:
			ctrl_b();
			break;

		    case BKSP_CHAR:
			bksp();
			break;

		    case TAB_CHAR:
			tab();
			break;

		    case CRETURN_CHAR:
			creturn();
			break;

		    case CTRL_N_CHAR:
			ctrl_n();
			break;

		    case CTRL_Y_CHAR:
			ctrl_y();
			break;


	 /*  ...for others, we must check the scan code and shift */
	 /* also as the char code may be shared with another key. */

		    case SHIFT_END_CHAR:
			if ( keydata.scan_code == END_SCAN  &&
			     keydata.shift_state < 3 )
			    shift_end();
			else insert( keydata.char_code );
			break;

		    case SHIFT_DOWN_CHAR:
			if ( keydata.scan_code == DOWN_SCAN  &&
			    keydata.shift_state < 3 )
			    shift_down();
			else insert( keydata.char_code );
			break;

		    case SHIFT_LEFT_CHAR:
			if ( keydata.scan_code == LEFT_SCAN  &&
			    keydata.shift_state < 3 )
			    shift_left();
			else insert( keydata.char_code );
			break;

		    case SHIFT_RIGHT_CHAR:
			if ( keydata.scan_code == RIGHT_SCAN  &&
			    keydata.shift_state < 3 )
			    shift_right();
			else insert( keydata.char_code );
			break;

		    case SHIFT_HOME_CHAR:
			if ( keydata.scan_code == HOME_SCAN  &&
			    keydata.shift_state < 3 )
			    shift_home();
		       else insert( keydata.char_code );
			break;

		    case SHIFT_UP_CHAR:
			if ( keydata.scan_code == UP_SCAN  &&
			    keydata.shift_state < 3 )
			    shift_up();
			else insert( keydata.char_code );
			break;

	 /*  There's one other possibility, that the char code is 0xE0 */
	 /*  which indicates an arrow key on the new AT03 keyboard.    */
	 /*  We will treat these in the same manner as special keys.   */

		  case ARROW_CHAR:
			special_dispatch( keydata.scan_code,
					  keydata.shift_state );
			break;


		}

	    }

	}


/*** special_dispatch(c,s) - dispatches control to the appropriate function
 *			     based on character and scan codes received
 *
 *   Characters with zero character codes or character code 0xE0 are sent
 *   here to be dispatched based on their scan code and shift status. These
 *   values are passed to special_dispatch as parameters.
 *
 *   The left and right arrow keys are handled in this routine rather than
 *   dispatched to their own functions in an attempt to gain some speed.
 *
 *   ENTRY:   scan  -	 scan code of the key press
 *	      shift -	 shift state of the key press
 *
 *   EFFECTS:	No global variables are directly effected by this function
 *		unless the key is an unshifted right or left arrow.
 *		In that case, effects are:
 *
 *			LinesMarked	-   this flag is cleared
 *			MarkedLine[]	-   all cleared
 *			CharsMarked	-   this flag is cleared
 *			MarkedChar[]	-   all cleared
 *			EditBuff[]	-   may be flushed if above were set
 *			EditBuffDirty	-   cleared if EditBuff[] flushed
 *			CurCol		-   incremented or decremented
 */

special_dispatch(scan,shift)	   /* dispatcher for "special keys" based on */
				   /* the scan code and shift code from KBD  */

    unsigned char scan; 		     /* scan code - must be unsigned */
    unsigned shift;					       /* shift code */

    {

    register short i;					/* indexing variable */

    switch( scan ) {		     /* dispatch based on what scan code was */
				    /*	- check the shift state if necessary */
	case LEFT_SCAN:
	    if( shift == 0 ) {		/* left arrow handled here for speed */

		if ( LinesMarked || CharsMarked ) {
		    LinesMarked = 0;			/* if there's marked */
		    CharsMarked = 0;			/* text, clear it    */
		    for ( i = 0; i < MAXLINES; i++ )
			MarkedLine[i] = 0;
		    for ( i = 0; i < LINESIZE; i++ )
			MarkedChar[i] = 0;
		    if (EditBuffDirty) {     /* flush EditBuff before redraw */
			EditBuffDirty = 0;
			flushline((TopRow + CurRow), EditBuff);
			}
		    drawscr(TopRow);			/* redraw the screen */
		    }

		if ( CurCol > 0 ) {
		    CurCol -= 1;		      /* move cursor left 1  */
		    VIOSETCURPOS( CurRow, CurCol, 0 );	  /* if there's room */
		    }

		}

	    else
		shift_left();
	    break;

	case RIGHT_SCAN:
	    if( shift == 0 ) {	       /* right arrow handled here for speed */

		if ( LinesMarked || CharsMarked ) {
		    LinesMarked = 0;			/* if there's marked */
		    CharsMarked = 0;			/* text, clear it    */
		    for ( i = 0; i < MAXLINES; i++ )
			MarkedLine[i] = 0;
		    for ( i = 0; i < LINESIZE; i++ )
			MarkedChar[i] = 0;
		    if (EditBuffDirty) {     /* flush EditBuff before redraw */
			EditBuffDirty = 0;
			flushline((TopRow + CurRow), EditBuff);
			}
		    drawscr(TopRow);			/* redraw the screen */
		    }

		if (CurCol < (LINESIZE -1)) {	   /* move cursor right one  */
		    CurCol += 1;		   /* column if there's room */
		    VIOSETCURPOS( CurRow, CurCol, 0 );
		    }

		}

	    else
		shift_right();
	    break;

	case UP_SCAN:
	    if( shift == 0 )
		up();
	    else
		shift_up();
	    break;

	case DOWN_SCAN:
	    if( shift == 0 )
		down();
	    else
		shift_down();
	    break;

	case HOME_SCAN:
	    if( shift == 0 )
		home();
	    else
		shift_home();
	    break;

	case END_SCAN:
	    if( shift == 0 )
		end_();
	    else
		shift_end();
	    break;

	case PGUP_SCAN:
	    pgup();
	    break;

	case PGDN_SCAN:
	    pgdn();
	    break;

	case CTRL_HOME_SCAN:
	    ctrl_home();
	    break;

	case CTRL_END_SCAN:
	    ctrl_end();
	    break;

	case CTRL_PGUP_SCAN:
	    ctrl_pgup();
	    break;

	case CTRL_PGDN_SCAN:
	    ctrl_pgdn();
	    break;

	case DEL_SCAN:
	    del();
	    break;

	case F9_SCAN:
	    F9();
	    break;

	case F10_SCAN:
	    F10();
	    break;

	default:
	    badkey();

	}

    }


/*** badkey() - sounds a beep when an invalid or inappropriate key is hit
 *
 *   EFFECTS:	None
 */

badkey()
    {
    DOSBEEP (500, 50);			 /*	 frequency = 500 Hertz */
    }					 /* duration = 50 milliseconds */


/*** clearscr() - clears the screen by writing blanks over entire screen
 *
 *   clearscr() writes blanks with normal attributes over the entire screen,
 *   effectively clearing it.
 *
 *   EFFECTS:	None
 */

void clearscr()
    {
	char buff[2];		  /* local buffer for filling cell value */

	buff[0] = ' ';			 /* cell to replicate is a blank */
	buff[1] = BackNorm + ForeNorm;	       /* with normal attributes */

	VIOSCROLLUP(0, 0, -1, -1, -1, (char far *)buff, 0);

				      /* when the first 5 parameters are */
				      /* set as above, the entire screen */
				      /* is filled with the cell in buff */
    }


/*** drawscr(startline) - redraws the screen starting at the specified line
 *
 *   drawscr() redraws the screen starting at the line specified in the
 *   parameter it is passed, startline. The screen is drawn with normal
 *   attributes, since this call will never be made with lines marked.
 *   Nothing is done with EditBuff[], that is left to the caller, as is
 *   the value of TopRow.
 *
 *   ENTRY:  startline -  the line in the file which will be at the top of
 *			  the screen when redrawn.
 *
 *   EFFECTS:  ScrBuff[][] -  gets reloaded
 */

void drawscr(startline)

    unsigned short startline;
    {

    register unsigned short   i, j;		    /* indexing variables */

    char normal = BackNorm + ForeNorm;		     /* normal attributes */

    for ( i = startline, j = 0 ;  j < PageSize ; i++, j++)
	getline( i, &ScrBuff[j][0] );	     /* load ScrBuff[][] with the */
					    /* part of the screen we want */

    VIOWRTCHARSTRATT ( (char far *)ScrBuff, ( PageSize * LINESIZE ), 0, 0,
		      &normal, 0 );
					    /* write it out to the screen */
    }


/*** drawline() - redraws the current line
 *
 *   drawline() redraws the line the cursor is currently on, using the
 *   contents of EditBuff[]. All marked characters will be drawn with the
 *   appropriate attributes. If the line is marked, all characters are
 *   drawn as marked; if the line is not marked, then the characters marked
 *   flag is checked. If characters are marked, the line is drawn in three
 *   sections - unmarked characters at the front of the line (may be 0),
 *   marked characters in the middle of the line (at least 1), and unmarked
 *   characters at the end of the line (may be 0). If no characters are marked
 *   either, then the line is drawn with all normal attributes.
 *
 *   EFFECTS:  None
 */

drawline()
    {
    unsigned short i;				     /* indexing variables */
    unsigned short j;

    char hilite = BackHilite + ForeHilite;	  /* attributes for marked */
    char normal = BackNorm + ForeNorm;	      /* attributes for not marked */

    if( MarkedLine[TopRow+CurRow] )
	VIOWRTCHARSTRATT( EditBuff, LINESIZE, CurRow, 0, &hilite, 0);
					     /* if line is marked, do this */
    else if (CharsMarked) {
	     for( i = 0; !MarkedChar[i] & (i < LINESIZE); i++ );
	     for( j = i; MarkedChar[j] & (j < LINESIZE); j++ );
	     VIOWRTCHARSTRATT( &EditBuff[0], i, CurRow, 0, &normal, 0);
	     VIOWRTCHARSTRATT( &EditBuff[i], (j - i), CurRow, i, &hilite, 0);
	     VIOWRTCHARSTRATT( &EditBuff[j], (LINESIZE - j), CurRow, j,
			       &normal, 0);
	     }			     /* else if characters marked, do this */

	 else VIOWRTCHARSTRATT( EditBuff, LINESIZE, CurRow, 0, &normal, 0);
    }					/* else if neither marked, do this */


/*** line25() - updates line number information on last line
 *
 *   line25() updates the line numbers maintained in the lower right hand
 *   corner of the screen - current line of cursor and total lines in file.
 *   These numbers are first converted from their zero-based internal values
 *   to a more friendly one-based number. Lots of standing on our heads is
 *   done to get the numbers into decimal characters without using C runtime
 *   routines or doing divisions.
 *
 *   Note that while the names of these routines refer to the 25th line,
 *   the pagesize is not hardcoded - if we are in 43 line mode, this
 *   information will appear on the 43rd line. However, the spacing within
 *   the line is hardcoded for an 80-column display.
 *
 *   EFFECTS -	None
 */

void line25()
    {

    int  flag0 = 0;		    /* flag to aid in supressing leading 0's */
    int  col = 67;			    /* starting column for this info */
    int  i, n;					       /* indexing variables */
    char buff[LINESIZE];		    /* buffer to build output string */
    char attrib = Back25 + Fore25;	     /* attributes for output string */

    n = (TopRow + CurRow + 1);		/* set n to line number cursor is on */

    for( i = 0; n > 9999; n -= 10000, i++ );
    if( i > 0 ) {
	flag0 = 1;			   /* 10,000's place of current line */
	buff[0] = i + 0x30;
	}
    else buff[0] = ' ';
    VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );
    col += 1;

    for( i = 0; n > 999; n -= 1000, i++ );
    if( i > 0 ) {
	flag0 = 1;			    /* 1,000's place of current line */
	buff[0] = i + 0x30;
	}
    else if( flag0 ) buff[0] = '0';
	else buff[0] = ' ';
    VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );
    col += 1;

    for( i = 0; n > 99; n -= 100, i++ );
    if( i > 0 ) {
	flag0 = 1;			      /* 100's place of current line */
	buff[0] = i + 0x30;
	}
    else if( flag0 ) buff[0] = '0';
	else buff[0] = ' ';
    VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );
    col += 1;

    for( i = 0; n > 9; n -= 10, i++ );
    if( i > 0 ) {
	flag0 = 1;			       /* 10's place of current line */
	buff[0] = i + 0x30;
	}
    else if( flag0 ) buff[0] = '0';
	else buff[0] = ' ';
    VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );
    col += 1;

    buff[0] = n + 0x30; 			/* 1's place of current line */
    VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );
    col += 1;

    buff[0] = ' ';
    buff[1] = '/';				    /* delimiting characters */
    buff[2] = ' ';
    VIOWRTCHARSTRATT( buff, 3, PageSize, col, &attrib, 0 );
    col += 3;

    flag0 = 0;
    n = TotalLines;				 /* n is total lines in file */

    for( i = 0; n > 9999; n -= 10000, i++ );
    if( i > 0 ) {
	flag0 = 1;			    /* 10,000's place of total lines */
	buff[0] = i + 0x30;
	VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );
	col += 1;
	}

    for( i = 0; n > 999; n -= 1000, i++ );
    if( i > 0 ) {
	flag0 = 1;			     /* 1,000's place of total lines */
	buff[0] = i + 0x30;
	VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );
	col += 1;
	}
    else if( flag0 ) {
	buff[0] = '0';
	VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );
	col += 1;
	}

    for( i = 0; n > 99; n -= 100, i++ );
    if( i > 0 ) {
	flag0 = 1;			       /* 100's place of total lines */
	buff[0] = i + 0x30;
	VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );
	col += 1;
	}
    else if( flag0 ) {
	buff[0] = '0';
	VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );
	col += 1;
	}

    for( i = 0; n > 9; n -= 10, i++ );
    if( i > 0 ) {
	flag0 = 1;				/* 10's place of total lines */
	buff[0] = i + 0x30;
	VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );
	col += 1;
	}
    else if( flag0 ) {
	buff[0] = '0';
	VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );
	col += 1;
	}

    buff[0] = n + 0x30; 			 /* 1's place of total lines */
    VIOWRTCHARSTRATT( buff, 1, PageSize, col, &attrib, 0 );

    col +=1;
    n = LINESIZE - col;      /* fill rest of line with blanks; n is how many */
    for( i = 0; i < LINESIZE; buff[i] = ' ', i++ );
    VIOWRTCHARSTRATT( buff, n, PageSize, col, &attrib, 0 );

    }

/*** name25() - draws prompt and displays filename on 25th line
 *
 *   This routine draws a prompt line with usage information about some
 *   of the less obvious function keys, and the name of the file currently
 *   being edited. Will be called at the start of execution, and whenever
 *   an error message has been displayed on the 25th line so that we need
 *   to re-draw the usage message after the error is responded to. The
 *   line25() routine takes care of the line numbers at the far right end
 *   of the line.
 *
 *   Note that while the names of these routines refer to the 25th line,
 *   the pagesize is not hardcoded - if we are in 43 line mode, this
 *   information will appear on the 43rd line. Spacing within the line
 *   is hardcoded for an 80-column display.
 *
 *   EFFECTS -	None
 */

void name25()
    {

    static char message[51] = "^N/^B=insert above/below ^Y=delete F9=save F10=quit";
    char  name[16];				/* for holding name of file */
    char  attrib = Back25 + Fore25;		  /* attributes for message */
    int   i, j, k;				      /* indexing variables */


					      /* first, print usage message */
    VIOWRTCHARSTRATT( message, 51, PageSize, 0, &attrib, 0 );


    /* build filename string */

    name[0] = ' ';
    name[1] = ' ';			    /* some blanks at start and end */
    name[2] = ' ';
    name[15] = ' ';

    for( i = 0; fname[i] != 0; i++);	  /* find null terminator for fname */

    if( i > 0 ) i--;			 /* point i to last letter in fname */

    for( j = i, k = 14;
	(fname[j] != '\\') && (fname[j] != ':') && (j >= 0) && (k >= 0);
	name[k] = fname[j], j--, k-- );
			  /* move the file name into name[] backwards until */
			  /* we hit a backslash, colon or start of the name */

    for( ; k >= 0; name[k] = ' ', k-- );    /* if any room left, blank fill */

    VIOWRTCHARSTRATT( name, 16, PageSize, 51, &attrib, 0 );	/* write it */

    line25();				    /* call the line number routine */

    }


/*** error25(error) - displays error messages on 25th line, accepts responses
 *
 *   error25() displays an error message on the 25th line when any of several
 *   error conditions are encountered, the type of error being passed as a
 *   parameter. In some cases we just abort; in others we accept input from
 *   the user to determine what to do next.
 *
 *   Note that while the names of these routines refer to the 25th line,
 *   the pagesize is not hardcoded - if we are in 43 line mode, this
 *   information will appear on the 43rd line. Spacing within the line
 *   is hardcoded for an 80-column display.
 *
 *   ENTRY   -	Error - an arbitrary number identifying the error encountered
 *
 *   EFFECTS -	May cause us to end execution; otherwise, none
 */

void error25(error)
    unsigned short error;
    {
    static char message1[51] = "ERROR: Out of memory or can't read file            ";
    static char message2[51] = "ERROR: Unable to create backup. Continue?  (y/n)   ";
    static char message3[51] = "File not found. Create?  (y/n)                     ";
    static char message4[51] = "ERROR: Unable to create file                       ";
    static char message5[51] = "ERROR: Unable to open file - denied write access   ";
    static char message6[51] = "ERROR: Unable to open file                         ";
    static char message7[51] = "ERROR: Out of memory.   S = save  Q = quit         ";
    static char message8[51] = "ERROR: No filename given on command line           ";
    static char message9[51] = "ERROR: Invalid argument on command line            ";
    static char message10[51] = "ERROR: Too many parameters on command line         ";
    static char message11[51] = "ERROR: Unable to save file.   C = continue         ";
    static char message99[51] = "ERROR: Unknown error. Continue?  (y/n)             ";
    struct  KeyData keydata;
    char  attrib = Back25 + Fore25;	     /* attributes for our messages */


    switch( error ) {	       /* take the appropriate action for the error */

	case 1:
	    VIOWRTCHARSTRATT( message1, 51, PageSize, 0, &attrib, 0 );
	    quit(1);
	    break;			     /* 1 -> print message and quit */

	case 2:
	    VIOWRTCHARSTRATT( message2, 51, PageSize, 0, &attrib, 0 );
	    VIOSETCURPOS( PageSize, 50, 0 );

	    while( 1 ) {		   /* 2 -> get a character from KBD */

		KBDCHARIN( &keydata, 0, 0 );

		if( (keydata.char_code == 'y') || (keydata.char_code == 'Y') ) {
		    name25();
		    break;				  /* if Y, go ahead */
		    }

		else if( (keydata.char_code == 'n') || (keydata.char_code == 'N') ) {
		    VIOWRTCHARSTRATT( &keydata.char_code, 1, PageSize, 50, &attrib, 0 );
		    quit(1);			   /* if N, quit with error */
		    break;
		    }

		else badkey();		  /* otherwise wait for another key */

		}
	    break;

	case 3:
	    VIOWRTCHARSTRATT( message3, 51, PageSize, 0, &attrib, 0 );
	    VIOSETCURPOS( PageSize, 32, 0 );

	    while( 1 ) {		   /* 3 -> get a character from KBD */

		KBDCHARIN( &keydata, 0, 0 );

		if( (keydata.char_code == 'y') || (keydata.char_code == 'Y') ) {
		    name25();
		    break;				  /* if Y, go ahead */
		    }

		else if( (keydata.char_code == 'n') || (keydata.char_code == 'N') ) {
		    VIOWRTCHARSTRATT( &keydata.char_code, 1, PageSize, 50, &attrib, 0 );
		    quit(1);			   /* if N, quit with error */
		    break;
		    }

		else badkey();		  /* otherwise wait for another key */

		}
	    break;

	case 4:
	    VIOWRTCHARSTRATT( message4, 51, PageSize, 0, &attrib, 0 );
	    quit(1);
	    break;			     /* 4 -> print message and quit */

	case 5:
	    VIOWRTCHARSTRATT( message5, 51, PageSize, 0, &attrib, 0 );
	    quit(1);
	    break;			     /* 5 -> print message and quit */

	case 6:
	    VIOWRTCHARSTRATT( message6, 51, PageSize, 0, &attrib, 0 );
	    quit(1);
	    break;			     /* 6 -> print message and quit */

	case 7:
	    VIOWRTCHARSTRATT( message7, 51, PageSize, 0, &attrib, 0 );
	    VIOSETCURPOS( PageSize, 44, 0 );

	    while( 1 ) {		   /* 7 -> get a character from KBD */

		KBDCHARIN( &keydata, 0, 0 );

		if( (keydata.char_code == 's') || (keydata.char_code == 'S') ) {
		    VIOWRTCHARSTRATT( &keydata.char_code, 1, PageSize, 50, &attrib, 0 );
		    if ( !savefile(fhandle)) {	/* if S, save file and quit */
			freesegs();
			VIOSETCURPOS( PageSize, 0, 0 );
			quit(1);
			}
		    else
			error25(11);	     /* call error 11 if can't save */
		    }

		else if( (keydata.char_code == 'q') || (keydata.char_code == 'Q') ) {
		    VIOWRTCHARSTRATT( &keydata.char_code, 1, PageSize, 50, &attrib, 0 );
		    quit(1);			   /* if Q, quit with error */
		    break;
		    }

		else badkey();		  /* otherwise wait for another key */

		}
	    break;

	case 8:
	    VIOWRTCHARSTRATT( message8, 51, PageSize, 0, &attrib, 0 );
	    quit(1);
	    break;			     /* 8 -> print message and quit */

	case 9:
	    VIOWRTCHARSTRATT( message9, 51, PageSize, 0, &attrib, 0 );
	    quit(1);
	    break;			     /* 9 -> print message and quit */

	case 10:
	    VIOWRTCHARSTRATT( message10, 51, PageSize, 0, &attrib, 0 );
	    quit(1);
	    break;			    /* 10 -> print message and quit */

	case 11:
	    VIOWRTCHARSTRATT( message11, 51, PageSize, 0, &attrib, 0 );
	    VIOSETCURPOS( PageSize, 45, 0 );

	    while( 1 ) {		  /* 11 -> get a character from KBD */

		KBDCHARIN( &keydata, 0, 0 );

		if( (keydata.char_code == 'c') || (keydata.char_code == 'C') ) {
		    name25();
		    break;				/* C means continue */
		    }
		else badkey();		  /* otherwise wait for another key */

		}
	    break;

	default:
	    VIOWRTCHARSTRATT( message99, 51, PageSize, 0, &attrib, 0 );
	    VIOSETCURPOS( PageSize, 40, 0 );

	    while( 1 ) {			   /* UNKNOWN error number! */
						/* get a character from KBD */
		KBDCHARIN( &keydata, 0, 0 );

		if( (keydata.char_code == 'y') || (keydata.char_code == 'Y') ) {
		    name25();
		    break;				  /* if Y, go ahead */
		    }

		else if( (keydata.char_code == 'n') || (keydata.char_code == 'N') ) {
		    VIOWRTCHARSTRATT( &keydata.char_code, 1, PageSize, 50, &attrib, 0 );
		    quit(1);			   /* if N, quit with error */
		    break;
		    }

		else badkey();		  /* otherwise wait for another key */

		}
	    break;

	}

    }
