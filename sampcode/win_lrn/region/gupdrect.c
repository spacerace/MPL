/*
Date: 7/11/88
Function(s) demonstrated in this program: GetUpdateRect
Compiler version: C 5.1
Description: This program paints the client area red, make the window a icon,
             and then restores the window to it original size. By doing this
             it creates a update region. With the update region, the function
             GetUpdateRect is used to find the smallest rectangle around the 
             update region. Then the program display the coordinates of the 
             update rectangle.
*/

#include <windows.h>
#include <stdio.h>
#include "gupdrect.h"

static char szFuncName [] = "GetUpdateRect";
static char szAppName [] = "GUPDRECT";

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
          wndclass.lpszMenuName  = "GUDR";
          wndclass.lpszClassName = szFuncName;

          if (!RegisterClass (&wndclass))
               return FALSE;
          }

     hMenu = LoadMenu ( hInstance, "GUDR" );


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
   RECT        rectUpdateRect;
   RECT        rectClientRect;
   HBRUSH      hSolidBrush;
   BOOL        bfGotUpdateRect;
   int         nIndex;
   char        szBuffer [100];

   switch(iMessage)
   {
   case WM_INITMENU:
      InvalidateRect ( hWnd, (LPRECT)NULL, TRUE );
      break;
    case WM_COMMAND:
       switch ( wParam )
          {
          case IDM_BOX:
             GetClientRect ( hWnd, (LPRECT)&rectClientRect );
     
             hDC = GetDC ( hWnd );
             hSolidBrush = CreateSolidBrush ( 0xFF0000 );
             FillRect ( hDC, (LPRECT)&rectClientRect, hSolidBrush );
             
             ShowWindow ( hWnd, SW_SHOWMINIMIZED );
             ShowWindow ( hWnd, SW_SHOWNORMAL );

             bfGotUpdateRect = GetUpdateRect ( hWnd, (LPRECT)&rectUpdateRect,
                FALSE );
     
             if ( bfGotUpdateRect == TRUE )
                {
                sprintf ( szBuffer, "%s %d%s %d%s %d%s %d%s",
                   "Update rectangle .top is", rectUpdateRect.top,
                   ",\nupdate rectangle .bottom is", rectUpdateRect.bottom,
                   ",\nupdate rectangle .left is", rectUpdateRect.left,
                   ",\nand update rectangle .right is", rectUpdateRect.right,
                   "." );
                MessageBox ( hWnd, (LPSTR)szBuffer, (LPSTR)szFuncName,
                   MB_OK );
                }
             else
                MessageBox ( hWnd, (LPSTR)"Update rectangle is empty.",
                   (LPSTR)szFuncName, MB_OK );
     
             ReleaseDC ( hWnd, hDC );
             break;
          default :
             return DefWindowProc (hWnd, iMessage, wParam, lParam);
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
