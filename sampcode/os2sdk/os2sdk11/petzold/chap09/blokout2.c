/*---------------------------------------------------
   BLOKOUT2.C -- Mouse Button & Capture Demo Program
  ---------------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "BlokOut2" ;
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

VOID DrawBoxOutline (HWND hwnd, POINTL *pptlStart, POINTL *pptlEnd)
     {
     HPS hps ;

     hps = WinGetPS (hwnd) ;
     GpiSetMix (hps, FM_INVERT) ;

     GpiMove (hps, pptlStart) ;
     GpiBox (hps, DRO_OUTLINE, pptlEnd, 0L, 0L) ;

     WinReleasePS (hps) ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOOL   fCapture, fValidBox ;
     static POINTL ptlStart, ptlEnd, ptlBoxStart, ptlBoxEnd ;
     HPS           hps ;

     switch (msg)
          {
          case WM_BUTTON1DOWN:
               ptlStart.x = ptlEnd.x = MOUSEMSG(&msg)->x ;
               ptlStart.y = ptlEnd.y = MOUSEMSG(&msg)->y ;

               DrawBoxOutline (hwnd, &ptlStart, &ptlEnd) ;

               WinSetCapture (HWND_DESKTOP, hwnd) ;
               fCapture = TRUE ;
               break ;                       // do default processing

          case WM_MOUSEMOVE:
               if (fCapture)
                    {
                    DrawBoxOutline (hwnd, &ptlStart, &ptlEnd) ;

                    ptlEnd.x = MOUSEMSG(&msg)->x ;
                    ptlEnd.y = MOUSEMSG(&msg)->y ;

                    DrawBoxOutline (hwnd, &ptlStart, &ptlEnd) ;
                    }
               break ;                       // do default processing

          case WM_BUTTON1UP:
               if (fCapture)
                    {
                    DrawBoxOutline (hwnd, &ptlStart, &ptlEnd) ;

                    ptlBoxStart = ptlStart ;
                    ptlBoxEnd.x = MOUSEMSG(&msg)->x ;
                    ptlBoxEnd.y = MOUSEMSG(&msg)->y ;

                    WinSetCapture (HWND_DESKTOP, NULL) ;
                    fCapture = FALSE ;
                    fValidBox = TRUE ;
                    WinInvalidateRect (hwnd, NULL, FALSE) ;
                    }
               return 0 ;

          case WM_CHAR:
               if (fCapture && CHARMSG(&msg)->fs   &  KC_VIRTUALKEY &&
                             !(CHARMSG(&msg)->fs   &  KC_KEYUP)     &&
			       CHARMSG(&msg)->vkey == VK_ESC)
                    {
                    DrawBoxOutline (hwnd, &ptlStart, &ptlEnd) ;

                    WinSetCapture (HWND_DESKTOP, NULL) ;
                    fCapture = FALSE ;
                    }
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               if (fValidBox)
                    {
                    GpiMove (hps, &ptlBoxStart) ;
                    GpiBox (hps, DRO_OUTLINEFILL, &ptlBoxEnd, 0L, 0L) ;
                    }
               if (fCapture)
                    {
                    GpiSetMix (hps, FM_INVERT) ;
                    GpiMove (hps, &ptlStart) ;
                    GpiBox (hps, DRO_OUTLINE, &ptlEnd, 0L, 0L) ;
                    }
               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
