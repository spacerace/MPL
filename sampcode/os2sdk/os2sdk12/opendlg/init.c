/***************************************************************************\
* INIT.C -- Library initialization funcitons
* Created by Microsoft Corporation, 1989
\***************************************************************************/

#include "tool.h"
/****************************************************************************\
* This function initializes the file dialog library (by loading strings).
*
* Note: Initialization will fail if CCHSTRINGSMAX is smaller than the
*       space taken up by all strings in the .rc file.  Fix by increasing
*       CCHSTRINGSMAX in wintool.h and maybe also the initial heap size
*       in wintool.def.
*
* Returns:
*   TRUE if initialization successful
*   FALSE otherwise
\***************************************************************************/

BOOL PASCAL InitLibrary()
{
    int i;
    int cch;
    PSTR pch;
    PSTR pmem;
    int cchRemaining;

    /* allocate memory for strings */
    if (!(pch = (pmem = WinAllocMem(vhheap, cchRemaining = CCHSTRINGSMAX))))
        return FALSE;

    /* load strings from resource file */
    for (i = 0; i < CSTRINGS; i++) {
        cch = 1 + WinLoadString(HABX, vhModule, i, cchRemaining, (PSZ)pch);
        if (cch < 2)
            /* loadstring failed */
            return FALSE;
        vrgsz[i] = pch;
        pch += cch;

        if ((cchRemaining -= cch) <= 0)
            /* ran out of space */
            return FALSE;
    }

    /* reallocate string space to size actually needed */
    WinReallocMem(vhheap, pmem, CCHSTRINGSMAX, CCHSTRINGSMAX - cchRemaining);

    return TRUE;
}
