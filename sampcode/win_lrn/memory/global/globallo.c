/*
 *   GlobalAlloc
 *
 *   This program demonstrates the use of the GlobalAlloc function. The
 *   GlobalAlloc function allocates memory from the Global heap. GlobalAlloc
 *   was called from WinMain in this sample application.
 *
 */

#include <windows.h>

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
    HANDLE hMemBlock;	  /* Handle to memory block	     */
    lpMyPtr  ThisPtr;	  /* Pointer to MYSTRUCT structure   */
    char szBuff[30];	  /* buffer for message box	     */

/* allocate space in global heap for a MYSTRUCT structure */
    hMemBlock = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
		    (long)sizeof(MYSTRUCT));

/* if memory allocated properly */
    if (hMemBlock != NULL)
	{
	MessageBox(NULL,(LPSTR)"The memory was allocated properly",
			(LPSTR)" ",MB_OK);

	/* lock memory into current address */
	ThisPtr = (lpMyPtr)GlobalLock(hMemBlock);

	/* if lock worked */
	if (ThisPtr != NULL)
	    {
	    /* use memory from global heap and output results*/
	    ThisPtr->x = 4;
	    ThisPtr->y = 4;
	    ThisPtr->x = ThisPtr->x*ThisPtr->y;
	    sprintf(szBuff,"ThisPtr->x * ThisPtr->y = %d",ThisPtr->x);
	    MessageBox(NULL,(LPSTR)szBuff,
			    (LPSTR)"Info from GlobalAlloc'ed memory",
			    MB_OK);
	    GlobalUnlock(hMemBlock);	/* unlock block */
	    GlobalFree(hMemBlock);	/* free block	*/
	    }
	else ;
	}
    else
	MessageBox(NULL,(LPSTR)"The memory was not allocated",
			(LPSTR)" ",MB_OK);
    return 0;
}
