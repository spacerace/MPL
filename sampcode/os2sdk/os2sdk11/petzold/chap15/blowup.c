/*------------------------------------
   BLOWUP.C -- Screen Capture Program
  ------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "blowup.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

CHAR szClientClass [] = "BlowUp" ;
HAB  hab ;

int main (void)
     {
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_MENU          | FCF_ACCELTABLE ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
                                     0L, NULL, ID_RESOURCE, &hwndClient) ;

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

BOOL BeginTracking (RECTL *prclTrack)
     {
     LONG      cxScreen, cyScreen, cxPointer, cyPointer ;
     TRACKINFO ti ;

     cxScreen  = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN) ;
     cyScreen  = WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN) ;
     cxPointer = WinQuerySysValue (HWND_DESKTOP, SV_CXPOINTER) ;
     cyPointer = WinQuerySysValue (HWND_DESKTOP, SV_CYPOINTER) ;

                                   // Set up track rectangle for moving

     ti.cxBorder = 1 ;                       // Border width
     ti.cyBorder = 1 ;
     ti.cxGrid = 0 ;                         // Not used
     ti.cyGrid = 0 ;
     ti.cxKeyboard = 4 ;                     // Pixel increment for keyboard
     ti.cyKeyboard = 4 ;

     ti.rclBoundary.xLeft   = 0 ;            // Area for tracking rectangle
     ti.rclBoundary.yBottom = 0 ;
     ti.rclBoundary.xRight  = cxScreen ;
     ti.rclBoundary.yTop    = cyScreen ;

     ti.ptlMinTrackSize.x = 1 ;              // Minimum rectangle size
     ti.ptlMinTrackSize.y = 1 ;

     ti.ptlMaxTrackSize.x = cxScreen ;       // Maximum rectangle size
     ti.ptlMaxTrackSize.y = cyScreen ;
                                             // Initial position

     ti.rclTrack.xLeft   = (cxScreen - cxPointer) / 2 ;
     ti.rclTrack.yBottom = (cyScreen - cyPointer) / 2 ;
     ti.rclTrack.xRight  = (cxScreen + cxPointer) / 2 ;
     ti.rclTrack.yTop    = (cyScreen + cyPointer) / 2 ;

     ti.fs = TF_MOVE | TF_STANDARD | TF_SETPOINTERPOS ;     // Flags

     if (!WinTrackRect (HWND_DESKTOP, NULL, &ti))
          return FALSE ;
                                   // Switch to "sizing" pointer
     WinSetPointer (HWND_DESKTOP,
               WinQuerySysPointer (HWND_DESKTOP, SPTR_SIZENESW, FALSE)) ;

                                   // Track rectangle for sizing

     ti.fs = TF_RIGHT | TF_TOP | TF_STANDARD | TF_SETPOINTERPOS ;

     if (!WinTrackRect (HWND_DESKTOP, NULL, &ti))
          return FALSE ;

     *prclTrack = ti.rclTrack ;    // Final rectangle

     return TRUE ;
     }

HBITMAP CopyScreenToBitmap (RECTL *prclTrack)
     {
     BITMAPINFOHEADER bmp ;
     HBITMAP          hbm ;
     HDC              hdcMemory ;
     HPS              hps, hpsMemory ;
     LONG             alBmpFormats[2] ;
     POINTL           aptl[3] ;
     SIZEL            sizl ;
                                   // Create memory DC and PS

     hdcMemory = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;

     sizl.cx = sizl.cy = 0 ;
     hpsMemory = GpiCreatePS (hab, hdcMemory, &sizl,
                              PU_PELS    | GPIF_DEFAULT |
                              GPIT_MICRO | GPIA_ASSOC) ;

                                   // Create bitmap for destination

     GpiQueryDeviceBitmapFormats (hpsMemory, 2L, alBmpFormats) ;

     bmp.cbFix     = sizeof bmp ;
     bmp.cx        = (USHORT) (prclTrack->xRight - prclTrack->xLeft) ;
     bmp.cy        = (USHORT) (prclTrack->yTop   - prclTrack->yBottom) ;
     bmp.cPlanes   = (USHORT) alBmpFormats[0] ;
     bmp.cBitCount = (USHORT) alBmpFormats[1] ;

     hbm = GpiCreateBitmap (hpsMemory, &bmp, 0L, NULL, NULL) ;

                                   // Copy from screen to bitmap
     if (hbm != NULL)
          {
          GpiSetBitmap (hpsMemory, hbm) ;
          hps = WinGetScreenPS (HWND_DESKTOP) ;

          aptl[0].x = 0 ;
          aptl[0].y = 0 ;
          aptl[1].x = bmp.cx ;
          aptl[1].y = bmp.cy ;
          aptl[2].x = prclTrack->xLeft ;
          aptl[2].y = prclTrack->yBottom ;

          WinLockVisRegions (HWND_DESKTOP, TRUE) ;

          GpiBitBlt (hpsMemory, hps, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE);

          WinLockVisRegions (HWND_DESKTOP, FALSE) ;

          WinReleasePS (hps) ;
          }
                                   // Clean up
     GpiDestroyPS (hpsMemory) ;
     DevCloseDC (hdcMemory) ;

     return hbm ;
     }

HBITMAP CopyBitmap (HBITMAP hbmSrc)
     {
     BITMAPINFOHEADER bmp ;
     HBITMAP          hbmDst ;
     HDC              hdcSrc, hdcDst ;
     HPS              hpsSrc, hpsDst ;
     POINTL           aptl[3] ;
     SIZEL            sizl ;

                                   // Create memory DC's and PS's

     hdcSrc = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;
     hdcDst = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;

     sizl.cx = sizl.cy = 0 ;
     hpsSrc = GpiCreatePS (hab, hdcSrc, &sizl, PU_PELS    | GPIF_DEFAULT |
                                               GPIT_MICRO | GPIA_ASSOC) ;

     hpsDst = GpiCreatePS (hab, hdcDst, &sizl, PU_PELS    | GPIF_DEFAULT |
                                               GPIT_MICRO | GPIA_ASSOC) ;

                                   // Create bitmap

     GpiQueryBitmapParameters (hbmSrc, &bmp) ;
     hbmDst = GpiCreateBitmap (hpsDst, &bmp, 0L, NULL, NULL) ;

                                   // Copy from source to destination

     if (hbmDst != NULL)
          {
          GpiSetBitmap (hpsSrc, hbmSrc) ;
          GpiSetBitmap (hpsDst, hbmDst) ;

          aptl[0].x = aptl[0].y = 0 ;
          aptl[1].x = bmp.cx ;
          aptl[1].y = bmp.cy ;
          aptl[2]   = aptl[0] ;

          GpiBitBlt (hpsDst, hpsSrc, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE) ;
          }
                                   // Clean up
     GpiDestroyPS (hpsSrc) ;
     GpiDestroyPS (hpsDst) ;
     DevCloseDC (hdcSrc) ;
     DevCloseDC (hdcDst) ;

     return hbmDst ;
     }

VOID BitmapCreationError (HWND hwnd)
     {
     WinMessageBox (HWND_DESKTOP, hwnd, "Cannot create bitmap.",
                    szClientClass, 0, MB_OK | MB_ICONEXCLAMATION) ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static HBITMAP   hbm ;
     static HWND      hwndMenu ;
     static SHORT     sDisplay = IDM_ACTUAL ;
     HBITMAP          hbmClip ;
     HPS              hps ;
     RECTL            rclTrack, rclClient ;
     USHORT           usfInfo ;

     switch (msg)
          {
          case WM_CREATE:
               hwndMenu = WinWindowFromID (
                               WinQueryWindow (hwnd, QW_PARENT, FALSE),
                               FID_MENU) ;
               return 0 ;

          case WM_INITMENU:
               switch (SHORT1FROMMP (mp1))
                    {
                    case IDM_EDIT:
                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (IDM_COPY, TRUE),
                                     MPFROM2SHORT (MIA_DISABLED,
                                          hbm != NULL ? 0 : MIA_DISABLED)) ;

                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (IDM_PASTE, TRUE),
                                     MPFROM2SHORT (MIA_DISABLED,
                              WinQueryClipbrdFmtInfo (hab, CF_BITMAP, &usfInfo)
                                        ? 0 : MIA_DISABLED)) ;
                         return 0 ;
                    }
               break ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_COPY:
                                        // Make copy of stored bitmap

                         hbmClip = CopyBitmap (hbm) ;

                                        // Set clipboard data to copy of bitmap

                         if (hbmClip != NULL)
                              {
                              WinOpenClipbrd (hab) ;
                              WinEmptyClipbrd (hab) ;
                              WinSetClipbrdData (hab, (ULONG) hbmClip,
                                                 CF_BITMAP, CFI_HANDLE) ;
                              WinCloseClipbrd (hab) ;
                              }
                         else
                              BitmapCreationError (hwnd) ;
                         return 0 ;

                    case IDM_PASTE:
                                         // Get bitmap from clipboard

                         WinOpenClipbrd (hab) ;
                         hbmClip = WinQueryClipbrdData (hab, CF_BITMAP) ;

                         if (hbmClip != NULL)
                              {
                              if (hbm != NULL)
                                   GpiDeleteBitmap (hbm) ;

                                        // Make copy of it

                              hbm = CopyBitmap (hbmClip) ;

                              if (hbm == NULL)
                                   BitmapCreationError (hwnd) ;
                              }
                         WinCloseClipbrd (hab) ;
                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;

                    case IDM_CAPTURE:
                         if (BeginTracking (&rclTrack))
                              {
                              if (hbm != NULL)
                                   GpiDeleteBitmap (hbm) ;

                              hbm = CopyScreenToBitmap (&rclTrack) ;

                              if (hbm == NULL)
                                   BitmapCreationError (hwnd) ;

                              WinInvalidateRect (hwnd, NULL, FALSE) ;
                              }
                         return 0 ;

                    case IDM_ACTUAL:
                    case IDM_STRETCH:
                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (sDisplay, TRUE),
                                     MPFROM2SHORT (MIA_CHECKED, 0)) ;

                         sDisplay = COMMANDMSG(&msg)->cmd ;

                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (sDisplay, TRUE),
                                     MPFROM2SHORT (MIA_CHECKED, MIA_CHECKED)) ;

                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;
                    }

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               if (hbm != NULL)
                    {
                    WinQueryWindowRect (hwnd, &rclClient) ;

                    WinDrawBitmap (hps, hbm, NULL, (PPOINTL) &rclClient,
                                   CLR_NEUTRAL, CLR_BACKGROUND,
                                   sDisplay == IDM_STRETCH ?
                                        DBM_STRETCH : DBM_NORMAL) ;
                    }
               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               if (hbm != NULL)
                    GpiDeleteBitmap (hbm) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
