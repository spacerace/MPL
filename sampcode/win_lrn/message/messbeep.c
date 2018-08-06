/*
 *  MessageBeep
 *
 *  This program demonstrates the use of the MessageBeep function.
 *
 */

#include <windows.h>

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  if (MessageBeep (MB_OK) == FALSE)
    MessageBox (NULL, (LPSTR)"MessageBeep Failed", (LPSTR)"ERROR!!!", MB_OK);
  return 0;
  }
