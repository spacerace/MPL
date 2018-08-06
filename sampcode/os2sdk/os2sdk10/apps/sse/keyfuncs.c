/****** KEYFUNCS.C - a collection of routines to service special keys
 *		     each routine has the name of the key it services
 */

#include <ctype.h>
#include <dos.h>
#include <doscalls.h>
#include <subcalls.h>
#include "ssedefs.h"
#include "keydefs.h"


/*** insert(c) - inserts the character c at the current cursor position
 *
 *   This routine is called when any regular printable character is hit.
 *
 *   insert(c) checks to see if there is room in EditBuff[] to insert the
 *   character it is passed by seeing if there is already a printable
 *   character in the 80th column. If so, no character can be inserted
 *   so we just call badkey(). If there is room, insert the character
 *   at the position indicated by CurCol and shift everything past that
 *   over one; mark EditBuff[] as dirty.
 *
 *   If there is text that has been marked, insert() will call del() to
 *   remove it before inserting its character where the marked text was.
 *
 *   The character we are passed has already been checked as being a
 *   valid printable characters.
 *
 *   insert(c)
 *
 *   ENTRY:	  c  -	the validated character to be inserted
 *
 *   EFFECTS:	  EditBuff[]	 - changes its contents if successful
 *		  EditBuffDirty  - sets this flag if EditBuff changed
 *		  LinesMarked	 - clears this if set
 *		  CharsMarked	 - clears this if set
 *		  CurCol	 - increments this one
 *
 *   The effects of del() should also be considered since it may be
 *   called from this function.
 */

insert(c)
    char c;		 /* the character we are to insert, already validated */
    {

    register short i;					 /* indexing variable */

    if (LinesMarked || CharsMarked)	   /* if there is marked text around, */
	del();				   /* we first want to delete it      */

    if ( EditBuff[LINESIZE - 1] == ' ')  /* can only insert when there's room */
	{
	for ( i = (LINESIZE - 1); i > CurCol; i-- )
	    EditBuff[i] = EditBuff[i-1];	 /* shift everything over one */
	EditBuff[CurCol] = c;			 /* put in the new character  */
	drawline();
	CurCol += 1;					    /* update display */
	VIOSETCURPOS( CurRow, CurCol, 0 );
	EditBuffDirty = 1;			 /* mark edit buffer as dirty */
	}

    else badkey();		       /* if no room, exit as for invalid key */

    }


/*** up() - moves the cursor up one row
 *
 *   up() moves the cursor position up one row. If we are at the top of
 *   the screen already, up() scrolls the screen down one line. If we
 *   are at the top of the file, it does nothing. If there is marked text
 *   around, up() will clear it without deleting. Because the current row
 *   in the file is changed by up(), EditBuff[] is flushed and refilled
 *   and line25() is called to update the line information at the bottom
 *   of the screen.
 *
 *   EFFECTS:	  EditBuff[]	 - flushes its current contents and reloads
 *		  EditBuffDirty  - clears this flag
 *		  LinesMarked	 - clears this flag
 *		  MarkedLine[]	 - clears all
 *		  CharsMarked	 - clears this flag
 *		  MarkedChar[]	 - clears all
 *		  TopRow	 - if at top of screen, decrements this one
 *		  CurRow	 - otherwise, decrements this one
 */


void up()
    {

    register short i;				      /* indexing variable */

    char buff[2];	      /* local buffer for scrolling fill character */
    buff[0] = 32;	      /*		 character = 32 = <blank>  */
    buff[1] = ForeNorm;       /*		      attribute = normal   */

    if (EditBuffDirty) {			  /* this call will change */
	EditBuffDirty = 0;			  /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);   /* edit buffer if dirty  */
	}

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;			 /* if there's marked text */
	CharsMarked = 0;			 /* around, clear it and   */
	for ( i = 0; i < MAXLINES; i++ )	 /* redraw the screen	   */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	drawscr(TopRow);
	}

    if ( (TopRow + CurRow) != 0 ) {	     /* if top of file, do nothing */
	if ( CurRow == 0 ) {
	    TopRow -= 1;		  /* if top of screen, scroll down */
	    VIOSCROLLDN( 0, 0, (PageSize - 1), (LINESIZE - 1), 1,
		       (char far *)buff, 0);
	    getline( (TopRow + CurRow), &EditBuff[0] );
	    drawline();
	    }
	else {
	    CurRow -= 1;		  /* otherwise just move cursor up */
	    getline( (TopRow + CurRow), &EditBuff[0] );
	    VIOSETCURPOS( CurRow, CurCol, 0 );
	    }
	}

    line25();	      /* reset line numbers on 25th line to reflect change */

    }


/*** down() - moves the cursor up one row
 *
 *   down() moves the cursor down one row. If we are at the top of
 *   the screen already, down() scrolls the screen up one line. If we
 *   are at the end of the file, it does nothing. If there is marked text
 *   around, down() will clear it without deleting. Because the current row
 *   in the file is changed by down(), EditBuff[] is flushed and refilled
 *   and line25() is called to update the line information at the bottom
 *   of the screen.
 *
 *   EFFECTS:	  EditBuff[]	 - flushes its current contents and reloads
 *		  EditBuffDirty  - clears this flag
 *		  LinesMarked	 - clears this flag
 *		  MarkedLine[]	 - clears all
 *		  CharsMarked	 - clears this flag
 *		  MarkedChar[]	 - clears all
 *		  TopRow	 - if at bottom of screen, increments this one
 *		  CurRow	 - otherwise, increments this one
 */


void down()
    {

    register short i;				      /* indexing variable */

    char buff[2];	      /* local buffer for scrolling fill character */
    buff[0] = 32;	      /*		  character = 32 = <blank> */
    buff[1] = ForeNorm;       /*		       attribute = normal  */

    if (EditBuffDirty) {			  /* this call will change */
	EditBuffDirty = 0;			  /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);   /* edit buffer if dirty  */
	}

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;			 /* if there's marked text */
	CharsMarked = 0;			 /* around, clear it and   */
	for ( i = 0; i < MAXLINES; i++ )	 /* redraw the screen	   */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	drawscr(TopRow);
	}

    if ( (TopRow + CurRow) < (TotalLines) ) {	     /* if EOF, do nothing */
	if ( CurRow == (PageSize - 1) ) {   /* if end of screen, scroll up */
	    TopRow += 1;
	    VIOSCROLLUP( 0, 0, CurRow, (LINESIZE - 1), 1,
		       (char far *)buff, 0);
	    getline( (TopRow + CurRow), &EditBuff[0] );
	    drawline();
	    }
	else {
	    CurRow += 1;			  /* else move cursor down */
	    getline( (TopRow + CurRow), &EditBuff[0] );
	    VIOSETCURPOS( CurRow, CurCol, 0 );
	    }
	}

    line25();	      /* reset line numbers on 25th line to reflect change */

    }


/*** home() - moves the cursor to beginning of the line
 *
 *   home() moves the cursor to the position of the first printable character
 *   in the current line. If there is marked text, home() will clear it
 *   without deleting. We need to be able to get correct information about
 *   the length of the current line from LineTable[], so we flush EditBuff[]
 *   on entry; but since the current line does not get changed, we do not
 *   reload it (current contents remain correct) and we do not call line25().
 *
 *   EFFECTS:	  EditBuffDirty  - clears this flag
 *		  LinesMarked	 - clears this flag
 *		  MarkedLine[]	 - clears all
 *		  CharsMarked	 - clears this flag
 *		  MarkedChar[]	 - clears all
 *		  CurCol	 - set to position of first printable character
 */

void home()
    {

    register short i, j;			       /* indexing variables */


    if (EditBuffDirty) {			   /* this call will need to */
	EditBuffDirty = 0;			   /* get correct info on    */
	flushline((TopRow + CurRow), EditBuff);    /* line length, so flush  */
	}					   /* edit buffer if dirty   */

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;			   /* if there's marked text */
	CharsMarked = 0;			   /* around, clear it and   */
	for ( i = 0; i < MAXLINES; i++ )	   /* redraw the screen      */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	drawscr(TopRow);
	}

    if( (TopRow + CurRow) < TotalLines ) {
	i = LineTable[TopRow + CurRow]->linelength;	/* i is position of  */
	i--;						/* last char in line */
	}
    else i = 0; 	    /* if we're off the end of the file, that's 0... */


    for( j = 0; ( j <= i )  &&	!isgraph(EditBuff[j]); j++ );

    CurCol = j; 					 /* j is position of */
    VIOSETCURPOS( CurRow, CurCol, 0 );			 /* first non-blank  */

    }


/*** end_() - moves the cursor to one position past the end of the current line
 *
 *   end_() moves the cursor to one position beyond the position of the last
 *   printable character in the current line. If the last printable character
 *   is already in column 80, we just go there. If there is marked text,
 *   end_() will clear it without deleting. We need to be able to get correct
 *   information about the length of the current line from LineTable[], so
 *   we flush EditBuff[] on entry; but since the current line does not get
 *   changed, we do not reload it (current contents remain correct) and we
 *   do not call line25(). Note that this routine had to be named end_() not
 *   end() since that is a reserved word under the compiler used.
 *
 *   EFFECTS:	  EditBuffDirty  - clears this flag
 *		  LinesMarked	 - clears this flag
 *		  MarkedLine[]	 - clears all
 *		  CharsMarked	 - clears this flag
 *		  MarkedChar[]	 - clears all
 *		  CurCol	 - set to position of last printable
 *				   character, plus one if possible
 */

void end_()
    {

    register short i;				       /* indexing variable */

    if (EditBuffDirty) {			  /* this call will need to */
	EditBuffDirty = 0;			  /* get correct info on    */
	flushline((TopRow + CurRow), EditBuff);   /* line length, so flush  */
	}					  /* edit buffer if dirty   */

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;			  /* if there's marked text */
	CharsMarked = 0;			  /* around, clear it and   */
	for ( i = 0; i < MAXLINES; i++ )	  /* redraw the screen	    */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	drawscr(TopRow);
	}

    if( (TopRow + CurRow) < TotalLines ) {
	i = LineTable[TopRow + CurRow]->linelength;    /* i is position of  */
	}					       /* last char in line */
    else i = 0;

    CurCol = i;
    VIOSETCURPOS( CurRow, CurCol, 0 );

    }


/*** pgup() - moves up one screen
 *
 *   pgup() subtracts PageSize from the current value of TopRow and then
 *   redraws the screen, effectively moving us up one screen. If TopRow
 *   is less than PageSize, we just go to the start of the file. The
 *   position of the cursor on the screen is not altered. If there is marked
 *   text, pgup() will clear it without deleting. Because this call changes
 *   the current line, we flush and reload EditBuff[].
 *
 *   EFFECTS:	  EditBuff[]	 - flushes and reloads
 *		  EditBuffDirty  - clears this flag
 *		  LinesMarked	 - clears this flag
 *		  MarkedLine[]	 - clears all
 *		  CharsMarked	 - clears this flag
 *		  MarkedChar[]	 - clears all
 *		  TopRow	 - gets PageSize subtracted from it
 *				   if possible, otherwise set to 0.
 */

void pgup()
    {

    register short i;				      /* indexing variable */

    if (EditBuffDirty) {			  /* this call will change */
	EditBuffDirty = 0;			  /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);   /* edit buffer if dirty  */
	}

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;			 /* if there's marked text */
	CharsMarked = 0;			 /* around, clear it and   */
	for ( i = 0; i < MAXLINES; i++ )	 /* redraw the screen	   */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	}

    if ( TopRow >= PageSize )		 /* go up one page if there's room */
	TopRow -= PageSize;
    else {
	TopRow = 0;		       /* otherwise just go to top of file */
	CurRow = 0;
	VIOSETCURPOS( CurRow, CurCol, 0 );
	}

    drawscr(TopRow);				     /* redraw screen and  */
    getline( (TopRow + CurRow), &EditBuff[0] );      /* update edit buffer */
    line25();	      /* reset line numbers on 25th line to reflect change */

    }


/*** pgdn() - moves down one screen
 *
 *   pgdn() adds PageSize to the current value of TopRow and then
 *   redraws the screen, effectively moving us down one screen. If the
 *   result of adding TopRow and PageSize is greater than TotalLines
 *   (in other words, we would be moving off the end of the file), then
 *   we just move to one beyond the end of the file. The position of the
 *   cursor on the screen is not altered. If there is marked text,
 *   pgdn() will clear it without deleting. Because this call changes
 *   the current line, we flush and reload EditBuff[].
 *
 *   EFFECTS:	  EditBuff[]	 - flushes and reloads
 *		  EditBuffDirty  - clears this flag
 *		  LinesMarked	 - clears this flag
 *		  MarkedLine[]	 - clears all
 *		  CharsMarked	 - clears this flag
 *		  MarkedChar[]	 - clears all
 *		  TopRow	 - gets PageSize added to it if possible,
 *				   otherwise set to TotalLines - PageSize - 1.
 */

void pgdn()
    {

    register short i;					/* indexing variable */

    if (EditBuffDirty) {			    /* this call will change */
	EditBuffDirty = 0;			    /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);     /* edit buffer if dirty  */
	}

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;			   /* if there's marked text */
	CharsMarked = 0;			   /* around, clear it	     */
	for ( i = 0; i < MAXLINES; i++ )
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	}

    TopRow += PageSize; 				/* go down one page  */
    if ( TopRow > (TotalLines - (PageSize -1)) ) {	/* if there's room,  */
	TopRow = (TotalLines - (PageSize -1));		/* else stop at last */
	CurRow = (PageSize -1); 			/* full page	     */
	VIOSETCURPOS(CurRow, CurCol, 0);
	}

    drawscr(TopRow);				       /* redraw screen and  */
    getline( (TopRow + CurRow), &EditBuff[0] );        /* update edit buffer */
    line25();		/* reset line numbers on 25th line to reflect change */

    }


/*** bksp() - deletes character to right of the cursor; moves cursor right one
 *
 *   bksp() clears any marked text without deleting it, flushes EditBuff[],
 *   moves the cursor right one position and calls del(). If we're at the
 *   beginning of a line so there's nothing to our right, calls badkey().
 *
 *   EFFECTS:	  EditBuff[]	 - flushes and reloads
 *		  EditBuffDirty  - clears this flag
 *		  LinesMarked	 - clears this flag
 *		  MarkedLine[]	 - clears all
 *		  CharsMarked	 - clears this flag
 *		  MarkedChar[]	 - clears all
 *		  CurCol	 - decrements if possible
 *
 *   The effects of del() should also be considered since it may be
 *   called from this function.
 */

void bksp()
    {

    register short i;				       /* indexing variable */

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;			  /* if there's marked text */
	CharsMarked = 0;			  /* around, clear it and   */
	for ( i = 0; i < MAXLINES; i++ )	  /* redraw the screen	    */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;

	if (EditBuffDirty) {			     /* flush before redraw */
	    EditBuffDirty = 0;			     /* to get any changes  */
	    flushline((TopRow + CurRow), EditBuff);
	    }
	drawscr(TopRow);
	}

    if( CurCol == 0 )		 /* can't backspace if already at left edge */
	badkey();
    else {
	CurCol -= 1;			/* otherwise, move left one and del */
	VIOSETCURPOS( CurRow, CurCol, 0 );
	del();
	}
    }


/*** del() - deletes all indicated text
 *
 *   del() is the function for deleting text. What gets deleted depends on
 *   what is marked. If there are marked lines around (as indicated by the
 *   LinesMarked flag) then we delete the marked lines, clear all the line
 *   marking flags, and adjust LineTable[] to reflect the change. Note that
 *   if there are lines marked, there cannot be characters marked too. If
 *   there are no lines marked, but characters marked, these characters are
 *   deleted and EditBuff[] is adjusted to reflect the change and flagged
 *   as dirty. If there are no lines or characters marked, then the character
 *   under the cursor is deleted and EditBuff[] is adjusted accordingly and
 *   marked as dirty.
 *
 *   EFFECTS:	  EditBuff[]	 - contents altered if no lines were marked
 *		  EditBuffDirty  - set if no lines were marked
 *		  LineTable[]	 - contents altered if lines were marked
 *		  TotalLines	 - altered if lines were marked
 *		  LinesMarked	 - clears this flag
 *		  MarkedLine[]	 - clears all
 *		  CharsMarked	 - clears this flag
 *		  MarkedChar[]	 - clears all
 *		  TopRow	 - may change, depending on what's marked
 *		  CurRow	 - may change, depending on what's marked
 *
 */

void del()
    {

    register short i, j;				/* indexing variables */

    if (LinesMarked) {			      /* if lines are marked, do this */
	for( i = 0; !MarkedLine[i]; i++ );
	j = i;				   /* i, j refer to first marked line */
	if( i <= TopRow ) {
	    if( i != 0 ) {
		TopRow = i - 1; 	  /* if first line to be deleted is   */
		CurRow = 1;		  /* off the top of the screen, then  */
		}			  /* set TopRow to last unmarked line */
	    else {
		TopRow = 0;		   /* or if there are no unmarked     */
		CurRow = 0;		   /* lines ahead of the marked ones, */
		}			   /* set TopRow to start of the file */
	    }
	else CurRow = i - TopRow;
	for( ; MarkedLine[i]; i++ )		   /* delete all marked lines */
	    deleteline( i );		   /* i refers to first unmarked line */
	for( ; i < TotalLines; i++,j++ )		/* adjust LineTable[] */
	    LineTable[j] = LineTable[i];
	TotalLines -= ( i - j );			 /* adjust TotalLines */
	LinesMarked = 0;
	for( i = 0; i < MAXLINES; i++ ) 	  /* clear line marking flags */
	    MarkedLine[i] = 0;
	drawscr(TopRow);				     /* redraw screen */
	VIOSETCURPOS(CurRow, CurCol, 0);
	getline( (TopRow + CurRow), &EditBuff[0] );	 /* reload EditBuff[] */
	}

    else if (CharsMarked) {   /* if no lines marked but chars marked, do this */
	for( i = 0; !MarkedChar[i] & ( i < LINESIZE ); i++ );
	j = i;			      /* i, j refer to first marked character */
	CurCol = j;
	VIOSETCURPOS(CurRow, CurCol, 0);	    /* adjust cursor position */
	for( ; MarkedChar[i] & (i < LINESIZE); i++ ); /* i now first unmarked */
	for( ; i < LINESIZE; i++, j++ )
	    EditBuff[j] = EditBuff[i];	 /* remove characters between i and j */
	for( ; j < LINESIZE; j++ )
	    EditBuff[j] = ' ';		      /* fill end of line with blanks */
	CharsMarked = 0;
	for( i = 0; i < LINESIZE; i++ )  /* clear all character marking flags */
	    MarkedChar[i] = 0;
	drawline();					       /* redraw line */
	EditBuffDirty = 1;			  /* mark EditBuff[] as dirty */
	}

    else {			      /* if no lines or chars marked, do this */
	for( i = (CurCol + 1); i < LINESIZE; i++ )
	   EditBuff[i-1] = EditBuff[i];      /* remove character under cursor */
	EditBuff[LINESIZE-1] = ' ';	       /* fill end of line with blank */
	drawline();					       /* redraw line */
	EditBuffDirty = 1;			  /* mark EditBuff[] as dirty */
	}

    line25();		/* reset line numbers on 25th line to reflect changes */

    }


/*** F9() - saves the file being edited and exits
 *
 *   F9() save the file being edited with any changes that have been made
 *   and exits the editor. The user is given a prompt to be sure they want
 *   do quit; hitting carriage return goes ahead with the save-and-quit,
 *   while any other key returns you to the editor.
 *
 *   EFFECTS:	  If quit not completed:    EditBuff[]	  -  gets flushed
 *					    EditBuffDirty -  gets cleared
 */

void F9()
    {

    static char message[LINESIZE] = "Save file as:";
    struct  KeyData keydata;
    char attrib = Fore25 + Back25;
    int i;

    if (EditBuffDirty) {		   /* flush the edit buffer if dirty */
	EditBuffDirty = 0;		   /* so all editting will be saved  */
	flushline((TopRow + CurRow), EditBuff);
	}

    for( i = 0; (i < 65) && (fname[i] != 0); message[i + 14] = fname[i], i++ );
					/* copy filename into prompt message */

    VIOWRTCHARSTRATT( message, 80, PageSize, 0, &attrib, 0 );
							/* write out message */

    KBDCHARIN( &keydata, 0, 0 );		   /* wait for user response */

    if( keydata.char_code == CRETURN_CHAR ) {	  /* if response was <CR>... */
	if ( !savefile(fhandle)) {
	    freesegs();
	    VIOSETCURPOS( 24, 0, 0 );		    /* ... clean up and quit */
	    quit(0);
	    }
	else error25(11);
	}
    name25();				 /* ... otherwise restore message on */
					 /* 25th line and return to editor   */
    }


/*** F10() - exits the editor without saving the file being edited
 *
 *   F10() exits from the editor without saving back the file being edited,
 *   leaving the file as it was before the editor was invoked. A prompt
 *   will be issued to give the user a chance to change their mind before
 *   all editing is lost.
 *
 *   EFFECTS:	  If quit not completed:    EditBuff[]	  -  gets flushed
 *					    EditBuffDirty -  gets cleared
 */


void F10()
    {
    static char message[51] = "WARNING: Any edits will be lost   (y/n)?           ";
    struct  KeyData keydata;
    char attrib = Fore25 + Back25;		   /* attributes for message */
    int  row, col;			  /* to save current cursor position */
    int  done = 0;		/* flag to be set when have a valid response */

    if (EditBuffDirty) {			/* flush EditBuff[] if dirty */
	EditBuffDirty = 0;
	flushline((TopRow + CurRow), EditBuff);
	}

    VIOWRTCHARSTRATT( message, 51, PageSize, 0, &attrib, 0 );
    VIOGETCURPOS( &row, &col, 0 );		    /* print prompt and set  */
    VIOSETCURPOS( PageSize, 42, 0 );		    /* cursor to await reply */

    while( !done ) {

	KBDCHARIN( &keydata, 0, 0 );		 /* get a character from KBD */

	if( (keydata.char_code == 'y') || (keydata.char_code == 'Y') ) {
	    VIOWRTCHARSTRATT( &keydata.char_code, 1, PageSize, 42, &attrib, 0 );
	    freesegs();
	    quit(0);				/* if Y, quit without saving */
	    }
	else if( (keydata.char_code == 'n') || (keydata.char_code == 'N') ) {
	    VIOSETCURPOS( row, col, 0 );
	    name25();
	    done = 1;				   /* if N, return to editor */
	    }
	else badkey();			   /* otherwise wait for another key */

	}

    }


/*** ctrl_home() - repositions cursor to upper left hand corner of screen
 *
 *   ctrl_home() moves the cursor to the upper left hand corner of the
 *   screen, coordinate position (0,0). Since the current cursor line is
 *   probably changed, we flush and reload EditBuff[]. If there is any
 *   text marked, this call will clear the marking without deleting.
 *   Line number information on 25th line is redrawn to reflect changes.
 *   The actual content of the screen - i.e. the value of TopRow - is
 *   unchanged by this call.
 *
 *   EFFECTS:	  EditBuff[]	 - flushes and reloads
 *		  EditBuffDirty  - clears this flag
 *		  LinesMarked	 - clears this flag
 *		  MarkedLine[]	 - clears all
 *		  CharsMarked	 - clears this flag
 *		  MarkedChar[]	 - clears all
 *		  CurRow	 - set to 0
 *		  CurCol	 - set to 0
 */

void ctrl_home()
    {

    register short i;				      /* indexing variable */

    if (EditBuffDirty) {			  /* this call will change */
	EditBuffDirty = 0;			  /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);   /* edit buffer if dirty  */
	}

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;			 /* if there's marked text */
	CharsMarked = 0;			 /* around, clear it and   */
	for ( i = 0; i < MAXLINES; i++ )	 /* redraw the screen	   */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	drawscr(TopRow);
	}

    if ( CurRow != 0  ||  CurCol != 0 ) {
	CurRow = 0;				    /* reposition to (0,0) */
	CurCol = 0;
	VIOSETCURPOS( CurRow, CurCol, 0 );
	getline( (TopRow + CurRow), &EditBuff[0] );
	}

    line25();	    /* update line numbers on 25th line to reflect changes */

    }


/*** ctrl_end() - repositions cursor to lower left hand corner of screen
 *
 *   ctrl_end() moves the cursor to the lower left hand corner of the
 *   screen. Since the current cursor line is probably changed, we flush
 *   and reload EditBuff[]. If there is any text marked, this call will
 *   clear the marking without deleting. Line number information on 25th
 *   line is redrawn to reflect changes. The actual content of the screen
 *   is unchanged by this call.
 *
 *   EFFECTS:	  EditBuff[]	 - flushes and reloads
 *		  EditBuffDirty  - clears this flag
 *		  LinesMarked	 - clears this flag
 *		  MarkedLine[]	 - clears all
 *		  CharsMarked	 - clears this flag
 *		  MarkedChar[]	 - clears all
 *		  CurRow	 - set to PAGESIZE - 1
 *		  CurCol	 - set to 0
 */

void ctrl_end()
    {

    register short i;

    if (EditBuffDirty) {			  /* this call will change */
	EditBuffDirty = 0;			  /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);   /* edit buffer if dirty  */
	}

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;			 /* if there's marked text */
	CharsMarked = 0;			 /* around, clear it and   */
	for ( i = 0; i < MAXLINES; i++ )	 /* redraw the screen	   */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	drawscr(TopRow);
	}

    if( CurRow != (PageSize - 1) || CurCol != 0 ) {
	CurRow = (PageSize - 1);		      /* reposition cursor */
	CurCol = 0;
	VIOSETCURPOS( CurRow, CurCol, 0 );
	getline( (TopRow + CurRow), &EditBuff[0] );
	}

    line25();	    /* update line numbers on 25th line to reflect changes */

    }


/*** ctrl_pgup() - repositions cursor to beginning of file
 *
 *   ctrl_pgup() redraws the screen with the first screenfull of the file
 *   and moves the cursor to the upper left hand corner of the screen,
 *   coordinate position (0,0). Since the current cursor line is
 *   probably changed, we flush and reload EditBuff[]. If there is any
 *   text marked, this call will clear the marking without deleting.
 *   Line number information on 25th line is redrawn to reflect changes.
 *
 *   EFFECTS:	  EditBuff[]	 - flushes and reloads
 *		  EditBuffDirty  - clears this flag
 *		  LinesMarked	 - clears this flag
 *		  MarkedLine[]	 - clears all
 *		  CharsMarked	 - clears this flag
 *		  MarkedChar[]	 - clears all
 *		  TopRow	 - set to 0
 *		  CurRow	 - set to 0
 *		  CurCol	 - set to 0
 */

void ctrl_pgup()
    {

    register short i;				      /* indexing variable */

    if (EditBuffDirty) {			  /* this call will change */
	EditBuffDirty = 0;			  /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);   /* edit buffer if dirty  */
	}

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;			 /* if there's marked text */
	CharsMarked = 0;			 /* around, clear it - no  */
	for ( i = 0; i < MAXLINES; i++ )	 /* need to redraw screen  */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	}

    TopRow = 0;
    CurRow = 0; 				  /* reposition and redraw */
    CurCol = 0;
    VIOSETCURPOS( CurRow, CurCol, 0 );
    drawscr(TopRow);
    getline( (TopRow + CurRow), &EditBuff[0] );       /* reload EditBuff[] */
    line25();					       /* update 25th line */

    }


/*** ctrl_pgdn() - repositions cursor to beginning of file
 *
 *   ctrl_pgdn() redraws the screen with the last screenfull of the file
 *   and moves the cursor to the lower left hand corner of the screen,
 *   one line past the end of the file. Since the current cursor line is
 *   probably changed, we flush and reload EditBuff[]. If there is any
 *   text marked, this call will clear the marking without deleting.
 *   Line number information on 25th line is redrawn to reflect changes.
 *
 *   EFFECTS:	  EditBuff[]	 - flushes and reloads
 *		  EditBuffDirty  - clears this flag
 *		  LinesMarked	 - clears this flag
 *		  MarkedLine[]	 - clears all
 *		  CharsMarked	 - clears this flag
 *		  MarkedChar[]	 - clears all
 *		  TopRow	 - set to TotalLines - (PageSize - 1)
 *		  CurRow	 - set to end of file + 1
 *		  CurCol	 - set to 0
 */

void ctrl_pgdn()
    {
    register short i;				      /* indexing variable */

    if (EditBuffDirty) {			  /* this call will change */
	EditBuffDirty = 0;			  /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);   /* edit buffer if dirty  */
	}

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;			 /* if there's marked text */
	CharsMarked = 0;			 /* around, clear it - no  */
	for ( i = 0; i < MAXLINES; i++ )	 /* need to redraw screen  */
	    MarkedLine[i] = 0;			 /* as it gets done later  */
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	}
    if (TotalLines > (PageSize - 1))
	TopRow = TotalLines - (PageSize - 1);	     /* recalculate TopRow */
    else TopRow = 0;
    CurRow = TotalLines - TopRow;	      /* set cursor row and column */
    CurCol = 0;
    VIOSETCURPOS( CurRow, CurCol, 0 );
    getline( (TopRow + CurRow), &EditBuff[0] );       /* reload EditBuff[] */
    drawscr(TopRow);					  /* redraw screen */
    line25();					       /* update 25th line */

    }


/*** shift_left() - moves cursor left one and marks that character for deletion.
 *
 *   shift_left() moves the cursor one position to the left. If we are on
 *   a line that is already marked, that's all it does - just like regular
 *   left, but markings are not cleared. If we are not on a marked line,
 *   then the character we just moved to is marked for deletion. Unless it
 *   already was marked for deletion, in which case it gets un-marked.
 *
 *   EFFECTS:	  CharsMarked	 - sets this flag if LinesMarked not set
 *		  MarkedChar[]	 - sets this for the character we just moved to
 *		  CurCol	 - decremented one
 */

void shift_left()
    {

    if ( LinesMarked ) {	   /* if this line is marked, just move left */
	if ( CurCol > 0 ) {
	    CurCol -= 1;
	    VIOSETCURPOS( CurRow, CurCol, 0 );
	    }
	}
    else if( CurCol > 0 ) {		/* otherwise, see if there's room... */
	CurCol -= 1;
	VIOSETCURPOS( CurRow, CurCol, 0 );	       /* ...if so move left */
	CharsMarked = 1;
	if (MarkedChar[CurCol])
	    MarkedChar[CurCol] = 0;    /* and set marking for that character */
	else MarkedChar[CurCol] = 1;
	drawline();			   /* redraw line to update markings */
	}

    }


/*** shift_right() - moves cursor right one and marks the character
 *		     we were previously on for deletion.
 *
 *   shift_right() moves the cursor one position to the right. If we are on
 *   a line that is already marked, that's all it does - just like regular
 *   right, but markings are not cleared. If we are not on a marked line,
 *   then the character we just moved from is marked for deletion. Unless it
 *   already was marked for deletion, in which case it gets un-marked.
 *
 *   EFFECTS:	  CharsMarked	 - sets this flag if LinesMarked not set
 *		  MarkedChar[]	 - sets this for the character we just left
 *		  CurCol	 - incremented one
 */

void shift_right()
    {

    if ( LinesMarked ) {	     /* if this line marked, just move right */
	if (CurCol < (LINESIZE - 1)) {
	    CurCol += 1;
	    VIOSETCURPOS( CurRow, CurCol, 0 );
	    }
	}
    else if( CurCol < (LINESIZE - 1)) {  /* otherwise, see if there's room.. */
	CharsMarked = 1;
	if (MarkedChar[CurCol]) 	   /* if so, mark character we're on */
	    MarkedChar[CurCol] = 0;
	else MarkedChar[CurCol] = 1;
	CurCol += 1;				      /* then move right one */
	VIOSETCURPOS( CurRow, CurCol, 0 );
	drawline();					  /* and redraw line */
	}

    }


/*** shift_up() - moves cursor up one line and marks lines for deletion
 *
 *   shift_up() moves the cursor one line up. If no lines are marked, then
 *   both the previous line and the line moved to get marked for deletion.
 *   If lines are already marked, and were marked starting with another
 *   shift_up, then only the line we move to gets marked (the one we just
 *   left would already be marked). If there are lines marked but they
 *   were marked starting with a shift_down, then we un-mark the line we
 *   just left and do nothing to the line moved to. Unless the line moved
 *   to is the last one marked, in which case we un-mark it also to clear all
 *   markings. To determine how lines first got marked - by a shift_up or
 *   shift_down - we give the flag LinesMarked two different TRUE values,
 *   1 or 2 respectively.
 *
 *   EFFECTS:	  EditBuff[]	 - flushes and reloads
 *		  EditBuffDirty  - clears
 *		  LinesMarked	 - sets this flag, unless un-marking only
 *				   marked line present, in which case clears it
 *		  MarkedLine[]	 - may set or clear this
 *		  CharsMarked	 - clears this flag as we now have marked lines
 *		  MarkedChar[]	 - clears all
 *		  TopRow	 - decremented one if at top of page
 *		  CurRow	 - decremented one if not at top of page
 */

void shift_up()
    {
    register short i;				      /* indexing variable */

    char buff[2];	      /* local buffer for scrolling fill character */
    buff[0] = 32;			       /* character = 32 = <blank> */
    buff[1] = ForeHilite + BackHilite;	       /*  attribute = highlighted */

    if (EditBuffDirty) {			  /* this call will change */
	EditBuffDirty = 0;			  /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);   /* edit buffer if dirty  */
	}

    if ( CharsMarked ) {			/* clear CharsMarked flags */
	CharsMarked = 0;			/* since we now will have  */
	for ( i = 0; i < LINESIZE; i++ )	/* the whole line marked.  */
	    MarkedChar[i] = 0;
	}

  /* if no lines currently marked, do this */
    if( !LinesMarked ) {
	LinesMarked = 1;	/* set flag to indicate marking started UP */
	MarkedLine[ TopRow + CurRow ] = 1;		      /* mark line */
	drawline();					      /* redraw it */
	if ( (TopRow + CurRow) != 0 ) { 	       /* if there's room, */
	    MarkedLine[ TopRow + CurRow - 1 ] = 1;	/* mark line above */
	    if ( CurRow == 0 ) {
		TopRow -= 1;			  /* ...may have to scroll */
		VIOSCROLLDN( 0, 0, (PageSize - 1), (LINESIZE - 1), 1,
			   (char far *)buff, 0);
		getline( (TopRow + CurRow), &EditBuff[0] );	 /* reload */
		}
	    else {			   /* ...or may not have to scroll */
		CurRow -= 1;
		getline( (TopRow + CurRow), &EditBuff[0] );	 /* reload */
		VIOSETCURPOS( CurRow, CurCol, 0 );
		}
	    drawline(); 				/* redraw new line */
	    }
	}

  /* if lines marked, and shift_up started marking, do this */
    else if( LinesMarked == 1 ) {
	if ( (TopRow + CurRow) != 0 ) {        /* if not at top of screen, */
	    MarkedLine[ TopRow + CurRow - 1 ] = 1;    /* mark next line up */
	    if ( CurRow == 0 ) {
		TopRow -= 1;			  /* ...may have to scroll */
		VIOSCROLLDN( 0, 0, (PageSize - 1), (LINESIZE - 1), 1,
			   (char far *)buff, 0);
		getline( (TopRow + CurRow), &EditBuff[0] );	 /* reload */
		}
	    else {
		CurRow -= 1;		   /* ...or may not have to scroll */
		getline( (TopRow + CurRow), &EditBuff[0] );	 /* reload */
		VIOSETCURPOS( CurRow, CurCol, 0 );
		}
	    drawline(); 			    /* redraw line move to */
	    }
	}

  /* if lines marked, and shift_down started marking, do this */
    else if( LinesMarked == 2 ) {
	MarkedLine[ TopRow + CurRow ] = 0;	   /* un-mark current line */
	drawline();					      /* redraw it */
	if( (TopRow + CurRow) != 0 ) {
	    if( CurRow == 0 ) { 	     /* ...may have to scroll down */
		TopRow -= 1;
		VIOSCROLLDN( 0, 0, (PageSize - 1), (LINESIZE - 1), 1,
			   (char far *)buff, 0);
		getline( (TopRow + CurRow), &EditBuff[0] );	 /* reload */
		}
	    else {			 /* or may not have to scroll down */
		CurRow -= 1;
		getline( (TopRow + CurRow), &EditBuff[0] );	 /* reload */
		VIOSETCURPOS( CurRow, CurCol, 0 );
		}
	    if( !MarkedLine[ TopRow + CurRow - 1 ] ) { /* if line moved to */
		MarkedLine[ TopRow + CurRow ] = 0;     /* is last marked,  */
		LinesMarked = 0;		       /* clear it too	   */
		drawline();
		}
	    }
	}

    line25();	    /* update line numbers on 25th line to reflect changes */

    }


/*** shift_down() - moves cursor down one line and marks lines for deletion
 *
 *   shift_down() moves the cursor one line down. If no lines are marked, then
 *   both the previous line and the line moved to get marked for deletion.
 *   If lines are already marked, and were marked starting with another
 *   shift_down, then only the line we move to gets marked (the one we just
 *   left would already be marked). If there are lines marked but they
 *   were marked starting with a shift_up, then we un-mark the line we
 *   just left and do nothing to the line moved to. Unless the line moved
 *   to is the last one marked, in which case we un-mark it also to clear all
 *   markings. To determine how lines first got marked - by a shift_up or
 *   shift_down - we give the flag LinesMarked two different TRUE values,
 *   1 or 2 respectively.
 *
 *   EFFECTS:	  EditBuff[]	 - flushes and reloads
 *		  EditBuffDirty  - clears
 *		  LinesMarked	 - sets this flag, unless un-marking only
 *				   marked line present, in which case clears it
 *		  MarkedLine[]	 - may set or clear this
 *		  CharsMarked	 - clears this flag as we now have marked lines
 *		  MarkedChar[]	 - clears all
 *		  TopRow	 - incremented one if at bottom of page
 *		  CurRow	 - incremented one if not at bottom of page
 */

void shift_down()
    {
    register short i;					/* indexing variable */

    char buff[2];			  /* local buffer for fill character */
    buff[0] = 32;				 /* character = 32 = <blank> */
    buff[1] = ForeHilite + BackHilite;		 /* attribute =  highlighted */

    if (EditBuffDirty) {			    /* this call will change */
	EditBuffDirty = 0;			    /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);     /* edit buffer if dirty  */
	}

    if ( CharsMarked ) {			     /* clear all character  */
	CharsMarked = 0;			     /* markings, since line */
	for ( i = 0; i < LINESIZE; i++ )	     /* is now marked.	     */
	    MarkedChar[i] = 0;
	}

  /* if no lines are currently marked, do this */
    if( !LinesMarked ) {
	LinesMarked = 2;       /* set flag to show marking started with DOWN */
	MarkedLine[ TopRow + CurRow ] = 1;		/* mark current line */
	drawline();					    /* and redraw it */
	if( ( TopRow + CurRow ) < TotalLines ) {    /* if room to move down, */
	    MarkedLine[ TopRow + CurRow + 1 ] = 1;	  /* mark line below */
	    if ( CurRow == (PageSize - 1) ) {
		TopRow += 1;			    /* ...may have to scroll */
		VIOSCROLLUP( 0, 0, CurRow, (LINESIZE - 1), 1,
			   (char far *)buff, 0);
		getline( (TopRow + CurRow), &EditBuff[0] );	   /* reload */
		}
	    else {			     /* ...or may not have to scroll */
		CurRow += 1;
		getline( (TopRow + CurRow), &EditBuff[0] );	   /* reload */
		VIOSETCURPOS( CurRow, CurCol, 0 );
		}
	    drawline(); 				  /* redraw new line */
	    }
	}

  /* if lines are marked, and first marking was another shift_down, do this */
    else if( LinesMarked == 2 ) {
	if ( (TopRow + CurRow) < TotalLines ) {     /* if room to move down */
	    MarkedLine[ TopRow + CurRow + 1 ] = 1;	 /* mark line below */
	    if ( CurRow == (PageSize - 1) ) {
		TopRow += 1;			   /* ...may have to scroll */
		VIOSCROLLUP( 0, 0, CurRow, (LINESIZE - 1), 1,
			   (char far *)buff, 0);
		getline( (TopRow + CurRow), &EditBuff[0] );	  /* reload */
		}
	    else {			    /* ...or may not have to scroll */
		CurRow += 1;
		getline( (TopRow + CurRow), &EditBuff[0] );	  /* reload */
		VIOSETCURPOS( CurRow, CurCol, 0 );
		}
	    drawline(); 	/* redraw the current line to show markings */
	    }
	}

  /* if lines are marked, and first marking was a shift_up, do this */
    else if( LinesMarked == 1 ) {
	MarkedLine[ TopRow + CurRow ] = 0;	    /* un-mark current line */
	drawline();					   /* and redraw it */
	if ( (TopRow + CurRow) < TotalLines ) {
	    if ( CurRow == (PageSize - 1) ) {
		TopRow += 1;			   /* ...may have to scroll */
		VIOSCROLLUP( 0, 0, CurRow, (LINESIZE - 1), 1,
			   (char far *)buff, 0);
		getline( (TopRow + CurRow), &EditBuff[0] );	  /* reload */
		}
	    else {			    /* ...or may not have to scroll */
		CurRow += 1;
		getline( (TopRow + CurRow), &EditBuff[0] );	  /* reload */
		VIOSETCURPOS( CurRow, CurCol, 0 );
		}
	    }
	if( (TopRow + CurRow) < TotalLines ) {
	    if( !MarkedLine[ TopRow + CurRow + 1 ] ) {
		MarkedLine[ TopRow + CurRow ] = 0;
		LinesMarked = 0;		     /* if only 1 line left */
		}				  /* marked, un-mark it too */
	    }
	drawline();			 /* redraw line to reflect markings */

	}

    line25();		 /* update line info on bottom line to show changes */

    }


/*** shift_home() - moves cursor to first character in current line and
 *		    marks all characters in between for deletion
 *
 *   shift_down() moves the cursor to the first printable character in the
 *   current line. If the line is already marked, that's all it does, just
 *   like a regular home except markings are not cleared. Otherwise, all
 *   characters between the cursor position when the function is called and
 *   the first character in the line get their markings toggled - that is, if
 *   they are not marked they get marked, if they are already marked they
 *   get un-marked.
 *
 *   EFFECTS:	  EditBuff[]	 - flushes and reloads
 *		  EditBuffDirty  - clears
 *		  CharsMarked	 - sets this flag unless line is marked
 *		  MarkedChar[]	 - sets for all appropriate characters
 *		  CurCol	 - set to position of first printable character
 */

void shift_home()
    {

    register short i, j;			       /* indexing variables */

    if (EditBuffDirty) {			   /* this call will need to */
	EditBuffDirty = 0;			   /* get correct info on    */
	flushline((TopRow + CurRow), EditBuff);    /* line length, so flush  */
	}					   /* edit buffer if dirty   */

    if( (TopRow + CurRow) < TotalLines ) {
	i = LineTable[TopRow + CurRow]->linelength;	/* i is position + 1 */
	}						/* last char in line */
    else i = 0;

    for( j = 0; ( j < i )  &&  !isgraph(EditBuff[j]); j++ );
							 /* j is position of */
							 /* first non-blank  */
    if( !LinesMarked ) {			  /* if no lines marked,     */
	if( j < CurCol ) {			  /* flag everything between */
	    CharsMarked = 1;			  /* CurCol and j as Marked  */
	    for( i = j; i < CurCol; i++ ) {
		if( !MarkedChar[i] )
		    MarkedChar[i] = 1;		 /* may be moving forward... */
		else MarkedChar[i] = 0;
		}
	    }
	else if( j > CurCol ) {
	    CharsMarked = 1;
	    for( i = CurCol; i < j; i++ ) {		  /* ... or backward */
		if( !MarkedChar[i] )
		    MarkedChar[i] = 1;
		else MarkedChar[i] = 0;
		}
	    }
	}

    CurCol = j; 			   /* if lines are marked, just move */
    VIOSETCURPOS( CurRow, CurCol, 0 );
    drawline(); 			     /* redraw line to show markings */

    }


/*** shift_end() - moves cursor to one beyond last character in current line
 *		   and marks all characters in between for deletion
 *
 *   shift_down() moves the cursor to one position beyond the last printable
 *   character in the current line. If the line is already marked, that's all
 *   it does, just like a regular end_ except markings are not cleared.
 *   Otherwise, all characters between the cursor position when the function
 *   is called and the final cursor position get their markings toggled - that
 *   is, if they are not marked they get marked, if they are already marked
 *   they get un-marked.
 *
 *   EFFECTS:	  EditBuff[]	 - flushes and reloads
 *		  EditBuffDirty  - clears
 *		  CharsMarked	 - sets this flag unless line is marked
 *		  MarkedChar[]	 - sets for all appropriate characters
 *		  CurCol	 - set to one beyond position of
 *				   last printable character
 */

void shift_end()
    {

    register short i, j;			      /* indexing variables */

    if (EditBuffDirty) {			  /* this call will need to */
	EditBuffDirty = 0;			  /* get correct info on    */
	flushline((TopRow + CurRow), EditBuff);   /* line length, so flush  */
	}					  /* edit buffer if dirty   */

    if( (TopRow + CurRow) < TotalLines ) {
	i = LineTable[TopRow + CurRow]->linelength;    /* i is position + 1 */
	}					       /* last char in line */
    else i = 0;

    if( !LinesMarked ) {			 /* if no lines marked,     */
	if( i < CurCol ) {			 /* flag everything between */
	    CharsMarked = 1;			 /* CurCol and i as Marked  */
	    for( j = i; j < CurCol; j++ ) {
		if( !MarkedChar[j] )		    /* may move forwards... */
		    MarkedChar[j] = 1;
		else MarkedChar[j] = 0;
		}
	    }
	else if( i > CurCol ) {
	    CharsMarked = 1;				/* ... or backwards */
	    for( j = CurCol; j < i; j++ ) {
		if( !MarkedChar[j] )
		    MarkedChar[j] = 1;
		else MarkedChar[j] = 0;
		}
	    }
	}

    CurCol = i; 			    /* if line is marked, just move */
    VIOSETCURPOS( CurRow, CurCol, 0 );
    drawline(); 						  /* redraw */

    }
