/*
 *  LocalFree
 *
 *  This program demonstrates the use of the function LocalFree.  It
 *  allocates a block of memory, using LocalAlloc, and then simply
 *  frees the block.
 */

#include <windows.h>

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)

HANDLE  hInstance, hPrevInstance;
LPSTR	lpszCmdLine;
int	cmdShow;
  {
  HANDLE	hMemBlock;	  /*  The memory block	*/
  WORD	Flags; 	     /*  Return Value from function  */

/*  Allocate 40 bytes of moveable, discardable memory and initialize
	*  all of the bytes to 0.
	*/
  hMemBlock = LocalAlloc (LMEM_ZEROINIT | LMEM_MOVEABLE | LMEM_DISCARDABLE,
      (WORD) 40);

  hMemBlock = LocalFree (hMemBlock);  /* Free the block of memory
					                         * and put the return code into
				                            * hMemBlock.
  					                         */

  if (hMemBlock == NULL)
    MessageBox (NULL, (LPSTR)"LocalFree function Successful",
        (LPSTR)"OK", MB_OK);
/*  Tells user if successful  */
  else
    MessageBox (NULL, (LPSTR)"LocalFree function Unsuccessful",
        (LPSTR)"OK", MB_OK);
/*  Tells user if not successful  */

  return 0;
  }
