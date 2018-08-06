/*-------------------------------------
   HELLOBIT.C -- "Hello, world" Bitmap
  -------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

HAB  hab ;

int main (void)
     {
     static CHAR  szClientClass [] = "HelloBit" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
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
     static CHAR      szHello [] = " Hello, world " ;
     static HBITMAP   hbm ;
     static HDC       hdcMemory ;
     static HPS       hpsMemory ;
     static SHORT     cxClient, cyClient, cxString, cyString ;
     BITMAPINFOHEADER bmp ;
     HPS              hps ;
     POINTL           aptl [4], ptl ;
     SHORT            x, y ;
     SIZEL            sizl ;

     switch (msg)
          {
          case WM_CREATE:

                         /*-------------------------------------------------
                            Open memory DC and create PS associated with it
                           -------------------------------------------------*/

               hdcMemory = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;

               sizl.cx = 0 ;
               sizl.cy = 0 ;
               hpsMemory = GpiCreatePS (hab, hdcMemory, &sizl,
					PU_PELS    | GPIF_DEFAULT |
                                        GPIT_MICRO | GPIA_ASSOC) ;

                         /*-------------------------------------
                            Determine dimensions of text string
                           -------------------------------------*/

               GpiQueryTextBox (hpsMemory, sizeof szHello - 1L,
                                szHello, 4L, aptl) ;

               cxString = (SHORT) (aptl [TXTBOX_TOPRIGHT].x -
                                   aptl [TXTBOX_TOPLEFT].x) ;

               cyString = (SHORT) (aptl [TXTBOX_TOPLEFT].y -
                                   aptl [TXTBOX_BOTTOMLEFT].y) ;

                         /*-------------------------------------------
                            Create bitmap and set it in the memory PS
                           -------------------------------------------*/

               bmp.cbFix     = sizeof bmp ;
               bmp.cx        = cxString ;
               bmp.cy        = cyString ;
               bmp.cPlanes   = 1 ;
               bmp.cBitCount = 1 ;

               hbm = GpiCreateBitmap (hpsMemory, &bmp, 0L, 0L, NULL) ;

               GpiSetBitmap (hpsMemory, hbm) ;

                         /*----------------------------------------
                            Write the text string to the memory PS
                           ----------------------------------------*/

               ptl.x = 0 ;
               ptl.y = - aptl [TXTBOX_BOTTOMLEFT].y ;

               GpiSetColor (hpsMemory, CLR_TRUE) ;
               GpiSetBackColor (hpsMemory, CLR_FALSE) ;
               GpiSetBackMix (hpsMemory, BM_OVERPAINT) ;
               GpiCharStringAt (hpsMemory, &ptl, sizeof szHello - 1L,
                                szHello) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;

               for (y = 0 ; y <= cyClient / cyString ; y++)
                    for (x = 0 ; x <= cxClient / cxString ; x++)
                         {
                         aptl[0].x = x * cxString ;    // target lower left
                         aptl[0].y = y * cyString ;

                         aptl[1].x = aptl[0].x + cxString ; // upper right
                         aptl[1].y = aptl[0].y + cyString ;

                         aptl[2].x = 0 ;               // source lower left
                         aptl[2].y = 0 ;

                         GpiBitBlt (hps, hpsMemory, 3L, aptl, ROP_SRCCOPY,
                                    BBO_AND) ;
                         }
               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               GpiDestroyPS (hpsMemory) ;
               DevCloseDC (hdcMemory) ;
               GpiDeleteBitmap (hbm) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
