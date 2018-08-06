/*
 *
 * Function (s) demonstrated in this program: CloseWindow
 *
 * Description:  This function makes a window iconic
 *
 */

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "CloseWin.h"

HWND     hWndMain;
HANDLE   hInstMain;

char	szOutputBuffer1 [70];
char	szOutputBuffer2 [500];

struct   {
  char	*szMessage; 
  } Messages [] =   {
  "About\0",
  "     This is a sample application to demonstrate the\n\
use of the CloseWindow Windows function.",

  "Help Message",
  "     This program uses the CloseWindow Windows\n\
function to minimize this window, the window is\n\
then restored to its original state.  Use the menu\n\
to test this function.",

  };

/****************************************************************************/

void ProcessMessage (HWND, int);

void ProcessMessage (hWnd, MessageNumber)
HWND     hWnd;
int	MessageNumber;
  {
  sprintf (szOutputBuffer1, "%s", Messages [MessageNumber]);
  sprintf (szOutputBuffer2, "%s", Messages [MessageNumber + 1]);
  MessageBox (hWnd, szOutputBuffer2, szOutputBuffer1, MB_OK);
  }

/****************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance;
LPSTR       lpszCmdLine;
int     nCmdShow;
  {
  static char   szAppName [] = "CloseWin";
  HWND        hWnd;
  WNDCLASS    wndclass;
  MSG msg;

  if (!hPrevInstance)
    {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName  = szAppName;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass (&wndclass))
      return FALSE;
    }

hWndMain = CreateWindow (szAppName,        /* window class name       */
                        "CloseWindow",              /* window caption          */
                        WS_OVERLAPPEDWINDOW,        /* window style            */
                        CW_USEDEFAULT,              /* initial x position      */
                        0,                          /* initial y position      */
                        CW_USEDEFAULT,              /* initial x size          */
                        0,                          /* initial y size          */
                        NULL,                       /* parent window handle    */
                        NULL,                       /* window menu handle      */
                        hInstance,                  /* program instance handle */
                        NULL);                     /* create parameters       */

  ShowWindow (hWndMain, nCmdShow);
  UpdateWindow (hWndMain);

  hInstMain = hInstance;

  while (GetMessage (&msg, NULL, 0, 0))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return (msg.wParam);
  }

/****************************************************************************/

long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  HMENU       hMenu;

  switch (iMessage)
    {
    case WM_CREATE:
      hMenu = GetSystemMenu (hWnd, FALSE);
      ChangeMenu (hMenu, NULL, "&About", IDM_ABOUT, MF_APPEND | MF_STRING);
      break;

    case WM_SYSCOMMAND:
      switch (wParam)
        {
        case IDM_ABOUT:
          ProcessMessage (hWnd, 0);
          break;
        default:
          return DefWindowProc (hWnd, iMessage, wParam, lParam);
        }
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case IDM_CLOSEWINDOW:
          MessageBox (hWnd, "About to Use CloseWindow", "CloseWindow", MB_OK);
          CloseWindow (hWnd);
          MessageBox (hWnd, "Restoring window to original state",
                      "CloseWindow", MB_OK);
          SendMessage (hWnd, WM_SYSCOMMAND, SC_RESTORE, 0L);
          break;

        case IDM_HELP:
          ProcessMessage (hWnd, 2);
          break;
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
