/*
 *
 *  HideCaret
 *
 *  This program demonstrates the use of the HideCaret function. The
 *  HideCaret function removes the caret from the specified window.
 */

#include "windows.h"

/* Procedure called when the application is loaded for the first time */
BOOL WinInit (hInstance)
HANDLE hInstance;
  {
  PWNDCLASS   pClass;

  pClass = (PWNDCLASS)LocalAlloc (LPTR, sizeof (WNDCLASS));
  pClass->hCursor        = LoadCursor (NULL, IDC_ARROW);
  pClass->lpszMenuName   = (LPSTR)NULL;
  pClass->lpszClassName  = (LPSTR)"Window";
  pClass->hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
  pClass->hInstance      = hInstance;
  pClass->style          = CS_HREDRAW | CS_VREDRAW;
  pClass->lpfnWndProc    = DefWindowProc;

  if (!RegisterClass ( (LPWNDCLASS)pClass))
    return FALSE;

  LocalFree ( (HANDLE) pClass);
  return TRUE;        /* Initialization succeeded */
  }

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  HWND         hWnd;                /* Handle to the parent window    */

  WinInit (hInstance);

  hWnd = CreateWindow ( (LPSTR)"Window",
      (LPSTR)"Sample Window",
      WS_OVERLAPPEDWINDOW,
      20, 20, 400, 200,
      (HWND)NULL,        /* no parent */
  (HMENU)NULL,       /* use class menu */
  (HANDLE)hInstance, /* handle to window instance */
  (LPSTR)NULL        /* no params to pass on */
 );

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  CreateCaret (hWnd, NULL, 8, 12);
  SetCaretPos (50, 50);
  ShowCaret (hWnd);
  MessageBox (hWnd, (LPSTR)"Here is the caret",
      (LPSTR)" ", MB_OK);
  MessageBox (hWnd, (LPSTR)"The caret is going to be hid",
      (LPSTR)"WARNING", MB_OK);
/** hide the caret **/
  HideCaret (hWnd);
/** output message box so application does not end before user **/
/** can see that the caret is gone                             **/
  MessageBox (hWnd, (LPSTR)"It's gone!", (LPSTR)" ", MB_OK);

  return 0;
  }
