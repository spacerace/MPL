/**************************************************************************
 * LNOTIFY.C - Demo Program for LocalNotify.
 *
 *
 * Compiled with MS C 5.1 & Windows SDK 2.03
 * Compiled on IBM-AT with 640K RAM.
 * 
 **************************************************************************/

#include <windows.h>

/* PROTOTYPES */
int	PASCAL WinMain (HANDLE, HANDLE, LPSTR, int);
BOOL FAR PASCAL NotifyProc(WORD, HANDLE, WORD);

FARPROC  lpNotifyProc;
FARPROC  oldNotifyProc;

/***************************************************************************/

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE	hInstance;
HANDLE	hPrevInstance;
LPSTR  	lpszCmdLine;
int	cmdShow;
{
  HANDLE   hMem;

  MessageBox(GetFocus(),
      (LPSTR)"This is a program to demonstrate LocalNotify. About to swap Notify Procedures.",
      (LPSTR)"LocalNotify",
      MB_OK);

/* Get a relocatable procedure address. */
  lpNotifyProc = MakeProcInstance((FARPROC)NotifyProc, hInstance);

/* Replace the local-heap notification handler */
  oldNotifyProc = LocalNotify(lpNotifyProc);

  MessageBox(GetFocus(),
      (LPSTR)"About to send a message to the new Notify Procedure.",
      (LPSTR)"LocalNotify",
      MB_OK);

  hMem = LocalAlloc(LMEM_DISCARDABLE | LHND, 1024);  /* Request a 1K buffer. */

  FreeProcInstance(LocalNotify(oldNotifyProc));                 /* Clean up. */

  MessageBox(GetFocus(), (LPSTR)"Finished", (LPSTR)"LocalNotify", MB_OK);

  return (0);
} /* WINMAIN */


/***************************************************************************/
/*	NOTIFYPROC:  Local memory discarding notification procedure. */

BOOL FAR PASCAL NotifyProc(function, arg1, arg2)
WORD   function;
HANDLE arg1;
WORD   arg2;
{
  switch (function) 
  {

  case LNOTIFY_OUTOFMEM:

    MessageBox(GetFocus(),
        (LPSTR)"Out of memory message received by new Notify Procedure.",
        (LPSTR)"LocalNotify",
        MB_OK);
    return(0);
    break;

  default:
    break;
  }
  return(arg2);
}



