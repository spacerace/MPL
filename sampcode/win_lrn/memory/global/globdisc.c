/*
 *   GlobalDiscard
 *   globdisc.c
 *
 *   This program demonstrates the use of the GlobalDiscard function.
 *   The GlobalDiscard function marks a block of memory for discarding.
 *   After GlobalDiscarding, to check if a block has really been discarded,
 *   try to GlobalLock. If the return value is NULL, then the block has
 *   been discarded.
 *
 */

#include "windows.h"

int sprintf();

typedef struct {		 /* structure we are going */
		char x[50];   /* to allocate and lock	*/
		int y;
	       } MYSTRUCT;

typedef MYSTRUCT far *lpMyPtr;	/* far pointer to MYSTRUCT type */

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HANDLE hMemBlock;	  /* Handle to memory block	     */
    lpMyPtr  ThisPtr;	  /* Pointer to myStruct	     */
    char szBuff[60];	  /* buffer for message box	     */
    HANDLE Discarded;	  /* return value from GlobalDiscard */

    (DWORD)GlobalCompact((DWORD)-1);

/* allocate space in global heap for a MYSTRUCT structure */
    hMemBlock = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DISCARDABLE,
		    (long)sizeof(MYSTRUCT));

/* if memory allocated properly */
    if (hMemBlock)
	{
	/* lock memory into current address */
	ThisPtr = (lpMyPtr)GlobalLock(hMemBlock);

	/* if lock worked */
	if (ThisPtr != NULL)
	    {

	      GlobalUnlock(hMemBlock);		/* unlock memory */

	 /* mark hMemBlock for discarding */
	      GlobalDiscard(hMemBlock);

	 /* if GlobalLock works (meaning hMemBlock not discarded) */
	    if ( (ThisPtr = (lpMyPtr)GlobalLock(hMemBlock)) != NULL)
		MessageBox(NULL,(LPSTR)"It has not been discarded",
				(LPSTR)" ",MB_OK);
	    else
		MessageBox(NULL,(LPSTR)"It HAS been discarded",
				(LPSTR)"Before compaction...",MB_OK);
	    GlobalCompact((DWORD)-1);

	 /* if GlobalLock works (meaning hMemBlock not discarded) */
	    if ((ThisPtr = (lpMyPtr)GlobalLock(hMemBlock)) != NULL)
		MessageBox(NULL,(LPSTR)"it has not been discarded",
				(LPSTR)"After compaction...",MB_OK);
	    else
		MessageBox(NULL,(LPSTR)"it HAS been discarded",
				(LPSTR)"After compaction...",MB_OK);
	    }
	}
    return 0;
}
