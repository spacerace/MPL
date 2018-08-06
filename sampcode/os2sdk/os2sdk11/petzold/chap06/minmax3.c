/*---------------------------------------
   MINMAX3.C -- Minimize-Maximize Bitmap
  ---------------------------------------*/

#define INCL_WIN
#include <os2.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "MinMax3" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
     HAB          hab ;
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

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static SHORT cxClient, cyClient ;
     HBITMAP      hbmMin, hbmMax ;
     HPS          hps ;
     POINTL       aptl [2] ;

     switch (msg)
          {
          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;

               hbmMin = WinGetSysBitmap (HWND_DESKTOP, SBMP_MINBUTTON) ;
               hbmMax = WinGetSysBitmap (HWND_DESKTOP, SBMP_MAXBUTTON) ;

               aptl[0].x = 0 ;               // Target lower left
               aptl[0].y = 0 ;
               aptl[1].x = cxClient / 2 ;    // Target upper right
               aptl[1].y = cyClient ;

               WinDrawBitmap (hps, hbmMin, NULL, aptl,
                              CLR_NEUTRAL, CLR_BACKGROUND, DBM_STRETCH) ;

               aptl[0].x = cxClient / 2 ;    // Target left
               aptl[1].x = cxClient ;        // Target right

               WinDrawBitmap (hps, hbmMax, NULL, aptl,
                              CLR_NEUTRAL, CLR_BACKGROUND, DBM_STRETCH) ;

               GpiDeleteBitmap (hbmMin) ;
               GpiDeleteBitmap (hbmMax) ;

               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
