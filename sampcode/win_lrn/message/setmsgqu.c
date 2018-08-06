/*
Windows Version 2.03 function demonstration application

Function(s) demonstrated in this program: SetMessageQueue

Compiler version: 5.01

Description:

This program calls SetMessageQueue to destroy the old message queue
and create a new message queue which can hold up to ten messages. The
default message queue can hold only eight messages.  The results of
the call to SetMessageQueue are displayed on the screen.  If the call
works then a new message queue is created and the old message queue
is destroyed, along with any messages it might contain.  If the call
fails then the old message queue is still destroyed but a new message
queue has not been created.  In the latter case the application must
continue calling SetMessageQueue with a smaller queue size until the
function returns a nonzero value.


****************************************************************************/

#include <windows.h>
#include <string.h>

long FAR PASCAL WndProc (HWND, unsigned, WORD, LONG) ;

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE      hInstance, hPrevInstance ;
     LPSTR       lpszCmdLine ;
     int         nCmdShow ;
     {
     static char szAppName [] = "SetMessageQueue" ;
     HWND        hWnd ;
     WNDCLASS    wndclass ;
     MSG msg;

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
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          if (!RegisterClass (&wndclass))
               return FALSE ;
          }

     hWnd = CreateWindow (szAppName,            /* window class name       */
                    "SetMessageQueue",          /* window caption          */
                    WS_OVERLAPPEDWINDOW,        /* window style            */
                    CW_USEDEFAULT,              /* initial x position      */
                    0,                          /* initial y position      */
                    CW_USEDEFAULT,              /* initial x size          */
                    0,                          /* initial y size          */
                    NULL,                       /* parent window handle    */
                    NULL,                       /* window menu handle      */
                    hInstance,                  /* program instance handle */
                    NULL) ;                     /* create parameters       */

     ShowWindow (hWnd, nCmdShow) ;

     UpdateWindow (hWnd) ;

     while (GetMessage(&msg, NULL, 0, 0))
     {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
     } 
     return (msg.wParam) ;     
     }

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned iMessage ;
WORD     wParam ;
LONG     lParam ;
{
 PAINTSTRUCT     ps;
 HDC             hDC;
 TEXTMETRIC      tm;
 static short    xChar,
                 yChar;
 short           iRow; /* An index to keep track of the current row *
                        * for text output
                       */

 switch(iMessage)
 {
  case WM_CREATE:
  {
   hDC = GetDC (hWnd);
   GetTextMetrics (hDC, &tm);
   xChar = tm.tmAveCharWidth;
   yChar = tm.tmHeight + tm.tmExternalLeading;
   ReleaseDC (hWnd, hDC);
   break;
  }
  case WM_PAINT:
  {
   hDC = BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);

   iRow = 1;

LOOP:
   if (SetMessageQueue(10) == 0)
      {
       TextOut (hDC,
               xChar,
               yChar * iRow,
               (LPSTR)"SetMessageQueue has failed.  The application has no message queue!",
               strlen("SetMessageQueue has failed.  The application has no message queue!")
              );

      iRow++;
      goto LOOP;
      }
   else
      {
       TextOut (hDC,
               xChar,
               yChar * iRow,
               (LPSTR)"SetMessageQueue works.",
               strlen("SetMessageQueue works.")
              );

       TextOut (hDC,
               xChar,
               yChar * ++iRow,
               (LPSTR)"The message queue for this application will now hold ten messages.",
               strlen("The message queue for this application will now hold ten messages.")
              );

       TextOut (hDC,
               xChar,
               yChar * ++iRow,
               (LPSTR)"Before the SetMessageQueue call it only held eight messages.",
               strlen("Before the SetMessageQueue call it only held eight messages.")
              );

      }
   EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
   break;
  }
  case WM_DESTROY:
  {
   PostQuitMessage(0);
   break;
  }
  default:
  {
   return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
  }
 }
 return (0L); 
}
