/*
 *  LocalReAlloc
 *
 *  This program demonstrates the use of LocalReAlloc.	It will allocate
 *  a block of memory using LocalAlloc, copy a string to the block of
 *  memory, enlarge the block, and then display the string using the
 *  MessageBox function.
 */

#include <windows.h>

int PASCAL WinMain ( hInstance , hPrevInstance, lpszCmdLine, cmdShow )

HANDLE	hInstance , hPrevInstance;
LPSTR	lpszCmdLine;
int	cmdShow;
  {
  HANDLE	hMemBlock;	  /*  The memory block	*/
  HANDLE	hNewBlock;	  /*  A handle to the new memory block	*/
  WORD		wFlags; 	  /*  Return Value from function  */
  char NEAR *	pBuffer;	  /*  Pointer to locked buffer	*/
  char *	strcpy();	  /*  Tell compiler that it returns
				   *  a pointer to a char
				   */

  hMemBlock = LocalAlloc( LMEM_ZEROINIT | LMEM_MOVEABLE ,
			 (WORD) 70 );
       /*  Allocate 70 bytes of moveable memory and initialize
	*  all of the bytes to 0
	*/

  pBuffer = LocalLock ( hMemBlock );
  if ( pBuffer != NULL )
    {
    strcpy ( pBuffer , "Both LocalReAlloc and LocalLock were Successfull" );
    LocalUnlock( hMemBlock );
       /*  Place string into pBuffer and unlock the memory.
	*  When increasing the size of the block, we don't have to worry
	*  about the contents.	They will still be intact, but there will
	*  be more memory available
	*/
    }

  hNewBlock = LocalReAlloc( hMemBlock , 80 ,
			    LMEM_MOVEABLE | LMEM_ZEROINIT );
       /*  Resized the block to 80 bytes.  It makes the new block moveable
	*  and also initializes any new bytes to zero
	*/

  if ( hNewBlock != NULL )	    /*	Check to see if successful  */
    {
    pBuffer = LocalLock ( hNewBlock );
       /*  Lock it and return the pointer  */

    if ( pBuffer != NULL )
      MessageBox( NULL , (LPSTR) pBuffer , (LPSTR) "OK" , MB_OK );
	 /*  Display the message that all is OK  */
    else
      MessageBox( NULL ,
		 (LPSTR)"LocalLock was not Successfull, but LocalReAlloc was",
		 (LPSTR)"OK" , MB_OK );
	 /*  Tell user that things went half right  */
    }
  else
    MessageBox( NULL , (LPSTR)"LocalReAlloc was not Successful" ,
		(LPSTR) "OK" , MB_OK );
	/*  Tell user that it didn't work at all  */

  return 0;
  }
