/*

Function(s) demonstrated in this program: ShowScrollBar

Description:  This function displays or hides a scroll bar, depending on the
   value of the the parameters. 

*/

#include <windows.h>
#include <stdio.h>
#include "ShowSB.h"
char     szOutputBuffer1 [70];
char     szOutputBuffer2 [500];

/****************************************************************************/
/************************    Message Structure      *************************/
/****************************************************************************/

struct { char *szMessage; }
       Messages [] = {
"About",
"     This is a sample  application to demonstrate\n\
the use of the ShowScrollBar Windows function.",

"Help Message",
"     This program uses the ShowScrollBar Windows\n\
function to change status of the Scroll bars of the\n\
window.  Use the menu to choose the status (show or\n\
hide)."

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
     HANDLE   hInstance, hPrevInstance ;
     LPSTR    lpszCmdLine ;
     int      nCmdShow ;
     {
     WNDCLASS wndclass ;
     HWND     hWnd ;
     MSG      msg ;
     static   char szAppName [] = "ShowSB" ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          if (!RegisterClass (&wndclass))
               return FALSE ;
          }

     hWnd = CreateWindow (szAppName, "Show Scroll Bar",
                         WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
                         CW_USEDEFAULT, 0,
                         CW_USEDEFAULT, 0,
                         NULL, NULL, hInstance, NULL) ;

     ShowWindow (hWnd, nCmdShow) ;
     UpdateWindow (hWnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

/****************************************************************************/


long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
     HWND         hWnd ;
     unsigned     iMessage ;
     WORD         wParam ;
     LONG         lParam ;
     {
     PAINTSTRUCT  ps ;
     HMENU        hMenu;

     switch (iMessage)
          {
          case WM_CREATE:
               hMenu = GetSystemMenu (hWnd, FALSE);
               ChangeMenu (hMenu, NULL, "&About", IDM_ABOUT, 
                           MF_APPEND | MF_STRING);
               break ;

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
                  case IDM_ABOUT:
                       ProcessMessage (hWnd, 0);
                       break;

                  case IDM_SHOW:
                       ShowScrollBar (hWnd, SB_BOTH, TRUE);
                       break;
        
                  case IDM_HIDE:
                       ShowScrollBar (hWnd, SB_BOTH, FALSE);
                       break;
        
                  case IDM_HELP:
                       ProcessMessage (hWnd, 2);
                       break;
               }
               break;

          case WM_PAINT:
               BeginPaint (hWnd, &ps) ;
               EndPaint (hWnd, &ps) ;
               break ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               break ;

          default:
               return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
          }
     return 0L ;
     }

