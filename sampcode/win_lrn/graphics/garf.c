/*
 *
 *  GetAspectRatioFilter
 *  
 *  This program demonstrates the use of the function GetAspectRatioFilter.
 *  This function retrieves the setting for the current aspect-ratio filter.
 *
 */

#include "windows.h"
#include <stdio.h>

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

static char	szAppName [] = "GARF";
static char	szFuncName [] = "GetAspectRatioFilter";

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance;
HANDLE    hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  WNDCLASS  rClass;
  HWND      hWnd;
  MSG       msg;

  if (!hPrevInstance)
    {
    rClass.style         = CS_HREDRAW | CS_VREDRAW;
    rClass.lpfnWndProc   = WndProc;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;
    rClass.hInstance     = hInstance;
    rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = (LPSTR)NULL;
    rClass.lpszClassName = szAppName;

    if (!RegisterClass (&rClass))
      return FALSE;
    }

hWnd = CreateWindow (szAppName,          /* window class name       */
                    szFuncName,          /* window caption          */
                    WS_OVERLAPPEDWINDOW, /* window style            */
                    CW_USEDEFAULT,       /* initial x position      */
                    0,                   /* initial y position      */
                    CW_USEDEFAULT,       /* initial x size          */
                    0,                   /* initial y size          */
                    NULL,                /* parent window handle    */
                    NULL,                /* window menu handle      */
                    hInstance,           /* program instance handle */
                    NULL);               /* create parameters       */

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage (&msg, NULL, 0, 0))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return (msg.wParam);
  }

long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  HDC   hDC;
  HMENU hMenu;
  DWORD dwAspectRatio;
  char	szBuffer[80];

  switch (iMessage)
    {
    case WM_CREATE:
      hMenu = CreateMenu ();
      ChangeMenu (hMenu, NULL, (LPSTR)"Aspect Ratio", 1000, MF_APPEND);
      SetMenu (hWnd, hMenu);
      break;

    case WM_COMMAND:
      if (wParam == 1000)
        {
        hDC = GetDC (hWnd);
        SetMapperFlags (hDC, (DWORD)0);
        dwAspectRatio = GetAspectRatioFilter (hDC);
        sprintf (szBuffer, "%s%hu%s%hu\0",
            "Aspect ratio of filter zero is: x = ", HIWORD (dwAspectRatio),
            " y = ", LOWORD (dwAspectRatio));
        MessageBox (GetFocus (), (LPSTR)szBuffer, (LPSTR)szFuncName, MB_OK);

        SetMapperFlags (hDC, (DWORD)1);
        dwAspectRatio = GetAspectRatioFilter (hDC);
        sprintf (szBuffer, "%s%hu%s%hu\0",
            "Aspect ratio of filter one is: x = ", HIWORD (dwAspectRatio),
            " y = ", LOWORD (dwAspectRatio));
        MessageBox (GetFocus (), (LPSTR)szBuffer, (LPSTR)szFuncName, MB_OK);

        ReleaseDC (hWnd, hDC);
        }
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam);
    }
  return (0L);
  }
