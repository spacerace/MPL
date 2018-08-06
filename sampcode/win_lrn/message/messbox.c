/*
 *  MessageBox
 *
 *  This program demonstrates the use of the MessageBox function.  It
 *  will display a simple message.
 */

#include <windows.h>

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MessageBox (GetFocus(),  /*  Because we have no parent window we feed it
                            *  the current focus.
                            */
  (LPSTR) "This is a Message Box",   /*  The message  */
  (LPSTR) "OK",                      /*  Message Header  */
  MB_OK);                            /* Type of buttons      */
  return 0;
  }
