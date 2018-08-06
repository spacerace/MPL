/*------------------------------
   TYPEAWAY.C -- Typing Program
  ------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include "easyfont.h"

#define LCID_FIXEDFONT 1L
#define BUFFER(x,y) (*(pBuffer + y * xMax + x))

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

CHAR szClientClass [] = "TypeAway" ;
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

VOID GetCharXY (HPS hps, SHORT *pcxChar, SHORT *pcyChar, SHORT *pcyDesc)
     {
     FONTMETRICS fm ;

     GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;
     *pcxChar = (SHORT) fm.lAveCharWidth ;
     *pcyChar = (SHORT) fm.lMaxBaselineExt ;
     *pcyDesc = (SHORT) fm.lMaxDescender ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOOL  fInsertMode = FALSE ;
     static CHAR  *pBuffer ;
     static SHORT cxClient, cyClient, cxChar, cyChar, cyDesc,
                  xCursor, yCursor, xMax,  yMax ;
     BOOL         fProcessed ;
     CHAR         szBuffer [20] ;
     HPS          hps ;
     POINTL       ptl ;
     RECTL        rcl ;
     SHORT        sRep, s ;

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

               GetCharXY (hps, &cxChar, &cyChar, &cyDesc) ;

               GpiSetCharSet (hps, LCID_DEFAULT) ;
               GpiDeleteSetId (hps, LCID_FIXEDFONT) ;
               WinReleasePS (hps) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;

               xMax = min (255, cxClient / cxChar) ;
               yMax = min (255, cyClient / cyChar - 2) ;

               if (pBuffer != NULL)
                    free (pBuffer) ;

               if (NULL == (pBuffer = malloc (xMax * yMax + 1)))
                    {
                    WinMessageBox (HWND_DESKTOP, hwnd,
                         "Cannot allocate memory for text buffer.\n"
                         "Try a smaller window.", szClientClass, 0,
                         MB_OK | MB_ICONEXCLAMATION) ;

                    xMax = yMax = 0 ;
                    }
               else
                    {                    
                    for (s = 0 ; s < xMax * yMax ; BUFFER (s++, 0) = ' ') ;

                    xCursor = 0 ;
                    yCursor = 0 ;
                    }

               if (hwnd == WinQueryFocus (HWND_DESKTOP, FALSE))
                    {
                    WinDestroyCursor (hwnd) ;

                    WinCreateCursor (hwnd, 0, cyClient - cyChar,
                                     cxChar, cyChar,
                                     CURSOR_SOLID | CURSOR_FLASH, NULL) ;

                    WinShowCursor (hwnd, xMax > 0 && yMax > 0) ;
                    }
               return 0 ;

          case WM_SETFOCUS:
               if (SHORT1FROMMP (mp2))
                    {
                    WinCreateCursor (hwnd, cxChar * xCursor,
                                     cyClient - cyChar * (1 + yCursor),
                                     cxChar, cyChar,
                                     CURSOR_SOLID | CURSOR_FLASH, NULL) ;

                    WinShowCursor (hwnd, xMax > 0 && yMax > 0) ;
                    }
               else
                    WinDestroyCursor (hwnd) ;
               return 0 ;

          case WM_CHAR:
               if (xMax == 0 || yMax == 0)
                    return 0 ;

               if (CHARMSG(&msg)->fs & KC_KEYUP)
                    return 0 ;

               if (CHARMSG(&msg)->fs & KC_INVALIDCHAR)
                    return 0 ;

               if (CHARMSG(&msg)->fs & KC_INVALIDCOMP)
                    {
                    xCursor = (xCursor + 1) % xMax ;        // Advance cursor
                    if (xCursor == 0)
                         yCursor = (yCursor + 1) % yMax ;

                    WinAlarm (HWND_DESKTOP, WA_ERROR) ;     // And beep
                    }

               for (sRep = 0 ; sRep < CHARMSG(&msg)->cRepeat ; sRep++)
                    {
                    fProcessed = FALSE ;

                    ptl.x = xCursor * cxChar ;
                    ptl.y = cyClient - cyChar * (yCursor + 1) + cyDesc ;

                              /*---------------------------
                                 Process some virtual keys
                                ---------------------------*/

                    if (CHARMSG(&msg)->fs & KC_VIRTUALKEY)
                         {
                         fProcessed = TRUE ;

                         switch (CHARMSG(&msg)->vkey)
                              {
                                        /*---------------
                                           Backspace key
                                          ---------------*/

                              case VK_BACKSPACE:
                                   if (xCursor > 0)
                                        {
                                        WinSendMsg (hwnd, WM_CHAR,
                                             MPFROM2SHORT (KC_VIRTUALKEY, 1),
                                             MPFROM2SHORT (0, VK_LEFT)) ;

                                        WinSendMsg (hwnd, WM_CHAR,
                                             MPFROM2SHORT (KC_VIRTUALKEY, 1),
                                             MPFROM2SHORT (0, VK_DELETE)) ;
                                        }
                                   break ;

                                        /*---------
                                           Tab key
                                          ---------*/

                              case VK_TAB:
                                   s = min (8 - xCursor % 8, xMax - xCursor) ;

                                   WinSendMsg (hwnd, WM_CHAR, 
                                        MPFROM2SHORT (KC_CHAR, s),
                                        MPFROM2SHORT ((USHORT) ' ', 0)) ;
                                   break ;

                                        /*-------------------------
                                           Backtab (Shift-Tab) key
                                          -------------------------*/

                              case VK_BACKTAB:
                                   if (xCursor > 0)
                                        {
                                        s = (xCursor - 1) % 8 + 1 ;

                                        WinSendMsg (hwnd, WM_CHAR,
                                             MPFROM2SHORT (KC_VIRTUALKEY, s),
                                             MPFROM2SHORT (0, VK_LEFT)) ;
                                        }
                                   break ;

                                        /*------------------------
                                           Newline and Enter keys
                                          ------------------------*/

                              case VK_NEWLINE:
                              case VK_ENTER:
                                   xCursor = 0 ;
                                   yCursor = (yCursor + 1) % yMax ;
                                   break ;

                              default:
                                   fProcessed = FALSE ;
                                   break ;
                              }
                         }

                              /*------------------------
                                 Process character keys
                                ------------------------*/

                    if (!fProcessed && CHARMSG(&msg)->fs & KC_CHAR)
                         {
                                                  // Shift line if fInsertMode
                         if (fInsertMode)
                              for (s = xMax - 1 ; s > xCursor ; s--)
                                   BUFFER (s, yCursor) =
                                        BUFFER (s - 1, yCursor) ;

                                                  // Store character in buffer

                         BUFFER (xCursor, yCursor) =
                                             (CHAR) CHARMSG(&msg)->chr ;

                                                  // Display char or new line

                         WinShowCursor (hwnd, FALSE) ;
                         hps = WinGetPS (hwnd) ;

                         EzfCreateLogFont (hps, LCID_FIXEDFONT,
                                           FONTFACE_COUR, FONTSIZE_10, 0) ;
                         GpiSetCharSet (hps, LCID_FIXEDFONT) ;
                         GpiSetBackMix (hps, BM_OVERPAINT) ;

                         if (fInsertMode)
                              GpiCharStringAt (hps, &ptl,
                                               (LONG) (xMax - xCursor),
                                               & BUFFER (xCursor, yCursor)) ;
                         else
                              GpiCharStringAt (hps, &ptl, 1L,
                                               (CHAR *) & CHARMSG(&msg)->chr) ;

                         GpiSetCharSet (hps, LCID_DEFAULT) ;
                         GpiDeleteSetId (hps, LCID_FIXEDFONT) ;
                         WinReleasePS (hps) ;
                         WinShowCursor (hwnd, TRUE) ;

                                                  // Increment cursor

                         if (!(CHARMSG(&msg)->fs & KC_DEADKEY))
                              if (0 == (xCursor = (xCursor + 1) % xMax))
                                   yCursor = (yCursor + 1) % yMax ;

                         fProcessed = TRUE ;
                         }

                              /*--------------------------------
                                 Process remaining virtual keys
                                --------------------------------*/

                    if (!fProcessed && CHARMSG(&msg)->fs & KC_VIRTUALKEY)
                         {
                         fProcessed = TRUE ;

                         switch (CHARMSG(&msg)->vkey)
                              {
                                        /*----------------------
                                           Cursor movement keys
                                          ----------------------*/

                              case VK_LEFT:
                                   xCursor = (xCursor - 1 + xMax) % xMax ;

                                   if (xCursor == xMax - 1)
                                        yCursor = (yCursor - 1 + yMax) % yMax ;
                                   break ;

                              case VK_RIGHT:
                                   xCursor = (xCursor + 1) % xMax ;

                                   if (xCursor == 0)
                                        yCursor = (yCursor + 1) % yMax ;
                                   break ;

                              case VK_UP:
                                   yCursor = max (yCursor - 1, 0) ;
                                   break ;

                              case VK_DOWN:
                                   yCursor = min (yCursor + 1, yMax - 1) ;
                                   break ;

			      case VK_PAGEUP:
                                   yCursor = 0 ;
                                   break ;

			      case VK_PAGEDOWN:
                                   yCursor = yMax - 1 ;
                                   break ;

                              case VK_HOME:
                                   xCursor = 0 ;
                                   break ;

                              case VK_END:
                                   xCursor = xMax - 1 ;
                                   break ;

                                        /*------------
                                           Insert key
                                          ------------*/

                              case VK_INSERT:
                                   fInsertMode = fInsertMode ? FALSE : TRUE ;
                                   WinSetRect (hab, &rcl, 0, 0,
                                               cxClient, cyChar) ;
                                   WinInvalidateRect (hwnd, &rcl, FALSE) ;
                                   break ;

                                        /*------------
                                           Delete key
                                          ------------*/

			      case VK_DELETE:
                                   for (s = xCursor ; s < xMax - 1 ; s++)
                                        BUFFER (s, yCursor) =
                                             BUFFER (s + 1, yCursor) ;

                                   BUFFER (xMax, yCursor) = ' ' ;

                                   WinShowCursor (hwnd, FALSE) ;
                                   hps = WinGetPS (hwnd) ;
                                   EzfCreateLogFont (hps, LCID_FIXEDFONT,
                                             FONTFACE_COUR, FONTSIZE_10, 0) ;
                                   GpiSetCharSet (hps, LCID_FIXEDFONT) ;
                                   GpiSetBackMix (hps, BM_OVERPAINT) ;

                                   GpiCharStringAt (hps, &ptl,
                                             (LONG) (xMax - xCursor), 
                                             & BUFFER (xCursor, yCursor)) ;

                                   GpiSetCharSet (hps, LCID_DEFAULT) ;
                                   GpiDeleteSetId (hps, LCID_FIXEDFONT) ;
                                   WinReleasePS (hps) ;
                                   WinShowCursor (hwnd, TRUE) ;
                                   break ;

                              default:
                                   fProcessed = FALSE ;
                                   break ;
                              }
                         }
                    }
               WinCreateCursor (hwnd, cxChar * xCursor,
                                      cyClient - cyChar * (1 + yCursor),
                                      0, 0, CURSOR_SETPOS, NULL) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;
               EzfCreateLogFont (hps, LCID_FIXEDFONT, FONTFACE_COUR,
                                                      FONTSIZE_10, 0) ;
               GpiSetCharSet (hps, LCID_FIXEDFONT) ;

               ptl.x = cxChar ;
               ptl.y = cyDesc ;
               GpiCharStringAt (hps, &ptl,
                                (LONG) sprintf (szBuffer, "Insert Mode: %s",
                                                fInsertMode ? "ON" : "OFF"),
                                szBuffer) ;

               ptl.x = 0 ;
               ptl.y = 3 * cyChar / 2 ;
               GpiMove (hps, &ptl) ;

               ptl.x = cxClient ;
               GpiLine (hps, &ptl) ;

               if (xMax > 0 && yMax > 0)
                    {
                    for (s = 0 ; s < yMax ; s++)
                         {
                         ptl.x = 0 ;
                         ptl.y = cyClient - cyChar * (s + 1) + cyDesc ;

                         GpiCharStringAt (hps, &ptl, (LONG) xMax,
                                                     & BUFFER (0, s)) ;
                         }
                    }
               GpiSetCharSet (hps, LCID_DEFAULT) ;
               GpiDeleteSetId (hps, LCID_FIXEDFONT) ;
               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               if (pBuffer != NULL)
                    free (pBuffer) ;
               break ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
