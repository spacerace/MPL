/*
 *   LoadString
 *
 *   This program demonstrates the use of the LoadString function. The
 *   LoadString Function loads string resources identified by the second
 *   parameter. In this sample application, the string resource identifiers
 *   are STRING1 and STRING2.
 *
 */

#include <windows.h>
#include "loadstr.h"

char	szBuffer[26];   /* buffer to hold strings loaded via LoadString	*/

long    FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);

/***************************************************************************/
/* Procedure called when the application is loaded for the first time */

BOOL HelloInit (hInstance)
HANDLE hInstance;
  {
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc (LMEM_FIXED, sizeof (WNDCLASS));

  pHelloClass->hCursor        = LoadCursor (NULL, IDC_ARROW);
  pHelloClass->hIcon                 = LoadIcon (hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)NULL;
  pHelloClass->lpszClassName  = (LPSTR)"Sample Application";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc    = HelloWndProc;

  if (!RegisterClass ( (LPWNDCLASS)pHelloClass))
    return FALSE;

  LocalFree ( (HANDLE)pHelloClass);
  return TRUE;        /* Initialization succeeded */
  }


/***************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;
  int	sizeString;  /* holds return value from LoadString */

  if (!hPrevInstance)
    HelloInit (hInstance);

  hWnd = CreateWindow ( (LPSTR)"Sample Application",
      (LPSTR)"Sample Application",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      (HWND)NULL,        /* no parent */
  (HMENU)NULL,      /* use class menu */
  (HANDLE)hInstance, /* handle to window instance */
  (LPSTR)NULL        /* no params to pass on */
 );

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

/* attempt to load string with identifier STRING1 */
  sizeString = LoadString (hInstance, STRING1, (LPSTR)szBuffer, 25);

  if (sizeString > 0)  /* if string with STRING1 identifier exists */
    MessageBox (hWnd, (LPSTR)szBuffer,
        (LPSTR)"The following string came via LoadString", MB_OK);
  else
    MessageBox (hWnd, (LPSTR)"No such string resource exists",
        (LPSTR)"ERROR", MB_OK);

/* attempt to load string with identifier STRING2 */
  sizeString = LoadString (hInstance, STRING2, (LPSTR)szBuffer, 25);

  if (sizeString > 0) /* if string with STRING2 identifier exists */
    MessageBox (hWnd, (LPSTR)szBuffer,
        (LPSTR)"The following string came via LoadString", MB_OK);
  else
    MessageBox (hWnd, (LPSTR)"No such string resource exists",
        (LPSTR)"ERROR", MB_OK);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  return (int)msg.wParam;
  }

/***************************************************************************/
long    FAR PASCAL HelloWndProc (hWnd, message, wParam, lParam)
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
  {
  switch (message)
    {
    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
