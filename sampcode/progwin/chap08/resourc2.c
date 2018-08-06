/*-----------------------------------------------------------
   RESOURC2.C -- Icon and Cursor Demonstration Program No. 2
                 (c) Charles Petzold, 1990
  -----------------------------------------------------------*/

#include <windows.h>

long FAR PASCAL WndProc  (HWND, WORD, WORD, LONG) ;

char   szAppName[] = "Resourc2" ;
HANDLE hInst ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HBITMAP  hBitmap ;
     HBRUSH   hBrush ;
     HWND     hwnd ;
     MSG      msg ;
     WNDCLASS wndclass ;

     hBitmap = LoadBitmap (hInstance, szAppName) ;
     hBrush = CreatePatternBrush (hBitmap) ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor       = LoadCursor (hInstance, szAppName) ;
          wndclass.hbrBackground = hBrush ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hInst = hInstance ;

     hwnd = CreateWindow (szAppName, "Icon and Cursor Demo",
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

     DeleteObject (hBrush) ;       // clean-up
     DeleteObject (hBitmap) ;

     return msg.wParam ;
     }

long FAR PASCAL WndProc (HWND hwnd, WORD message, WORD wParam, LONG lParam)
     {
     static HICON hIcon ;
     static short cxIcon, cyIcon, cxClient, cyClient ;
     HDC          hdc ;
     PAINTSTRUCT  ps ;
     RECT         rect ;
     short        x, y ;

     switch (message)
          {
          case WM_CREATE:
               hIcon = LoadIcon (hInst, szAppName) ;
               cxIcon = GetSystemMetrics (SM_CXICON) ;
               cyIcon = GetSystemMetrics (SM_CYICON) ;
               return 0 ;

          case WM_SIZE:
               cxClient = LOWORD (lParam) ;
               cyClient = HIWORD (lParam) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               for (y = cyIcon ; y < cyClient ; y += 2 * cyIcon)
                    for (x = cxIcon ; x < cxClient ; x += 2 * cxIcon)
                         DrawIcon (hdc, x, y, hIcon) ;

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
