#include <ctype.h>
#include <subcalls.h>
#include "ssedefs.h"




/*** creturn - handles carage return
 *
 *   creturn breaks the line reference by linenum at the
 *     current cursor position and inserts the second half of
 *     the 'line' one line below.
 *
 *   creturn()
 *
 *   ENTRY - none
 *
 *   creturn will EXIT the program by calling error25 if
 *     it is unable to add a line to the file or the number
 *     of lines in the file becomes greater than MAXLINES.
 *
 *   EFFECTS  TotalLines    - by incrementing it once or twice
 *	      EditBuffDirty - by clearing it if set
 *	      LinesMarked   - by clearing it if set
 *	      CharsMarked   - by clearing it if set
 *	      MarkedChar    - by clearing it if set
 *	      MarkedLine    - by clearing it if set
 *	      LineTable     - by insterting the second part of the line
 *				 and via 'addline'
 *	      CurRow	    - by incrementing it
 *	      TopRow	    - by incrementing it
 *	      CurCol	    - by setting it to zero
 *	      EditBuff	    - by getting the second part of the line
 *				 via 'getline'
 *	      SegTable	    - by using free space for the both parts
 *				 of the line via 'allocseg'
 *			      by marking the segment containing the original
 *				 line as needing compacting via 'deleteline'
 *			      by marking the original line in the segment
 *				 as being deleted. via 'deleteline'
 *	      TotalSegs     - via 'alloseg'
 *	      memory	    - by allocating segments via 'allocseg'
 *	      the screen    - via 'drawscr'
 */


void creturn()
{
   register unsigned short i;
   register unsigned short linenum;

/* clear flags if set */
   if (EditBuffDirty) { 			  /* this call will change */
       EditBuffDirty = 0;			  /* cursor line, so flush */
       flushline((TopRow + CurRow), EditBuff);	  /* edit buffer if dirty  */
   }
/* clear flags if set */
   if ( LinesMarked || CharsMarked ) {
       LinesMarked = 0; 		       /* if there's marked text */
       CharsMarked = 0; 		       /* around, clear it and	 */
       for ( i = 0; i < MAXLINES; i++ )        /* redraw the screen	 */
	   MarkedLine[i] = 0;
       for ( i = 0; i < LINESIZE; i++ )
	   MarkedChar[i] = 0;
   }

/* expand LineTable for second part of line */
   linenum = TopRow + CurRow;
   if ( !(linenum == TotalLines)) {
      deleteline(linenum);		  /* mark orginal line as deleted */
      for (i = TotalLines; i > (linenum +1); i--)
	  LineTable[i] = LineTable[i -1];
      ++TotalLines;
   }
   else {
      ++TotalLines;
      ++TotalLines;
   }

/* find first non blank in first part of line to set new line length to it */
   for (i = (CurCol -1); !isgraph(EditBuff[i]) && inline(i, 0); i--);
   if ( (addline(linenum, (i +1), EditBuff) != 0) || !(TotalLines < MAXLINES))
      error25(7);
/*    call error mesage to save or quit */

   ++linenum;
/* find first non blank in second part of line to set new line length to it */
   for (i = (LINESIZE -1); !(isgraph(EditBuff[i])) &&  inline(i, CurCol); i--);
   if (addline(linenum, (i - CurCol +1), &EditBuff[CurCol]) != 0)
      error25(7);
/*    call error message to save or quit */

/* move cursor to begining of the second part of the line */
   if (CurRow < (PageSize - 1))
      ++CurRow;
   else
      ++TopRow;
   CurCol = 0;
   VIOSETCURPOS(CurRow, CurCol, 0);
   drawscr(TopRow);

   getline(linenum, EditBuff);	  /* set EditBuff to the current line */

   line25();
}




/***   ctrl_b - insert line below
 *
 *     ctrl_b inserts a blank line below the current line
 *	 and moves the cursor to the inserted line.
 *
 *     ctrl_b ()
 *
 *     ctrl_b expands LineTable to insert the new line below
 *	 the current line and then inserts the new line via
 *	 'addline' with a length of zero. Also ctrl_b places
 *	 the cursor on the insert line in the same column as
 *	 the current line.
 *	 ctrl_b will EXIT the program by calling error25 if
 *	 it is unable to add a line to the file or the number
 *	 of lines in the file becomes greater than MAXLINES.
 *
 *   EFFECTS  TotalLines    - by incrementing it once or twice
 *	      EditBuffDirty - by clearing it if set
 *	      LinesMarked   - by clearing it if set
 *	      CharsMarked   - by clearing it if set
 *	      MarkedChar    - by clearing it if set
 *	      MarkedLine    - by clearing it if set
 *	      LineTable     - by insterting the new line and via 'addline'
 *	      CurRow	    - by incrementing it
 *	      TopRow	    - by incrementing it
 *	      EditBuff	    - by setting it to a line of blanks via 'getline'
 *	      SegTable	    - by using free space for the new line
 *				 via 'allocseg'
 *			      by marking the current line in the segment
 *				 as being deleted if it was changed. via
 *				 'fushline' via 'deleteline'
 *	      TotalSegs     - via 'alloseg'
 *	      memory	    - by allocating segments via 'allocseg'
 *	      the screen    - via 'drawscr'
 */


void ctrl_b()
    {
    register unsigned short i;

    if (EditBuffDirty) {			  /* this call will change */
	EditBuffDirty = 0;			  /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);   /* edit buffer if dirty  */
	}

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;		       /* if there's marked text */
	CharsMarked = 0;		       /* around, clear it and	 */
	for ( i = 0; i < MAXLINES; i++ )       /* redraw the screen	 */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	}

/*   expand LineTable to insert the new line below */
     for (i = TotalLines; i > (TopRow + CurRow +1); i--)
       LineTable[i] = LineTable[i -1];
     if ( (TopRow + CurRow) == TotalLines) {
	 ++TotalLines;
	 if ((addline(TopRow +CurRow, 0, 0) != 0) || !(TotalLines < MAXLINES))
	    error25(7);
/*	    call error message to save or quit */

	 }
     ++TotalLines;
     if ((addline(TopRow + CurRow +1, 0, 0) != 0) || !(TotalLines < MAXLINES))
	 error25(7);
/*	 call error message to save or quit */

/*   place the cursor on the new line below */
     if (CurRow < (PageSize - 1))
       ++CurRow;
     else
       ++TopRow;

/*   redraw the screen with the new line below */
     VIOSETCURPOS(CurRow, CurCol, 0);
     drawscr(TopRow);

     getline( (TopRow + CurRow), &EditBuff[0] );

     line25();

}




/***   ctrl_n - insert line above
 *
 *     ctrl_n inserts a blank line above the current line
 *	 and move the cursor to the inserted line.
 *
 *     ctrl_n ()
 *
 *     ctrl_n expands LineTable to insert the new line above
 *	 the current line and then inserts the new line via
 *	 'addline' with a length of zero. Also ctrl_n places
 *	 the cursor on the inserted line in the same column as
 *	 the current line.
 *	 ctrl_n will EXIT the program by calling error25 if
 *	 it is unable to add a line to the file or the number
 *	 of lines in the file becomes greater than MAXLINES.
 *
 *   EFFECTS  TotalLines    - by incrementing it once or twice
 *	      EditBuffDirty - by clearing it if set
 *	      LinesMarked   - by clearing it if set
 *	      CharsMarked   - by clearing it if set
 *	      MarkedChar    - by clearing it if set
 *	      MarkedLine    - by clearing it if set
 *	      LineTable     - by insterting the new line and via 'addline'
 *	      CurRow	    - by decrementing it
 *	      TopRow	    - by incrementing it
 *	      EditBuff	    - by setting it to a line of blanks via 'getline'
 *	      SegTable	    - by using free space for the new line
 *				 via 'allocseg'
 *			      by marking the current line in the segment
 *				 as being deleted if it was changed. via
 *				 'fushline' via 'deleteline'
 *	      TotalSegs     - via 'alloseg'
 *	      memory	    - by allocating segments via 'allocseg'
 *	      the screen    - via 'drawscr'
 */


void ctrl_n()
{
    register unsigned short i;

    if (EditBuffDirty) {			  /* this call will change */
	EditBuffDirty = 0;			  /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);   /* edit buffer if dirty  */
	}

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;		       /* if there's marked text */
	CharsMarked = 0;		       /* around, clear it and	 */
	for ( i = 0; i < MAXLINES; i++ )       /* redraw the screen	 */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	}

 /* expand LineTable to insert the new line above */
    for (i = TotalLines; i > (TopRow + CurRow); i--)
      LineTable[i] = LineTable[i -1];
 /* insert the new line above current line */
    if ( (TopRow + CurRow) == TotalLines) {
      ++TotalLines;
      if ((addline(TopRow + CurRow + 1, 0, 0) != 0) || !(TotalLines < MAXLINES))
	    error25(7);
/*	    call error message to save or quit */
    }
    ++TotalLines;
    if ((addline(TopRow + CurRow, 0, 0) != 0) || !(TotalLines < MAXLINES))
	error25(7);
/*	call error message to save or quit */

/*  redraw the screen with the cursor on the insert line above */
    if (CurRow > 0) {
	--CurRow;
	++TopRow;
	VIOSETCURPOS(CurRow, CurCol, 0);
    }
    drawscr(TopRow);
    getline( (TopRow + CurRow), &EditBuff[0] );
    line25();
}




/***   ctrl_y - delete current line
 *
 *     ctrl_y deletes the current line.
 *
 *     ctrl_y ()
 *
 *     ctrl_y marks the current line as deleted and
 *	 compacts the LineTable.
 *
 *
 *   EFFECTS  TotalLines    - by decrementing it
 *	      EditBuffDirty - by clearing it if set
 *	      LinesMarked   - by clearing it if set
 *	      CharsMarked   - by clearing it if set
 *	      MarkedChar    - by clearing it if set
 *	      MarkedLine    - by clearing it if set
 *	      LineTable     - by marking the current as deleted via
 *				 'deleteline'
 *	      EditBuff	    - by replacing it with the line below the
 *				 the current line via  'getline'
 *	      SegTable	    - by marking the current line in the segment
 *				 as being deleted  via 'deleteline'
 *			      by marking the segment containing the
 *				 line  as needing compacting via 'deleteline'
 *	      the screen    - via 'drawscr'
 */


void ctrl_y()
{
    register unsigned short i;

    if (EditBuffDirty) {			  /* this call will change */
	EditBuffDirty = 0;			  /* cursor line, so flush */
	flushline((TopRow + CurRow), EditBuff);   /* edit buffer if dirty  */
	}

    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;		       /* if there's marked text */
	CharsMarked = 0;		       /* around, clear it and	 */
	for ( i = 0; i < MAXLINES; i++ )       /* redraw the screen	 */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	}

/*  compact LineTable */
    if( !((TopRow + CurRow) == TotalLines)) {
	deleteline(TopRow + CurRow);
	for(i = (TopRow + CurRow); i < TotalLines; i++)
	   LineTable[i] = LineTable[i + 1];
	--TotalLines;
    }

/*  redraw the screen */
    drawscr(TopRow);
    getline( (TopRow + CurRow), &EditBuff[0] );
    line25();
}




/*** tab - moves cursor to next tab
 *
 *   tab insert spaces up to the next tab if it will
 *     not cause the line to grow greater than line size.
 *
 *   tab()
 *
 *   ENTRY  -  none
 *
 *   EXIT   -  none
 *
 *   EFFECTS  LinesMarked   - by clearing it if set
 *	      CharsMarked   - by clearing it if set
 *	      MarkedChar    - by clearing it if set
 *	      MarkedLine    - by clearing it if set
 *	      EditBuffDirty - by setting if the tab is inserted
 *	      EditBuff	    - by inserting the tab
 *	      CurCol	    - by incrementing it by
 */


void tab()
{
    unsigned short ensp = FALSE;   /* enough space to add the tab */
    unsigned short tabshift;	   /* number of spaces to the next tab */
    unsigned short i, j;	   /* indexes */


    if ( LinesMarked || CharsMarked ) {
	LinesMarked = 0;		       /* if there's marked text */
	CharsMarked = 0;		       /* around, clear it and	 */
	for ( i = 0; i < MAXLINES; i++ )       /* redraw the screen	 */
	    MarkedLine[i] = 0;
	for ( i = 0; i < LINESIZE; i++ )
	    MarkedChar[i] = 0;
	}

    tabshift = (TABSIZE - (CurCol % TABSIZE)); /* number of spaces to next tab */

/*  is there enough space to insert the tab */
    if ((CurCol + tabshift) < LINESIZE) {
      for (i = (LINESIZE - tabshift);
	   (i < LINESIZE) && (ensp = !isgraph(EditBuff[i])); i++);

      if (ensp) {
/*	  enough space was found for the tab - now insert it */
	  for (i = (LINESIZE - tabshift -1), j = (LINESIZE - 1);
		inline(i, CurCol);  i--, j--)  {
	      EditBuff[j] = EditBuff[i];
	      EditBuff[i] = ' ';
	  }
/*	  redraw the line with the tab */
	  drawline();
	  CurCol += tabshift;
	  VIOSETCURPOS(CurRow, CurCol, 0);
	  EditBuffDirty = TRUE;
      }
      else badkey();
    }
    else badkey();
}
