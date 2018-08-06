#include <ctype.h>
#include <dos.h>
#include "ssedefs.h"




/*** addline - allocates and puts lines into segments
 *
 *   addline adds a line to a segment and if nessessary
 *     allocates a new segment.
 *
 *   addline (linenum, linelength, line)
 *
 *   ENTRY   linenum	- the line number of 'line' in the file
 *	     linelength - the line length of 'line'
 *	     line	- the 'line' to place in the segment
 *
 *   EXIT    rc 	- return code for allocating a segment
 *
 *   addline places the current line the in the segment
 *     containing the previous line if possible to
 *     reduce scattering, otherwise it scans other
 *     segments for first fit and if this fail allocates
 *     a new segment.
 *
 *   WARNING: do not modify the value of 'i' it is the
 *	      result of the segment searches.
 *
 *   EFFECTS: LineTable - by adding new lines
 *	      SegTable	- by using free space for the line
 *			  and via 'allocseg'
 *	      TotalSegs - via 'allocseg'
 *	      memory	- by allocating segments via 'allocseg'
 */

short addline(linenum, linelength, line)
unsigned short linenum;
unsigned char  linelength;
unsigned char  line[];
{
    register unsigned short i, j;
    unsigned char  bytesneeded;
    unsigned short selector;
    short	   rc;	 /* return code of allocseg */

    bytesneeded = (HEADERSIZE + linelength);
    rc = 0;

/*  use segment of previous line to reduce scattering */
    if (linenum == 0)
	selector = FP_SEG(LineTable[linenum]);
    else
	selector = FP_SEG(LineTable[linenum -1]);

/*  find segment contianing line number -1 */
    for(i = 0; (selector != SegTable[i].segment) && (i < TotalSegs); i++);

/*  scan for first fit if not enough space in segment containing line -1 */
    if (SegTable[i].free < bytesneeded) {
	for(i = 0; (SegTable[i].free < bytesneeded) && (i < TotalSegs); i++);
/*	allocate a segment if not eough space is found */
	if (i >= TotalSegs)
	    rc = allocseg();
    }

    if (rc == 0) {	 /* no error allocating a segment */
/*	point line table at segment */
	FP_SEG(LineTable[linenum]) = SegTable[i].segment;
	FP_OFF(LineTable[linenum]) = SEGSIZE - SegTable[i].free;

/*	place line in segment */
	SegTable[i].free -= bytesneeded;
	LineTable[linenum]->linelength = linelength;
	for(j = 0; j < linelength; j++)
	LineTable[linenum]->firstchar[j] = line[j];
    }
    return(rc);
}




/*** deleteline - marks lines as deleted
 *
 *   deleteline marks a line as deteted in the segment
 *     it is contained in and marks the setment containing
 *     the line as needing compacting.
 *
 *   deleteline (linenum)
 *
 *   Entry	linenum - line number of line to mark as deleted
 *
 *   EFFECTS: SegTable - by marking the segment containing the line
 *			   as needing compacting.
 *			 by marking the line in the segment as deleted.
 */

void deleteline(linenum)
unsigned short linenum;
{
    register unsigned short i;
    unsigned short selector;

    LineTable[linenum]->deleted = TRUE;
    selector = FP_SEG(LineTable[linenum]);

    for(i = 0; (selector != SegTable[i].segment) && (i < TotalSegs); i++);

    SegTable[i].flags = TRUE;

}




/*** flushline - flushes editbuffer to segment
 *
 *   flushline rewrites the contents of a line to a segment
 *     by using deleteline and addline.
 *
 *   flushline	(linenum, line)
 *
 *   ENTRY	linenum - linenumber of line to rewrite.
 *		line	- line to be rewriten.
 *
 *   flushline checks if the current line is beyond the end of
 *     file, if so it adds the line to the file using addline,
 *     else it replaces it by deleting the the old copy of the
 *     line using deleteline and then adding the line back using
 *     addline. flushline will EXIT the program by calling error25
 *     if it is unable to add a line to the file or the number of
 *     lines in the file becomes greater than MAXLINES.
 *
 *   EFFECTS: TotalLines - by incrementing it
 *	      SegTable	 - by marking the segment containing the line
 *			     as needing compacting, via 'deleteline'
 *			   by marking the line in the segment as deleted.
 *			     via 'deleteline'
 *	      LineTable  - by adding new lines via 'addline'
 *	      SegTable	 - by using free space for the line via 'addline'
 *			   and via 'allocseg'
 *	      TotalSegs  - via 'allocseg'
 *	      memory	 - by allocating segments via 'allocseg'
 */

flushline(linenum, line)
unsigned short linenum;
unsigned char  line[];
{
    register unsigned char  i;

/*  delete the line if it's not beyond the end of the file */
    if ( !(linenum == TotalLines))
	deleteline(linenum);
    else
	++TotalLines;

    for (i = (LINESIZE -1); !isgraph(line[i]) && inline(i, 0); i--);

    if ( (addline(linenum, i +1, line) != 0) || !(TotalLines < MAXLINES) )
	error25(7);    /* call error message and quit */;
}




/*** getline - fills line from segment
 *
 *   getline copies a 'line' from a segment to the
 *     address given by line.
 *
 *   getline (linenum, line)
 *
 *   ENTRY   linenum  -  the line number of 'line' in the file
 *	     line     -  address of where to copy 'line'
 *
 *   getline fills the line out to LINESIZE with spaces and
 *     returns a line of spaces if the linenum is not a valid
 *     line number.
 *
 *   WARNING getline does not check if the address given by line is
 *	     valid.
 *
 *   EFFECTS memory - by copying the line to the address given by line.
 */

void getline(linenum, line)
unsigned short linenum;
unsigned char  *line;
{
    register unsigned short i;

    i = 0;

/*  copy 'line' to address given by line */
    if (linenum < TotalLines)  /* a valid line number */
	for ( ; i < (LineTable[linenum]->linelength); i++)
	    line[i] = LineTable[linenum]->firstchar[i];

/*  pad line with spaces out to LINESIZE */
    for ( ; i < LINESIZE; i++)
	line[i] = ' ';

}
