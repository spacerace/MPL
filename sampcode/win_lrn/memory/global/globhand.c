/*
 *   GlobalHandle
 *   globlock.c
 *
 *   This program demonstrates the use of the GlobalHandle function. The
 *   GlobalHandle function returns the handle of the specified global
 *   memory block. GlobalHandle is called from WinMain in this sample
 *   application.
 *
 */

#include "windows.h"
#include "dos.h"		/* FP_SEG located in dos.h */

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
    HANDLE hMemBlock1;  /* Handle to memory block */
    DWORD hMemBlock2;   /* Handle to memory block returned from GlobalHandle */
    lpMyPtr  FirstPtr;  /* Pointer to MYSTRUCT structure */
    char szBuff[30];	/* buffer for message box	 */

/* allocate space in global heap for a MYSTRUCT structure */
    hMemBlock1 = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
		    (long)sizeof(MYSTRUCT));

/* if memory allocated properly */
    if (hMemBlock1 != NULL)
	{

	/* lock memory into current address */
	FirstPtr = (lpMyPtr)GlobalLock(hMemBlock1);

	/* if lock worked */
	if (FirstPtr != NULL)
	    {
	    /* get handle of segment pointed to by FirstPtr */
	    hMemBlock2 = GlobalHandle(FP_SEG(FirstPtr));

	    /* if GlobalHandle returned original handle */
	    if ((HANDLE)hMemBlock2 == hMemBlock1)
	       MessageBox(NULL,(LPSTR)"DID return the correct handle",
			       (LPSTR)"GlobalHandle...",MB_OK);
	    else
	       MessageBox(NULL,(LPSTR)"DID NOT return the correct handle",
			       (LPSTR)"GlobalHandle...",MB_OK);

	    GlobalUnlock(hMemBlock1);	 /* unlock memory */
	    GlobalFree(hMemBlock1);	 /* free memory   */
	    }
	else
	    MessageBox(NULL,(LPSTR)"The lock DID NOT work properly",
		       (LPSTR)" ",MB_OK);
	}
    return 0;
}
