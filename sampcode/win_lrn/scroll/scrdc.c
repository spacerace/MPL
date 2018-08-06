/*
Function(s) demonstrated in this program: ScrollDC
Compiler version: C 5.1
Description:
*/

#include <windows.h>
#include "scrdc.h"

static char szFuncName [] = "ScrollDC";
static char szAppName [] = "SCRDC";

int PASCAL WinMain ( hInstance, hPrevInstance, lpszCmdLine, nCmdShow )
HANDLE      hInstance, hPrevInstance;
LPSTR       lpszCmdLine;
int         nCmdShow;
{
     HWND        hWnd;
     WNDCLASS    wndclass;
     MSG         msg;
     HMENU       hMenu;

     if (!hPrevInstance) {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW;
          wndclass.lpfnWndProc   = WndProc;
          wndclass.cbClsExtra    = 0;
          wndclass.cbWndExtra    = 0;
          wndclass.hInstance     = hInstance;
          wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
          wndclass.lpszMenuName  = "SCR";
          wndclass.lpszClassName = szFuncName;

          if (!RegisterClass (&wndclass))
               return FALSE;
          }

     hMenu = LoadMenu ( hInstance, "SCR" );


     hWnd = CreateWindow (szFuncName,           /* window class name       */
                    szAppName,                  /* window caption          */
                    WS_OVERLAPPEDWINDOW,        /* window style            */
                    CW_USEDEFAULT,              /* initial x position      */
                    0,                          /* initial y position      */
                    CW_USEDEFAULT,              /* initial x size          */
                    0,                          /* initial y size          */
                    NULL,                       /* parent window handle    */
                    hMenu,                      /* window menu handle      */
                    hInstance,                  /* program instance handle */
                    NULL);                      /* create parameters       */

     ShowWindow (hWnd, nCmdShow);
     UpdateWindow (hWnd);

     while (GetMessage(&msg, NULL, 0, 0))
     {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
     } 
     return (msg.wParam);     
}

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
{
   HDC         hDC;
   PAINTSTRUCT ps;
   RECT        strRectangle;
   RECT        strClipRectangle;
   RECT        strDirtyRectangle;
   HBRUSH      hSolidBrush;
   BOOL        bfScrolled;
   int         nIndex;

   switch(iMessage)
   {
   case WM_INITMENU:
      InvalidateRect ( hWnd, (LPRECT)NULL, TRUE );
      break;
    case WM_COMMAND:
       switch ( wParam )
          {
          case IDM_BOX:
             strRectangle.top = 0;
             strRectangle.bottom = 100;
             strRectangle.left = 0;
             strRectangle.right = 100;
             strClipRectangle.top = 0;
             strClipRectangle.bottom = 100;
             strClipRectangle.left = 0;
             strClipRectangle.right = 250;
     
             hDC = GetDC ( hWnd );
             hSolidBrush = CreateSolidBrush ( 0xFF0000 );
             FillRect ( hDC, (LPRECT)&strRectangle, hSolidBrush );
     
             MessageBox ( hWnd, (LPSTR)"Going to scroll the DC.",
                (LPSTR)szFuncName, MB_OK );
     
             bfScrolled = ScrollDC ( hDC, 110, 0, (LPRECT)&strRectangle,
                (LPRECT)&strClipRectangle, NULL, (LPRECT)&strDirtyRectangle );
     
             if ( bfScrolled == TRUE )
                {
                MessageBox ( hWnd, (LPSTR)"Going to invalidate the box.",
                   (LPSTR)szFuncName, MB_OK );
                InvalidateRect ( hWnd, (LPRECT)&strRectangle, TRUE );
                }
             else
                MessageBox ( hWnd, (LPSTR)"Could not scroll the DC.",
                   (LPSTR)szFuncName, MB_OK | MB_ICONHAND );
     
             ReleaseDC ( hWnd, hDC );
     
             break;
          default :
             break;
          }
       break;
     case WM_DESTROY:
       PostQuitMessage(0);
       break;
     default:
       return DefWindowProc (hWnd, iMessage, wParam, lParam);
     return (0L); 
   }
}
