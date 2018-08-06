/*---------------------------------------------------------
   POPPADP0.C -- Popup Notepad Printing -- dummy functions
  ---------------------------------------------------------*/

#include <windows.h>

extern char szAppName [] ;              // in POPPAD.C

BOOL FAR PASCAL PrintDlgProc (HWND hDlg, WORD message,
                              WORD wParam, LONG lParam)
     {
     return FALSE ;
     }

BOOL FAR PASCAL AbortProc (HDC hPrinterDC, short nCode)
     {
     return FALSE ;
     }

BOOL PrintFile (HANDLE hInstance, HWND hwnd, HWND hwndEdit, char *szFileName)
     {
     MessageBox (hwnd, "Printing not yet implemented", szAppName,
                       MB_OK | MB_ICONEXCLAMATION) ;
     return FALSE ;
     }
