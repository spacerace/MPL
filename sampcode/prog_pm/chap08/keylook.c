/*----------------------------------------
   KEYLOOK.C -- Displays WM_CHAR Messages
  ----------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdio.h>
#include "easyfont.h"

#define LCID_FIXEDFONT 1L
#define MAX_KEYS       100

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

CHAR szClientClass [] = "KeyLook" ;
HAB  hab ;

int main (void)
     {
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
     if (hwndFrame != NULL)
          {
          WinSendMsg (hwndFrame, WM_SETICON,
                      WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                      NULL) ;

          while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
               WinDispatchMsg (hab, &qmsg) ;

          WinDestroyWindow (hwndFrame) ;
          }
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static CHAR   szHeader [] = "Scan  Rept  IN TG IC CM DK LK PD KU"
                                 " AL CT SH SC VK CH  Virt  Char" ;
     static CHAR   szUndrLn [] = "----  ----  -- -- -- -- -- -- -- --"
                                 " -- -- -- -- -- --  ----  ----" ;
     static CHAR   szFormat [] = "%4X %4dx  %2d %2d %2d %2d %2d %2d %2d %2d"
                                 " %2d %2d %2d %2d %2d %2d  %4X  %4X  %c" ;

     static SHORT  cxChar, cyChar, cyDesc, cxClient, cyClient, sNextKey ;
     static struct {
                   MPARAM mp1 ;
                   MPARAM mp2 ;
                   BOOL   fValid ;
                   }
                   key [MAX_KEYS] ;
     CHAR          szBuffer [80] ;
     FONTMETRICS   fm ;
     HPS           hps ;
     POINTL        ptl ;
     RECTL         rcl, rclInvalid ;
     SHORT         sKey, sIndex, sFlag ;

     switch (msg)
          {
          case WM_CREATE:
               hps = WinGetPS (hwnd) ;
               EzfQueryFonts (hps) ;

               if (!EzfCreateLogFont (hps, LCID_FIXEDFONT, FONTFACE_COUR,
                                                           FONTSIZE_10, 0))
                    {
                    WinReleasePS (hps) ;

                    WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
                         "Cannot find a fixed-pitch font.  Load the Courier "
                         "fonts from the Control Panel and try again.",
                         szClientClass, 0, MB_OK | MB_ICONEXCLAMATION) ;

                    return 1 ;
                    }

               GpiSetCharSet (hps, LCID_FIXEDFONT) ;

               GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;
               cxChar = (SHORT) fm.lAveCharWidth ;
               cyChar = (SHORT) fm.lMaxBaselineExt ;
               cyDesc = (SHORT) fm.lMaxDescender ;

               GpiSetCharSet (hps, LCID_DEFAULT) ;
               GpiDeleteSetId (hps, LCID_FIXEDFONT) ;
               WinReleasePS (hps) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_CHAR:
               key [sNextKey].mp1 = mp1 ;
               key [sNextKey].mp2 = mp2 ;
               key [sNextKey].fValid = TRUE ;

               sNextKey = (sNextKey + 1) % MAX_KEYS ;

               WinSetRect (hwnd, &rcl,
                           0, 2 * cyChar, cxClient, cyClient - 2 * cyChar) ;

               WinScrollWindow (hwnd, 0, cyChar, &rcl, &rcl, NULL, NULL,
                                                 SW_INVALIDATERGN) ;
               WinUpdateWindow (hwnd) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, &rclInvalid) ;
               GpiErase (hps) ;
               EzfCreateLogFont (hps, LCID_FIXEDFONT, FONTFACE_COUR,
                                                      FONTSIZE_10, 0) ;
               GpiSetCharSet (hps, LCID_FIXEDFONT) ;

               ptl.x = cxChar ;
               ptl.y = cyDesc ;
               GpiCharStringAt (hps, &ptl, sizeof szHeader - 1L, szHeader) ;

               ptl.y += cyChar ;
               GpiCharStringAt (hps, &ptl, sizeof szUndrLn - 1L, szUndrLn) ;

               for (sKey = 0 ; sKey < MAX_KEYS ; sKey++)
                    {
                    ptl.y += cyChar ;

                    sIndex = (sNextKey - sKey - 1 + MAX_KEYS) % MAX_KEYS ;

                    if (ptl.y > rclInvalid.yTop ||
                              ptl.y > cyClient - 2 * cyChar ||
                                   !key [sIndex].fValid)
                         break ;
                         
                    mp1 = key [sIndex].mp1 ;
                    mp2 = key [sIndex].mp2 ;

                    sFlag = CHARMSG(&msg)->fs ;

                    GpiCharStringAt (hps, &ptl, 
                         (LONG) sprintf (szBuffer, szFormat,
                                   CHARMSG(&msg)->scancode,
                                   CHARMSG(&msg)->cRepeat,
                                   sFlag & KC_INVALIDCHAR ? 1 : 0,
                                   sFlag & KC_TOGGLE      ? 1 : 0,
                                   sFlag & KC_INVALIDCOMP ? 1 : 0,
                                   sFlag & KC_COMPOSITE   ? 1 : 0,
                                   sFlag & KC_DEADKEY     ? 1 : 0,
                                   sFlag & KC_LONEKEY     ? 1 : 0,
                                   sFlag & KC_PREVDOWN    ? 1 : 0,
                                   sFlag & KC_KEYUP       ? 1 : 0,
                                   sFlag & KC_ALT         ? 1 : 0,
                                   sFlag & KC_CTRL        ? 1 : 0,
                                   sFlag & KC_SHIFT       ? 1 : 0,
                                   sFlag & KC_SCANCODE    ? 1 : 0,
                                   sFlag & KC_VIRTUALKEY  ? 1 : 0,
                                   sFlag & KC_CHAR        ? 1 : 0,
                                   CHARMSG(&msg)->vkey,
                                   CHARMSG(&msg)->chr,
                                   sFlag & KC_CHAR ? CHARMSG(&msg)->chr : ' '),
                              szBuffer) ;
                    }
               ptl.y = cyClient - cyChar + cyDesc ;
               GpiCharStringAt (hps, &ptl, sizeof szHeader - 1L, szHeader) ;

               ptl.y -= cyChar ;
               GpiCharStringAt (hps, &ptl, sizeof szUndrLn - 1L, szUndrLn) ;

               GpiSetCharSet (hps, LCID_DEFAULT) ;
               GpiDeleteSetId (hps, LCID_FIXEDFONT) ;
               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
