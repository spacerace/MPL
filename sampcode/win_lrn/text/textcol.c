/*
 *  SetTextColor
 *  TEXTCOL.C,
 *
 *  This program demonstrates the use of the function SetTextColor.
 *  SetTextColor changes the color of future text output.
 *  This program gets a DC and outputs black text on a white backround.
 *  Next the text backround is changed to black.  The text is changed to
 *  white using the function SetTextColor.  Finally, the white text is
 *  output to the screen.
 *
 */

#include <windows.h>
#include <string.h>

char	*szMessage = "This is the text color before invoking SetTextColor.";
char	*szWhite = "The text color is now white.";

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  HWND       hWnd;
  HDC        hDC;
  DWORD      rgbcolor;
  DWORD      dwblack = RGB (0x00, 0x00, 0x00);
  DWORD      dwwhite = RGB (0xff, 0xff, 0xff);
  WNDCLASS   Class;

  Class.hCursor        = LoadCursor (NULL, IDC_ARROW);
  Class.lpszMenuName   = (LPSTR)NULL;
  Class.lpszClassName  = (LPSTR)"Window";
  Class.hbrBackground  = (HBRUSH)GetStockObject (BLACK_BRUSH);
  Class.hInstance      = hInstance;
  Class.style          = CS_HREDRAW | CS_VREDRAW;
  Class.lpfnWndProc    = DefWindowProc;

  if (!RegisterClass ( (LPWNDCLASS)&Class))
    return FALSE;

  hWnd = CreateWindow ( (LPSTR)"Window",
                      (LPSTR)"SetTextColor",
                      WS_TILEDWINDOW,
                      20, 20, 400, 200,
                      (HWND)NULL,        /* no parent */
                      (HMENU)NULL,       /* use class menu */
                      (HANDLE)hInstance, /* handle to window instance */
                      (LPSTR)NULL);      /* no params to pass on */

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  hDC = GetDC (hWnd);

  TextOut (hDC, 0, 0, (LPSTR)szMessage, strlen (szMessage));

  SetBkColor (hDC, dwblack);

  rgbcolor = SetTextColor (hDC, dwwhite);

  TextOut (hDC, 0, 15, (LPSTR)szWhite, strlen (szWhite));
  ReleaseDC (hWnd, hDC);

  MessageBox (GetFocus (), (LPSTR)"Program Finished",
      (LPSTR)"SetTextColor", MB_OK);

  return 0;
  }
