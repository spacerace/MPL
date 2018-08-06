/*-------------------------------------------------------------------
   PRINT.C -- Common Routines for Print1, Print2, Print3, and Print4
  -------------------------------------------------------------------*/

#include <windows.h>
#include <string.h>

long FAR PASCAL WndProc (HWND, WORD, WORD, LONG) ;
BOOL PrintMyPage (HWND) ;

extern HANDLE hInst ;
extern char szAppName [] ;
extern char szCaption [] ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HWND     hwnd ;
     MSG      msg ;
     WNDCLASS wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hInst = hInstance ;

     hwnd = CreateWindow (szAppName, szCaption,
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
     HMENU hMenu ;

     switch (message)
          {
          case WM_CREATE:
               hMenu = GetSystemMenu (hwnd, FALSE);
               AppendMenu (hMenu, MF_SEPARATOR, 0, NULL) ;
               AppendMenu (hMenu, 0, 1, "&Print") ;
               return 0 ;

          case WM_SYSCOMMAND:
               if (wParam == 1)
                    {
                    if (PrintMyPage (hwnd))
                         MessageBox (hwnd, "Could not print page",
                              szAppName, MB_OK | MB_ICONEXCLAMATION) ;
                    return 0 ;
                    }
               break ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }

HDC GetPrinterDC (void)
     {
     static char szPrinter [80] ;
     char        *szDevice, *szDriver, *szOutput ;

     GetProfileString ("windows", "device", ",,,", szPrinter, 80) ;

     if ((szDevice = strtok (szPrinter, "," )) &&
         (szDriver = strtok (NULL,      ", ")) && 
         (szOutput = strtok (NULL,      ", ")))
          
               return CreateDC (szDriver, szDevice, szOutput, NULL) ;

     return 0 ;
     }

void PageGDICalls (HDC hdcPrn, short cxPage, short cyPage)
     {
     static char szTextStr [] = "Hello Printer!" ;
     DWORD       dwExtent ;

     Rectangle (hdcPrn, 0, 0, cxPage, cyPage) ;

     MoveTo (hdcPrn, 0, 0) ;
     LineTo (hdcPrn, cxPage, cyPage) ;
     MoveTo (hdcPrn, cxPage, 0) ;
     LineTo (hdcPrn, 0, cyPage) ;

     SaveDC (hdcPrn) ;

     SetMapMode (hdcPrn, MM_ISOTROPIC) ;
     SetWindowExt   (hdcPrn, 1000, 1000) ;
     SetViewportExt (hdcPrn, cxPage / 2, -cyPage / 2) ;
     SetViewportOrg (hdcPrn, cxPage / 2,  cyPage / 2) ;

     Ellipse (hdcPrn, -500, 500, 500, -500) ;

     dwExtent = GetTextExtent (hdcPrn, szTextStr, sizeof szTextStr - 1) ;
     TextOut (hdcPrn, LOWORD (dwExtent) / 2, HIWORD (dwExtent) / 2,
              szTextStr, sizeof szTextStr - 1) ;

     RestoreDC (hdcPrn, -1) ;
     }
