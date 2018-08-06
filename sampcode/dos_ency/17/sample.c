/* SAMPLE.C -- Demonstration Windows Program */

#include <windows.h>
#include "sample.h"

long FAR PASCAL WndProc (HWND, unsigned, WORD, LONG) ;

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE      hInstance, hPrevInstance ;
     LPSTR       lpszCmdLine ;
     int         nCmdShow ;
     {
     WNDCLASS    wndclass ;
     HWND        hWnd ;
     MSG         msg ;
     static char szAppName [] = "Sample" ;

               /*---------------------------*/
               /* Register the Window Class */
               /*---------------------------*/

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = NULL ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

               /*----------------------------------*/
               /* Create the window and display it */
               /*----------------------------------*/

     hWnd = CreateWindow (szAppName, "Demonstration Windows Program",
                          WS_OVERLAPPEDWINDOW,
                          (int) CW_USEDEFAULT, 0,
                          (int) CW_USEDEFAULT, 0,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hWnd, nCmdShow) ;
     UpdateWindow (hWnd) ;

                /*----------------------------------------------*/
                /* Stay in message loop until a WM_QUIT message */
                /*----------------------------------------------*/

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
     HWND         hWnd ;
     unsigned     iMessage ;
     WORD         wParam ;
     LONG         lParam ;
     {
     PAINTSTRUCT  ps ;
     HFONT        hFont ;
     HMENU        hMenu ;
     static short xClient, yClient, nCurrentFont = IDM_SCRIPT ;
     static BYTE  cFamily [] = { FF_SCRIPT, FF_MODERN, FF_ROMAN } ;
     static char  *szFace [] = { "Script",  "Modern",  "Roman"  } ;

     switch (iMessage)
          {

                    /*---------------------------------------------*/
                    /* WM_COMMAND message: Change checkmarked font */
                    /*---------------------------------------------*/

          case WM_COMMAND:
               hMenu = GetMenu (hWnd) ;
               CheckMenuItem (hMenu, nCurrentFont, MF_UNCHECKED) ;
               nCurrentFont = wParam ;
               CheckMenuItem (hMenu, nCurrentFont, MF_CHECKED) ;
               InvalidateRect (hWnd, NULL, TRUE) ;
               break ;

                    /*--------------------------------------------*/
                    /* WM_SIZE message: Save dimensions of window */
                    /*--------------------------------------------*/

          case WM_SIZE:
               xClient = LOWORD (lParam) ;
               yClient = HIWORD (lParam) ;
               break ;

                    /*-----------------------------------------------*/
                    /* WM_PAINT message: Display "Windows" in Script */
                    /*-----------------------------------------------*/

          case WM_PAINT:
               BeginPaint (hWnd, &ps) ;

               hFont = CreateFont (yClient, xClient / 8,
                                   0, 0, 400, 0, 0, 0, OEM_CHARSET, 
                                   OUT_STROKE_PRECIS, OUT_STROKE_PRECIS,
                                   DRAFT_QUALITY, (BYTE) VARIABLE_PITCH |
                                   cFamily [nCurrentFont - IDM_SCRIPT],
                                   szFace  [nCurrentFont - IDM_SCRIPT]) ;

               hFont = SelectObject (ps.hdc, hFont) ;
               TextOut (ps.hdc, 0, 0, "Windows", 7) ;

               DeleteObject (SelectObject (ps.hdc, hFont)) ;
               EndPaint (hWnd, &ps) ;
               break ;

                    /*---------------------------------------*/
                    /* WM_DESTROY message: Post Quit message */
                    /*---------------------------------------*/

          case WM_DESTROY:
               PostQuitMessage (0) ;
               break ;

                    /*---------------------------------------*/
                    /* Other messages: Do default processing */
                    /*---------------------------------------*/

          default:
               return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
          }
     return 0L ;
     }
