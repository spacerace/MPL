/**************************************************************************
 *	LOCSHRNK	version 1.00
 *
 * This program tests the LocalShrink function, which shrinks the size of
 * the local heap and returns the current size. If the shrinksize is larger
 * than the current size, it will only return the current size.
 *
 * Using NULL for the first parameter will default to the current data
 * segment.
 *
 * compiled on IBM AT w640K EGA running WINDOWS 2.03
 **************************************************************************/

#include <windows.h>
#include <stdio.h>

#define	MADMAX	45000
#define	MAXHEAP	20480
#define	HEAPSIZE	2048

int PASCAL			WinMain(HANDLE,HANDLE,LPSTR,int);

char	szPurpose[80] = "This is a program to initialize the local heap";

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE   hInstance;
HANDLE   hPrevInstance;
LPSTR    lpszCmdLine;
int      cmdShow;
{
		HANDLE   hMemBlock;
		char	   MesBuf[250];
		DWORD	   dwMemSize;

   MessageBox(GetFocus(),(LPSTR)szPurpose,(LPSTR)"LOCINIT",MB_OK);

	/* Get size of local heap by giving meaningless shrink value */
   dwMemSize = LocalShrink(NULL, MADMAX);

   sprintf(MesBuf,"%s%lu","Size of local heap: ",dwMemSize);
   MessageBox(GetFocus(),(LPSTR)MesBuf,(LPSTR)"LocolShrink",MB_OK);

	/* Increase the size of the local heap */
   hMemBlock = LocalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,(WORD)MAXHEAP);

   /* Get new size */
   dwMemSize = LocalShrink(NULL, MADMAX);

   sprintf(MesBuf,"%s%lu","New size of local heap: ",dwMemSize);
   MessageBox(GetFocus(),(LPSTR)MesBuf,(LPSTR)"Shrink",MB_OK);

	LocalFree(hMemBlock);

   /* Shrink local heap down to original size */
   dwMemSize = LocalShrink(NULL, HEAPSIZE);

   sprintf(MesBuf,"Size of local heap after actually shrinking: %lu",
   	dwMemSize);
   MessageBox(GetFocus(),(LPSTR)MesBuf,(LPSTR)"Shrink",MB_OK);

   MessageBox(GetFocus(),(LPSTR)"Initialized and Accessed Local Heap Succesful",
   	(LPSTR)"LocalInit",MB_OK);
   
   return (0);
} /* WINMAIN */


