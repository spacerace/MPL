/*
 *   GlobalCompact
 *   globcmpc.c, jeffst, v1.00, 22-Sept-1987
 *
 *   This program demonstrates the use of the GlobalCompact function. The
 *   GlobalCompact function compacts the global heap. GlobalCompact was
 *   called from WinMain in this sample application.
 *
 *   Microsoft Product Support Services
 *   Windows Version 2.0 function demonstration application
 *   Copyright (c) Microsoft 1987
 *
 */

#include "windows.h"

int sprintf();

typedef struct {
		int x;
		int y;
	       } MYSTRUCT;
typedef MYSTRUCT far *lpMyPtr;

typedef struct {
		char smallstruct[2];
	       } SMALLSTRUCT;
typedef struct {
		char bigstruct[20000];
	       } BIGSTRUCT;

typedef BIGSTRUCT far *lpBIGPtr;
typedef SMALLSTRUCT far *lpSMALLPtr;


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HANDLE	  hMemBlock1;	  /* Handle to memory block */
    HANDLE	  hMemBlock2;	  /* Handle to memory block */
    lpMyPtr	  ThisPtr;	  /* Pointer to myStruct    */
    lpBIGPtr	  HugePtr;
    lpSMALLPtr	  MiniPtr;
    char szBuff[50];
    DWORD ContigFreeBytes;

/* GlobalCompact to ask for FFFF continguous bytes */
    ContigFreeBytes = (DWORD)GlobalCompact((DWORD)-1);
    sprintf(szBuff,"Number of contiguous free bytes before allocation = %lu",
						      ContigFreeBytes);
    MessageBox(NULL,(LPSTR)szBuff,(LPSTR)" ",MB_OK);

/* Allocate 2 bytes from global heap */
    hMemBlock1 = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DISCARDABLE,
		    (long)sizeof(SMALLSTRUCT));
/* if block allocated properly */
    if (hMemBlock1 != NULL)
	/* lock block into memory */
	MiniPtr = (lpSMALLPtr)GlobalLock(hMemBlock1);

/* ask for FFFF contiguous free bytes */
    ContigFreeBytes = (DWORD)GlobalCompact((DWORD)-1);
    sprintf(szBuff,"Number of contiguous free bytes = %lu",ContigFreeBytes);
    MessageBox(NULL,(LPSTR)szBuff,(LPSTR)"After allocating 2 bytes...",MB_OK);

/* Allocate 50000 bytes from global heap */
    hMemBlock2 = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE |GMEM_DISCARDABLE,
		    (long)sizeof(BIGSTRUCT));
/* if memory allocated properly */
    if (hMemBlock2 != NULL)
	/* lock block into memory */
	HugePtr = (lpBIGPtr)GlobalLock(hMemBlock2);

/* ask for FFFF contiguous free bytes */
    ContigFreeBytes = (DWORD)GlobalCompact((DWORD)-1);
    sprintf(szBuff,"Number of contiguous free bytes = %lu",ContigFreeBytes);
    MessageBox(NULL,(LPSTR)szBuff,(LPSTR)"After allocating 50000 bytes...",MB_OK);

    GlobalUnlock(hMemBlock2);	  /* unlock the blocks */
    GlobalUnlock(hMemBlock1);	  /*		       */


/* ask for FFFF contiguous free bytes */
    ContigFreeBytes = (DWORD)GlobalCompact((DWORD)-1);
    sprintf(szBuff,"Number of contiguous free bytes = %lu",ContigFreeBytes);
    MessageBox(NULL,(LPSTR)szBuff,(LPSTR)"After unlocking 50000 bytes...",MB_OK);


    return 0;
}
