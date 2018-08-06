/*
 *  LocalFlags
 *
 *  This program demonstrates the use of the LocalFlags function. The
 *  program will allocate some memory using LocalAlloc, and then tell
 *  the user if that memory block has been discarded or if it is marked
 *  as discarded.
 */

#include <windows.h>

/***************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE  hInstance, hPrevInstance;
LPSTR	lpszCmdLine;
int	cmdShow;
  {
  HANDLE	hMemBlock;	       /*  The memory block */
  WORD		wFlags; 	       /*  Return Value from function */
  char	szBuffer[80];	 /*  Temporary Buffer for a string */
  int   sprintf ();          /*  Tell the compiler what will
				                 *  be returned.
  				                 */

/*  Allocate 40 bytes of moveable, discardable memory and initialize
	*  all of the bytes to 0.
	*/
  hMemBlock = LocalAlloc (LMEM_ZEROINIT | LMEM_MOVEABLE | LMEM_DISCARDABLE,
      (WORD) 40);

  wFlags = LocalFlags (hMemBlock);   /*  Get the info from LocalFlags  */

  if (wFlags & LMEM_DISCARDED)
    MessageBox (NULL, (LPSTR)"The allocated memory has been discarded.",
        (LPSTR)"Ok", MB_OK);

  if (wFlags & LMEM_DISCARDABLE)
    MessageBox (NULL, (LPSTR)"The allocated memory is discardable.",
        (LPSTR)"Ok", MB_OK);

  sprintf (szBuffer, "The Reference Count is %d",
      wFlags & LMEM_LOCKCOUNT);
/*  Masking out the lock or reference count  */

  MessageBox (NULL, (LPSTR) szBuffer, (LPSTR) "Ok", MB_OK);
/*  Display the lock count via a message box  */

  return 0;
  }
