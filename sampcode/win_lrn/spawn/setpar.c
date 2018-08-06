/*

Function(s) demonstrated in this program: SetParent

Description:  This function changes the parent of the respective child 
    window.

*/

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "SetPar.h"


HWND     hWndParent1, hWndParent2, hWndChild1, hWndChild2;
HANDLE   hInstMain;

char     szOutputBuffer1 [70];
char     szOutputBuffer2 [500];


/****************************************************************************/
/************************    Message Structure      *************************/
/****************************************************************************/

struct { char *szMessage; }
       Messages [] = {
"About\0",
"     This is a sample application to demonstrate the\n\
use  of  the  SetParent Windows function.",

"Help Message",
"     This program uses the SetParent Windows\n\
function to change the parent of the child windows.\n\
Use the menu to choose the parent."

};	

/****************************************************************************/

void ProcessMessage (HWND, int); 

void ProcessMessage (hWnd, MessageNumber) 
     HWND     hWnd;
     int      MessageNumber;
{
     sprintf (szOutputBuffer1, "%s", Messages [MessageNumber]);
     sprintf (szOutputBuffer2, "%s", Messages [MessageNumber + 1]);
     MessageBox (hWnd, szOutputBuffer2, szOutputBuffer1, MB_OK);
}       

/****************************************************************************/

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE      hInstance, hPrevInstance ;
     LPSTR       lpszCmdLine ;
     int         nCmdShow ;
     {
     static char szAppName [] = "SetPar" ;
     static char szChildClass [] = "SetParChild" ;
     HWND        hWnd ;
     WNDCLASS    wndclass ;
     MSG msg;
     short       xScreen, yScreen ;

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

          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = ChildWndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = NULL ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_CROSS) ;
          wndclass.hbrBackground = GetStockObject (LTGRAY_BRUSH) ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szChildClass ;

          if (!RegisterClass (&wndclass))
               return FALSE ;

          }

     xScreen = GetSystemMetrics (SM_CXSCREEN) ;
     yScreen = GetSystemMetrics (SM_CYSCREEN) ;

     hWndParent1 = CreateWindow (szAppName,     /* window class name       */
                    "Parent 1",                 /* window caption          */
                    WS_OVERLAPPEDWINDOW,        /* window style            */
                    xScreen / 9,                /* initial x position      */
                    yScreen / 7,                /* initial y position      */
                    xScreen * 3 / 9,            /* initial x size          */
                    yScreen * 6 / 9,            /* initial y size          */
                    NULL,                       /* parent window handle    */
                    NULL,                       /* window menu handle      */
                    hInstance,                  /* program instance handle */
                    NULL) ;                     /* create parameters       */

     ShowWindow (hWndParent1, nCmdShow) ;
     UpdateWindow (hWndParent1) ;

     hWndParent2 = CreateWindow (szAppName,     /* window class name       */
                    "Parent 2",                 /* window caption          */
                    WS_OVERLAPPEDWINDOW,        /* window style            */
                    xScreen * 5 / 9,            /* initial x position      */
                    yScreen / 7,                /* initial y position      */
                    xScreen * 3 / 9,            /* initial x size          */
                    yScreen * 6 / 9,            /* initial y size          */
                    NULL,                       /* parent window handle    */
                    NULL,                       /* window menu handle      */
                    hInstance,                  /* program instance handle */
                    NULL) ;                     /* create parameters       */

     ShowWindow (hWndParent2, nCmdShow) ;
     UpdateWindow (hWndParent2) ;

     hInstMain = hInstance;

     hWndChild1 = CreateWindow (szChildClass, "Child 1", WS_CHILD | 
                    WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX |
                    WS_CLIPSIBLINGS,    
                    0, 0, xScreen / 8, yScreen / 6,
                    hWndParent1, 1, hInstance, NULL) ;     

     ShowWindow (hWndChild1, SW_SHOWNORMAL) ;
     UpdateWindow (hWndChild1) ;

     hWndChild2 = CreateWindow (szChildClass, "Child 2", WS_CHILD | 
                    WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX |
                    WS_CLIPSIBLINGS,    
                    0, yScreen / 5, xScreen / 8, yScreen / 6,    
                    hWndParent1, 1, hInstance, NULL) ;         

     ShowWindow (hWndChild2, SW_SHOWNORMAL) ;
     UpdateWindow (hWndChild2) ;

     while (GetMessage(&msg, NULL, 0, 0))
     {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
     } 
     return (msg.wParam) ;     
     }

/****************************************************************************/

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned iMessage ;
WORD     wParam ;
LONG     lParam ;
{
 HMENU       hMenu;
 PAINTSTRUCT ps;
 static int  xClient, yClient;
 static int  xDevisor, yDevisor, randNum, Index;
 char        szOutputBuffer [40];

 switch(iMessage)
 {
  case WM_CREATE:
       hMenu = GetSystemMenu (hWnd, FALSE);

       ChangeMenu (hMenu, NULL, "&About", IDM_ABOUT, 
                   MF_APPEND | MF_STRING);
       break;

  case WM_SYSCOMMAND:
       switch (wParam) {
          case IDM_ABOUT:
               ProcessMessage (hWnd, 0);
               break;
          default:
               return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
       }
       break;

  case WM_COMMAND:
       switch (wParam) {
          case IDM_SETPARENT1:
               SetParent (hWndChild1, hWndParent1);
               SetParent (hWndChild2, hWndParent1);
               break;

          case IDM_SETPARENT2:
               SetParent (hWndChild1, hWndParent2);
               SetParent (hWndChild2, hWndParent2);
               break;

          case IDM_HELP:
               ProcessMessage (hWnd, 2);
               break;
       }
       break;

  case WM_PAINT:
       BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
       EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
       break;

  case WM_DESTROY:
       PostQuitMessage(0);
       break;

  default:
  {
   return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
  }
 }
 return (0L); 
}

/****************************************************************************/

long FAR PASCAL ChildWndProc (hWnd, iMessage, wParam, lParam)
     HWND     hWnd ;
     unsigned iMessage ;
     WORD     wParam ;
     LONG     lParam ;
     {
     return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
     }


