/*--------------------------------------------
   PMCAP.C -- Captures PM Screen to .BMP File
  --------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include "pmcap.h"

#define ID_TIMER    1

typedef struct
     {
     BOOL  fMonochrome ;
     BOOL  fHideWindow ;
     BOOL  fAutoDialog ;
     BOOL  fIncludePtr ;
     SHORT sDelay ;
     SHORT sDisplay ;
     }
     SETTINGS ;

MRESULT CALLBACK ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT CALLBACK AboutDlgProc  (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT CALLBACK SaveDlgProc   (HWND, USHORT, MPARAM, MPARAM) ;

CHAR szClientClass [] = "PMCAP" ;
HAB  hab ;

int main (void)
     {
     static SWCNTRL swctl ;
     static ULONG   flFrameFlags = FCF_TITLEBAR   | FCF_SYSMENU |
                                   FCF_SIZEBORDER | FCF_MINMAX  |
                                   FCF_MENU       | FCF_ACCELTABLE |
                                   FCF_SHELLPOSITION;
     HMQ            hmq ;
     HSWITCH        hsw ;
     HWND           hwndFrame, hwndClient ;
     QMSG           qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass,
                                     "PMCAP - Captures PM Screen",
                                     0L, NULL, ID_RESOURCE, &hwndClient) ;

     WinSendMsg (hwndFrame, WM_SETICON,
                 WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                 NULL) ;

     swctl.hwnd = hwndFrame ;
     hsw = WinAddSwitchEntry (&swctl) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinRemoveSwitchEntry (hsw) ;
     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT CALLBACK ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static CHAR     szFilename [81],
                     szFilePaint [81],
                     szAppName [] = "PMCAP",
                     szKeySet  [] = "SETTINGS",
                     szKeyFile [] = "FILENAME" ,
                     szKeyPaint[] = "FILEPAINT" ;
     static HBITMAP  hbm ;
     static HWND     hwndFrame, hwndMenu ;
     static SETTINGS set = { FALSE, FALSE, TRUE, FALSE,
                             IDM_DELAY15, IDM_ACTUAL } ;
     static SHORT    cxScreen, cyScreen, sCountDown ;
     HBITMAP         hbmClip ;
     HPS             hps ;
     RECTL           rcl ;
     SHORT           sSaveResult, sLen ;
     USHORT          usfInfo ;

     switch (msg)
          {
          case WM_CREATE:
               cxScreen = (SHORT) WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);
               cyScreen = (SHORT) WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN);

               WinQueryProfileData   (hab, szAppName, szKeySet, &set, &sLen);
               WinQueryProfileString (hab, szAppName, szKeyFile,
                                      "BITMAP00.BMP", szFilename,
                                      sizeof szFilename - 1) ;
               WinQueryProfileString (hab, szAppName, szKeyPaint,
                                      "PAINT00.MSP", szFilePaint,
                                      sizeof szFilePaint - 1) ;

               SaveColorSettings (TRUE, set.fMonochrome) ;

               hwndFrame = WinQueryWindow (hwnd, QW_PARENT, FALSE) ;
               hwndMenu  = WinWindowFromID (hwndFrame, FID_MENU) ;

               CheckMenuItem (hwndMenu, IDM_MONO,     set.fMonochrome) ;
               CheckMenuItem (hwndMenu, IDM_HIDE,     set.fHideWindow) ;
               CheckMenuItem (hwndMenu, IDM_AUTODLG,  set.fAutoDialog) ;
               CheckMenuItem (hwndMenu, IDM_POINTER,  set.fIncludePtr) ;
               CheckMenuItem (hwndMenu, set.sDelay,   TRUE) ;
               CheckMenuItem (hwndMenu, set.sDisplay, TRUE) ;

               AddItemToSysMenu (hwndFrame) ;
               return 0 ;

          case WM_INITMENU:
               switch (SHORT1FROMMP (mp1))
                    {
                    case IDM_FILE:
                         EnableMenuItem (hwndMenu, IDM_SAVE, hbm != NULL) ;
                         EnableMenuItem (hwndMenu, IDM_SAVEPAINT,
                                         IsBitmapMonoEGA (hbm)) ;
                         return 0 ;

                    case IDM_EDIT:
                         EnableMenuItem (hwndMenu, IDM_COPY, hbm != NULL) ;
                         EnableMenuItem (hwndMenu, IDM_PASTE,
                              WinQueryClipbrdFmtInfo (hab, CF_BITMAP, &usfInfo)
                                   && usfInfo == CFI_HANDLE) ;
                         return 0 ;
                    }
               break ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_ABOUT:
                         WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc,
                                    NULL, IDD_ABOUT, NULL) ;
                         return 0 ;

                    case IDM_SAVE:
                         if (!WinDlgBox (HWND_DESKTOP, hwnd, SaveDlgProc,
                                         NULL, IDD_SAVE, szFilename))
                              return 0 ;

                         if (hbm != NULL)
                              {
                              WinSetPointer (HWND_DESKTOP,
                                   WinQuerySysPointer (HWND_DESKTOP,
                                                       SPTR_WAIT, FALSE)) ;

                              sSaveResult = SaveBitmap (hbm, szFilename) ;

                              WinSetPointer (HWND_DESKTOP,
                                   WinQuerySysPointer (HWND_DESKTOP,
                                                       SPTR_ARROW, FALSE)) ;

                              if (sSaveResult)
                                   ErrorMessage (hwnd, sSaveResult) ;
                              else
                                   IncrementFilename (szFilename) ;
                              }
                         return 0 ;

                    case IDM_SAVEPAINT:
                         if (!WinDlgBox (HWND_DESKTOP, hwnd, SaveDlgProc,
                                         NULL, IDD_SAVEPAINT, szFilePaint))
                              return 0 ;

                         if (IsBitmapMonoEGA (hbm))
                              {
                              WinSetPointer (HWND_DESKTOP,
                                   WinQuerySysPointer (HWND_DESKTOP,
                                                       SPTR_WAIT, FALSE)) ;

                              sSaveResult = SavePaintFormat (hbm, szFilePaint);

                              WinSetPointer (HWND_DESKTOP,
                                   WinQuerySysPointer (HWND_DESKTOP,
                                                       SPTR_ARROW, FALSE)) ;

                              if (sSaveResult)
                                   ErrorMessage (hwnd, sSaveResult) ;
                              else
                                   IncrementFilename (szFilePaint) ;
                              }
                         return 0 ;

                    case IDM_COPY:
                                             // Make copy of stored bitmap

                         hbmClip = CopyBitmap (hbm) ;

                                             // Set clipboard data to copy

                         if (hbmClip != NULL)
                              {
                              WinOpenClipbrd (hab) ;
                              WinEmptyClipbrd (hab) ;
                              WinSetClipbrdData (hab, (ULONG) hbmClip,
                                                 CF_BITMAP, CFI_HANDLE) ;
                              WinCloseClipbrd (hab) ;
                              }
                         else
                              ErrorMessage (hwnd, IDS_BMPCREATE) ;
                         return 0 ;

                    case IDM_PASTE:
                                              // Get bitmap from clipboard

                         WinOpenClipbrd (hab) ;
                         hbmClip = WinQueryClipbrdData (hab, CF_BITMAP) ;

                         if (hbmClip != NULL)
                              {
                              if (hbm != NULL)
                                   GpiDeleteBitmap (hbm) ;

                                             // Make copy of it

                              hbm = CopyBitmap (hbmClip) ;

                              if (hbm == NULL)
                                   ErrorMessage (hwnd, IDS_BMPCREATE) ;
                              }
                         WinCloseClipbrd (hab) ;
                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;

                    case IDM_BEGIN:
                         if (WinStartTimer (hab, hwnd, ID_TIMER, 1000))
                              {
                              EnableMenuItem (hwndMenu, IDM_BEGIN, FALSE) ;
                              EnableMenuItem (hwndMenu, IDM_ABORT, TRUE) ;
                              sCountDown = set.sDelay - IDM_DELAY ;

                              if (set.fHideWindow)
                                   WinShowWindow (hwndFrame, FALSE) ;
                              }
                         else
                              ErrorMessage (hwnd, IDS_TIMER) ;

                         return 0 ;

                    case IDM_ABORT:
                         EnableMenuItem (hwndMenu, IDM_BEGIN, TRUE) ;
                         EnableMenuItem (hwndMenu, IDM_ABORT, FALSE) ;

                         WinStopTimer (hab, hwnd, ID_TIMER) ;
                         return 0 ;

                    case IDM_MONO:
                         CheckMenuItem (hwndMenu, IDM_MONO,
                                        set.fMonochrome = !set.fMonochrome) ;
                         
                         SaveColorSettings (FALSE, set.fMonochrome) ;
                         return 0 ;

                    case IDM_HIDE:
                         CheckMenuItem (hwndMenu, IDM_HIDE,
                                        set.fHideWindow = !set.fHideWindow) ;
                         return 0 ;

                    case IDM_AUTODLG:
                         CheckMenuItem (hwndMenu, IDM_AUTODLG,
                                        set.fAutoDialog = !set.fAutoDialog) ;
                         return 0 ;

                    case IDM_POINTER:
                         CheckMenuItem (hwndMenu, IDM_POINTER,
                                        set.fIncludePtr = !set.fIncludePtr) ;
                         return 0 ;

                    case IDM_DELAY5:
                    case IDM_DELAY10:
                    case IDM_DELAY15:
                    case IDM_DELAY30:
                    case IDM_DELAY60:
                         CheckMenuItem (hwndMenu, set.sDelay, FALSE) ;
                         set.sDelay = COMMANDMSG(&msg)->cmd ;
                         CheckMenuItem (hwndMenu, set.sDelay, TRUE) ;
                         return 0 ;

                    case IDM_ACTUAL:
                    case IDM_STRETCH:
                         CheckMenuItem (hwndMenu, set.sDisplay, FALSE) ;
                         set.sDisplay = COMMANDMSG(&msg)->cmd ;
                         CheckMenuItem (hwndMenu, set.sDisplay, TRUE) ;

                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;
                    }

          case WM_TIMER:
               switch (SHORT1FROMMP (mp1))
                    {
                    case ID_TIMER:
                         if (--sCountDown)
                              {
                              DosBeep (1024, 100) ;
                              return 0 ;
                              }

                         DosBeep (1024, 1000) ;
                         WinStopTimer (hab, hwnd, ID_TIMER) ;
                         EnableMenuItem (hwndMenu, IDM_BEGIN, TRUE) ;
                         EnableMenuItem (hwndMenu, IDM_ABORT, FALSE) ;

                                             // Delete old bitmap
                         if (hbm != NULL)
                              GpiDeleteBitmap (hbm) ;

                                             // Copy screen to bitmap

                         hbm = ScreenToBitmap (cxScreen, cyScreen,
                                               set.fIncludePtr,
                                               set.fMonochrome) ;

                         if (set.fHideWindow)
                              WinShowWindow (hwndFrame, TRUE) ;

                         if (hbm == NULL)
                              ErrorMessage (hwnd, IDS_BMPCREATE) ;

                         else if (set.fAutoDialog)
                              WinPostMsg (hwnd, WM_COMMAND,
                                   MPFROMSHORT (
                                        set.fMonochrome ? IDM_SAVEPAINT
                                                        : IDM_SAVE),
                                        NULL) ;

                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         WinUpdateWindow (hwnd) ;
                         return 0 ;
                    }
               break ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               if (hbm != NULL)
                    {
                    WinQueryWindowRect (hwnd, &rcl) ;

                    WinDrawBitmap (hps, hbm, NULL, (PPOINTL) &rcl,
                                   CLR_NEUTRAL, CLR_BACKGROUND,
                                   set.sDisplay == IDM_STRETCH ?
                                        DBM_STRETCH : DBM_NORMAL) ;
                    }
               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               if (hbm != NULL)
                    GpiDeleteBitmap (hbm) ;

               WinWriteProfileString (hab, szAppName, szKeyFile, szFilename) ;
               WinWriteProfileString (hab, szAppName, szKeyPaint,szFilePaint);
               WinWriteProfileData   (hab, szAppName, szKeySet,
                                      &set, sizeof set) ;

               if (set.fMonochrome)
                    SaveColorSettings (FALSE, FALSE) ;

               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT CALLBACK AboutDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
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

MRESULT CALLBACK SaveDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static PCHAR szFilename ;
     FILE         *file ;

     switch (msg)
          {
          case WM_INITDLG:
               szFilename = PVOIDFROMMP (mp2) ;

               WinSendDlgItemMsg (hwnd, IDD_FILENAME, EM_SETTEXTLIMIT,
                                  MPFROMSHORT (80), NULL) ;

               WinSetDlgItemText (hwnd, IDD_FILENAME, szFilename) ;
               break ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                         WinQueryDlgItemText (hwnd, IDD_FILENAME,
                                              80, szFilename) ;

                                   // Test if file exists

                         if (file = fopen (szFilename, "r"))
                              {
                              fclose (file) ;

                              if (MBID_NO == WinMessageBox (HWND_DESKTOP, hwnd,
                                                  "File exists.  Replace it?",
                                                  szClientClass, 0,
                                                  MB_YESNO | MB_ICONQUESTION |
                                                  MB_DEFBUTTON2 | MB_MOVEABLE))
                                   return 0 ;
                              }

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
