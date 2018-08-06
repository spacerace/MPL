/*
Microsoft Systems Journal
Volume 2; Issue 2; July, 1987

Code Listings For:

	BLOWUP
	pp. 51-58

Author(s): Charles Petzold
Title:     BLOWUP: A Windows Utility for Viewing and Manipulating Bitmaps
*/




/* BLOWUP.C -- Capture Screen Image to Clipboard by Charles Petzold */

#include <windows.h>
#include <stdlib.h>

long FAR PASCAL WndProc (HWND, unsigned, WORD, LONG) ;

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE         hInstance, hPrevInstance ;
     LPSTR          lpszCmdLine ;
     int            nCmdShow ;
     {
     static char    szAppName [] = "Blowup" ;
     HWND           hWnd ;
     MSG            msg ;
     WNDCLASS       wndclass ;

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
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          if (!RegisterClass (&wndclass))
               return FALSE ;
          }

     hWnd = CreateWindow (szAppName, szAppName, 
                    WS_TILEDWINDOW,
                    0, 0, 0, 0,
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

void InvertBlock (hWnd, org, len)
     HWND  hWnd ;
     POINT org, len ;
     {
     HDC   hDC ;

     hDC = CreateDC ("DISPLAY", NULL, NULL, NULL) ;
     ClientToScreen (hWnd, &org) ;
     PatBlt (hDC, org.x, org.y, len.x, len.y, DSTINVERT) ;
     DeleteDC (hDC) ;
     }

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
     HWND           hWnd ;
     unsigned       iMessage ;
     WORD           wParam ;
     LONG           lParam ;
     {
     static BOOL    bCapturing, bBlocking ;
     static HWND    hWndNext ;
     static POINT   org, len ;
     static short   xClient, yClient ;
     BITMAP         bm ;
     HDC            hDC, hMemDC ;
     HBITMAP        hBitmap ;
     PAINTSTRUCT    ps ;

     switch (iMessage)
          {
          case WM_CREATE:
               hWndNext = SetClipboardViewer (hWnd) ;
               break ;

          case WM_SIZE:
               xClient = LOWORD (lParam) ;
               yClient = HIWORD (lParam) ;
               break ;

          case WM_LBUTTONDOWN:
               if (!bCapturing)
                    {
                    bCapturing = TRUE ;
                    SetCapture (hWnd) ;
                    SetCursor (LoadCursor (NULL, IDC_CROSS)) ;
                    }
               else if (!bBlocking)
                    {
                    bBlocking = TRUE ;
                    org = MAKEPOINT (lParam) ;
                    }
               break ;

          case WM_MOUSEMOVE:
               if (bCapturing)
                    SetCursor (LoadCursor (NULL, IDC_CROSS)) ;

               if (bBlocking)
                    {
                    len = MAKEPOINT (lParam) ;
                    len.x -= org.x ;
                    len.y -= org.y ;

                    InvertBlock (hWnd, org, len) ;
                    InvertBlock (hWnd, org, len) ;
                    }
               break ;

          case WM_LBUTTONUP:
               if (!bBlocking)
                    break ;

               bCapturing = bBlocking = FALSE ;
               SetCursor (LoadCursor (NULL, IDC_ARROW)) ;
               ReleaseCapture () ;

               if (len.x == 0 || len.y == 0)
                    break ;

               hDC = GetDC (hWnd) ;
               hMemDC = CreateCompatibleDC (hDC) ;
               hBitmap = CreateCompatibleBitmap (hDC,
                                   abs (len.x), abs (len.y)) ;
               if (hBitmap)
                    {
                    SelectObject (hMemDC, hBitmap) ;
                    StretchBlt (hMemDC, 0, 0, abs (len.x), abs (len.y),
                         hDC, org.x, org.y, len.x, len.y, SRCCOPY) ;

                    OpenClipboard (hWnd) ;
                    EmptyClipboard () ;
                    SetClipboardData (CF_BITMAP, hBitmap) ;
                    CloseClipboard () ;
                    }
               else
                    MessageBeep (0) ;

               DeleteDC (hMemDC) ;
               ReleaseDC (hWnd, hDC) ;
               break ;

          case WM_PAINT:
               hDC = BeginPaint (hWnd, &ps) ;
               OpenClipboard (hWnd) ;

               if (hBitmap = GetClipboardData (CF_BITMAP))
                    {
                    SetCursor (LoadCursor (NULL, IDC_WAIT)) ;

                    hMemDC = CreateCompatibleDC (hDC) ;
                    SelectObject (hMemDC, hBitmap) ;
                    GetObject (hBitmap, sizeof (BITMAP), (LPSTR) &bm) ;

                    SetStretchBltMode (hDC, COLORONCOLOR) ;
                    StretchBlt (hDC, 0, 0, xClient, yClient,
                                hMemDC, 0, 0, bm.bmWidth, bm.bmHeight,
                                                  SRCCOPY) ;

                    SetCursor (LoadCursor (NULL, IDC_ARROW)) ;
                    DeleteDC (hMemDC) ;
                    }
               CloseClipboard () ;
               EndPaint (hWnd, &ps) ;
               break ;

          case WM_DRAWCLIPBOARD :
               if (hWndNext)
                    SendMessage (hWndNext, iMessage, wParam, lParam) ;

               InvalidateRect (hWnd, NULL, TRUE) ;
               break;

          case WM_CHANGECBCHAIN :
               if (wParam == hWndNext)
                    hWndNext = LOWORD (lParam) ;

               else if (hWndNext)
                    SendMessage (hWndNext, iMessage, wParam, lParam) ;
               break ;

          case WM_DESTROY:
               ChangeClipboardChain (hWnd, hWndNext) ;
               PostQuitMessage (0) ;
               break ;

          default:
               return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
          }
     return 0L ;
     }
