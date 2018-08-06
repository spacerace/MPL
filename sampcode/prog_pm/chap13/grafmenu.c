/*------------------------------------
   GRAFMENU.C -- A Menu with Graphics
  ------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "grafmenu.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

CHAR szClientClass[] = "GrafMenu" ;

int main (void)
     {
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_MENU ;
     HAB          hab ;
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

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static MENUITEM miBigHelp = { 0,                       // iPosition
                                   MIS_BITMAP | MIS_HELP,   // afStyle
                                   0,                       // afAttribute
                                   IDM_HELP,                // id
                                   NULL,                    // hwndSubMenu
                                   NULL } ;                 // hItem
     FONTMETRICS     fm ;
     HBITMAP         hbm ;
     HPS             hps ;
     HWND            hwndMenu ;

     switch (msg)
          {
          case WM_CREATE:

                    /*----------------------
                       Load bitmap resource
                      ----------------------*/

               hps = WinGetPS (hwnd) ;
               GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;
               hbm = GpiLoadBitmap (hps, NULL, IDB_BIGHELP,
                                    64 * fm.lAveCharWidth / 3,
                                    64 * fm.lMaxBaselineExt / 8) ;
               WinReleasePS (hps) ;

                    /*-----------------------
                       Attach bitmap to menu
                      -----------------------*/

               miBigHelp.hItem = (ULONG) hbm ;

               hwndMenu = WinWindowFromID (
                               WinQueryWindow (hwnd, QW_PARENT, FALSE),
                               FID_MENU) ;

               WinSendMsg (hwndMenu, MM_SETITEM,
                           MPFROM2SHORT (0, TRUE), MPFROMP (&miBigHelp)) ;
               return 0 ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_NEW:
                    case IDM_OPEN:
                    case IDM_SAVE:
                    case IDM_SAVEAS:
                    case IDM_ABOUT:
                         WinAlarm (HWND_DESKTOP, WA_NOTE) ;
                         return 0 ;
                    }
               break ;

          case WM_HELP:
               WinMessageBox (HWND_DESKTOP, hwnd,
                              "Help not yet implemented",
                              szClientClass, 0, MB_OK | MB_ICONEXCLAMATION) ;
               return 0 ;

          case WM_ERASEBACKGROUND:
               return 1 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
