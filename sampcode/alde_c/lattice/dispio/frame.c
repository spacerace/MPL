/*                             *** frame.c ***                       */
/*                                                                   */
/* IBM-PC microsoft "C" under PC-DOS                                 */
/*                                                                   */
/* Function to draw a box given the upper left corner and the lower  */
/* right corner.  Uses the extended character set - graphics board   */
/* not needed.  Uses direct BIOS calls.  Returns a 0 if successful   */
/* or a -1 if invalid parameters.                                    */
/*                                                                   */
/* *** NOTE ***                                                      */
/* The upper left corner of the screen is 0,0 and the bottom right   */
/* corner is 24,79.                                                  */
/*                                                                   */
/* Written by L. Cuthbertson, May 1984.                              */
/*                                                                   */
/*********************************************************************/
/*                                                                   */
/* Added the nlines argument.  If = 1, single line frame.            */
/* If = 2, double line frame.                                        */
/* Lew Paper, 7/4/85                                                 */

#include "dispio.h"

#define biosset CURPOS
#define bioswc(char, n) write_ch(char, PAGE, n)

int dispio(), write_ch();

int frame(ulrow,ulcol,lrrow,lrcol, nlines)
int ulrow,ulcol,lrrow,lrcol, nlines;
{
        static int horbar[2] = {0xC4, 0xCD};
        static int verbar[2] = {0xB3, 0xBA};
        static int ulcorn[2] = {0xDA, 0xC9};
        static int urcorn[2] = {0xBF, 0xBB};
        static int llcorn[2] = {0xC0, 0xC8};
        static int lrcorn[2] = {0xD9, 0xBC};
        int count,irow;
        int ndx, verchar;

        /* error checking */
        if (ulrow < 0 || ulrow > 24) return(-1);
        if (ulcol < 0 || ulcol > 79) return(-1);
        if (lrrow < 0 || lrrow > 24) return(-1);
        if (lrcol < 0 || lrcol > 79) return(-1);
        if (lrrow < ulrow) return(-1);
        if (lrcol < ulcol) return(-1);
        if (nlines < 1 || nlines > 2) return(-1);
        
        ndx = nlines - 1;
        verchar = verbar[ndx];

        /* do top line first */
        biosset(ulrow,ulcol);
        bioswc(ulcorn[ndx],1);
        count = lrcol-ulcol-1;
        if (count > 0) {
                biosset(ulrow,ulcol+1);
                bioswc(horbar[ndx],count);
        }
        biosset(ulrow,lrcol);
        bioswc(urcorn[ndx],1);

        /* do both sides at once */
        irow = ulrow + 1;
        while (irow < lrrow) {
                biosset(irow,ulcol);
                bioswc(verchar,1);
                biosset(irow,lrcol);
                bioswc(verchar,1);
                irow++;
        }

        /* do bottom line */
        biosset(lrrow,ulcol);
        bioswc(llcorn[ndx],1);
        count = lrcol-ulcol-1;
        if (count > 0) {
                biosset(lrrow,ulcol+1);
                bioswc(horbar[ndx],count);
        }
        biosset(lrrow,lrcol);
        bioswc(lrcorn[ndx],1);

        /* done */
        return(0);
}
