/*--------------------------------------------------------
   SYSVALS4.C -- System Values Display Program using AVIO
  --------------------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#define INCL_VIO
#define INCL_AVIO
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include "sysvals.h"

#define MAXWIDTH 60

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

HAB  hab ;

int main (void)
     {
     static CHAR  szClientClass [] = "SysVals4" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_VERTSCROLL    | FCF_HORZSCROLL ;
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

BYTE RgbToVioColor (COLOR clrRgb)
     {
     BYTE bIrgb ;
     RGB  rgb ;

     rgb = MAKETYPE (clrRgb, RGB) ;

     if (rgb.bBlue  >= 0x80) bIrgb |= '\x01' ;
     if (rgb.bGreen >= 0x80) bIrgb |= '\x02' ;
     if (rgb.bRed   >= 0x80) bIrgb |= '\x04' ;

     if (rgb.bBlue >= 0xC0 || rgb.bGreen >= 0xC0 || rgb.bRed >= 0xC0)
          bIrgb |= 8 ;

     if (bIrgb == 0 && rgb.bBlue >= 0x40 && rgb.bGreen >= 0x40 &&
                       rgb.bRed  >= 0x40)
          bIrgb = 8 ;

     return bIrgb ;
     }

BYTE ConstructDefaultAttribute (VOID)
     {
     return RgbToVioColor (
                 WinQuerySysColor (HWND_DESKTOP, SYSCLR_WINDOW, 0L)) << 4 |
            RgbToVioColor (
                 WinQuerySysColor (HWND_DESKTOP, SYSCLR_WINDOWTEXT, 0L)) ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BYTE   bBlankCell [2] = " " ;
     static HVPS   hvps ;
     static HWND   hwndHscroll, hwndVscroll ;
     static HPS    hps ;
     static SHORT  sHscrollPos, sVscrollPos,
                   cxChar, cyChar, cxClient, cyClient ;
     CHAR          szBuffer [80] ;
     HDC           hdc ;
     SIZEL         sizl ;
     USHORT        usRow ;

     switch (msg)
          {
          case WM_CREATE:
               hdc = WinOpenWindowDC (hwnd) ;

               sizl.cx = sizl.cy = 0 ;
               hps = GpiCreatePS (hab, hdc, &sizl, PU_PELS    | GPIF_DEFAULT |
                                                   GPIT_MICRO | GPIA_ASSOC) ;

               VioCreatePS (&hvps, NUMLINES, MAXWIDTH, 0, 1, NULL) ;
               VioAssociate (hdc, hvps) ;
               VioGetDeviceCellSize (&cyChar, &cxChar, hvps) ;

               bBlankCell[1] = ConstructDefaultAttribute () ;
               VioScrollUp (0, 0, -1, -1, -1, bBlankCell, hvps) ;

               for (usRow = 0 ; usRow < NUMLINES ; usRow++)
                    VioWrtCharStr (szBuffer,
                                   sprintf (szBuffer, "%-20s%-35s%5ld",
                                            sysvals[usRow].szIdentifier,
                                            sysvals[usRow].szDescription,
                                            WinQuerySysValue (HWND_DESKTOP,
                                            sysvals[usRow].sIndex)),
                                   usRow, 0, hvps) ;

               hwndHscroll = WinWindowFromID (
                                   WinQueryWindow (hwnd, QW_PARENT, FALSE),
                                   FID_HORZSCROLL) ;

               WinSendMsg (hwndHscroll, SBM_SETSCROLLBAR,
                                        MPFROM2SHORT (sHscrollPos, 0),
                                        MPFROM2SHORT (0, MAXWIDTH - 1)) ;

               hwndVscroll = WinWindowFromID (
                                   WinQueryWindow (hwnd, QW_PARENT, FALSE),
                                   FID_VERTSCROLL) ;

               WinSendMsg (hwndVscroll, SBM_SETSCROLLBAR,
                                        MPFROM2SHORT (sVscrollPos, 0),
                                        MPFROM2SHORT (0, NUMLINES - 1)) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;

               WinDefAVioWindowProc (hwnd, msg, mp1, mp2) ;
               return 0 ;

          case WM_HSCROLL:
               switch (SHORT2FROMMP (mp2))
                    {
                    case SB_LINELEFT:
                         sHscrollPos -= 1 ;
                         break ;

                    case SB_LINERIGHT:
                         sHscrollPos += 1 ;
                         break ;

                    case SB_PAGELEFT:
                         sHscrollPos -= 8 ;
                         break ;

                    case SB_PAGERIGHT:
                         sHscrollPos += 8 ;
                         break ;

                    case SB_SLIDERPOSITION:
                         sHscrollPos = SHORT1FROMMP (mp2) ;
                         break ;
                    }
               sHscrollPos = max (0, min (sHscrollPos, MAXWIDTH - 1)) ;

               if (sHscrollPos != SHORT1FROMMR (WinSendMsg (hwndHscroll,
                                       SBM_QUERYPOS, NULL, NULL)))
                    {
                    VioSetOrg (sVscrollPos, sHscrollPos, hvps) ;

                    WinSendMsg (hwndHscroll, SBM_SETPOS,
                                MPFROM2SHORT (sHscrollPos, 0), NULL) ;
                    }
               return 0 ;

          case WM_VSCROLL:
               switch (SHORT2FROMMP (mp2))
                    {
                    case SB_LINEUP:
                         sVscrollPos -= 1 ;
                         break ;

                    case SB_LINEDOWN:
                         sVscrollPos += 1 ;
                         break ;

                    case SB_PAGEUP:
                         sVscrollPos -= cyClient / cyChar ;
                         break ;

                    case SB_PAGEDOWN:
                         sVscrollPos += cyClient / cyChar ;
                         break ;

                    case SB_SLIDERPOSITION:
                         sVscrollPos = SHORT1FROMMP (mp2) ;
                         break ;
                    }
               sVscrollPos = max (0, min (sVscrollPos, NUMLINES - 1)) ;

               if (sVscrollPos != SHORT1FROMMR (WinSendMsg (hwndVscroll,
                                       SBM_QUERYPOS, NULL, NULL)))
                    {
                    VioSetOrg (sVscrollPos, sHscrollPos, hvps) ;

                    WinSendMsg (hwndVscroll, SBM_SETPOS,
                                MPFROM2SHORT (sVscrollPos, 0), NULL) ;
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
               WinBeginPaint (hwnd, hps, NULL) ;
               GpiErase (hps) ;

               VioShowBuf (0, MAXWIDTH * NUMLINES * 2, hvps) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               VioAssociate (NULL, hvps) ;
               VioDestroyPS (hvps) ;
               GpiDestroyPS (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
