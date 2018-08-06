/*
 *  LocalSize
 *
 *  This program demonstrates the use of the function LocalSize.  It
 *  allocates a block of memory, using LocalAlloc, Locks it, copies a string
 *  into it, displays it in a message box along with the size of the
 *  memory block.  (Which is returned by LocalSize)
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
  int           bytes;            /*  Number of bytes allocated  */

  hMemBlock = LocalAlloc( LMEM_ZEROINIT | LMEM_MOVEABLE ,
			 (WORD) 80 );

       /*  Allocate 80 bytes of moveable memory and initialize
	*  all of the bytes to 0
	*/

  bytes = LocalSize( hMemBlock );
                  /*  Find out the size of hMemBlock  */

  pBuffer = LocalLock ( hMemBlock );
       /*  Lock it and return the pointer  */

  if ( pBuffer != NULL )
    {
    sprintf( (char *) pBuffer , "LocalAlloc allocated %d bytes" , bytes );
    MessageBox( NULL , (LPSTR) pBuffer , (LPSTR) "OK" , MB_OK );
       /*  Display the message that all is OK  */
    }
  else
    MessageBox( NULL , (LPSTR)"LocalLock was not Successfull" ,
		(LPSTR)"OK" , MB_OK );

  return 0;
  }
