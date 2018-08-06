/*---------------------------------------
   BEEPER1.C -- Timer Demo Program No. 1
  ---------------------------------------*/

#define INCL_WIN
#include <os2.h>

#define ID_TIMER 1

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static char  szClientClass [] = "Beeper1" ;
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

     WinStartTimer (hab, hwndClient, ID_TIMER, 1000) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinStopTimer (hab, hwndClient, ID_TIMER) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOOL fFlipFlop ;
     HPS         hps ;
     RECTL       rcl ;

     switch (msg)
          {
          case WM_TIMER:
               WinAlarm (HWND_DESKTOP, WA_NOTE) ;
               fFlipFlop = !fFlipFlop ;
               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;

               WinQueryWindowRect (hwnd, &rcl) ;
               WinFillRect (hps, &rcl, fFlipFlop ? CLR_BLUE : CLR_RED) ;

               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
