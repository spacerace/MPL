/*

Description:

This program first invokes the function GetRelAbs to find and display
the current mode.  The mode is set to another value using the SetRelAbs
function. GetRelAbs is called again to verify that the mode has indeed
changed.  Below are descriptions for the demonstrated functions.

SetRelAbs sets the relative or absolute mode. This function specifies
whether the coordinates in GDI functions are relative to the origin
of the given device (ABSOLUTE), or relative to the current position
(RELATIVE).  The mode affects the output created by the LineTo and
Polyline functions.

GetRelAbs retrieves the relative-absolute flag. The return value
specifies the current mode.  It can be ABSOLUTE or RELATIVE.

****************************************************************************/

#include <windows.h>
#include <string.h>

long FAR PASCAL WndProc (HWND, unsigned, WORD, LONG) ;

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE      hInstance, hPrevInstance ;
     LPSTR       lpszCmdLine ;
     int         nCmdShow ;
     {
     static char szAppName [] = "GetRelAbs" ;
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
                    "GetRelAbs",  /* window caption          */
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
 short           fRelAbsFlag;
 HDC             hDC;
 TEXTMETRIC      tm;
 static short    xChar,
                 yChar;

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

   fRelAbsFlag = GetRelAbs (hDC);                        /* Check the mode */

   if (fRelAbsFlag == RELATIVE)
      {
      TextOut (hDC,
               xChar,
               yChar,
               (LPSTR)"1. Function GetRelAbs returns RELATIVE, so the mode is currently RELATIVE.",
               strlen("1. Function GetRelAbs returns RELATIVE, so the mode is currently RELATIVE.")
              );

      SetRelAbs (hDC, ABSOLUTE);
      }
   else /* fRelAbsFlag ==  ABSOLUTE */         /* Set the mode to ABSOLUTE */
      {
      TextOut (hDC,
               xChar,
               yChar,
               (LPSTR)"1. Function GetRelAbs returns ABSOLUTE, so the mode is currently ABSOLUTE.",
               strlen("1. Function GetRelAbs returns ABSOLUTE, so the mode is currently ABSOLUTE.")
              );

      SetRelAbs (hDC, RELATIVE);               /* Set the mode to RELATIVE */
      }

   fRelAbsFlag = GetRelAbs (hDC);                        /* Check the mode */

   if (fRelAbsFlag == RELATIVE)
      {
      TextOut (hDC,
               xChar,
               yChar * 2,
               (LPSTR)"2. Function SetRelAbs sets mode to RELATIVE.  Function GetRelAbs returns RELATIVE.",
               strlen("2. Function SetRelAbs sets mode to RELATIVE.  Function GetRelAbs returns RELATIVE.")
              );
      }
   else /* fRelAbsFlag ==  ABSOLUTE */
      {
      TextOut (hDC,
               xChar,
               yChar * 2,
               (LPSTR)"2. Function SetRelAbs sets mode to ABSOLUTE.  Function GetRelAbs returns ABSOLUTE.",
               strlen("2. Function SetRelAbs sets mode to ABSOLUTE.  Function GetRelAbs returns ABSOLUTE.")
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
