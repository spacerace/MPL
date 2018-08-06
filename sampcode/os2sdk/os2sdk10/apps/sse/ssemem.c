#include <doscalls.h>
#include <dos.h>
#include "ssedefs.h"




/*** allocseg - allocates segments.
 *
 *   allocseg uses DOSALLOCSEG to allocate segments from DOS.
 *
 *   This routine will return an error code if it is unable to
 *     allocate the segment.
 *
 *   EXIT   rc	- return code of DOSALLOCSEG
 *
 *
 *   EFFECTS : allocates memory from DOS
 *	       modifies SegTable (by adding a new segment)
 *	       modifies TotalSegs (by incrementing it by one)
 */

short allocseg()
{
  short  rc;	/* return code of DOS call */

  rc = DOSALLOCSEG(SEGSIZE,
		   (unsigned far *)&(SegTable[TotalSegs].segment),
		   NOTSHARED);

  if (rc == 0) {
      SegTable[TotalSegs].free = SEGSIZE;
      ++TotalSegs;
  }

  return(rc);
} /* end allocseg */




/*** freesegs - free segments
 *
 *   freesegs uses DOSFREESEG to free all the segments
 *     allocated by allocseg.
 *
 *   EFFECTS: frees all alocated memory (segments) to DOS
 *	      sets TotalSegs to zero
 *	      makes SegTable invalid
 */

void freesegs()
{
  register unsigned short i;

  for (i = 0; i < TotalSegs; i++) {
      DOSFREESEG(SegTable[i].segment);
  }
  TotalSegs = 0;
} /* end freesegs */
