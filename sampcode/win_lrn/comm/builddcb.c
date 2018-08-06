/*
 *
 * Function(s) demonstrated in this program: BuildCommDCB
 * 
 * Windows version:  2.03
 * 
 * Windows SDK version:  2.00
 * 
 * Compiler version:  C 5.10
 * 
 * Description:  This function translates the definition string specified
 *    by the parameter into appropriate device-control block codes and places
 * 	these codes in the device control block specified by the other parmeter.
 * 
 * Additional Comments:
 * 
 */

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "BuildDCB.h"


HWND     hWndParent1;
HANDLE   hInstMain;

char	szOutputBuffer1 [70];
char	szOutputBuffer2 [500];
char	szMode          [40] = "COM1:9600,N,8,1";


/****************************************************************************/
/************************    Message Structure      *************************/
/****************************************************************************/

struct { 
  char	*szMessage; 
} Messages [] = {
  "About\0",
  "     This is a sample application to demonstrate the\n\
use of the BuildCommDCB Windows function.",

  "Help Message",
  "     This program uses the BuildCommDCB Windows\n\
function to build a device control block from the\n\
given string.  Use the menu to invoke this function.",

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

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance ;
LPSTR       lpszCmdLine ;
int	nCmdShow ;
{
  static char	szAppName [] = "BuildDCB" ;
  HWND        hWnd ;
  WNDCLASS    wndclass ;
  MSG msg;
  short	xScreen, yScreen ;

  if (!hPrevInstance)
  {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
    wndclass.lpfnWndProc   = WndProc ;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
    wndclass.lpszMenuName  = szAppName ;
    wndclass.lpszClassName = szAppName ;

    if (!RegisterClass (&wndclass))
      return FALSE ;
  }

  xScreen = GetSystemMetrics (SM_CXSCREEN) ;
  yScreen = GetSystemMetrics (SM_CYSCREEN) ;

hWndParent1 = CreateWindow (szAppName,     /* window class name       */
"BuildCommDCB",             /* window caption          */
WS_OVERLAPPEDWINDOW,        /* window style            */
CW_USEDEFAULT,              /* initial x position      */
0,                          /* initial y position      */
CW_USEDEFAULT,              /* initial x size          */
0,                          /* initial y size          */
NULL,                       /* parent window handle    */
NULL,                       /* window menu handle      */
hInstance,                  /* program instance handle */
  NULL) ;                     /* create parameters       */

  ShowWindow (hWndParent1, nCmdShow) ;
  UpdateWindow (hWndParent1) ;

  hInstMain = hInstance;

  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return (msg.wParam) ;
}


/****************************************************************************/

long	FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned	iMessage ;
WORD     wParam ;
LONG     lParam ;
{
  HMENU       hMenu;
  HDC         hDC;
  DCB         CommDCB;
  int	ComVal;
  PAINTSTRUCT ps;

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
      return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
    }
    break;

  case WM_COMMAND:
    switch (wParam) 
    {
    case IDM_BUILDDCB:
      sprintf(szOutputBuffer1, "Definition string = COM1:9600,N,8,1");
      MessageBox (hWnd, szOutputBuffer1, "BuildCommDCB", MB_OK);
      ComVal = BuildCommDCB ((LPSTR)szMode, (DCB FAR * ) & CommDCB);
      sprintf (szOutputBuffer2, "%s%s%s%i", 
          "     If the return value is 0 then the BuildCommDCB ",
          "function was performed correctly.\n\n",
          "               The return value = ", ComVal);
      MessageBox (hWnd, szOutputBuffer2, "BuildCommDCB", MB_OK);
      break;

    case IDM_HELP:
      ProcessMessage (hWnd, 2);
      break;
    }
    break;

  case WM_PAINT:
    BeginPaint(hWnd, (LPPAINTSTRUCT) & ps);
    EndPaint(hWnd, (LPPAINTSTRUCT) & ps);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
  }
  return (0L);
}



