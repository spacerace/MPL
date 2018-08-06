/*-----------------------------------------------------------
   SHOWBIT.C -- Shows bitmaps in BITLIB dynamic link library
                (c) Charles Petzold, 1990
  -----------------------------------------------------------*/

#include <windows.h>

long FAR PASCAL WndProc (HWND, WORD, WORD, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static   char szAppName [] = "ShowBit" ;
     HWND     hwnd ;
     MSG      msg ;
     WNDCLASS wndclass ;

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

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "Show Bitmaps from BITLIB (Press Key)",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

void DrawBitmap (HDC hdc, short xStart, short yStart, HBITMAP hBitmap)
     {
     BITMAP bm ;
     DWORD  dwSize ;
     HDC    hMemDC ;
     POINT  pt ;

     hMemDC = CreateCompatibleDC (hdc) ;
     SelectObject (hMemDC, hBitmap) ;
     GetObject (hBitmap, sizeof (BITMAP), (LPSTR) &bm) ;
     pt.x = bm.bmWidth ;
     pt.y = bm.bmHeight ;

     BitBlt (hdc, xStart, yStart, pt.x, pt.y, hMemDC, 0, 0, SRCCOPY) ;

     DeleteDC (hMemDC) ;
     }

long FAR PASCAL WndProc (HWND hwnd, WORD message, WORD wParam, LONG lParam)
     {
     static HANDLE hLibrary ;
     static short  nCurrent = 1 ;
     HANDLE        hBitmap ;
     HDC           hdc ;
     PAINTSTRUCT   ps ;

     switch (message)
          {
          case WM_CREATE:
               if ((hLibrary = LoadLibrary ("BITLIB.DLL")) < 32)
                    MessageBeep (0) ;

               return 0 ;

          case WM_CHAR:
               if (hLibrary >= 32)
                    {
                    nCurrent ++ ;
                    InvalidateRect (hwnd, NULL, TRUE) ;
                    }
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               if (hLibrary >= 32)
                    {
                    if (NULL == (hBitmap = LoadBitmap (hLibrary,
                                             MAKEINTRESOURCE (nCurrent))))
                         {
                         nCurrent = 1 ;
                         hBitmap = LoadBitmap (hLibrary,
                                             MAKEINTRESOURCE (nCurrent)) ;
                         }

                    if (hBitmap)
                         DrawBitmap (hdc, 0, 0, hBitmap) ;
                    }

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               if (hLibrary >= 32)
                    FreeLibrary (hLibrary) ;

               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
