/*
 *   This program demonstrates the use of the GlobalFlags function. The
 *   GlobalFlags function returns information about the specified block
 *   of memory allocated in the Global heap. GlobalFlags was called from
 *   WinMain in this sample application.
 */

#include "windows.h"
#include <stdio.h>

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    HANDLE hMemBlock;    /* Handle to memory block */
    char far *lpszMem;   /* Pointer to Double Word in Global Memory. */
    char szBuff[30];     /* buffer for message box */
    BOOL unlocked;       /* return value from GlobalUnlock */
    WORD FlagCall;       /* return value from GlobalFlags */

/* allocate space in global heap for MOVEABLE and DISDCARDABLE memory */
    MessageBox(GetFocus(), (LPSTR)"Obtaining Global Memory.",
               (LPSTR)"GlobalAlloc()", MB_OK);
    hMemBlock = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_DISCARDABLE,
                            (long)sizeof(DWORD));

/* if memory allocated properly */
    if (hMemBlock != NULL)
	{
	/* lock memory into current address */
	lpszMem = GlobalLock(hMemBlock);
	/* if lock worked */
	if (lpszMem != NULL)
	    {
       MessageBox(GetFocus(), (LPSTR)"Locking Global Memory.",
                  (LPSTR)"GlobalLock()", MB_OK);
       FlagCall = GlobalFlags(hMemBlock);	/* get global flags */
       /* if discardable */
       if ((FlagCall & GMEM_DISCARDABLE))
          MessageBox(GetFocus(), (LPSTR)"This block is discardable.",
                     (LPSTR)"According to GlobalFlags()...", MB_OK);
       else
       MessageBox (GetFocus(), (LPSTR)"This block is NOT discardable.",
                   (LPSTR)"According to GlobalFlags()...", MB_OK);

       /* if lockcount 0 */
       if ((FlagCall & GMEM_LOCKCOUNT) == 0)
          MessageBox(GetFocus(),(LPSTR)"The lock count is 0.",
                     (LPSTR)"Before unlocking, according to GlobalFlags()...",
                     MB_OK);
       else
          MessageBox(GetFocus(),(LPSTR)"The lock count is NOT 0.",
                     (LPSTR)"Before unlocking, according to GlobalFlags()...",
                     MB_OK);

       MessageBox(GetFocus(), (LPSTR)"Unlocking Global Memory.",
                  (LPSTR)"GlobalUnlock()", MB_OK);
       unlocked = GlobalUnlock(hMemBlock);    /* unlock memory */
       FlagCall = GlobalFlags(hMemBlock);
       /* if lockcount 0 */
       if ((FlagCall & GMEM_LOCKCOUNT) == 0)
          {
          MessageBox(GetFocus(),(LPSTR)"The lock count is 0.",
                     (LPSTR)"After unlocking, According to GlobalFlags()...",
                     MB_OK);
          GlobalFree(hMemBlock);              /* free memory   */
          }
       else
          MessageBox(GetFocus(),(LPSTR)"The lock count is NOT 0.",
                     (LPSTR)"Before unlocking, according to GlobalFlags()...",
                     MB_OK);
       }
    else
       MessageBox(GetFocus(),(LPSTR)"The lock DID NOT work properly.",
                  (LPSTR)" ", MB_OK);
    }
    return 0;
}
