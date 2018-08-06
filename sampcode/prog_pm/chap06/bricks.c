/*--------------------------------------------
   BRICKS.C -- Customized Pattern from Bitmap
  --------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>

#define LCID_BRICKS_BITMAP    1L

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "Bricks" ;
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
     static BYTE      abBrick [] = {
                                   0x00, 0x00, 0x00, 0x00,
                                   0xF3, 0x00, 0x00, 0x00,
                                   0xF3, 0x00, 0x00, 0x00,
                                   0xF3, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00,
                                   0x3F, 0x00, 0x00, 0x00,
                                   0x3F, 0x00, 0x00, 0x00,
                                   0x3F, 0x00, 0x00, 0x00
                                   } ;
     static HBITMAP   hbm ;
     static POINTL    aptl [2] ;
     BITMAPINFO       *pbmi ;
     BITMAPINFOHEADER bmp ;
     HPS              hps ;

     switch (msg)
          {
          case WM_CREATE:
                              /*----------------------
                                 Create 8 by 8 bitmap
                                ----------------------*/

               bmp.cbFix     = sizeof bmp ;
               bmp.cx        = 8 ;
               bmp.cy        = 8 ;
               bmp.cPlanes   = 1 ;
               bmp.cBitCount = 1 ;

               pbmi = malloc (sizeof (BITMAPINFO) + sizeof (RGB)) ;

               pbmi->cbFix     = sizeof bmp ;
               pbmi->cx        = 8 ;
               pbmi->cy        = 8 ;
               pbmi->cPlanes   = 1 ;
               pbmi->cBitCount = 1 ;

               pbmi->argbColor[0].bBlue  = 0 ;
               pbmi->argbColor[0].bGreen = 0 ;
               pbmi->argbColor[0].bRed   = 0 ;
               pbmi->argbColor[1].bBlue  = 0xFF ;
               pbmi->argbColor[1].bGreen = 0xFF ;
               pbmi->argbColor[1].bRed   = 0xFF ;

               hps = WinGetPS (hwnd) ;
               hbm = GpiCreateBitmap (hps, &bmp, CBM_INIT, abBrick, pbmi) ;

               WinReleasePS (hps) ;
               free (pbmi) ;
               return 0 ;

          case WM_SIZE:
               aptl[1].x = SHORT1FROMMP (mp2) ;
               aptl[1].y = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;

               GpiSetBitmapId (hps, hbm, LCID_BRICKS_BITMAP) ;
               GpiSetPatternSet (hps, LCID_BRICKS_BITMAP) ;

               GpiBitBlt (hps, NULL, 2L, aptl, ROP_PATCOPY, BBO_AND) ;

               GpiSetPatternSet (hps, LCID_DEFAULT) ;
               GpiDeleteSetId (hps, LCID_BRICKS_BITMAP) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               GpiDeleteBitmap (hbm) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
