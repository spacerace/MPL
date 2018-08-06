/*
 *   GlobalSize
 *
 *   This program demonstrates the use of the GlobalSize function. The
 *   GlobalSize function returns the size in bytes of the specified
 *   global memory block. GlobalSize was called from WinMain in this
 *   sample application.
 *
 */

#include <windows.h>
#include <stdio.h>

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HANDLE hMemBlock;	  /* Handle to memory block	    */
    char szBuff[ 40 ];    /* Local buffer for sprintf     */
    DWORD dwSizeMem;	  /* holds sizes of memory blocks */

    hMemBlock = GlobalAlloc(GMEM_ZEROINIT | GMEM_NODISCARD, 400L );
         /* allocate space in global heap for a buffer */

    if ( hMemBlock != NULL )   /* if memory allocated properly */
	  {
      dwSizeMem = GlobalSize( hMemBlock );
       /* Get the size of the block of memory  */
	  sprintf( szBuff, "Size allocated = %lu", dwSizeMem);
	  MessageBox( NULL, (LPSTR)szBuff, (LPSTR)"", MB_OK);
       /* Put size into string and output the string  */
      GlobalFree( hMemBlock );         /* free hMemblock */
	  }

 return FALSE;
}
