/*---------------------------------------
   PRINT4.C -- Printing with Banding
               (c) Charles Petzold, 1990
  ---------------------------------------*/

#include <windows.h>

HDC  GetPrinterDC (void) ;              // in PRINT.C

typedef BOOL (FAR PASCAL * ABORTPROC) (HDC, short) ;

HANDLE hInst ;
char   szAppName [] = "Print4" ;
char   szCaption [] = "Print Program 4 (Banding)" ;

BOOL   bUserAbort ;
HWND   hDlgPrint ;

BOOL FAR PASCAL PrintDlgProc (HWND hDlg, WORD message, WORD wParam, LONG lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
               SetWindowText (hDlg, szAppName) ;
               EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE,
                                                            MF_GRAYED) ;
               return TRUE ;

          case WM_COMMAND:
               bUserAbort = TRUE ;
               EnableWindow (GetParent (hDlg), TRUE) ;
               DestroyWindow (hDlg) ;
               hDlgPrint = 0 ;
               return TRUE ;
          }
     return FALSE ;
     }          

BOOL FAR PASCAL AbortProc (HDC hdcPrn, short nCode)
     {
     MSG   msg ;

     while (!bUserAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          {
          if (!hDlgPrint || !IsDialogMessage (hDlgPrint, &msg))
               {
               TranslateMessage (&msg) ;
               DispatchMessage (&msg) ;
               }
          }
     return !bUserAbort ;
     }

BOOL PrintMyPage (HWND hwnd)
     {
     static char szSpMsg [] = "Print4: Printing" ; 
     static char szText  [] = "Hello Printer!" ;
     ABORTPROC   lpfnAbortProc ;
     BOOL        bError = FALSE ;
     DWORD       dwExtent ;
     FARPROC     lpfnPrintDlgProc ;
     HDC         hdcPrn ;
     POINT       ptExtent ;
     RECT        rect ;
     short       xPage, yPage ;

     if (NULL == (hdcPrn = GetPrinterDC ()))
          return TRUE ;

     xPage = GetDeviceCaps (hdcPrn, HORZRES) ;
     yPage = GetDeviceCaps (hdcPrn, VERTRES) ;

     EnableWindow (hwnd, FALSE) ;

     bUserAbort = FALSE ;
     lpfnPrintDlgProc = MakeProcInstance (PrintDlgProc, hInst) ;
     hDlgPrint = CreateDialog (hInst, "PrintDlgBox", hwnd, lpfnPrintDlgProc) ;

     lpfnAbortProc = MakeProcInstance (AbortProc, hInst) ;
     Escape (hdcPrn, SETABORTPROC, 0, (LPSTR) lpfnAbortProc, NULL) ;
                                        
     if (Escape (hdcPrn, STARTDOC, sizeof szSpMsg - 1, szSpMsg, NULL) > 0 &&
         Escape (hdcPrn, NEXTBAND, 0, NULL, (LPSTR) &rect) > 0)
          {
          while (!IsRectEmpty (&rect) && !bUserAbort)
               {
               (*lpfnAbortProc) (hdcPrn, 0) ;

               Rectangle (hdcPrn, rect.left, rect.top, rect.right, 
                                                       rect.bottom) ;
               (*lpfnAbortProc) (hdcPrn, 0) ;
          
               MoveTo (hdcPrn, 0, 0) ;
               LineTo (hdcPrn, xPage, yPage) ;

               (*lpfnAbortProc) (hdcPrn, 0) ;

               MoveTo (hdcPrn, xPage, 0) ;
               LineTo (hdcPrn, 0, yPage) ;

               SaveDC (hdcPrn) ;

               SetMapMode (hdcPrn, MM_ISOTROPIC) ;
               SetWindowExt   (hdcPrn, 1000, 1000) ;
               SetViewportExt (hdcPrn, xPage / 2, -yPage / 2) ;
               SetViewportOrg (hdcPrn, xPage / 2,  yPage / 2) ;

               (*lpfnAbortProc) (hdcPrn, 0) ;

               Ellipse (hdcPrn, -500, 500, 500, -500) ;

               (*lpfnAbortProc) (hdcPrn, 0) ;

               dwExtent = GetTextExtent (hdcPrn, szText, sizeof szText - 1) ;
               ptExtent = MAKEPOINT (dwExtent) ;
               TextOut (hdcPrn, -ptExtent.x / 2, ptExtent.y / 2, szText,
                                                  sizeof szText - 1) ;

               RestoreDC (hdcPrn, -1) ;

               (*lpfnAbortProc) (hdcPrn, 0) ;

               if (Escape (hdcPrn, NEXTBAND, 0, NULL, (LPSTR) &rect) < 0)
                    {
                    bError = TRUE ;
                    break ;
                    }
               }
          }
     else
          bError = TRUE ;

     if (!bError)
          {
          if (bUserAbort)
               Escape (hdcPrn, ABORTDOC, 0, NULL, NULL) ;
          else
               Escape (hdcPrn, ENDDOC, 0, NULL, NULL) ;
          }

     if (!bUserAbort)
          {
          EnableWindow (hwnd, TRUE) ;     
          DestroyWindow (hDlgPrint) ;
          }

     FreeProcInstance (lpfnPrintDlgProc) ;
     FreeProcInstance (lpfnAbortProc) ;
     DeleteDC (hdcPrn) ;

     return bError || bUserAbort ;
     }
