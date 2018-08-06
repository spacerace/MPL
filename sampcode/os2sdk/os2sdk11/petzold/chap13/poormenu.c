/*----------------------------------
   POORMENU.C -- Poor Person's Menu
  ----------------------------------*/

#define INCL_WIN
#include <os2.h>

#define IDM_ABOUT   10
#define IDM_HELP    11

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

CHAR szCaption [] = "Poor Person\'s Menu" ;

int main (void)
     {
     static CHAR  szClientClass[] = "PoorMenu" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
     HAB          hab ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, 0L, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
                                     0L, NULL, 0, &hwndClient) ;

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
     static CHAR     *szMenuText [3] = { NULL,
                                         "A~bout PoorMenu...",
                                         "~Help..." } ;
     static MENUITEM mi [3] = {
                              MIT_END, MIS_SEPARATOR, 0, 0,         NULL, NULL,
                              MIT_END, MIS_TEXT,      0, IDM_ABOUT, NULL, NULL,
                              MIT_END, MIS_TEXT,      0, IDM_HELP,  NULL, NULL
                              } ;
     HWND            hwndSysMenu, hwndSysSubMenu ;
     MENUITEM        miSysMenu ;
     SHORT           sItem, idSysMenu ;

     switch (msg)
          {
          case WM_CREATE:
               hwndSysMenu = WinWindowFromID (
                                  WinQueryWindow (hwnd, QW_PARENT, FALSE),
                                  FID_SYSMENU) ;

               idSysMenu = SHORT1FROMMR (WinSendMsg (hwndSysMenu,
                                                     MM_ITEMIDFROMPOSITION,
                                                     NULL, NULL)) ;

               WinSendMsg (hwndSysMenu, MM_QUERYITEM,
                           MPFROM2SHORT (idSysMenu, FALSE),
                           MPFROMP (&miSysMenu)) ;

               hwndSysSubMenu = miSysMenu.hwndSubMenu ;

               for (sItem = 0 ; sItem < 3 ; sItem++)
                    WinSendMsg (hwndSysSubMenu, MM_INSERTITEM,
                                MPFROMP (mi + sItem),
                                MPFROMP (szMenuText [sItem])) ;
               return 0 ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_ABOUT:
                         WinMessageBox (HWND_DESKTOP, hwnd,
                                   "(C) Charles Petzold, 1988",
                                   szCaption, 0, MB_OK | MB_ICONASTERISK) ;
                         return 0 ;

                    case IDM_HELP:
                         WinMessageBox (HWND_DESKTOP, hwnd,
                                   "Help not yet implemented",
                                   szCaption, 0, MB_OK | MB_ICONEXCLAMATION) ;
                         return 0 ;
                    }
               break ;

          case WM_ERASEBACKGROUND:
               return 1 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
