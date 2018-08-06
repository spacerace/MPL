/*------------------------------------------
   ABOUT1.C -- About Box Demo Program No. 1
	       (c) Charles Petzold, 1990
  ------------------------------------------*/

#include <windows.h>
#include "about1.h"

long FAR PASCAL WndProc (HWND, WORD, WORD, LONG);

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
		    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName [] = "About1" ;
     MSG         msg;
     HWND        hwnd ;
     WNDCLASS    wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "About Box Demo Program",
                          WS_OVERLAPPEDWINDOW,
			  CW_USEDEFAULT, CW_USEDEFAULT,
			  CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd); 

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

BOOL FAR PASCAL AboutDlgProc (HWND hDlg, WORD message, WORD wParam, LONG lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
               return TRUE ;

          case WM_COMMAND:
               switch (wParam)
                    {
		    case IDOK:
		    case IDCANCEL:
                         EndDialog (hDlg, 0) ;
			 return TRUE ;
                    }
               break ;
          }
     return FALSE ;
     }

long FAR PASCAL WndProc (HWND hwnd, WORD message, WORD wParam, LONG lParam)
     {
     static FARPROC lpfnAboutDlgProc ;
     static HANDLE  hInstance ;

     switch (message)
          {
          case WM_CREATE:
               hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;

               lpfnAboutDlgProc = MakeProcInstance (AboutDlgProc, hInstance) ;
               return 0 ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDM_ABOUT:
                         DialogBox (hInstance, "AboutBox", hwnd,
                                        lpfnAboutDlgProc) ;
                         return 0 ;
                    }
               break ;
     
          case WM_DESTROY :
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
