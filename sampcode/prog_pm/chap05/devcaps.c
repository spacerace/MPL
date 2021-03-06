/*--------------------------------------------------
   DEVCAPS.C -- Device Capabilities Display Program
  --------------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "devcaps.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "DevCaps" ;
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

LONG RtJustCharStringAt (HPS hps, POINTL *pptl, LONG lLength, CHAR *pchText)
     {
     POINTL aptlTextBox[TXTBOX_COUNT] ;

     GpiQueryTextBox (hps, lLength, pchText, TXTBOX_COUNT, aptlTextBox) ;

     pptl->x -= aptlTextBox[TXTBOX_CONCAT].x ;

     return GpiCharStringAt (hps, pptl, lLength, pchText) ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static HDC   hdc ;
     static SHORT cxClient, cyClient, cxCaps, cyChar, cyDesc ;
     CHAR         szBuffer [12] ;
     FONTMETRICS  fm ;
     LONG         lValue ;
     POINTL       ptl ;
     HPS          hps ;
     SHORT        sLine ;

     switch (msg)
          {
          case WM_CREATE:
               hps = WinGetPS (hwnd) ;
               GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;
               cxCaps = (SHORT) fm.lEmInc ;
               cyChar = (SHORT) fm.lMaxBaselineExt ;
               cyDesc = (SHORT) fm.lMaxDescender ;
               WinReleasePS (hps) ;

               hdc = WinOpenWindowDC (hwnd) ;
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
                    ptl.y = cyClient - cyChar * (sLine + 2) + cyDesc ;

                    if (sLine >= (NUMLINES + 1) / 2)
                         {
                         ptl.x += cxCaps * 35 ;
                         ptl.y += cyChar * (NUMLINES + 1) / 2 ;
                         }

                    DevQueryCaps (hdc, devcaps[sLine].lIndex, 1L, &lValue) ;

                    GpiCharStringAt (hps, &ptl,
                              (LONG) strlen (devcaps[sLine].szIdentifier),
                              devcaps[sLine].szIdentifier) ;

                    ptl.x += 33 * cxCaps ;
                    RtJustCharStringAt (hps, &ptl,
                              (LONG) strlen (ltoa (lValue, szBuffer, 10)),
                              szBuffer) ;
                    }
               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
                                                       
