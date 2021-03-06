/*---------------------------------------------------
   SYSVALS1.C -- System Values Display Program No. 1
  ---------------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "sysvals.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "SysVals1" ;
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
     static SHORT cxChar, cxCaps, cyChar, cyDesc, cxClient, cyClient ;
     CHAR         szBuffer [10] ;
     FONTMETRICS  fm ;
     HPS          hps ;
     POINTL       ptl ;
     SHORT        sLine ;

     switch (msg)
          {
          case WM_CREATE:
               hps = WinGetPS (hwnd) ;
               GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;

               cxChar = (SHORT) fm.lAveCharWidth ;
               cxCaps = (SHORT) fm.lEmInc ;
               cyChar = (SHORT) fm.lMaxBaselineExt ;
               cyDesc = (SHORT) fm.lMaxDescender ;

               WinReleasePS (hps) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               for (sLine = 0 ; sLine < NUMLINES ; sLine++)
                    {
                    ptl.x = cxCaps ;
                    ptl.y = cyClient - cyChar * (sLine + 1) + cyDesc ;

                    GpiCharStringAt (hps, &ptl,
                              (LONG) strlen (sysvals[sLine].szIdentifier),
                              sysvals[sLine].szIdentifier) ;

                    ptl.x += 20 * cxCaps ;
                    GpiCharStringAt (hps, &ptl,
                              (LONG) strlen (sysvals[sLine].szDescription),
                              sysvals[sLine].szDescription) ;

                    ltoa (WinQuerySysValue (HWND_DESKTOP,
                               sysvals[sLine].sIndex), szBuffer, 10) ;

                    ptl.x += 38 * cxChar ;
                    GpiCharStringAt (hps, &ptl, (LONG) strlen (szBuffer),
                                     szBuffer) ;
                    }
               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
                                                                                                                    
                
