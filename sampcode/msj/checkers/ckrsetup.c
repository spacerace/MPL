/*-----------------------------------------------------------
   CKRSETUP.C -- SetupCntlProc for setup-board, Version 0.40
                 (c) 1990, Charles Petzold
  -----------------------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include "checkers.h"
#include "ckrdraw.h"

extern HAB hab ;

MRESULT EXPENTRY SetupCntlProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOARD brd ;
     static HPS   hps ;
     static RECTL rclOrigViewport ;
     static SHORT sBottom ;
     HDC          hdc ;
     LONG         lScale ;
     POINTL       ptlMouse ;
     RECTL        rclWindow, rclViewport ;
     PSETUPDLG    psetupdlg ;
     SHORT        x, y, i ;
     SIZEL        sizlPage ;
     ULONG        ulBit ;

     switch (msg)
          {
          case WM_CREATE:
               CkdQueryBoardDimensions (&sizlPage) ;

               hdc = WinOpenWindowDC (hwnd) ;
               hps = GpiCreatePS (hab, hdc, &sizlPage,
                                  PU_ARBITRARY | GPIF_DEFAULT |
                                  GPIT_MICRO   | GPIA_ASSOC) ;

               GpiQueryPageViewport (hps, &rclOrigViewport) ;

               WinQueryWindowRect (hwnd, &rclWindow) ;

               if (rclWindow.xRight == 0)
                    DosBeep (1000, 1000) ;

                    // Calculate scaling factor

               lScale = min (65536L * rclWindow.xRight / rclOrigViewport.xRight,
                             65536L * rclWindow.yTop   / rclOrigViewport.yTop) ;

                    // Adjust page viewport of PS

               rclViewport.xLeft   = 0 ;
               rclViewport.yBottom = 0 ;
               rclViewport.xRight  = lScale * rclOrigViewport.xRight / 65536L ;
               rclViewport.yTop    = lScale * rclOrigViewport.yTop   / 65536L ;

               rclViewport.xLeft   = (rclWindow.xRight - rclViewport.xRight) / 2 ;
               rclViewport.yBottom = (rclWindow.yTop   - rclViewport.yTop)   / 2 ;
               rclViewport.xRight += rclViewport.xLeft ;
               rclViewport.yTop   += rclViewport.yBottom ;

               GpiSetPageViewport (hps, &rclViewport) ;

               return 0 ;

          case WM_BUTTON1DOWN:
               WinSetActiveWindow (HWND_DESKTOP, hwnd) ;

          case WM_BUTTON1DBLCLK:
          case WM_BUTTON2DOWN:
          case WM_BUTTON2DBLCLK:
               psetupdlg = WinQueryWindowPtr (hwnd, 0) ;
               brd       = psetupdlg->brd ;
               sBottom   = psetupdlg->sBottom ;

                                        // get mouse coords and index

               ptlMouse.x = MOUSEMSG(&msg)->x ;
               ptlMouse.y = MOUSEMSG(&msg)->y ;
               CkdQueryHitCoords (hps, ptlMouse, &x, &y) ;
               i = CkdConvertCoordsToIndex (x, y, sBottom) ;

               if (i == -1)             // didn't hit black square
                    {
                    WinAlarm (HWND_DESKTOP, WA_ERROR) ;
                    return 0 ;
                    }

               ulBit = 1L << i ;

               if (msg == WM_BUTTON1DOWN || msg == WM_BUTTON1DBLCLK)
                    {
                    if (brd.ulBlack & ulBit)
                         {
                         if (brd.ulKing & ulBit)
                              {
                              brd.ulBlack &= ~ulBit ;
                              brd.ulKing  &= ~ulBit ;
                              }
                         else
                              brd.ulKing |= ulBit ;
                         }
                    else
                         {
                         brd.ulBlack |=  ulBit ;
                         brd.ulWhite &= ~ulBit ;
                         brd.ulKing  &= ~ulBit ;
                         }
                    }
               else
                    {
                    if (brd.ulWhite & ulBit)
                         {
                         if (brd.ulKing & ulBit)
                              {
                              brd.ulWhite &= ~ulBit ;
                              brd.ulKing  &= ~ulBit ;
                              }
                         else
                              brd.ulKing |= ulBit ;
                         }
                    else
                         {
                         brd.ulBlack &= ~ulBit ;
                         brd.ulWhite |=  ulBit ;
                         brd.ulKing  &= ~ulBit ;
                         }
                    }

               CkdErasePiece (hps, x, y) ;
               CkdDrawOnePieceDirect (hps, x, y, &brd, sBottom) ;

               psetupdlg->brd = brd ;
               return 0 ;

          case WM_PAINT:
               WinBeginPaint (hwnd, hps, NULL) ;

               psetupdlg = WinQueryWindowPtr (hwnd, 0) ;
               brd       = psetupdlg->brd ;
               sBottom   = psetupdlg->sBottom ;

               CkdDrawWholeBoard (hps) ;
               CkdDrawAllPiecesDirect (hps, &brd, sBottom) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               GpiDestroyPS (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
