/*------------------------------
   LINETYPE.C -- GPI Line Types
  ------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <string.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "LineType" ;
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
                   LONG lLineType ;
                   CHAR *szLineType ;
                   }
                   show [] = 
                   {
                   LINETYPE_DEFAULT       , "LINETYPE_DEFAULT"       , 
                   LINETYPE_DOT           , "LINETYPE_DOT"           , 
                   LINETYPE_SHORTDASH     , "LINETYPE_SHORTDASH"     , 
                   LINETYPE_DASHDOT       , "LINETYPE_DASHDOT"       , 
                   LINETYPE_DOUBLEDOT     , "LINETYPE_DOUBLEDOT"     , 
                   LINETYPE_LONGDASH      , "LINETYPE_LONGDASH"      , 
                   LINETYPE_DASHDOUBLEDOT , "LINETYPE_DASHDOUBLEDOT" , 
                   LINETYPE_SOLID         , "LINETYPE_SOLID"         , 
                   LINETYPE_INVISIBLE     , "LINETYPE_INVISIBLE"     , 
                   LINETYPE_ALTERNATE     , "LINETYPE_ALTERNATE"
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
                    GpiSetLineType (hps, show [sIndex].lLineType) ;

                    ptl.x = cxCaps ;
                    ptl.y = cyClient - 2 * (sIndex + 1) * cyChar + cyDesc ;

                    GpiCharStringAt (hps, &ptl,
                                     (LONG) strlen (show [sIndex].szLineType),
                                     show [sIndex].szLineType) ;

                    if (cxClient > 25 * cxCaps)
                         {
                         ptl.x = 24 * cxCaps ;
                         ptl.y += cyChar / 2 - cyDesc ;
                         GpiMove (hps, &ptl) ;

                         ptl.x = cxClient - cxCaps ;
                         GpiLine (hps, &ptl) ;
                         }
                    }
               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
