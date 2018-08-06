/*
 *  Function (s) demonstrated in this program: LocalHandle
 *  Description:
 *     This program will demonstrate the use of the LocalHandle function.
 *  It will allocate memory using LocalAlloc, get a pointer to that
 *  memory, and then check to make sure that LocalHandle returns the
 *  same handle that LocalAlloc did.
 *
 */

#include <windows.h>

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

/***************************************************************************/
int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance;
LPSTR       lpszCmdLine;
int         nCmdShow;
  {
  HANDLE      hLocalMem;          /*  Handle to Local Memory  */
  HANDLE      hLocalHandleMem;    /*  Will hold returned handle  */
  PSTR        pszLocalMem;       /*  Near Pointer to memory  */

  hLocalMem = LocalAlloc (LMEM_MOVEABLE, 40);         /* Allocate memory */
  pszLocalMem = (PSTR) LocalLock (hLocalMem);         /* Get a pointer   */
  hLocalHandleMem = LocalHandle ( (WORD) pszLocalMem);

/*  Get the handle to the memory  */
  if (hLocalHandleMem == hLocalMem)
    MessageBox (GetFocus (), (LPSTR)"The two handles match", (LPSTR)"", MB_OK);
  else
    MessageBox (GetFocus (), (LPSTR)"The two handles don't match",
        (LPSTR)"", MB_OK);

  LocalUnlock (hLocalMem);                          /* Unlock the memory */
  LocalFree (hLocalMem);                            /* Free the memory   */

  return (0L);
  }
