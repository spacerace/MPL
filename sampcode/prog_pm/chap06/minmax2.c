/*-----------------------------------------------
   MINMAX2.C -- Bitblt of Minimize-Maximize Menu
  -----------------------------------------------*/

#define INCL_WIN
#include <os2.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "MinMax2" ;
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
     static LONG  cxMinMax, cyMinMax ;
     static SHORT cxClient, cyClient ;
     HPS          hps ;
     POINTL       aptl[4] ;

     switch (msg)
          {
          case WM_CREATE:
               cxMinMax = WinQuerySysValue (HWND_DESKTOP, SV_CXMINMAXBUTTON) ;
               cyMinMax = WinQuerySysValue (HWND_DESKTOP, SV_CYMINMAXBUTTON) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               aptl[0].x = 0 ;                    // target lower left
               aptl[0].y = 0 ;

               aptl[1].x = cxClient ;             // target upper right
               aptl[1].y = cyClient ;

               aptl[2].x = cxClient - cxMinMax ;  // source lower left
               aptl[2].y = cyClient ;

               aptl[3].x = cxClient ;             // source upper right
               aptl[3].y = cyClient + cyMinMax ;

               GpiBitBlt (hps, hps, 4L, aptl, ROP_SRCCOPY, BBO_AND) ;

               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
