/*
 *  LocalLock
 *
 *  This program demonstrates the use of the function LocalLock.  It
 *  allocates a block of memory, using LocalAlloc, Locks it, copies a string
 *  into it and displays it in a message box.
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
       /*  Lock it and return the pointer  */

  if ( pBuffer != NULL )
    {
    strcpy ( pBuffer , "LocalLock was Successfull" );
    MessageBox( NULL , (LPSTR) pBuffer , (LPSTR) "OK" , MB_OK );
       /*  Display the message that all is OK  */
    }
  else
    MessageBox( NULL , (LPSTR)"LocalLock was not Successfull" ,
		(LPSTR)"OK" , MB_OK );

  return 0;
  }
