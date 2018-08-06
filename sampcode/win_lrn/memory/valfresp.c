/*
Name               : valfresp.c
Function(s)
demonstrated in
this program       : ValidateFreeSpaces()
Windows version    : 2.03
Compiler version   : 5.1
Description        : ValidateFreeSpaces() checks free memory for valid
                     contents, returning the address of the first invalid
                     byte or in the event of no invalid bytes, NULL
*/

#include <windows.h>
#include "valfresp.h"

long FAR PASCAL WndProc (HWND, unsigned, WORD, LONG) ;
int sprintf(PSTR, PSTR, int);

static int ErrorCheck;
static char szResName [] = "ResMenu";

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE      hInstance, hPrevInstance ;
     LPSTR       lpszCmdLine ;
     int         nCmdShow ;
     {
     static char szAppName [] = "valfresp" ;
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
                    "ValidateFreeSpaces",       /* window caption          */
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
 LPSTR BoneBad;
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
     BoneBad = ValidateFreeSpaces();
     if (BoneBad!=NULL)
     {
      sprintf(szBuf, "address %iH contains an invalid byte",(unsigned)BoneBad);
      MessageBox(GetFocus(), szBuf, "ABORTING!", MB_OK);
     } else
     {
      MessageBox(GetFocus(), "No invalid bytes encountered", "FINISHED", MB_OK);
     }
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
