/*------------------------------------------
   BITCAT1.C -- Bitmap Creation and Display
  ------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include "bitcat.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "BitCat1" ;
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

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static HBITMAP   hbm ;
     BITMAPINFO       *pbmi ;
     BITMAPINFOHEADER bmp ;
     HPS              hps ;
     RECTL            rcl ;

     switch (msg)
          {
          case WM_CREATE:

                         /*-----------------------------------
                            Create 32-by-32 monochrome bitmap
                           -----------------------------------*/

               bmp.cbFix     = sizeof bmp ;
               bmp.cx        = 32 ;
               bmp.cy        = 32 ;
               bmp.cPlanes   = 1 ;
               bmp.cBitCount = 1 ;

               pbmi = malloc (sizeof (BITMAPINFO) + sizeof (RGB)) ;

               pbmi->cbFix     = sizeof bmp ;
               pbmi->cx        = 32 ;
               pbmi->cy        = 32 ;
               pbmi->cPlanes   = 1 ;
               pbmi->cBitCount = 1;

               pbmi->argbColor[0].bBlue  = 0 ;
               pbmi->argbColor[0].bGreen = 0 ;
               pbmi->argbColor[0].bRed   = 0 ;
               pbmi->argbColor[1].bBlue  = 0xFF ;
               pbmi->argbColor[1].bGreen = 0xFF ;
               pbmi->argbColor[1].bRed   = 0xFF ;

               hps = WinGetPS (hwnd) ;
               hbm = GpiCreateBitmap (hps, &bmp, CBM_INIT, abBitCat, pbmi) ;

               WinReleasePS (hps) ;
               free (pbmi) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;

               WinQueryWindowRect (hwnd, &rcl) ;

               WinDrawBitmap (hps, hbm, NULL, (PPOINTL) &rcl,
                              CLR_NEUTRAL, CLR_BACKGROUND, DBM_STRETCH) ;

               WinDrawBitmap (hps, hbm, NULL, (PPOINTL) &rcl,
                              CLR_NEUTRAL, CLR_BACKGROUND, DBM_NORMAL) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               GpiDeleteBitmap (hbm) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
