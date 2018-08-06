/*---------------------------------
   PATTERNS.C -- GPI Area Patterns
  ---------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <string.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "Patterns" ;
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
     static struct {
                   LONG lPatternSymbol ;
                   CHAR *szPatternSymbol ;
                   }
                   show [] = 
                   {
                   PATSYM_DEFAULT  , "PATSYM_DEFAULT"  , 
                   PATSYM_DENSE1   , "PATSYM_DENSE1"   , 
                   PATSYM_DENSE2   , "PATSYM_DENSE2"   , 
                   PATSYM_DENSE3   , "PATSYM_DENSE3"   , 
                   PATSYM_DENSE4   , "PATSYM_DENSE4"   , 
                   PATSYM_DENSE5   , "PATSYM_DENSE5"   , 
                   PATSYM_DENSE6   , "PATSYM_DENSE6"   , 
                   PATSYM_DENSE7   , "PATSYM_DENSE7"   , 
                   PATSYM_DENSE8   , "PATSYM_DENSE8"   , 
                   PATSYM_VERT     , "PATSYM_VERT"     , 
                   PATSYM_HORIZ    , "PATSYM_HORIZ"    , 
                   PATSYM_DIAG1    , "PATSYM_DIAG1"    , 
                   PATSYM_DIAG2    , "PATSYM_DIAG2"    , 
                   PATSYM_DIAG3    , "PATSYM_DIAG3"    , 
                   PATSYM_DIAG4    , "PATSYM_DIAG4"    , 
                   PATSYM_NOSHADE  , "PATSYM_NOSHADE"  , 
                   PATSYM_SOLID    , "PATSYM_SOLID"    , 
                   PATSYM_HALFTONE , "PATSYM_HALFTONE" , 
                   PATSYM_BLANK    , "PATSYM_BLANK"    , 
                   } ;
     static SHORT  cxClient, cyClient, cxCaps, cyChar, cyDesc,
                   sNumTypes = sizeof show / sizeof show[0] ;
     FONTMETRICS   fm ;
     HPS           hps ;
     POINTL        ptl ;
     SHORT         sIndex ;

     switch (msg)
          {
          case WM_CREATE:
               hps = WinGetPS (hwnd) ;
               GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;
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

               for (sIndex = 0 ; sIndex < sNumTypes ; sIndex ++)
                    {
                    GpiSetPattern (hps, show [sIndex].lPatternSymbol) ;

                    ptl.x = (sIndex < 10 ? 1 : 33) * cxCaps ;
                    ptl.y = cyClient - (sIndex % 10 * 5 + 4) * cyChar / 2
                                                       + cyDesc ;

                    GpiCharStringAt (hps, &ptl,
                          (LONG) strlen (show [sIndex].szPatternSymbol),
                               show [sIndex].szPatternSymbol) ;

                    ptl.x = (sIndex < 10 ? 20 : 52) * cxCaps ;
       	            ptl.y -= cyDesc + cyChar / 2 ;
                    GpiMove (hps, &ptl) ;

                    ptl.x += 10 * cxCaps ;
                    ptl.y += 2 * cyChar ;
                    GpiBox (hps, DRO_FILL, &ptl, 0L, 0L) ;
                    }
               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
