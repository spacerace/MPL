/*
 *  LocalUnlock
 *
 *  This program demonstrates the use of the LocalUnlock function.  It
 *  will allocate a block of memory using LocalAlloc, lock the
 *  memory with LocalLock, copy a string to it and display it with
 *  the MessageBox function, and then unlock the memory block with the
 *  local Unlock function.
 */

#include <windows.h>

int PASCAL WinMain ( hInstance , hPrevInstance, lpszCmdLine, cmdShow )

HANDLE	hInstance , hPrevInstance;
LPSTR	lpszCmdLine;
int	cmdShow;
  {
  HANDLE	hMemBlock;	  /*  The memory block	*/
  WORD		wFlags; 	  /*  Return Value from function  */
  char NEAR *	pBuffer;	  /*  Pointer to locked buffer	*/
  char *	strcpy();	  /*  Tell compiler that it returns
				   *  a pointer to a char
				   */

  hMemBlock = LocalAlloc( LMEM_ZEROINIT | LMEM_MOVEABLE ,
			 (WORD) 80 );

       /*  Allocate 80 bytes of moveable memory and initialize
	*  all of the bytes to 0
	*/

  pBuffer = LocalLock ( hMemBlock );
       /*  Lock it and return the pointer to the locked block  */

  if ( pBuffer != NULL )  /*  Make sure that it was locked
			   *  Returns NULL if it wasn't
			   */
    {
    strcpy ( pBuffer , "LocalLock was Successfull" );
       /*  Place the string into the block  */

    MessageBox( NULL , (LPSTR) pBuffer , (LPSTR) "OK" , MB_OK );
       /*  Display the message that all is OK  */

    LocalUnlock( hMemBlock );
       /*  Unlock Memory Block	*/
    }
  else
    MessageBox( NULL , (LPSTR)"LocalLock was not Successfull" ,
		(LPSTR)"ERROR!!!" , MB_OK );

  return 0;
  }
