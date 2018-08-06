/*--------------------------------------------------
   PATTDLG.C -- Select GPI Patterns from Dialog Box
  --------------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "pattdlg.h"

typedef struct
     {
     SHORT sPattern ;
     SHORT sColor ;
     BOOL  fBorder ;
     }
     PATTERNSDATA ;

typedef PATTERNSDATA FAR *PPATTERNSDATA ;

MRESULT EXPENTRY ClientWndProc  (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY AboutDlgProc   (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY PatternDlgProc (HWND, USHORT, MPARAM, MPARAM) ;

HAB  hab ;

int main (void)
     {
     static CHAR  szClientClass[] = "PattDlg" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_MENU ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
				     0L, NULL, ID_RESOURCE, &hwndClient) ;

     WinSendMsg (hwndFrame, WM_SETICON,
                 WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                 NULL) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static CHAR         szAppName [] = "PATTDLG" ;
     static CHAR         szKeyName [] = "SETTINGS" ;
     static PATTERNSDATA pdCurrent = { IDD_DENSE1, IDD_BKGRND, TRUE } ;
     static SHORT        cxClient, cyClient ;
     HPS                 hps ;
     POINTL              ptl ;
     SHORT               sDataLength ;

     switch (msg)
          {
          case WM_CREATE:
               sDataLength = sizeof pdCurrent ;

               WinQueryProfileData (hab, szAppName, szKeyName, &pdCurrent,
                                    &sDataLength) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_PATTERNS:
                         if (WinDlgBox (HWND_DESKTOP, hwnd, PatternDlgProc,
                                        NULL, IDD_PATTERNS, &pdCurrent))

                              WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;

                    case IDM_ABOUT:
                         WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc,
                                    NULL, IDD_ABOUT, NULL) ;
                         return 0 ;
                    }
               break ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               GpiSetColor (hps, pdCurrent.sColor -
                                 IDD_BKGRND + CLR_BACKGROUND) ;

               GpiSetPattern (hps, pdCurrent.sPattern -
                                   IDD_DENSE1 + PATSYM_DENSE1) ;

               ptl.x = cxClient / 4 ;
               ptl.y = cyClient / 4 ;
               GpiMove (hps, &ptl) ;

               ptl.x *= 3 ;
               ptl.y *= 3 ;
               GpiBox (hps, pdCurrent.fBorder ? DRO_OUTLINEFILL : DRO_FILL,
                            &ptl, 0L, 0L) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               if (MBID_YES == WinMessageBox (HWND_DESKTOP, hwnd,
                                    "Save current settings?", szAppName, 0,
                                    MB_YESNO | MB_ICONQUESTION))

                    WinWriteProfileData (hab, szAppName, szKeyName, &pdCurrent,
                                         sizeof pdCurrent) ;
               break ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT EXPENTRY AboutDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     switch (msg)
          {
          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                    case DID_CANCEL:
                         WinDismissDlg (hwnd, TRUE) ;
                         return 0 ;
                    }
               break ;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT EXPENTRY PatternDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static PATTERNSDATA  pdLocal ;
     static PPATTERNSDATA ppdCurrent ;

     switch (msg)
          {
          case WM_INITDLG:
               ppdCurrent = MPFROMP (mp2) ;
               pdLocal = *ppdCurrent ;

               WinSendDlgItemMsg (hwnd, pdLocal.sPattern, BM_SETCHECK,
                                  MPFROM2SHORT (TRUE, 0), NULL) ;

               WinSendDlgItemMsg (hwnd, pdLocal.sColor, BM_SETCHECK,
                                  MPFROM2SHORT (TRUE, 0), NULL) ;

               WinSendDlgItemMsg (hwnd, IDD_BORDER, BM_SETCHECK,
                                  MPFROM2SHORT (pdLocal.fBorder, 0), NULL) ;

               WinSetFocus (HWND_DESKTOP,
                            WinWindowFromID (hwnd, pdLocal.sPattern)) ;
               return 1 ;

          case WM_CONTROL:
               if (SHORT1FROMMP (mp1) >= IDD_DENSE1 &&
                   SHORT1FROMMP (mp1) <= IDD_HALFTONE)
                    {
                    WinSendDlgItemMsg (hwnd, pdLocal.sPattern, BM_SETCHECK,
                                       MPFROM2SHORT (FALSE, 0), NULL) ;

                    pdLocal.sPattern = SHORT1FROMMP (mp1) ;

                    WinSendDlgItemMsg (hwnd, pdLocal.sPattern, BM_SETCHECK,
                                       MPFROM2SHORT (TRUE, 0), NULL) ;
                    }

               else if (SHORT1FROMMP (mp1) >= IDD_BKGRND &&
                        SHORT1FROMMP (mp1) <= IDD_PALEGRAY)
                    {
                    WinSendDlgItemMsg (hwnd, pdLocal.sColor, BM_SETCHECK,
                                       MPFROM2SHORT (FALSE, 0), NULL) ;

                    pdLocal.sColor = SHORT1FROMMP (mp1) ;

                    WinSendDlgItemMsg (hwnd, pdLocal.sColor, BM_SETCHECK,
                                             MPFROM2SHORT (TRUE, 0), NULL) ; 
                    }
               return 0 ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                         pdLocal.fBorder = (BOOL) WinSendDlgItemMsg (hwnd,
                                   IDD_BORDER, BM_QUERYCHECK, NULL, NULL) ;

                         *ppdCurrent = pdLocal ;

                         WinDismissDlg (hwnd, TRUE) ;
                         return 0 ;

                    case DID_CANCEL:
                         WinDismissDlg (hwnd, FALSE) ;
                         return 0 ;
                    }
               break ;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }
