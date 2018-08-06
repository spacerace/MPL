/*
Name               : swaparea.c
Date               : 07-05-88
Function(s)
demonstrated in
this program       : SetSwapAreaSize(), GlobalCompact().
Windows version    : 2.03
Compiler version   : 5.1
Description        : SetSwapAreaSize() increases the amount of memory that
                     an application uses for its code segment.  The memory
                     used for this application may not be allocated for
                     data.
*/

#include <windows.h>
#include "swaparea.h"

long FAR PASCAL WndProc (HWND, unsigned, WORD, LONG) ;
int sprintf(PSTR, PSTR, int);

static int ErrorCheck;
static char szResName [] = "ResMenu";

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE      hInstance, hPrevInstance ;
     LPSTR       lpszCmdLine ;
     int         nCmdShow ;
     {
     static char szAppName [] = "swaparea" ;
     HWND        hWnd ;
     WNDCLASS    wndclass ;
     MSG msg;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          if (!RegisterClass (&wndclass))
               return FALSE ;
          }

     hWnd = CreateWindow (szAppName,            /* window class name       */
                    "The Joy of Swapping",       /* window caption          */
                    WS_OVERLAPPEDWINDOW,        /* window style            */
                    CW_USEDEFAULT,              /* initial x position      */
                    0,                          /* initial y position      */
                    CW_USEDEFAULT,              /* initial x size          */
                    0,                          /* initial y size          */
                    NULL,                       /* parent window handle    */
                    NULL,                       /* window menu handle      */
                    hInstance,                  /* program instance handle */
                    NULL) ;                     /* create parameters       */

     ShowWindow (hWnd, nCmdShow) ;

     UpdateWindow (hWnd) ;

     while (GetMessage(&msg, NULL, 0, 0))
     {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
     } 
     return (msg.wParam) ;     
     }

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned iMessage ;
WORD     wParam ;
LONG     lParam ;
{
 static HANDLE hInstance;
 DWORD TotalFree;
 PAINTSTRUCT ps;
 char szBuf[50];
 HMENU hMenu;
 switch(iMessage)
 {
  case WM_CREATE:
  {
   hInstance = GetWindowWord(hWnd, GWW_HINSTANCE);
   hMenu = LoadMenu(hInstance, "ResMenu");
   SetMenu(hWnd, hMenu);
   DrawMenuBar(hWnd);
   break;
  }
  case WM_PAINT:
  {
   BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
   EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
   break;
  }
  case WM_COMMAND:
  {
   switch(wParam)
   {
    case IDM_EXECUTE:
    {
     /* Compact the memory to maximize the amount of free space, then
        allocate 1/2 the total available for this applications code
        segment.  This can only be done once.
     */
     TotalFree = (GlobalCompact(NULL)/(16*1024))/2;    
     ErrorCheck = SetSwapAreaSize((WORD)TotalFree);
     sprintf(szBuf, "%i paragraphs that have been obtained for code segment use.", ErrorCheck);
     MessageBox(GetFocus(), szBuf, "There are", MB_OK);
    }
   }
   break;
  }
  case WM_DESTROY:
  {
   PostQuitMessage(0);
   break;
  }
  default:
  {
   return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
  }
 }
 return (0L); 
}
