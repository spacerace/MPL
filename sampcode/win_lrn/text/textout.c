/*
 *  TextOut
 *  textout.c
 *
 *  This Program outputs text to the upper left hand corner of the client
 *  area using the TextOut function. The TextOut function outputs a character
 *  string on the specified display, using the currently selected font.
 *  The starting position of the string is given by the X and Y parameters.
 *
 */

#include <windows.h>
#include <string.h>

char	*szTextOutMsg = "The TextOut function wrote this text.";


int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  HWND  hWnd;         /* Handle to the parent window */
  HDC   hDC;          /* Display context of client area */
  BOOL  flag;         /* Return value for TextOut function*/
  WNDCLASS   Class;

  Class.hCursor        = LoadCursor (NULL, IDC_ARROW);
  Class.lpszMenuName   = (LPSTR)NULL;
  Class.lpszClassName  = (LPSTR)"Window";
  Class.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
  Class.hInstance      = hInstance;
  Class.style          = CS_HREDRAW | CS_VREDRAW;
  Class.lpfnWndProc    = DefWindowProc;

  if (!RegisterClass ( (LPWNDCLASS)&Class))
    return FALSE;

  hWnd = CreateWindow ( (LPSTR)"Window",
                      (LPSTR)"TextOut",
                      WS_TILEDWINDOW,
                      20, 20, 400, 200,
                      (HWND)NULL,
                      (HMENU)NULL,
                      (HANDLE)hInstance,
                      (LPSTR)NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  hDC = GetDC (hWnd);

  flag = TextOut (hDC, 0, 0, (LPSTR)szTextOutMsg, strlen (szTextOutMsg));

  ReleaseDC (hWnd, hDC);

  if (flag == TRUE)
    MessageBox (hWnd, (LPSTR)"The TextOut fuction returns TRUE",
        (LPSTR)"Done", MB_OK);
  return 0;
  }
