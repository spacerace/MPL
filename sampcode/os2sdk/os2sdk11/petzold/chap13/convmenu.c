/*-------------------------------------
   CONVMENU.C -- Conventional Menu Use
  -------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "convmenu.h"

#define ID_TIMER    1

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

CHAR szClientClass[] = "ConvMenu" ;
HAB  hab ;

int main (void)
     {
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_MENU          | FCF_ACCELTABLE ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, 0L, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
                                     0L, NULL, ID_RESOURCE, &hwndClient) ;

     WinSendMsg (hwndFrame, WM_SETICON,
                 WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                 NULL) ;

     while (TRUE)
          {
          while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
               WinDispatchMsg (hab, &qmsg) ;

          if (MBID_OK == WinMessageBox (HWND_DESKTOP, hwndClient,
                                        "Really want to end program?",
                                        szClientClass, 0,
                                        MB_OKCANCEL | MB_ICONQUESTION))
               break ;

          WinCancelShutdown (hmq, FALSE) ;
          }

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOOL  fTimerGoing = FALSE ;
     static COLOR colBackground [] = {
                                     0xFFFFFFL, 0xC0C0C0L, 0x808080L,
                                     0x404040L, 0x000000L
                                     } ;
     static HWND  hwndMenu ;
     static SHORT sCurrentBackground = IDM_WHITE ;
     HPS          hps ;
     RECTL        rcl ;

     switch (msg)
          {
          case WM_CREATE:
               hwndMenu = WinWindowFromID (
                              WinQueryWindow (hwnd, QW_PARENT, FALSE),
                              FID_MENU) ;
               return 0 ;

          case WM_INITMENU:
               switch (SHORT1FROMMP (mp1))
                    {
                    case IDM_TIMER:
                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                   MPFROM2SHORT (IDM_START, TRUE),
                                   MPFROM2SHORT (MIA_DISABLED, 
                                             !fTimerGoing &&
                              WinQuerySysValue (HWND_DESKTOP, SV_CTIMERS) ?
                                             0 : MIA_DISABLED)) ;

                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                   MPFROM2SHORT (IDM_STOP, TRUE),
                                   MPFROM2SHORT (MIA_DISABLED,
                                        fTimerGoing ? 0 : MIA_DISABLED)) ;
                         return 0 ;
                    }
               break ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_NEW:
                         WinMessageBox (HWND_DESKTOP, hwnd,
                                   "Bogus \"New\" Dialog",
                                   szClientClass, 0, MB_OK | MB_ICONASTERISK) ;
                         return 0 ;

                    case IDM_OPEN:
                         WinMessageBox (HWND_DESKTOP, hwnd,
                                   "Bogus \"Open\" Dialog",
                                   szClientClass, 0, MB_OK | MB_ICONASTERISK) ;
                         return 0 ;

                    case IDM_SAVE:
                         WinMessageBox (HWND_DESKTOP, hwnd,
                                   "Bogus \"Save\" Dialog",
                                   szClientClass, 0, MB_OK | MB_ICONASTERISK) ;
                         return 0 ;

                    case IDM_SAVEAS:
                         WinMessageBox (HWND_DESKTOP, hwnd,
                                   "Bogus \"Save As\" Dialog",
                                   szClientClass, 0, MB_OK | MB_ICONASTERISK) ;
                         return 0 ;

                    case IDM_EXIT:
                         WinSendMsg (hwnd, WM_CLOSE, 0L, 0L) ;
                         return 0 ;

                    case IDM_ABOUT:
                         WinMessageBox (HWND_DESKTOP, hwnd,
                                   "Bogus \"About\" Dialog",
                                   szClientClass, 0, MB_OK | MB_ICONASTERISK) ;
                         return 0 ;

                    case IDM_START:
                         if (WinStartTimer (hab, hwnd, ID_TIMER, 1000))
                              fTimerGoing = TRUE ;
                         else
                              WinMessageBox (HWND_DESKTOP, hwnd,
                                   "Too many clocks or timers",
                                   szClientClass, 0,
                                   MB_OK | MB_ICONEXCLAMATION) ;
                         return 0 ;

                    case IDM_STOP:
                         WinStopTimer (hab, hwnd, ID_TIMER) ;
                         fTimerGoing = FALSE ;
                         return 0 ;

                    case IDM_WHITE:
                    case IDM_LTGRAY:
                    case IDM_GRAY:
                    case IDM_DKGRAY:
                    case IDM_BLACK:
                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (sCurrentBackground, TRUE),
                                     MPFROM2SHORT (MIA_CHECKED, 0)) ;

                         sCurrentBackground = COMMANDMSG(&msg)->cmd ;

                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (sCurrentBackground, TRUE),
                                     MPFROM2SHORT (MIA_CHECKED, MIA_CHECKED)) ;

                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;
                    }
               break ;

          case WM_HELP:
               WinMessageBox (HWND_DESKTOP, hwnd,
                              "Help not yet implemented",
                              szClientClass, 0, MB_OK | MB_ICONEXCLAMATION) ;
               return 0 ;

          case WM_TIMER:
               WinAlarm (HWND_DESKTOP, WA_NOTE) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiSavePS (hps) ;

               GpiCreateLogColorTable (hps, 0L, LCOLF_RGB, 0L, 0L, NULL) ;

               WinQueryWindowRect (hwnd, &rcl) ;

               WinFillRect (hps, &rcl,
                            colBackground [sCurrentBackground - IDM_WHITE]) ;

               GpiRestorePS (hps, -1L) ;
               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               if (fTimerGoing)
                    {
                    WinStopTimer (hab, hwnd, ID_TIMER) ;
                    fTimerGoing = FALSE ;
                    }
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
