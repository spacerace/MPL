/*--------------------------------------------
   SYSVALS.C -- System Values Display Program
  --------------------------------------------*/
 
#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "sysvals.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "SysVals" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_VERTSCROLL    | FCF_HORZSCROLL ;
     HAB            hab ;
     HMQ            hmq ;
     HWND           hwndFrame, hwndClient ;
     QMSG           qmsg ;

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

LONG RtJustCharStringAt (HPS hps, POINTL *pptl, LONG lLength, CHAR *pchText)
     {
     POINTL aptlTextBox[TXTBOX_COUNT] ;

     GpiQueryTextBox (hps, lLength, pchText, TXTBOX_COUNT, aptlTextBox) ;

     pptl->x -= aptlTextBox[TXTBOX_CONCAT].x ;

     return GpiCharStringAt (hps, pptl, lLength, pchText) ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static HWND  hwndHscroll, hwndVscroll ;
     static SHORT sHscrollMax, sVscrollMax, sHscrollPos, sVscrollPos,
                  cxChar, cxCaps, cyChar, cyDesc, cxClient, cyClient,
                  cxTextTotal ;
     CHAR         szBuffer [10] ;
     FONTMETRICS  fm ;
     HPS          hps ;
     POINTL       ptl ;
     SHORT        sLine, sPaintBeg, sPaintEnd, sHscrollInc, sVscrollInc ;
     RECTL        rclInvalid ;

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

               cxTextTotal = 28 * cxCaps + 38 * cxChar ;

               hwndHscroll = WinWindowFromID (
                                   WinQueryWindow (hwnd, QW_PARENT, FALSE),
                                   FID_HORZSCROLL) ;

               hwndVscroll = WinWindowFromID (
                                   WinQueryWindow (hwnd, QW_PARENT, FALSE),
                                   FID_VERTSCROLL) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;

               sHscrollMax = max (0, cxTextTotal - cxClient) ;
               sHscrollPos = min (sHscrollPos, sHscrollMax) ;

               WinSendMsg (hwndHscroll, SBM_SETSCROLLBAR,
                                        MPFROM2SHORT (sHscrollPos, 0),
                                        MPFROM2SHORT (0, sHscrollMax)) ;

               WinEnableWindow (hwndHscroll, sHscrollMax ? TRUE : FALSE) ;

               sVscrollMax = max (0, NUMLINES - cyClient / cyChar) ;
               sVscrollPos = min (sVscrollPos, sVscrollMax) ;

               WinSendMsg (hwndVscroll, SBM_SETSCROLLBAR,
                                        MPFROM2SHORT (sVscrollPos, 0),
                                        MPFROM2SHORT (0, sVscrollMax)) ;

               WinEnableWindow (hwndVscroll, sVscrollMax ? TRUE : FALSE) ;
               return 0 ;

          case WM_HSCROLL:
               switch (SHORT2FROMMP (mp2))
                    {
                    case SB_LINELEFT:
                         sHscrollInc = -cxCaps ;
                         break ;

                    case SB_LINERIGHT:
                         sHscrollInc = cxCaps ;
                         break ;

                    case SB_PAGELEFT:
                         sHscrollInc = -8 * cxCaps ;
                         break ;

                    case SB_PAGERIGHT:
                         sHscrollInc = 8 * cxCaps ;
                         break ;

                    case SB_SLIDERPOSITION:
                         sHscrollInc = SHORT1FROMMP (mp2) - sHscrollPos;
                         break ;

                    default:
                         sHscrollInc = 0 ;
                         break ;
                    }

               sHscrollInc = max (-sHscrollPos,
                             min (sHscrollInc, sHscrollMax - sHscrollPos)) ;

               if (sHscrollInc != 0)
                    {
                    sHscrollPos += sHscrollInc ;
                    WinScrollWindow (hwnd, -sHscrollInc, 0,
                                   NULL, NULL, NULL, NULL, SW_INVALIDATERGN) ;

                    WinSendMsg (hwndHscroll, SBM_SETPOS,
                                MPFROMSHORT (sHscrollPos), NULL) ;
                    }
               return 0 ;

          case WM_VSCROLL:
               switch (SHORT2FROMMP (mp2))
                    {
                    case SB_LINEUP:
                         sVscrollInc = -1 ;
                         break ;

                    case SB_LINEDOWN:
                         sVscrollInc = 1 ;
                         break ;

                    case SB_PAGEUP:
                         sVscrollInc = min (-1, -cyClient / cyChar) ;
                         break ;

                    case SB_PAGEDOWN:
                         sVscrollInc = max (1, cyClient / cyChar) ;
                         break ;

                    case SB_SLIDERTRACK:
                         sVscrollInc = SHORT1FROMMP (mp2) - sVscrollPos;
                         break ;

                    default:
                         sVscrollInc = 0 ;
                         break ;
                    }

               sVscrollInc = max (-sVscrollPos,
                             min (sVscrollInc, sVscrollMax - sVscrollPos)) ;

               if (sVscrollInc != 0) ;
                    {
                    sVscrollPos += sVscrollInc ;
                    WinScrollWindow (hwnd, 0, cyChar * sVscrollInc,
                                   NULL, NULL, NULL, NULL, SW_INVALIDATERGN) ;

                    WinSendMsg (hwndVscroll, SBM_SETPOS,
                                MPFROMSHORT (sVscrollPos), NULL) ;
                    WinUpdateWindow (hwnd) ;
                    }
               return 0 ;

          case WM_CHAR:
               switch (CHARMSG(&msg)->vkey)
                    {
                    case VK_LEFT:
                    case VK_RIGHT:
                         return WinSendMsg (hwndHscroll, msg, mp1, mp2) ;
                    case VK_UP:
                    case VK_DOWN:
                    case VK_PAGEUP:
                    case VK_PAGEDOWN:
                         return WinSendMsg (hwndVscroll, msg, mp1, mp2) ;
                    }
               break ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, &rclInvalid) ;
               GpiErase (hps) ;

               sPaintBeg = max (0, sVscrollPos +
                              (cyClient - (SHORT) rclInvalid.yTop) / cyChar) ;
               sPaintEnd = min (NUMLINES, sVscrollPos +
                              (cyClient - (SHORT) rclInvalid.yBottom)
                                   / cyChar + 1) ;

               for (sLine = sPaintBeg ; sLine < sPaintEnd ; sLine++)
                    {
                    ptl.x = cxCaps - sHscrollPos ;
                    ptl.y = cyClient - cyChar * (sLine + 1 - sVscrollPos)
                                     + cyDesc ;

                    GpiCharStringAt (hps, &ptl,
                              (LONG) strlen (sysvals[sLine].szIdentifier),
                              sysvals[sLine].szIdentifier) ;

                    ptl.x += 20 * cxCaps ;
                    GpiCharStringAt (hps, &ptl,
                              (LONG) strlen (sysvals[sLine].szDescription),
                              sysvals[sLine].szDescription) ;

                    ltoa (WinQuerySysValue (HWND_DESKTOP,
                               sysvals[sLine].sIndex), szBuffer, 10) ;

                    ptl.x += 38 * cxChar + 6 * cxCaps ;
                    RtJustCharStringAt (hps, &ptl, (LONG) strlen (szBuffer),
                                        szBuffer) ;
                    }

               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
                                                                                                          
