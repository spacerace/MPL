/*
Microsoft Systems Journal
Volume 2; Issue 2; May, 1987

Code Listings For:

	COLORSCR
	pp. 67-75

Author(s): Charles Petzold
Title:     A Simple Windows Application for Custom Color Mixing



==============================================================================
Figure 6: COLORSCR.C

==============================================================================
==============================================================================
*/

/* COLORSCR.C -- Color Scroll (using child window controls) */

#include <windows.h>
#include <stdlib.h>

long FAR PASCAL WndProc    (HWND, unsigned, WORD, LONG) ;
long FAR PASCAL ScrollProc (HWND, unsigned, WORD, LONG) ;

FARPROC lpfnOldScr[3] ;
HWND    hChScrol[3], hChLabel[3], hChValue[3], hChRect ;
short   color[3], nFocus = 0;

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE   hInstance, hPrevInstance;
     LPSTR    lpszCmdLine;
     int      nCmdShow;
     {
     MSG      msg;
     HWND     hWnd ;
     WNDCLASS wndclass ;
     FARPROC  lpfnScrollProc ;
     short    n ;
     static   char *szColorLabel[] = { "Red", "Green", "Blue" } ;
     static   char szAppName[] = "ColorScr" ;-

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = NULL ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = CreateSolidBrush (0L) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     if (!RegisterClass (&wndclass)) return FALSE;

     lpfnScrollProc = MakeProcInstance ((FARPROC) ScrollProc, 
                                       hInstance) ;

     hWnd = CreateWindow (szAppName, " Color Scroll ",
                    WS_TILEDWINDOW | WS_CLIPCHILDREN,
                    0, 0, 0, 0, NULL, NULL, hInstance, NULL) ;

     hChRect = CreateWindow ("static", NULL,
                    WS_CHILD | WS_VISIBLE | SS_WHITERECT,
                    0, 0, 0, 0, hWnd, 9, hInstance, NULL) ;

     for (n = 0 ; n < 3 ; n++) 
          {
          hChScrol[n] = CreateWindow ("scrollbar", NULL,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SBS_VERT,
                        0, 0, 0, 0, hWnd, n, hInstance, NULL) ;

          hChLabel[n] = CreateWindow ("static", szColorLabel[n],
                        WS_CHILD | WS_VISIBLE | SS_CENTER,
                        0, 0, 0, 0, hWnd, n + 3, hInstance, NULL) ;

          hChValue[n] = CreateWindow ("static", "0",
                        WS_CHILD | WS_VISIBLE | SS_CENTER,
                        0, 0, 0, 0, hWnd, n + 6, hInstance, NULL) ; 

          lpfnOldScr[n] = (FARPROC) GetWindowLong (hChScrol[n], 
                          GWL_WNDPROC) ;
          SetWindowLong (hChScrol[n], GWL_WNDPROC, 
                          (LONG) lpfnScrollProc) ;

          SetScrollRange (hChScrol[n], SB_CTL, 0, 255, FALSE) ;
          SetScrollPos   (hChScrol[n], SB_CTL, 0, FALSE) ;
          }

     ShowWindow (hWnd, nCmdShow) ;
     UpdateWindow (hWnd);

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage  (&msg) ;
          }
     return msg.wParam ;
     }

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
     HWND       hWnd;
     unsigned   iMessage;
     WORD       wParam;
     LONG       lParam;
     {
     HDC        hDC ;
     TEXTMETRIC tm ;
     char       szbuffer[10] ;
     short      n, xClient, yClient, yChar ;

     switch (iMessage)
          {
          case WM_SIZE :
               xClient = LOWORD (lParam) ;
               yClient = HIWORD (lParam) ;

               hDC = GetDC (hWnd) ;
               GetTextMetrics (hDC, &tm) ;
               yChar = tm.tmHeight ;
               ReleaseDC (hWnd, hDC) ;

               MoveWindow (hChRect, 0, 0, xClient / 2, yClient, TRUE);

               for (n = 0 ; n < 3 ; n++)
                    {
                    MoveWindow (hChScrol[n],
                         (2 * n + 1) * xClient / 14, 2 * yChar,
                         xClient / 14, yClient - 4 * yChar, TRUE) ;

                    MoveWindow (hChLabel[n],
                         (4 * n + 1) * xClient / 28, yChar / 2,
                         xClient / 7, yChar, TRUE) ;

                    MoveWindow (hChValue[n],
                         (4 * n + 1) * xClient / 28, 
                         yClient - 3 * yChar / 2,
                         xClient / 7, yChar, TRUE) ;
                    }
               SetFocus (hWnd) ;
               break ;

          case WM_SETFOCUS:
               SetFocus (hChScrol[nFocus]) ;
               break ;

          case WM_VSCROLL :
               n = GetWindowWord (HIWORD (lParam), GWW_ID) ;

               switch (wParam)
                    {
                    case SB_PAGEDOWN :
                         color[n] += 15 ;         /* fall through */
                    case SB_LINEDOWN :
                         color[n] = min (255, color[n] + 1) ;
                         break ;
                    case SB_PAGEUP :
                         color[n] Ñ= 15 ;         /* fall through */
                    case SB_LINEUP :
                         color[n] = max (0, color[n] Ñ 1) ;
                         break ;
                    case SB_TOP:
                         color[n] = 0 ;
                         break ;
                    case SB_BOTTOM :
                         color[n] = 255 ;
                         break ;
                    case SB_THUMBPOSITION :
                    case SB_THUMBTRACK :
                         color[n] = LOWORD (lParam) ;
                         break ;
                    default :
                         break ;
                    }
               SetScrollPos  (hChScrol[n], SB_CTL, color[n], TRUE) ;
               SetWindowText (hChValue[n], 
                             itoa (color[n], szbuffer, 10)) ;

               DeleteObject (GetClassWord (hWnd, GCW_HBRBACKGROUND)) ;
               SetClassWord (hWnd, GCW_HBRBACKGROUND, CreateSolidBrush
                            (RGB (color[0], color[1], color[2]))) ;

               InvalidateRect (hWnd, NULL, TRUE) ;
               break ;

          case WM_DESTROY:
               DeleteObject (GetClassWord (hWnd, GCW_HBRBACKGROUND)) ;
               PostQuitMessage (0) ;
               break ;

          default :
               return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
          }
     return 0L ;
     }

long FAR PASCAL ScrollProc (hWnd, iMessage, wParam, lParam)
     HWND      hWnd ;
     unsigned  iMessage ;
     WORD      wParam ;
     LONG      lParam ;
     {
     short     n = GetWindowWord (hWnd, GWW_ID) ;

     switch (iMessage)
          {
          case WM_KEYDOWN:
               if (wParam == VK_TAB)
                    SetFocus (hChScrol[(n +
                         (GetKeyState (VK_SHIFT) < 0 ? 2 : 1)) % 3]) ;
               break ;

          case WM_SETFOCUS:
               nFocus = n ;
               break ;
          }
     return CallWindowProc (lpfnOldScr[n], hWnd, iMessage, wParam, 
                           lParam) ;
     }
