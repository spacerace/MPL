/*
Microsoft Systems Journal
Volume 3; Issue 1; January, 1988

Code Listings For:

	Hexcalc
	pp. 39-48

Author(s): Charles Petzold
Title:     HEXCALC: An Instructive Pop-Up Calculator for Microsoft Windows



Figure 6w
=========

*/

/*--------------------------------------------------------
   HEXCALCW.C -- Hexadecimal Calculator (Windows version)
  --------------------------------------------------------*/

#include <windows.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

long FAR PASCAL WndProc (HWND, unsigned, WORD, LONG) ;

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE      hInstance, hPrevInstance;
     LPSTR       lpszCmdLine;
     int         nCmdShow;
     {
     static char szAppName [] = "HexCalcW" ;
     HWND        hWnd ;
     MSG         msg;
     WNDCLASS    wndclass ;

     if (!hPrevInstance)
          {
          wndclass.style          = CS_HREDRAW | CS_VREDRAW;
          wndclass.lpfnWndProc    = WndProc ;
          wndclass.cbClsExtra     = 0 ;
          wndclass.cbWndExtra     = 0 ;
          wndclass.hInstance      = hInstance ;
          wndclass.hIcon          = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor        = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground  = COLOR_WINDOW + 1 ;
          wndclass.lpszMenuName   = NULL ;
          wndclass.lpszClassName  = szAppName ;

          if (!RegisterClass (&wndclass))
               return FALSE ;
          }

     hWnd = CreateDialog (hInstance, szAppName, 0, NULL) ;

     ShowWindow (hWnd, nCmdShow) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

void ShowNumber (hWnd, dwNumber)
     HWND  hWnd ;
     DWORD dwNumber ;
     {
     char  szBuffer [20] ;

     SetDlgItemText (hWnd, VK_ESCAPE,
                         strupr (ltoa (dwNumber, szBuffer, 16))) ;
     }

DWORD CalcIt (dwFirstNum, nOperation, dwNum)
     DWORD dwFirstNum, dwNum ;
     short nOperation ;
     {
     switch (nOperation)
          {
          case '=' : return dwNum ;
          case '+' : return dwFirstNum +  dwNum ;
          case '-' : return dwFirstNum -  dwNum ;
          case '*' : return dwFirstNum *  dwNum ;
          case '&' : return dwFirstNum &  dwNum ;
          case '|' : return dwFirstNum |  dwNum ;
          case '^' : return dwFirstNum ^  dwNum ;
          case '<' : return dwFirstNum << dwNum ;
          case '>' : return dwFirstNum >> dwNum ;
          case '/' : return dwNum ? dwFirstNum / dwNum : ULONG_MAX ;
          case '%' : return dwNum ? dwFirstNum % dwNum : ULONG_MAX ;
          default  : return 0L ;
          }
     }

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
     HWND         hWnd;
     unsigned     iMessage;
     WORD         wParam;
     LONG         lParam;
     {
     static BOOL  bNewNumber = TRUE ;
     static DWORD dwNumber, dwFirstNum ;
     static short nOperation = '=' ;
     HWND         hButton ;

     switch (iMessage)
          {
          case WM_KEYDOWN:              /* left arrow --> backspace */
               if (wParam != VK_LEFT)
                    break ;
               wParam = VK_BACK ;
                                             /* fall through */
          case WM_CHAR:
               if ((wParam = toupper (wParam)) == VK_RETURN)
                    wParam = '=' ;

               if (hButton = GetDlgItem (hWnd, wParam))
                    {
                    SendMessage (hButton, BM_SETSTATE, 1, 0L) ;
                    SendMessage (hButton, BM_SETSTATE, 0, 0L) ;
                    }
               else
                    {
                    MessageBeep (0) ;
                    break ;
                    }
                                             /* fall through */
          case WM_COMMAND:
               SetFocus (hWnd) ;

               if (wParam == VK_BACK)                  /* backspace */
                    ShowNumber (hWnd, dwNumber /= 16) ;

               else if (wParam == VK_ESCAPE)           /* escape */
                    ShowNumber (hWnd, dwNumber = 0L) ;

               else if (isxdigit (wParam))             /* hex digit */
                    {
                    if (bNewNumber)
                         {
                         dwFirstNum = dwNumber ;
                         dwNumber = 0L ;
                         }
                    bNewNumber = FALSE ;

                    if (dwNumber <= ULONG_MAX >> 4)
                         ShowNumber (hWnd, dwNumber =
                              16 * dwNumber + wParam -
                              (isdigit (wParam) ? '0' : 'A' - 10)) ;
                    else
                         MessageBeep (0) ;
                    }
               else                                    /* operation */
                    {
                    if (!bNewNumber)
                         ShowNumber (hWnd, dwNumber =
                              CalcIt (dwFirstNum, nOperation,
                                      dwNumber)) ;
                    bNewNumber = TRUE ;
                    nOperation = wParam ;
                    }
               break ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               break ;

          default :
               return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
          }
     return 0L ;
     }
