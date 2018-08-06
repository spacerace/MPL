/*
 *   LocalCompact
 *
 *   This program demonstrates the use of the LocalCompact function. The
 *   LocalCompact function compacts the global heap. LocalCompact was
 *   called from WinMain in this sample application.
 */

#include "windows.h"

int sprintf();

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HANDLE	  hMemBlock1;	  /* Handle to memory block */
    HANDLE	  hMemBlock2;	  /* Handle to memory block */
    char szBuff[90];
    WORD ContigFreeBytes;
    char NEAR *   Rasta1, Rasta2;

/* LocalCompact to ask for 4096 continguous bytes */
    ContigFreeBytes = (WORD)LocalCompact((WORD)4096);
    sprintf(szBuff,"Number of contiguous free bytes before allocation = %u",
						      ContigFreeBytes);
    MessageBox(NULL,(LPSTR)szBuff,(LPSTR)" ",MB_OK);

/* Allocate 2 bytes from global heap */
    hMemBlock1 = LocalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DISCARDABLE,
		    (long)2);
/* if block allocated properly */
    if (hMemBlock1 != NULL)
	/* lock block into memory */
	Rasta1 = (char NEAR *)LocalLock(hMemBlock1);

/* ask for 4096 contiguous free bytes */
    ContigFreeBytes = (WORD)LocalCompact((WORD)4096);
    sprintf(szBuff,"Number of contiguous free bytes = %u",ContigFreeBytes);
    MessageBox(NULL,(LPSTR)szBuff,(LPSTR)"After allocating 2 bytes...",MB_OK);

/* Allocate 200 bytes from local heap */
    hMemBlock2 = LocalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE |GMEM_DISCARDABLE,
		    (long)200);
/* if memory allocated properly */
    if (hMemBlock2 != NULL)
	/* lock block into memory */
	Rasta2 = (char NEAR *)LocalLock(hMemBlock2);

/* ask for 4096 contiguous free bytes */
    ContigFreeBytes = (WORD)LocalCompact((WORD)4096);
    sprintf(szBuff,"Number of contiguous free bytes = %u",ContigFreeBytes);
    MessageBox(NULL,(LPSTR)szBuff,(LPSTR)"After allocating 200 bytes...",MB_OK);

    LocalUnlock(hMemBlock1);
    LocalUnlock(hMemBlock2);

    LocalFree(hMemBlock1);
    LocalFree(hMemBlock2);

/* ask for FFFF contiguous free bytes */
    ContigFreeBytes = (WORD)LocalCompact((WORD)4096);

    sprintf(szBuff,"Number of contiguous free bytes = %u",ContigFreeBytes);
    MessageBox(NULL,(LPSTR)szBuff,(LPSTR)"After unlocking 200 bytes...",MB_OK);

    return 0;
}
