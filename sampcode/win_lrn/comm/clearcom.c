/*
 *
 * Function (s) demonstrated in this program: ClearCommBreak, CloseComm,
 *    OpenComm, SetCommBreak
 *
 * Compiler version:  C 5.10
 *
 * Description:  This function restores character transmission and places the
 *    transmission line in a nonbreak state.
 *
 */

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "ClearCom.h"

HWND     hWndParent1;
HANDLE   hInstMain;

char	szOutputBuffer1 [70];
char	szOutputBuffer2 [500];


/****************************************************************************/
/************************    Message Structure      *************************/
/****************************************************************************/

struct   {
  char	*szMessage; 
  } Messages [] =   {
  "About\0",
  "     This is a sample application to demonstrate the\n\
use of the ClearCommBreak, CloseComm, OpenComm, and\n\
SetCommBreak Windows functions.",

      "Help Message",
  "     This program uses the ClearCommBreak Windows\n\
function to restore character transmission to the\n\
comm port and places the transmission line in a\n\
nonbreak state.  nResult should be 0 if the function\n\
was successful.  Use the menu to invoke this\n\
function.",

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
int         nCmdShow;
  {
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
    wndclass.lpszMenuName  = "ClearCom";
    wndclass.lpszClassName = "ClearCom";

    if (!RegisterClass (&wndclass))
      return FALSE;
    }

  hWndParent1 = CreateWindow (szAppName,          /* window class name       */
                             "ClearCommBreak",    /* window caption          */
                             WS_OVERLAPPEDWINDOW, /* window style            */
                             CW_USEDEFAULT,       /* initial x position      */
                             0,                   /* initial y position      */
                             CW_USEDEFAULT,       /* initial x size          */
                             0,                   /* initial y size          */
                             NULL,                /* parent window handle    */
                             NULL,                /* window menu handle      */
                             hInstance,           /* program instance handle */
                             NULL);               /* create parameters       */

  ShowWindow (hWndParent1, nCmdShow);
  UpdateWindow (hWndParent1);

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
  int    nCid;
  int    nResult;
  HMENU  hMenu;

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
        case IDM_CLEARCOM:
          nCid = OpenComm ( (LPSTR)"LPT1", 255, 256);
          sprintf (szOutputBuffer1, "nCid = %i", nCid);
          MessageBox (hWnd, szOutputBuffer1, "OpenComm", MB_OK);

          nResult = SetCommBreak (nCid);
          sprintf (szOutputBuffer1, "nResult = %i", nResult);
          MessageBox (hWnd, szOutputBuffer1, "SetCommBreak", MB_OK);

          nResult = ClearCommBreak (nCid);
          sprintf (szOutputBuffer1, "nResult = %i", nResult);
          MessageBox (hWnd, szOutputBuffer1, "ClearCommBreak", MB_OK);

          nResult = CloseComm (nCid);
          sprintf (szOutputBuffer1, "nResult = %i", nResult);
          MessageBox (hWnd, szOutputBuffer1, "CloseComm", MB_OK);
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
