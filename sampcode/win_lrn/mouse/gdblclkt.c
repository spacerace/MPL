/*
 * GetDoubleClickTime
 * This function returns the Double Click Time of the mouse.
 *
 */

#include "windows.h"
#include <stdio.h>

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    nCmdShow;
  {
  WORD wTime;        /* The current double click time for the mouse. */
  char szOutBuf[80]; /* Output buffer for message box.               */

      /* Get and display the current double click time for the mouse. */
  wTime = GetDoubleClickTime ();
  sprintf (szOutBuf, "%s%hu", "Double click time in milliseconds is: ", wTime);
  MessageBox (GetFocus (), (LPSTR)szOutBuf, "GetDoubleClickTime ()", MB_OK);

  return 0;
  }
