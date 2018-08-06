/*--------------------------------------------------------
   COLORSCR.C -- Color Scroll using child window controls
  --------------------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY ScrollProc (HWND, USHORT, MPARAM, MPARAM) ;

HWND  hwndScroll[3], hwndFocus ;
PFNWP pfnOldScroll[3] ;

int main (void)
     {
     static CHAR  szClientClass[] = "ColorScr" ;
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

     WinSetFocus (HWND_DESKTOP, hwndFocus = hwndScroll[0]) ;

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
     static CHAR  *szColorLabel[] = { "Red", "Green", "Blue" } ;
     static HWND  hwndLabel[3], hwndValue[3] ;
     static SHORT cyChar, sColor[3] ;
     static RECTL rclRightHalf ;
     CHAR         szBuffer[10] ;
     FONTMETRICS  fm ;
     HPS          hps ;
     SHORT        s, id, cxClient, cyClient ;

     switch (msg)
          {
          case WM_CREATE :
               hps = WinGetPS (hwnd) ;
               GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;
               cyChar = (SHORT) fm.lMaxBaselineExt ;
               WinReleasePS (hps) ;

               for (s = 0 ; s < 3 ; s++)
                    {
                    hwndScroll[s] = WinCreateWindow (
                                        hwnd,               // Parent
                                        WC_SCROLLBAR,       // Class
                                        NULL,               // Text
                                        WS_VISIBLE |        // Style
                                             SBS_VERT,
                                        0, 0,               // Position
                                        0, 0,               // Size
                                        hwnd,               // Owner
                                        HWND_BOTTOM,        // Placement
                                        s,                  // ID
                                        NULL,               // Ctrl Data
                                        NULL) ;             // Pres Params

                    hwndLabel[s]  = WinCreateWindow (
                                        hwnd,               // Parent
                                        WC_STATIC,          // Class
                                        szColorLabel[s],    // Text
                                        WS_VISIBLE |        // Style
                                          SS_TEXT | DT_CENTER,
                                        0, 0,               // Position
                                        0, 0,               // Size
                                        hwnd,               // Owner
                                        HWND_BOTTOM,        // Placement
                                        s + 3,              // ID
                                        NULL,               // Ctrl Data
                                        NULL) ;             // Pres Params

                    hwndValue[s]  = WinCreateWindow (
                                        hwnd,               // Parent
                                        WC_STATIC,          // Class
                                        "0",                // Text
                                        WS_VISIBLE |        // Style
                                          SS_TEXT | DT_CENTER,
                                        0, 0,               // Position
                                        0, 0,               // Size
                                        hwnd,               // Owner
                                        HWND_BOTTOM,        // Placement
                                        s + 6,              // ID
                                        NULL,               // Ctrl Data
                                        NULL) ;             // Pres Params

                    pfnOldScroll[s] =
                              WinSubclassWindow (hwndScroll[s], ScrollProc) ;

                    WinSendMsg (hwndScroll[s], SBM_SETSCROLLBAR,
				MPFROM2SHORT (0, 0), MPFROM2SHORT (0, 255)) ;
                    }
               return 0 ;

          case WM_SIZE :
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;

               for (s = 0 ; s < 3 ; s++)
                    {
                    WinSetWindowPos (hwndScroll[s], NULL,
                                     (2 * s + 1) * cxClient / 14, 2 * cyChar,
                                     cxClient / 14, cyClient - 4 * cyChar,
                                     SWP_SIZE | SWP_MOVE) ;

                    WinSetWindowPos (hwndLabel[s], NULL,
                                     (4 * s + 1) * cxClient / 28,
                                     cyClient - 3 * cyChar / 2,
                                     cxClient / 7, cyChar,
                                     SWP_SIZE | SWP_MOVE) ;

                    WinSetWindowPos (hwndValue[s], NULL,
                                     (4 * s + 1) * cxClient / 28, cyChar / 2,
                                     cxClient / 7, cyChar,
                                     SWP_SIZE | SWP_MOVE) ;
                    }

               WinQueryWindowRect (hwnd, &rclRightHalf) ;
               rclRightHalf.xLeft = rclRightHalf.xRight / 2 ;
               return 0 ;

          case WM_VSCROLL :
               id = SHORT1FROMMP (mp1) ;          // ID of scroll bar

               switch (SHORT2FROMMP (mp2))
                    {
                    case SB_LINEDOWN :
                         sColor[id] = min (255, sColor[id] + 1) ;
                         break ;

                    case SB_LINEUP :
                         sColor[id] = max (0, sColor[id] - 1) ;
                         break ;

                    case SB_PAGEDOWN :
                         sColor[id] = min (255, sColor[id] + 16) ;
                         break ;

                    case SB_PAGEUP :
                         sColor[id] = max (0, sColor[id] - 16) ;
                         break ;

                    case SB_SLIDERTRACK :
                         sColor[id] = SHORT1FROMMP (mp2) ;
                         break ;

                    default :
                         return 0 ;
                    }
               WinSendMsg (hwndScroll[id], SBM_SETPOS,
                           MPFROM2SHORT (sColor[id], 0), NULL) ;

               WinSetWindowText (hwndValue[id],
                                 itoa (sColor[id], szBuffer, 10)) ;
               WinInvalidateRect (hwnd, &rclRightHalf, FALSE) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;

               GpiCreateLogColorTable (hps, LCOL_RESET, LCOLF_RGB,
                                            0L, 0L, NULL) ;

               WinFillRect (hps, &rclRightHalf, (ULONG) sColor[0] << 16 |
                                                (ULONG) sColor[1] <<  8 |
                                                (ULONG) sColor[2]) ;
               WinEndPaint (hps) ;
               return 0 ;

          case WM_ERASEBACKGROUND:
               return 1 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT EXPENTRY ScrollProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     USHORT id ;

     id = WinQueryWindowUShort (hwnd, QWS_ID) ;   // ID of scroll bar

     switch (msg)
          {
          case WM_CHAR:
               if (!(CHARMSG(&msg)->fs & KC_VIRTUALKEY))
                    break ;

               switch (CHARMSG(&msg)->vkey)
                    {
                    case VK_TAB:
                         if (!(CHARMSG(&msg)->fs & KC_KEYUP))
                              {
                              hwndFocus = hwndScroll[(id + 1) % 3] ;
                              WinSetFocus (HWND_DESKTOP, hwndFocus) ;
                              }
                         return 1 ;

                    case VK_BACKTAB:
                         if (!(CHARMSG(&msg)->fs & KC_KEYUP))
                              {
                              hwndFocus = hwndScroll[(id + 2) % 3] ;
                              WinSetFocus (HWND_DESKTOP, hwndFocus) ;
                              }
                         return 1 ;

                    default:
                         break ;
                    }
               break ;

          case WM_BUTTON1DOWN:
               WinSetFocus (HWND_DESKTOP, hwndFocus = hwnd) ;
               break ;
          }
     return pfnOldScroll[id] (hwnd, msg, mp1, mp2) ;
     }
