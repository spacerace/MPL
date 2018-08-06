/*----------------------------------------------
   POPPADP.C -- Popup Editor Printing Functions
  ----------------------------------------------*/

#include <windows.h>
#include <string.h>
#include "filedlg.h"                    // for IDD_FNAME definition

extern char szAppName [] ;              // in POPPAD.C

BOOL bUserAbort ;
HWND hDlgPrint ;

BOOL FAR PASCAL PrintDlgProc (HWND hDlg, WORD message, WORD wParam, LONG lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
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

BOOL FAR PASCAL AbortProc (HDC hPrinterDC, short nCode)
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

BOOL PrintFile (HANDLE hInst, HWND hwnd, HWND hwndEdit, char *szFileName)
     {
     BOOL       bError = FALSE ;
     char       szMsg [40] ;
     FARPROC    lpfnAbortProc, lpfnPrintDlgProc ;
     HDC        hdcPrn ;
     NPSTR      psBuffer ;
     RECT       rect ;
     short      yChar, nCharsPerLine, nLinesPerPage,
                nTotalLines, nTotalPages, nPage, nLine, nLineNum = 0 ;
     TEXTMETRIC tm ;

     if (0 == (nTotalLines = (short) SendMessage (hwndEdit,
                                             EM_GETLINECOUNT, 0, 0L)))
          return FALSE ;

     if (NULL == (hdcPrn = GetPrinterDC ()))
          return TRUE ;

     GetTextMetrics (hdcPrn, &tm) ;
     yChar = tm.tmHeight + tm.tmExternalLeading ;

     nCharsPerLine = GetDeviceCaps (hdcPrn, HORZRES) / tm.tmAveCharWidth ;
     nLinesPerPage = GetDeviceCaps (hdcPrn, VERTRES) / yChar ;
     nTotalPages   = (nTotalLines + nLinesPerPage - 1) / nLinesPerPage ;

     psBuffer = (NPSTR) LocalAlloc (LPTR, nCharsPerLine) ;

     EnableWindow (hwnd, FALSE) ;

     bUserAbort = FALSE ;
     lpfnPrintDlgProc = MakeProcInstance (PrintDlgProc, hInst) ;
     hDlgPrint = CreateDialog (hInst, "PrintDlgBox", hwnd, lpfnPrintDlgProc) ;
     SetDlgItemText (hDlgPrint, IDD_FNAME, szFileName) ;

     lpfnAbortProc = MakeProcInstance (AbortProc, hInst) ;
     Escape (hdcPrn, SETABORTPROC, 0, (LPSTR) lpfnAbortProc, NULL) ;

     strcat (strcat (strcpy (szMsg, szAppName), " - "), szFileName) ;
                                        
     if (Escape (hdcPrn, STARTDOC, strlen (szMsg), szMsg, NULL) > 0)
          {
          for (nPage = 0 ; nPage < nTotalPages ; nPage++)
               {
               for (nLine = 0 ; nLine < nLinesPerPage &&
                                nLineNum < nTotalLines ; nLine++, nLineNum++)
                    {
                    *(short *) psBuffer = nCharsPerLine ;

                    TextOut (hdcPrn, 0, yChar * nLine, psBuffer,
                         (short) SendMessage (hwndEdit, EM_GETLINE,
                                        nLineNum, (LONG) (LPSTR) psBuffer)) ;
                    }

               if (Escape (hdcPrn, NEWFRAME, 0, NULL, (LPSTR) &rect) < 0)
                    {
                    bError = TRUE ;
                    break ;
                    }

               if (bUserAbort)
                    break ;
               }
          }
     else
          bError = TRUE ;

     if (!bError)
          Escape (hdcPrn, ENDDOC, 0, NULL, NULL) ;

     if (!bUserAbort)
          {
          EnableWindow (hwnd, TRUE) ;
          DestroyWindow (hDlgPrint) ;
          }

     if (bError || bUserAbort)
          {
          strcat (strcpy (szMsg, "Could not print: "), szFileName) ;
          MessageBox (hwnd, szMsg, szAppName, MB_OK | MB_ICONEXCLAMATION) ;
          }

     LocalFree ((LOCALHANDLE) psBuffer) ;
     FreeProcInstance (lpfnPrintDlgProc) ;
     FreeProcInstance (lpfnAbortProc) ;
     DeleteDC (hdcPrn) ;

     return bError || bUserAbort ;
     }
