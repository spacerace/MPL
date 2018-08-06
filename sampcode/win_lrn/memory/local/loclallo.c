/*
 *   LocalAlloc
 *
 *   This program demonstrates the use of the LocalAlloc function.
 *   The LocalAlloc function allocates space for use in the local heap.
 */

#include <windows.h>

int sprintf();

typedef struct {		/* structure we are going */
		int x;		/* to allocate and lock   */
		int y;
	       } MYSTRUCT;

typedef MYSTRUCT *lpMyPtr;  /* far pointer to MYSTRUCT type */

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HANDLE hMemBlock;	  /* Handle to memory block	     */
    lpMyPtr  ThisPtr;	  /* Pointer to MYSTRUCT structure   */
    char szBuff[80];	  /* buffer for message box	     */

/* allocate space in global heap for a MYSTRUCT structure */
    hMemBlock = LocalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
		    (long)sizeof(MYSTRUCT));

/* if memory allocated properly */
    if (hMemBlock != NULL)
	{
	MessageBox(NULL,(LPSTR)"The memory was allocated properly",
			(LPSTR)" ",MB_OK);

	/* lock memory into current address */
	ThisPtr = (lpMyPtr)LocalLock(hMemBlock);

	/* if lock worked */
	if (ThisPtr != NULL)
	    {
	    /* use memory from global heap and output results*/
	    ThisPtr->x = 4;
	    ThisPtr->y = 4;
	    ThisPtr->x = ThisPtr->x*ThisPtr->y;
	    sprintf(szBuff,"ThisPtr->x * ThisPtr->y = %d",ThisPtr->x);
	    MessageBox(NULL,(LPSTR)szBuff,
			    (LPSTR)"Info from LocalAlloc'ed memory",
			    MB_OK);
	    LocalUnlock(hMemBlock);    /* unlock block */
	    LocalFree(hMemBlock);      /* free block   */
	    }
	else ;
	}
    else
	MessageBox(NULL,(LPSTR)"The memory was not allocated",
			(LPSTR)" ",MB_OK);
    return 0;
}
