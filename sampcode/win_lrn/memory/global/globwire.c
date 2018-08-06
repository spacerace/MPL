/*
 *   GlobalWire
 *   globwire.c
 *
 *   This program demonstrates the use of the GlobalWire function. The
 *   GlobalWire function locks a block into low memory. GlobalWire is
 *   called from WinMain in this sample application. GlobalWire is often
 *   used when a block is going to be locked in memory for a long time
 *   so there will be larger continuous free spaces than if GlobalLock was
 *   used. Before allocation  and after GlobalWire'ing, GlobalCompact is
 *   called to show the affect of locking the block in memory has on the
 *   largest continuous block
 *
 */

#include "windows.h"

int sprintf();

typedef struct {
		char smallstruct[20];
	       } SMALLSTRUCT;	      /* structure we're going to allocate */
						/*   space for	  */
typedef SMALLSTRUCT far *lpSMALLPtr;  /* pointer to SMALLSTRUCT structure */

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HANDLE	   hMemBlock;	     /* Handle to memory block		*/
    lpSMALLPtr	   ThisPtr;	     /* Pointer to myStruct		*/
    char	   szBuff[80];	     /* buffer for output		*/
    DWORD	   ContigFreeBytes;  /* return value from GlobalCompact */

/* Allocate space for SMALLSTRUCT in global heap */
    hMemBlock = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
		    (long)sizeof(SMALLSTRUCT));
/* if memory allocated properly */
    if (hMemBlock)
	/* GlobalWire the block into low memory */
	ThisPtr = (lpSMALLPtr)GlobalWire(hMemBlock);
	if (ThisPtr == NULL)
	    MessageBox(NULL,(LPSTR)"GlobalWire Failure",
			    (LPSTR)" ",MB_OK);
	else
	    MessageBox(NULL,(LPSTR)"GlobalWire worked properly",
			    (LPSTR)" ",MB_OK);

/* GlobalUnwire the block */
    GlobalUnWire(hMemBlock);

/* Free the block */
    GlobalFree(hMemBlock);

    return 0;
}
