/*-------------------------
   RULER.C -- Draw a Ruler
  -------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdio.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "Ruler" ;
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
     static SHORT sTick[16] = { 100, 25, 35, 25, 50, 25, 35, 25,
                                 70, 25, 35, 25, 50, 25, 35, 25 } ;
     static SHORT cxClient, cyClient, cxChar, cyChar, cyDesc ;
     static SIZEL sizl ;
     CHAR         szBuffer [4] ;
     FONTMETRICS  fm ;
     HPS          hps ;
     POINTL       ptl ;
     SHORT        sIndex ;

     switch (msg)
          {
          case WM_CREATE:
               hps = WinGetPS (hwnd) ;
               GpiSetPS (hps, &sizl, PU_LOENGLISH) ;

               GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;
               cxChar = (SHORT) fm.lAveCharWidth ;
               cyChar = (SHORT) fm.lMaxBaselineExt ;
               cyDesc = (SHORT) fm.lMaxDescender ;

               WinReleasePS (hps) ;
               return 0 ;

          case WM_SIZE:
               ptl.x = SHORT1FROMMP (mp2) ;
               ptl.y = SHORT2FROMMP (mp2) ;

               hps = WinGetPS (hwnd) ;
               GpiSetPS (hps, &sizl, PU_LOENGLISH) ;
               GpiConvert (hps, CVTC_DEVICE, CVTC_PAGE, 1L, &ptl) ;
               WinReleasePS (hps) ;

               cxClient = (SHORT) ptl.x ;
               cyClient = (SHORT) ptl.y ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiSetPS (hps, &sizl, PU_LOENGLISH) ;
               GpiErase (hps) ;

               for (sIndex = 0 ; sIndex < 16 * (SHORT) cxClient / 100 ;
                                 sIndex ++)
                    {
                    ptl.x = 100 * sIndex / 16 ;
                    ptl.y = 0 ;
                    GpiMove (hps, &ptl) ;

                    ptl.y = sTick [sIndex % 16] ;
                    GpiLine (hps, &ptl) ;

                    if (sIndex % 16 == 0)
                         {
                         ptl.x -= cxChar / (sIndex > 160 ? 1 : 2) ;
                         ptl.y += cyDesc ;
                         GpiCharStringAt (hps, &ptl,
                              (LONG) sprintf (szBuffer, "%d", sIndex / 16),
                              szBuffer) ;
                         }
                    }
               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
