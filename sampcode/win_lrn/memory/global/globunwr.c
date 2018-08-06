/*
 *   GlobalUnWire
 *   globwire.c
 *
 *   This program demonstrates the use of the GlobalUnWire function. The
 *   GlobalUnWire function unlocks a block that has been locked into low
 *   memory using the GlobalWire function. GlobalUnWire is called from
 *   WinMain in this sample application.
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
    HANDLE	  hMemBlock;	 /* Handle to memory block	     */
    lpSMALLPtr	     ThisPtr;	 /* Pointer to SMALLSTRUCT structure */
    char szBuff[50];		 /* buffer for output		     */

/* Allocate space for SMALLSTRUCT in global heap */
    hMemBlock = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
		    (long)sizeof(SMALLSTRUCT));
/* if memory allocated properly */
    if (hMemBlock)
	/* GlobalWire the block into low memory */
	ThisPtr = (lpSMALLPtr)GlobalWire(hMemBlock);

/******************** GlobalUnwire the block *******************************/
    GlobalUnWire(hMemBlock);

    MessageBox(NULL,(LPSTR)"The block has been GlobalUnwire'ed",
		    (LPSTR)" ",MB_OK);

/* Free the block */
    GlobalFree(hMemBlock);  /* free the block */

    return 0;
}
