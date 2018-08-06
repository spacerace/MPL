/*

Function(s) demonstrated in this program: GetModuleFileName, GetModuleHandle,
  GetModuleUsage

Windows version:  2.03

Windows SDK version:  2.00

Compiler version:  C 5.10

Description:  This function returns the reference count of the module.

Additional Comments:

*/

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "GetMod.h"


HWND     hWndParent1;
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
use of the GetModuleFileName, GetModuleHandle, and\n\
GetModuleUsage Windows functions.",

"Help Message",
"     This program uses the GetModuleUsage Windows\n\
function to get the reference count of the current\n\
module.  To test this function, first start several\n\
instances of this program.  Then use the menu to\n\
invoke the function.  The count displayed should\n\
directly represent the number of instances you have\n\
started.",

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
     static char szAppName [] = "GetMod" ;
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
          }

     xScreen = GetSystemMetrics (SM_CXSCREEN) ;
     yScreen = GetSystemMetrics (SM_CYSCREEN) ;

     hWndParent1 = CreateWindow (szAppName,     /* window class name       */
                    "GetModuleUsage",           /* window caption          */
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

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned iMessage ;
WORD     wParam ;
LONG     lParam ;
{
 int         nUsage;
 HMENU       hMenu;
 HDC         hDC;
 PAINTSTRUCT ps;
 HANDLE      hModule;

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
          case IDM_GETMODF:
               GetModuleFileName(GetModuleHandle((LPSTR)"GetMod"), 
                                 szOutputBuffer1, 70);
               MessageBox (hWnd, szOutputBuffer1, "GetModuleFileName", MB_OK);
               break;

          case IDM_GETMODH:
               hModule = GetModuleHandle ("GetMod");
               sprintf (szOutputBuffer1, "Module handle = %x", hModule);
               MessageBox (hWnd, szOutputBuffer1, "GetModuleHandle", MB_OK);
               break;

          case IDM_GETMODU:
               nUsage = GetModuleUsage (hInstMain);
               sprintf (szOutputBuffer1, 
                        "The module usage of the current module = %i",nUsage);
               MessageBox (hWnd, szOutputBuffer1, "GetModuleUsage", MB_OK);
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



