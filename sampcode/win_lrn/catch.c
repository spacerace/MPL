/*
 * Function(s) demonstrated in this program: Catch, Throw
 * Compiler version:  C 5.10
 * Description:  The Catch function catches the current execution environment
 *    and stores it in a buffer.  The Throw function jump to an execution
 *         state described by the parameter.
 *
 */

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "Catch.h"

HWND     hWndParent1;
HANDLE   hInstMain;

char	szOutputBuffer1 [70];
char	szOutputBuffer2 [500];
CATCHBUF CatchBuf;
int	CatchVal;

/****************************************************************************/
/************************    Message Structure      *************************/
/****************************************************************************/

struct   {
  char	*szMessage;
  } Messages [] =   {
  "About",
  "     This is a sample application to demonstrate the\n\
use of the Catch and Throw Windows functions.",

  "Help Message",
  "     This program uses the Catch Windows function\n\
to copy the current state of the execution environment.\n\
Then the Throw Windows function is used to return to\n\
the original execution environment.  Use the menu to\n\
invoke this function.",

  };


/****************************************************************************/

void ProcessMessage (HWND, int);

void ProcessMessage (hWnd, MessageNumber)
HWND    hWnd;
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
int	nCmdShow;
  {
  static char	szAppName [] = "Catch";
  HWND        hWnd;
  WNDCLASS    wndclass;
  MSG msg;
  short xScreen, yScreen;

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

  xScreen = GetSystemMetrics (SM_CXSCREEN);
  yScreen = GetSystemMetrics (SM_CYSCREEN);

hWndParent1 = CreateWindow (szAppName,         /* window class name       */
                           "Catch N Throw",    /* window caption          */
                           WS_OVERLAPPEDWINDOW,/* window style            */
                           CW_USEDEFAULT,      /* initial x position      */
                           0,                  /* initial y position      */
                           CW_USEDEFAULT,      /* initial x size          */
                           0,                  /* initial y size          */
                           NULL,               /* parent window handle    */
                           NULL,               /* window menu handle      */
                           hInstance,          /* program instance handle */
                           NULL);              /* create parameters       */

  ShowWindow (hWndParent1, nCmdShow);
  UpdateWindow (hWndParent1);

  hInstMain = hInstance;

  while (GetMessage(&msg, NULL, 0, 0))
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
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

      ChangeMenu (hMenu, NULL, "&About", IDM_ABOUT,
                  MF_APPEND | MF_STRING);
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
        case IDM_CATCH:
          if (CatchVal = Catch((LPCATCHBUF)CatchBuf))
            {
            switch (CatchVal)
              {
              case ID_ERROR:
                MessageBox(hWnd,
                           "This message made possible by the Throw function",
                           "Catch N Throw",
                           MB_OK | MB_ICONHAND | MB_SYSTEMMODAL);
                break;
              }
            return 0L;
            }
          MessageBox (hWnd, "    About to use Throw to make a jump",
                      "Catch N Throw", MB_OK);
          Throw((LPCATCHBUF)CatchBuf, ID_ERROR);
          break;

        case IDM_HELP:
          ProcessMessage (hWnd, 2);
          break;
        }
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

      default:
        return DefWindowProc (hWnd, iMessage, wParam, lParam);
    }
  return (0L);
  }
