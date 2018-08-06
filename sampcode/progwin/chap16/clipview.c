/*-----------------------------------------
   CLIPVIEW.C -- Simple Clipboard Viewer
                 (c) Charles Petzold, 1990
  -----------------------------------------*/

#include <windows.h>

long FAR PASCAL WndProc (HWND, WORD, WORD, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static    char szAppName [] = "ClipView" ;
     HWND      hwnd ;
     MSG       msg ;
     WNDCLASS  wndclass ;

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

     hwnd = CreateWindow (szAppName, "Simple Clipboard Viewer (Text Only)",
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

long FAR PASCAL WndProc (HWND hwnd, WORD message, WORD wParam, LONG lParam)
     {
     static HWND hwndNextViewer ;
     HANDLE      hGMem ;
     HDC         hdc ;
     LPSTR       lpGMem ;
     PAINTSTRUCT ps ;
     RECT        rect ;

     switch (message)
          {
          case WM_CREATE:
               hwndNextViewer = SetClipboardViewer (hwnd) ;
               return 0 ;

          case WM_CHANGECBCHAIN :
               if (wParam == hwndNextViewer)
                    hwndNextViewer = LOWORD (lParam) ;

               else if (hwndNextViewer)
                    SendMessage (hwndNextViewer, message, wParam, lParam) ;

               return 0 ;

          case WM_DRAWCLIPBOARD :
               if (hwndNextViewer)
                    SendMessage (hwndNextViewer, message, wParam, lParam) ;

               InvalidateRect (hwnd, NULL, TRUE) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;
               GetClientRect (hwnd, &rect) ;
               OpenClipboard (hwnd) ;

               if (hGMem = GetClipboardData (CF_TEXT))
                    {
                    lpGMem = GlobalLock (hGMem) ;
                    DrawText (hdc, lpGMem, -1, &rect, DT_EXPANDTABS) ;
                    GlobalUnlock (hGMem) ;
                    }

               CloseClipboard () ;
               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               ChangeClipboardChain (hwnd, hwndNextViewer) ;
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
