/*
 *
 * Function (s) demonstrated in this program: CallMsgFilter, SetWindowsHook,
 *    UnhookWindowsHook
 * Compiler version:  C 5.10
 * Description:  This function calls the Message Filter with the given message.
 *    The message filter is inserted in the Message Filter chain by use of
 *    the SetWindowsHook, and removed using the UnhookWindowsHook.
 *
 */

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "CallMsgF.h"

extern HWND   hWndMain;

HWND     hWndMain;
HANDLE   hInstMain;

char	szOutputBuffer1 [70];
char	szOutputBuffer2 [500];

/****************************************************************************/
/************************    Message Structure      *************************/
/****************************************************************************/

struct   {
  char	*szMessage; 
  } Messages [] =   {
"About",
"     This is a sample  application to  demonstrate\n\
the use of the CallMsgFilter, SetWindowsHook, and\n\
UnhookWindowsHook Windows functions.",

"Help Message",
"     This program uses the CallMsgFilter Windows\n\
function to send a special message to the main\n\
window.  To test this function use the menu to\n\
select the option to invoke the function.",

"CallMsgFilter",
"     This message was passed to the main window\n\
by the message filter."

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
int	nCmdShow;
  {
  static char	szAppName [] = "CallMsgF";
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

    hWndMain = CreateWindow (szAppName,       /* window class name       */
               "CallMsgFilter",               /* window caption          */
                WS_OVERLAPPEDWINDOW,          /* window style            */
                CW_USEDEFAULT,                /* initial x position      */
                0,                            /* initial y position      */
                CW_USEDEFAULT,                /* initial x size          */
                0,                            /* initial y size          */
                NULL,                         /* parent window handle    */
                NULL,                         /* window menu handle      */
                hInstance,                    /* program instance handle */
                NULL);                        /* create parameters       */

    ShowWindow (hWndMain, nCmdShow);
    UpdateWindow (hWndMain);

    hInstMain = hInstance;
  
    InitHook (hInstMain);

    while (GetMessage (&msg, NULL, 0, 0))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }

    KillHook ();

    return (msg.wParam);
}


/****************************************************************************/

long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned	iMessage;
WORD     wParam;
LONG     lParam;
  {
  HMENU       hMenu;
  MSG         msg;

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
        case IDM_CALLMSGFILTER:
          MessageBox (hWnd, "Calling the Message Filter.",
                      "CallMsgFilter", MB_OK);
          msg.hwnd    = hWnd;
          msg.message = WM_COMMAND;
          msg.wParam  = IDM_FILTERMSG;
          msg.lParam  = 0L;

          CallMsgFilter (&msg, HC_ACTION);
          break;

        case IDM_FROMMSGFILTER:
          ProcessMessage (hWnd, 4);
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
