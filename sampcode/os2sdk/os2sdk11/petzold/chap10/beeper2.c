/*--------------------------------------- 
   BEEPER2.C -- Timer Demo Program No. 2
  ---------------------------------------*/

#define INCL_WIN
#include <os2.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

BOOL fFlipFlop ;

int main (void)
     {
     static char  szClientClass [] = "Beeper2" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
     HAB          hab ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;
     USHORT       idTimer ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
				     0L, NULL, 0, &hwndClient) ;

     idTimer = WinStartTimer (hab, NULL, 0, 1000) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          {
          if (qmsg.msg == WM_TIMER && SHORT1FROMMP (qmsg.mp1) == idTimer)
               {
               WinAlarm (HWND_DESKTOP, WA_NOTE) ;
               fFlipFlop = !fFlipFlop ;
               WinInvalidateRect (hwndClient, NULL, FALSE) ;
               }
          else
               WinDispatchMsg (hab, &qmsg) ;
          }

     WinStopTimer (hab, NULL, idTimer) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     HPS   hps ;
     RECTL rcl ;

     switch (msg)
          {
          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;

               WinQueryWindowRect (hwnd, &rcl) ;
               WinFillRect (hps, &rcl, fFlipFlop ? CLR_BLUE : CLR_RED) ;

               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
