/*
 *   GlobalReAlloc
 *   globreal.c
 *
 *   This program demonstrates the use of the GlobalReAlloc function. The
 *   GlobalRealloc changes the size of the specified allocated memory block
 *   and returns a handle to the different size memory block. GlobalSize
 *   was called from WinMain in this sample application.
 *
 */

#include <windows.h>
#include <stdio.h>

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
    HANDLE hInstance, hPrevInstance;
    LPSTR lpszCmdLine;
    int cmdShow;
    {
    HANDLE hMemBlock;	  /* Handle to memory block		*/
    char szBuff[80];
    DWORD SizeMem;	  /* holds sizes of memory blocks	*/

    hMemBlock = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, 200L);

    /* if memory allocated properly */
    if (hMemBlock)
	{
	MessageBox(NULL,"The memory was allocated properly",
		   "GlobalAlloc",MB_OK);

	/* get the size of hMemBlock */
	SizeMem = GlobalSize(hMemBlock);
	sprintf(szBuff,"Size allocated = %lu",SizeMem);
	MessageBox(NULL, szBuff, "Before GlobalReAlloc...", MB_OK);

	hMemBlock = GlobalReAlloc(hMemBlock,	  /* change the size of the */
				  (DWORD)30,	  /* allocated block to 30  */
				  GMEM_MOVEABLE); /*	   bytes	    */
	if (hMemBlock)
	    {
	    SizeMem = GlobalSize(hMemBlock);
	    sprintf(szBuff,"Size allocated = %lu",SizeMem);
	    MessageBox(NULL, szBuff, "After GlobalReAlloc...",MB_OK);
	    }
	else
	    MessageBox(NULL, "GlobalReAlloc failed", "GlobaReAlloc",MB_OK);

	GlobalFree(hMemBlock);	    /* free hMemblock */
	}
    return 0;
    }
