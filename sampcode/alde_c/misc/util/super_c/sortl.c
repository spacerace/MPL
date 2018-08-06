/*                  Sort Lines

        This file contains the function that sorts the lines that
        rsort.c read in.
*/

#include <stdio.h>
#include <string.h>
#include "rsort.h"

/*      sortLines()

        Function: Sort the lPtr array according to the lines pointed
        to by lPtr[].start; but sort from the column pointed to by
        lPtr[].sortAt.

        Algorithm: A selection sort. The smallest item is found and
        swaped with the first entry, then the smallest of the remaining
        items is found, then... Until the entire array is sorted.

        Comments: strcmp() is used to compare lines, which defines
        what we mean by "sorted." There are other definitions that
        could make sense here, from fairly simple ones like descending
        order to much more exotic ones.
*/

sortLines()

{
        struct linePtr *sortLPtr;       /* Start of unsorted lines. */
        struct linePtr *srchLPtr;       /* Line being checked now. */
        struct linePtr *lowLPtr;        /* Lowest line found so far. */
        struct linePtr newLPtr;         /* Temp for swapping sort/low. */

        /* For each line in the array... */
        for (sortLPtr = lPtr; sortLPtr < nextLPtr; sortLPtr++) {
                /* Find the lowest remaining line. */
                for (lowLPtr = srchLPtr = sortLPtr;
                     srchLPtr < nextLPtr; srchLPtr++)
                        if (strcmp(lowLPtr->sortAt,srchLPtr->sortAt) > 0)
                                lowLPtr = srchLPtr;
                /* Swap it with the first unsorted line. */
                newLPtr = *sortLPtr;
                *sortLPtr = *lowLPtr;
                *lowLPtr = newLPtr;
        };
}

