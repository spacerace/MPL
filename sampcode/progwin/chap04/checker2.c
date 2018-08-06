/*-------------------------------------------------
   CHECKER2.C -- Mouse Hit-Test Demo Program No. 2
                 (c) Charles Petzold, 1990
  -------------------------------------------------*/

#include <windows.h>
#define DIVISIONS 5

long FAR PASCAL WndProc (HWND, WORD, WORD, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "Checker2" ;
     HWND        hwnd ;
     MSG         msg ;
     WNDCLASS    wndclass ;

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

     hwnd = CreateWindow (szAppName, "Checker2 Mouse Hit-Test Demo",
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
     static BOOL  fState[DIVISIONS][DIVISIONS] ;
     static short cxBlock, cyBlock ;
     HDC          hdc ;
     PAINTSTRUCT  ps ;
     POINT        point ;
     RECT         rect ;
     short        x, y ;

     switch (message)
          {
          case WM_SIZE:
               cxBlock = LOWORD (lParam) / DIVISIONS ;
               cyBlock = HIWORD (lParam) / DIVISIONS ;
               return 0 ;

          case WM_SETFOCUS:
               ShowCursor (TRUE) ;
               return 0 ;

          case WM_KILLFOCUS:
               ShowCursor (FALSE) ;
               return 0 ;

          case WM_KEYDOWN:
               GetCursorPos (&point) ;
               ScreenToClient (hwnd, &point) ;

               x = max (0, min (DIVISIONS - 1, point.x / cxBlock)) ;
               y = max (0, min (DIVISIONS - 1, point.y / cyBlock)) ;
  
               switch (wParam)
                    {
                    case VK_UP:
                         y -- ;
                         break ;

                    case VK_DOWN:
                         y ++ ;
                         break ;

                    case VK_LEFT:
                         x -- ;
                         break ;

                    case VK_RIGHT:
                         x ++ ;
                         break ;

                    case VK_HOME:
                         x = y = 0 ;
                         break ;

                    case VK_END:
                         x = y = DIVISIONS - 1 ;
                         break ;

                    case VK_RETURN:
                    case VK_SPACE:
                         SendMessage (hwnd, WM_LBUTTONDOWN, MK_LBUTTON,
                                   MAKELONG (x * cxBlock, y * cyBlock)) ;
                         break ;
                    }
               x = (x + DIVISIONS) % DIVISIONS ;
               y = (y + DIVISIONS) % DIVISIONS ;

               point.x = x * cxBlock + cxBlock / 2 ;
               point.y = y * cyBlock + cyBlock / 2 ;

               ClientToScreen (hwnd, &point) ;
               SetCursorPos (point.x, point.y) ;
               return 0 ;

          case WM_LBUTTONDOWN:
               x = LOWORD (lParam) / cxBlock ;
               y = HIWORD (lParam) / cyBlock ;

               if (x < DIVISIONS && y < DIVISIONS)
                    {
                    fState[x][y] ^= 1 ;

                    rect.left   = x * cxBlock ;
                    rect.top    = y * cyBlock ;
                    rect.right  = (x + 1) * cxBlock ;
                    rect.bottom = (y + 1) * cyBlock ;

                    InvalidateRect (hwnd, &rect, FALSE) ;
                    }
               else
                    MessageBeep (0) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               for (x = 0 ; x < DIVISIONS ; x++)
                    for (y = 0 ; y < DIVISIONS ; y++)
                         {
                         Rectangle (hdc, x * cxBlock, y * cyBlock,
                                   (x + 1) * cxBlock, (y + 1) * cyBlock) ;

                         if (fState [x][y])
                              {
                              MoveTo (hdc,  x    * cxBlock,  y    * cyBlock) ;
                              LineTo (hdc, (x+1) * cxBlock, (y+1) * cyBlock) ;
                              MoveTo (hdc,  x    * cxBlock, (y+1) * cyBlock) ;
                              LineTo (hdc, (x+1) * cxBlock,  y    * cyBlock) ;
                              }
                         }
               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
