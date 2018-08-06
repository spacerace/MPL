/*--------------------------------------------------------
   POEPOEM.C -- Demonstrates Programmer-Defined Resources
  --------------------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#include <os2.h>
#include <stdlib.h>
#include "poepoem.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [10] ;
     static CHAR  szTitleBar [40] ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_VERTSCROLL    | FCF_ICON ;
     HAB          hab ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinLoadString (hab, NULL, IDS_CLASS, sizeof szClientClass, szClientClass);
     WinLoadString (hab, NULL, IDS_TITLE, sizeof szTitleBar,    szTitleBar) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, szTitleBar,
                                     0L, NULL, ID_RESOURCE, &hwndClient) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static HWND   hwndScroll ;
     static PCHAR  pResource ;
     static SEL    selResource ;
     static SHORT  cxClient, cyClient, cxChar, cyChar, cyDesc,
                   sScrollPos, sNumLines ;
     FONTMETRICS   fm ;
     HPS           hps ;
     PCHAR         pText ;
     POINTL        ptl ;
     SHORT         sLineLength, sLine ;
     ULONG         ulSegSize ;
     
     switch (msg)
          {
          case WM_CREATE:

                    /*-----------------------------------------
                       Load the resource, get size and address
                      -----------------------------------------*/

               DosGetResource (NULL, IDT_TEXT, IDT_POEM, &selResource) ;
               DosSizeSeg (selResource, &ulSegSize) ;
               pResource = MAKEP (selResource, 0) ;

                    /*-----------------------------------------------
                       Determine how many text lines are in resource
                      -----------------------------------------------*/

               pText = pResource ;

               while (pText - pResource < (USHORT) ulSegSize)
                    {
                    if (*pText == '\0' || *pText == '\x1A')
                         break ;

                    if (*pText == '\r')
                         sNumLines ++ ;

                    pText++ ;
                    }

                    /*------------------------------------------
                       Initialize scroll bar range and position
                      ------------------------------------------*/

               hwndScroll = WinWindowFromID (
                                   WinQueryWindow (hwnd, QW_PARENT, FALSE),
                                   FID_VERTSCROLL) ;

               WinSendMsg (hwndScroll, SBM_SETSCROLLBAR,
                                       MPFROM2SHORT (sScrollPos, 0),
                                       MPFROM2SHORT (0, sNumLines - 1)) ;

                    /*----------------------
                       Query character size
                      ----------------------*/

               hps = WinGetPS (hwnd) ;

               GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;
               cxChar = (SHORT) fm.lAveCharWidth ;
               cyChar = (SHORT) fm.lMaxBaselineExt ;
               cyDesc = (SHORT) fm.lMaxDescender ;

               WinReleasePS (hps) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_CHAR:
               return WinSendMsg (hwndScroll, msg, mp1, mp2) ;

          case WM_VSCROLL:
               switch (SHORT2FROMMP (mp2))
                    {
                    case SB_LINEUP:
                         sScrollPos -= 1 ;
                         break ;

                    case SB_LINEDOWN:
                         sScrollPos += 1 ;
                         break ;

                    case SB_PAGEUP:
                         sScrollPos -= cyClient / cyChar ;
                         break ;

                    case SB_PAGEDOWN:
                         sScrollPos += cyClient / cyChar ;
                         break ;

                    case SB_SLIDERPOSITION:
                         sScrollPos = SHORT1FROMMP (mp2) ;
                         break ;
                    }
               sScrollPos = max (0, min (sScrollPos, sNumLines - 1)) ;

               if (sScrollPos != (SHORT) WinSendMsg (hwndScroll,
                                                     SBM_QUERYPOS, 0L, 0L))
                    {
                    WinSendMsg (hwndScroll, SBM_SETPOS,
                                MPFROM2SHORT (sScrollPos, 0), NULL) ;
                    WinInvalidateRect (hwnd, NULL, FALSE) ;
                    }
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               pText = pResource ;

               for (sLine = 0 ; sLine < sNumLines ; sLine++)
                    {
                    sLineLength = 0 ;

                    while (pText [sLineLength] != '\r')
                         sLineLength ++ ;

                    ptl.x = cxChar ;
                    ptl.y = cyClient - cyChar * (sLine + 1 - sScrollPos)
                                     + cyDesc ;
                    
                    GpiCharStringAt (hps, &ptl, (LONG) sLineLength, pText) ;

                    pText += sLineLength + 2 ;
                    }
               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               DosFreeSeg (selResource) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
