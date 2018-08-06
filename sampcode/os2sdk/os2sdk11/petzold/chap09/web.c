/*--------------------------------------
   WEB.C -- Mouse Movement Demo Program 
  --------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "Web" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
     HAB          hab ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc,
                       CS_SIZEREDRAW | CS_SYNCPAINT, 0) ;

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

VOID DrawWeb (HPS hps, POINTL *pptlPointerPos, POINTL *pptlClient)
     {
     POINTL ptl ;
                                   // Lower Left --> Pointer --> Upper Right
     ptl.x = 0 ;
     ptl.y = 0 ;
     GpiMove (hps, &ptl) ;
     GpiLine (hps, pptlPointerPos) ;
     GpiLine (hps, pptlClient) ;
                                   // Upper Left --> Pointer --> Lower Right
     ptl.x = 0 ;
     ptl.y = pptlClient->y ;
     GpiMove (hps, &ptl) ;
     GpiLine (hps, pptlPointerPos) ;

     ptl.x = pptlClient->x ;
     ptl.y = 0 ;
     GpiLine (hps, &ptl) ;
                                   // Lower Center --> Pointer --> Upper Center
     ptl.x = pptlClient->x / 2 ;
     ptl.y = 0 ;
     GpiMove (hps, &ptl) ;
     GpiLine (hps, pptlPointerPos) ;

     ptl.y = pptlClient->y ;
     GpiLine (hps, &ptl) ;
                                   // Left Center --> Pointer --> Right Center
     ptl.x = 0 ;
     ptl.y = pptlClient->y / 2 ;
     GpiMove (hps, &ptl) ;
     GpiLine (hps, pptlPointerPos) ;

     ptl.x = pptlClient->x ;
     GpiLine (hps, &ptl) ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static POINTL ptlClient, ptlPointerPos ;
     HPS           hps ;

     switch (msg)
          {
          case WM_SIZE:
               ptlClient.x = SHORT1FROMMP (mp2) ;
               ptlClient.y = SHORT2FROMMP (mp2) ;
               return 0 ;
     
          case WM_MOUSEMOVE:
               hps = WinGetPS (hwnd) ;
               GpiSetMix (hps, FM_INVERT) ;

               DrawWeb (hps, &ptlPointerPos, &ptlClient) ;

               ptlPointerPos.x = MOUSEMSG(&msg)->x ;
               ptlPointerPos.y = MOUSEMSG(&msg)->y ;

               DrawWeb (hps, &ptlPointerPos, &ptlClient) ;

               WinReleasePS (hps) ;
               break ;                       // do default processing

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;
               GpiSetMix (hps, FM_INVERT) ;

               DrawWeb (hps, &ptlPointerPos, &ptlClient) ;

               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
