/*--------------------------------------------------------------
   CHECKER3.C -- Mouse Hit-Test Demo Program with Child Windows
  --------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>

#define DIVISIONS 5

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY ChildWndProc  (HWND, USHORT, MPARAM, MPARAM) ;

HAB  hab ;

int main (void)
     {
     static CHAR  szClientClass [] = "Checker3" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
                                     0L, NULL, 0, &hwndClient) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

VOID DrawLine (HPS hps, LONG x1, LONG y1, LONG x2, LONG y2)
     {
     POINTL ptl ;

     ptl.x = x1 ;  ptl.y = y1 ;  GpiMove (hps, &ptl) ;
     ptl.x = x2 ;  ptl.y = y2 ;  GpiLine (hps, &ptl) ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static CHAR szChildClass [] = "Checker3.Child" ;
     static HWND hwndChild [DIVISIONS][DIVISIONS] ;
     SHORT       xBlock, yBlock, x, y ;

     switch (msg)
          {
          case WM_CREATE:
               WinRegisterClass (hab, szChildClass, ChildWndProc,
                                 CS_SIZEREDRAW, sizeof (USHORT)) ;

               for (x = 0 ; x < DIVISIONS ; x++)
                    for (y = 0 ; y < DIVISIONS ; y++)

                         hwndChild [x][y] =
                              WinCreateWindow (
                                        hwnd,          // Parent window
                                        szChildClass,  // Window class
                                        NULL,          // Window text
                                        WS_VISIBLE,    // Window style
                                        0, 0, 0, 0,    // Position & size
                                        hwnd,          // Owner window
                                        HWND_BOTTOM,   // Placement
                                        y << 8 | x,    // Child window ID
                                        NULL,          // Control data
                                        NULL) ;        // Pres. Params
               return 0 ;

          case WM_SIZE:
               xBlock = SHORT1FROMMP (mp2) / DIVISIONS ;
               yBlock = SHORT2FROMMP (mp2) / DIVISIONS ;

               for (x = 0 ; x < DIVISIONS ; x++)
                    for (y = 0 ; y < DIVISIONS ; y++)

                         WinSetWindowPos (hwndChild [x][y], NULL,
                              x * xBlock, y * yBlock, xBlock, yBlock,
                              SWP_MOVE | SWP_SIZE) ;      
               return 0 ;

          case WM_BUTTON1DOWN:
          case WM_BUTTON1DBLCLK:
               WinAlarm (HWND_DESKTOP, WA_WARNING) ;
               break ;                       // do default processing

          case WM_ERASEBACKGROUND:
               return 1 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT EXPENTRY ChildWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     HPS   hps ;
     RECTL rcl ;

     switch (msg)
          {
          case WM_CREATE:
               WinSetWindowUShort (hwnd, 0, 0) ;
               return 0 ;

          case WM_BUTTON1DOWN:
          case WM_BUTTON1DBLCLK:
               WinSetActiveWindow (HWND_DESKTOP, hwnd) ;
               WinSetWindowUShort (hwnd, 0, !WinQueryWindowUShort (hwnd, 0)) ;
               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;

               WinQueryWindowRect (hwnd, &rcl) ;

               WinDrawBorder (hps, &rcl, 1, 1, CLR_NEUTRAL, CLR_BACKGROUND,
                                   DB_STANDARD | DB_INTERIOR) ;

               if (WinQueryWindowUShort (hwnd, 0))
                    {
                    DrawLine (hps, rcl.xLeft,  rcl.yBottom,
                                   rcl.xRight, rcl.yTop) ;
                    DrawLine (hps, rcl.xLeft,  rcl.yTop,
                                   rcl.xRight, rcl.yBottom) ;
                    }
               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
