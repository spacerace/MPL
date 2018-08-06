/*
Function(s) demonstrated in this program: SelectObject
Compiler version:5.1
Description: This example creates a object (a pen), selects the object into
             the hDC, and checks the return value.
*/

#include <windows.h>
#include "selobj.h"

static char szAppName [] = "SELOBJ";
static char szFuncName [] = "SelectObject";

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance;
HANDLE      hPrevInstance;
LPSTR       lpszCmdLine;
int         nCmdShow;
{

   HWND        hWnd;
   WNDCLASS    wndclass;
   MSG         msg;
   HMENU       hMenu;

   if (!hPrevInstance)
      {
      wndclass.style         = CS_HREDRAW | CS_VREDRAW;
      wndclass.lpfnWndProc   = WndProc;
      wndclass.cbClsExtra    = 0;
      wndclass.cbWndExtra    = 0;
      wndclass.hInstance     = hInstance;
      wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
      wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
      wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
      wndclass.lpszMenuName  = (LPSTR)"Select";
      wndclass.lpszClassName = szAppName;

      if (!RegisterClass (&wndclass))
         return FALSE;
      }

   hMenu = LoadMenu ( hInstance, (LPSTR)"Select" );

   hWnd = CreateWindow (szAppName,            /* window class name       */
                  szFuncName,                 /* window caption          */
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
   HANDLE   hObject;
   HDC      hDC;
   HPEN     hPen;

   switch(iMessage)
   {
   case WM_COMMAND:
      switch ( wParam )
         {
         case IDM_SELECT:
            hDC = GetDC ( hWnd );
            hPen = CreatePen ( 0, 2, (DWORD)0xFF00FF );
            if ( !hPen )
               {
               MessageBox ( hWnd, (LPSTR)"Pen was not created",
                  (LPSTR)szFuncName, MB_OK );
               return (0L);
               }
            
            hObject = SelectObject ( hDC, hPen );
            if ( hObject )
               MessageBox ( hWnd, (LPSTR)"The object (a pen) was selected",
                  (LPSTR)szFuncName, MB_OK );
            else
               MessageBox ( hWnd, (LPSTR)"The object (a pen) was not selected",
                  (LPSTR)szFuncName, MB_OK );

            DeleteObject ( hObject );
            ReleaseDC ( hWnd, hDC );
            break;

         default:
            return DefWindowProc (hWnd, iMessage, wParam, lParam);
         }
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;

   default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam);
   }
   return (0L); 
}
