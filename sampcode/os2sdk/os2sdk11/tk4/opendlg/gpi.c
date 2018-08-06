/***************************************************************************\
* GPI.C -- GPI Helper routines
* Created by Microsoft Corporation, 1989
\***************************************************************************/


#define INCL_GPI
#include "tool.h"

/***************************************************************************\
* GetTextExtent helper function
\***************************************************************************/

ULONG CALLBACK GetTextExtent(HPS hps, PCH lpstr, int cch) {
    POINTL rgptl[TXTBOX_COUNT];

    if (cch) {
        GpiQueryTextBox(hps, (LONG)cch, lpstr, 5L, rgptl);
        return(MAKEULONG((SHORT)(rgptl[TXTBOX_CONCAT].x - rgptl[TXTBOX_BOTTOMLEFT].x),
                     (SHORT)(rgptl[TXTBOX_TOPLEFT].y - rgptl[TXTBOX_BOTTOMLEFT].y)));
    } else
        return(0L);
    }
