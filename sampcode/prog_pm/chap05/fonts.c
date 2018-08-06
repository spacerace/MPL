/*----------------------------
   FONTS.C -- GPI Image Fonts
  ----------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include "easyfont.h"

#define LCID_MYFONT 1L

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "Fonts" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_VERTSCROLL    | FCF_HORZSCROLL ;
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
     static CHAR   *szFace[] = { "System", "Courier",
                                 "Helv",   "Tms Rmn" } ;
     static CHAR   *szSize[] = { "8", "10", "12", "14", "18", "24" } ;
     static CHAR   *szSel[]  = { "Normal",     "Italic",  "Underscore",
                                 "Strike-out", "Bold" } ;
     static CHAR   szBuffer[80] ;
     static HWND   hwndVscroll, hwndHscroll ;
     static USHORT idFace[] = { FONTFACE_SYSTEM, FONTFACE_COUR,
                                FONTFACE_HELV,   FONTFACE_TIMES } ;
     static USHORT idSize[] = { FONTSIZE_8,  FONTSIZE_10, FONTSIZE_12,
                                FONTSIZE_14, FONTSIZE_18, FONTSIZE_24 } ;
     static USHORT afsSel[] = { 0, FATTR_SEL_ITALIC,    FATTR_SEL_UNDERSCORE,
                                   FATTR_SEL_STRIKEOUT, FATTR_SEL_BOLD } ;
     static SHORT  sVscrollMax = sizeof idFace / sizeof idFace[0] - 1,
                   sHscrollMax = sizeof afsSel / sizeof afsSel[0] - 1,
                   cxClient, cyClient, sHscrollPos, sVscrollPos ;
     FONTMETRICS   fm ;
     HPS           hps;
     HWND          hwndFrame ;
     POINTL        ptl ;
     SHORT         sIndex ;

     switch (msg)
          {
          case WM_CREATE:
               hps = WinGetPS (hwnd) ;
               EzfQueryFonts (hps) ;
               WinReleasePS (hps) ;

               hwndFrame   = WinQueryWindow (hwnd, QW_PARENT, FALSE),
               hwndVscroll = WinWindowFromID (hwndFrame, FID_VERTSCROLL) ;
               hwndHscroll = WinWindowFromID (hwndFrame, FID_HORZSCROLL) ;

               WinSendMsg (hwndVscroll, SBM_SETSCROLLBAR,
                           MPFROM2SHORT (sVscrollPos, 0),
                           MPFROM2SHORT (0, sVscrollMax)) ;

               WinSendMsg (hwndHscroll, SBM_SETSCROLLBAR,
                           MPFROM2SHORT (sHscrollPos, 0),
                           MPFROM2SHORT (0, sHscrollMax)) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_VSCROLL:
               switch (SHORT2FROMMP (mp2))
                    {
                    case SB_LINEUP:
                    case SB_PAGEUP:
                         sVscrollPos = max (0, sVscrollPos - 1) ;
                         break ;

                    case SB_LINEDOWN:
                    case SB_PAGEDOWN:
                         sVscrollPos = min (sVscrollMax, sVscrollPos + 1) ;
                         break ;

                    case SB_SLIDERPOSITION:
                         sVscrollPos = SHORT1FROMMP (mp2) ;
                         break ;

                    default:
                         return 0 ;
                    }
               WinSendMsg (hwndVscroll, SBM_SETPOS,
                           MPFROM2SHORT (sVscrollPos, 0), NULL) ;

               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_HSCROLL:
               switch (SHORT2FROMMP (mp2))
                    {
                    case SB_LINELEFT:
                    case SB_PAGELEFT:
                         sHscrollPos = max (0, sHscrollPos - 1) ;
                         break ;

                    case SB_LINERIGHT:
                    case SB_PAGERIGHT:
                         sHscrollPos = min (sHscrollMax, sHscrollPos + 1) ;
                         break ;

                    case SB_SLIDERPOSITION:
                         sHscrollPos = SHORT1FROMMP (mp2) ;
                         break ;

                    default:
                         return 0 ;
                    }
               WinSendMsg (hwndHscroll, SBM_SETPOS,
                           MPFROM2SHORT (sHscrollPos, 0), NULL) ;

               WinInvalidateRect (hwnd, NULL, FALSE) ;
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
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               ptl.x = 0 ;
               ptl.y = cyClient ;

               for (sIndex = 0 ; sIndex < 6 ; sIndex++)
                    if (EzfCreateLogFont (hps, LCID_MYFONT,
                                          idFace[sVscrollPos],
                                          idSize[sIndex],
                                          afsSel[sHscrollPos]))
                         {
                         GpiSetCharSet (hps, LCID_MYFONT) ;
                         GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;

                         ptl.y -= fm.lMaxBaselineExt ;

                         GpiCharStringAt (hps, &ptl,
                              (LONG) sprintf (szBuffer, "%s, %s point, %s",
                                              szFace[sVscrollPos],
                                              szSize[sIndex],
                                              szSel[sHscrollPos]),
                              szBuffer) ;

                         GpiSetCharSet (hps, LCID_DEFAULT) ;
                         GpiDeleteSetId (hps, LCID_MYFONT) ;
                         }

               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
