/*
 *   GlobalUnlock
 *
 *   This program demonstrates the use of the GlobalUnlock function. The
 *   GlobalUnlock function decrements the reference count of a block of
 *   memory allocated in the Global heap. GlobalUnlock returns zero if the
 *   reference count of the block is zero. GlobalUnlock was called from
 *   WinMain in this sample application.
 *
 */

#include "windows.h"

int sprintf();

typedef struct {		/* structure we are going */
		int x;		/* to allocate and lock   */
		int y;
	       } MYSTRUCT;

typedef MYSTRUCT far *lpMyPtr;	/* far pointer to MYSTRUCT type */

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HANDLE hMemBlock;	  /* Handle to memory block */
    lpMyPtr  ThisPtr;	  /* Pointer to myStruct    */
    char szBuff[30];	  /* buffer for message box */
    BOOL unlocked;	  /* return value from GlobalUnlock */

/* allocate space in global heap for a MYSTRUCT structure */
    hMemBlock = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
		    (long)sizeof(MYSTRUCT));

/* if memory allocated properly */
    if (hMemBlock != NULL)
	{

	/* lock memory into current address */
	ThisPtr = (lpMyPtr)GlobalLock(hMemBlock);

	/* if lock worked */
	if (ThisPtr != NULL)
	    {
	    MessageBox(NULL,(LPSTR)"The lock worked properly",
		       (LPSTR)" ",MB_OK);
	    /* use memory from global heap and output results*/
	    ThisPtr->x = 4;
	    ThisPtr->y = 4;
	    ThisPtr->x = ThisPtr->x*ThisPtr->y;
	    sprintf(szBuff,"ThisPtr->x * ThisPtr->y = %d",ThisPtr->x);
	    MessageBox(NULL,(LPSTR)szBuff,
			    (LPSTR)"Info from GlobalLock'ed memory",
			    MB_OK);

	    /* unlock the block (decrement reference count) */
	    unlocked = GlobalUnlock(hMemBlock);
	    if (unlocked == 0)	  /* if reference count == 0 */
		MessageBox (NULL,(LPSTR)"The block was properly unlocked",
				 (LPSTR)" ",MB_OK);
	    else    /* if (reference count != 0) */
		MessageBox (NULL,(LPSTR)"The block was NOT unlocked",
				 (LPSTR)" ",MB_OK);

	    GlobalFree(hMemBlock);	/* free memory	 */
	    }
	else
	    MessageBox(NULL,(LPSTR)"The lock DID NOT work properly",
		       (LPSTR)" ",MB_OK);
	}
    return 0;
}
