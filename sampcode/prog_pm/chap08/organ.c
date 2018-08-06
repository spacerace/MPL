/*--------------------------------------
   ORGAN.C --  Play Organ from Keyboard
  --------------------------------------*/

#define INCL_DOS
#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "organ.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;
VOID    EXPENTRY Speaker (USHORT usFreq) ;

SHORT  xOffset, yOffset, cxCaps, cyChar ;
USHORT usLastScan ;

int main (void)
     {
     static CHAR  szClientClass [] = "Organ" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
     HAB          hab ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     if (DosPortAccess (0, 0, 0x42, 0x61))   // Don't run if port access fails
          return 1 ;

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
     DosPortAccess (0, 1, 0x42, 0x61) ;
     return 0 ;
     }

VOID DrawKey (HPS hps, USHORT usScanCode, BOOL fInvert)
     {
     RECTL rcl ;

     rcl.xLeft   = 3 * cxCaps * key[usScanCode].xPos / 2 + xOffset ;
     rcl.yBottom = 3 * cyChar * key[usScanCode].yPos / 2 + yOffset ;
     rcl.xRight  = rcl.xLeft   + 3 * cxCaps ;
     rcl.yTop    = rcl.yBottom + 3 * cyChar / 2 ;

     WinDrawText (hps, -1, key[usScanCode].szKey, &rcl,
                  CLR_NEUTRAL, CLR_BACKGROUND,
                  DT_CENTER | DT_VCENTER | DT_ERASERECT) ;
     if (fInvert)
          WinInvertRect (hps, &rcl) ;

     WinDrawBorder (hps, &rcl, 1, 1, CLR_NEUTRAL, CLR_BACKGROUND,
                    DB_STANDARD) ;
     }

VOID ProcessKey (HPS hps, USHORT usScanCode, USHORT fsFlags)
     {
     static USHORT ausOctFreq [] = { 262, 277, 294, 311, 330, 349,
                                     370, 392, 415, 440, 466, 494 } ;
     USHORT        usOct, usFreq ;

     if (usScanCode >= NUMSCANS)                       // No scan codes over 53
          return ;
     if ((usOct = key[usScanCode].sOctave) == -1)      // Non-music key
          return ;

     if (fsFlags & KC_KEYUP)                           // For key up
          {
          if (usLastScan == usScanCode)                // If that's the note
               {
               Speaker (0) ;                           // turn off speaker
               DrawKey (hps, usScanCode, FALSE) ;      // and redraw key
               usLastScan = 0 ;
               }
          return ;
          }
     if (fsFlags & KC_PREVDOWN)                        // Ignore typematics
          return ;

     usFreq = ausOctFreq [key[usScanCode].sNote] ;     // Get frequency

     if (fsFlags & KC_SHIFT)
          usOct += fsFlags & KC_ALT ? 2 : 1 ;          // Higher octave
     else if (fsFlags & KC_CTRL)
          usOct -= fsFlags & KC_ALT ? 2 : 1 ;          // Lower octave

     if (usOct > 4)                                    // Shift frequency
          usFreq <<= (usOct - 4) ;                     //   for octave
     else if (usOct < 4)
          usFreq >>= (4 - usOct) ;

     Speaker (usFreq) ;                                // Turn on speaker
     DrawKey (hps, usScanCode, TRUE) ;                 // Draw the inverted key

     if (usLastScan != 0)
          DrawKey (hps, usLastScan, FALSE) ;           // Redraw previous key
     usLastScan = usScanCode ;                         // Save scan code
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     FONTMETRICS fm ;
     HPS         hps ;
     SHORT       cxClient, cyClient ;
     USHORT      usScanCode ;

     switch (msg)
          {
          case WM_CREATE:
               hps = WinGetPS (hwnd) ;
               GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;
               cxCaps = (SHORT) fm.lEmInc ;
               cyChar = (SHORT) fm.lMaxBaselineExt ;
               WinReleasePS (hps) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;

               xOffset = (cxClient - 25 * 3 * cxCaps / 2) / 2 ;
               yOffset = (cyClient - 6 * cyChar) / 2 ;
               return 0 ;

          case WM_CHAR:
               if (!(CHARMSG(&msg)->fs & KC_SCANCODE))
                    break ;

               hps = WinGetPS (hwnd) ;
               ProcessKey (hps, CHARMSG(&msg)->scancode, CHARMSG(&msg)->fs) ;
               WinReleasePS (hps) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               for (usScanCode = 0 ; usScanCode < NUMSCANS ; usScanCode++)
                    if (key[usScanCode].xPos != -1)
                         DrawKey (hps, usScanCode, usScanCode == usLastScan) ;

               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
