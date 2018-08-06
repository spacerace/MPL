/*--------------------------------------------
   TYPECLIP.C -- Clipboard Text Demonstration
  --------------------------------------------*/

#define INCL_WIN
#define INCL_VIO
#define INCL_AVIO
#include <os2.h>
#include <stdlib.h>
#include "typeclip.h"

#define WM_ADJUST_ORG     (WM_USER + 0)
#define WM_SET_BLOCKOUT   (WM_USER + 1)
#define WM_CLEAR_BLOCKOUT (WM_USER + 2)

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

HAB  hab ;

int main (void)
     {
     static CHAR  szClientClass [] = "TypeClip" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_MENU          | FCF_ACCELTABLE ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, 0L, 0) ;

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

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOOL  fCapture, fValidBlock ;
     static HPS   hps ;
     static HVPS  hvps ;
     static HWND  hwndMenu ;
     static LONG  cxChar, cyChar ;
     static SHORT cxClient, cyClient, cxVioSize, cyVioSize,
                  xBlockBeg, yBlockBeg, xBlockEnd, yBlockEnd,
                  xBeg , yBeg, xEnd, yEnd ;
     CHAR         *pchText ;
     HDC          hdc ;
     PCHAR        pchClipText ;
     RECTL        rcl ;
     SEL          selClipText ;
     SHORT        sRep, sLen, x, y, s,
                  xCursor, yCursor, xOrigin, yOrigin ;
     SIZEL        sizl ;
     USHORT       usfInfo ;

     switch (msg)
          {
                              /*---------------------------------------
                                 Create, paint, and destroy processing
                                ---------------------------------------*/
          case WM_CREATE:
               hdc = WinOpenWindowDC (hwnd) ;

               DevQueryCaps (hdc, CAPS_CHAR_WIDTH,  1L, &cxChar) ;
               DevQueryCaps (hdc, CAPS_CHAR_HEIGHT, 1L, &cyChar) ;

               cxVioSize = (SHORT) (WinQuerySysValue (HWND_DESKTOP,
                                        SV_CXFULLSCREEN) / cxChar) ;

               cyVioSize = (SHORT) (WinQuerySysValue (HWND_DESKTOP,
                                        SV_CYFULLSCREEN) / cyChar) ;

               sizl.cx = sizl.cy = 0 ;
               hps = GpiCreatePS (hab, hdc, &sizl, PU_PELS    | GPIF_DEFAULT |
                                                   GPIT_MICRO | GPIA_ASSOC) ;

               VioCreatePS (&hvps, cyVioSize, cxVioSize, 0, 1, NULL) ;
               VioAssociate (hdc, hvps) ;

               hwndMenu = WinWindowFromID (
                                WinQueryWindow (hwnd, QW_PARENT, FALSE),
                                FID_MENU) ;
               return 0 ;

          case WM_PAINT:
               WinBeginPaint (hwnd, hps, NULL) ;

               WinQueryWindowRect (hwnd, &rcl) ;
               WinFillRect (hps, &rcl, CLR_BLACK) ;

               VioShowBuf (0, cxVioSize * cyVioSize * 2, hvps) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               VioAssociate (NULL, hvps) ;
               VioDestroyPS (hvps) ;
               GpiDestroyPS (hps) ;
               return 0 ;
                              /*-------------------------------------------
                                 Window size, keyboard and origin handling
                                -------------------------------------------*/
          case WM_SIZE:
               WinDefAVioWindowProc (hwnd, msg, mp1, mp2) ;

               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;

               VioSetOrg (0, 0, hvps) ;
               WinSendMsg (hwnd, WM_ADJUST_ORG, NULL, NULL) ;
               return 0 ;

          case WM_CHAR:
               if (!(CHARMSG(&msg)->fs & KC_CHAR)        ||
                    (CHARMSG(&msg)->fs & KC_KEYUP)       ||
                    (CHARMSG(&msg)->fs & KC_INVALIDCHAR) ||
                    (CHARMSG(&msg)->fs & KC_DEADKEY))
                         return 0 ;

               for (sRep = 0 ; sRep < CHARMSG(&msg)->cRepeat ; sRep++)
                    {
                    VioWrtTTY ((PCHAR) & CHARMSG(&msg)->chr, 1, hvps) ;

                    switch (CHARMSG(&msg)->chr)
                         {
                         case '\b':                         // Backspace
                              VioWrtTTY (" \b", 2, hvps) ;
                              break ;

                         case '\r':                         // Return
                              VioWrtTTY ("\n", 1, hvps) ;
                              break ;
                         }
                    }
               WinSendMsg (hwnd, WM_ADJUST_ORG, NULL, NULL) ;
               return 1 ;

          case WM_ADJUST_ORG:
               VioGetOrg    (&yOrigin, &xOrigin, hvps) ;
               VioGetCurPos (&yCursor, &xCursor, hvps) ;

               if (xCursor < xOrigin)
                    xOrigin = xCursor ;

               else if (xCursor >= xOrigin + cxClient / (SHORT) cxChar)
                    xOrigin = xCursor - cxClient / (SHORT) cxChar + 1 ;

               if (yCursor < yOrigin)
                    yOrigin = yCursor ;

               else if (yCursor >= yOrigin + cyClient / (SHORT) cyChar)
                    yOrigin = yCursor - cyClient / (SHORT) cyChar + 1 ;

               VioSetOrg (yOrigin, xOrigin, hvps) ;
               return 0 ;
                              /*-----------------------------
                                 Mouse and blockout handling
                                -----------------------------*/
          case WM_BUTTON1DOWN:
               x = MOUSEMSG(&msg)->x / (SHORT) cxChar ;
               y = (cyClient - MOUSEMSG(&msg)->y) / (SHORT) cyChar ;

               WinSendMsg (hwnd, WM_CLEAR_BLOCKOUT, NULL, NULL) ;

               WinSetCapture (HWND_DESKTOP, hwnd) ;
               fCapture = TRUE ;
               fValidBlock = FALSE ;

               VioGetOrg (&yOrigin, &xOrigin, hvps) ;

               xBlockBeg = xOrigin + x ;
               yBlockBeg = yOrigin + y ;

               WinSendMsg (hwnd, WM_SET_BLOCKOUT, mp1, mp2) ;
               break ;

          case WM_MOUSEMOVE:
               if (fCapture)
                    WinSendMsg (hwnd, WM_SET_BLOCKOUT, mp1, mp2) ;
               break ;

          case WM_BUTTON1UP:
               if (fCapture)
                    {
                    WinSetCapture (HWND_DESKTOP, NULL) ;
                    fCapture = FALSE ;
                    fValidBlock = TRUE ;

                    WinSendMsg (hwnd, WM_SET_BLOCKOUT, mp1, mp2) ;
                    }
               return 1 ;

          case WM_SET_BLOCKOUT:
               x = MOUSEMSG(&msg)->x / (SHORT) cxChar ;
               y = (cyClient - MOUSEMSG(&msg)->y) / (SHORT) cyChar ;

               VioGetOrg (&yOrigin, &xOrigin, hvps) ;

               xBlockEnd = xOrigin + x ;
               yBlockEnd = yOrigin + y ;

               if (cxVioSize * yBlockBeg + xBlockBeg <
                   cxVioSize * yBlockEnd + xBlockEnd)
                    {
                    xBeg = xBlockBeg ;
                    yBeg = yBlockBeg ;
                    xEnd = xBlockEnd ;
                    yEnd = yBlockEnd ;
                    }
               else
                    {
                    xBeg = xBlockEnd ;
                    yBeg = yBlockEnd ;
                    xEnd = xBlockBeg ;
                    yEnd = yBlockBeg ;
                    }

               VioWrtNAttr ("\x07", cxVioSize * yBeg + xBeg, 0, 0, hvps) ;

               VioWrtNAttr ("\x70",  cxVioSize * yEnd + xEnd -
                                    (cxVioSize * yBeg + xBeg) + 1,
                                    yBeg, xBeg, hvps) ;

               VioWrtNAttr ("\x07",  cxVioSize * cyVioSize -
                                    (cxVioSize * yEnd + xEnd) - 1,
                                    yEnd, xEnd + 1, hvps) ;
               break ;

          case WM_CLEAR_BLOCKOUT:
               VioWrtNAttr ("\x07", cxVioSize * cyVioSize, 0, 0, hvps) ;
               fValidBlock = FALSE ;
               break ;
                              /*--------------------
                                 Clipboard handling
                                --------------------*/
          case WM_INITMENU:
               switch (SHORT1FROMMP (mp1))
                    {
                    case IDM_EDIT:
                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (IDM_CUT, TRUE),
                                     MPFROM2SHORT (MIA_DISABLED,
                                          fValidBlock ? 0 : MIA_DISABLED)) ;

                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (IDM_COPY, TRUE),
                                     MPFROM2SHORT (MIA_DISABLED,
                                          fValidBlock ? 0 : MIA_DISABLED)) ;

                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (IDM_CLEAR, TRUE),
                                     MPFROM2SHORT (MIA_DISABLED,
                                          fValidBlock ? 0 : MIA_DISABLED)) ;

                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (IDM_PASTE, TRUE),
                                     MPFROM2SHORT (MIA_DISABLED,
                              WinQueryClipbrdFmtInfo (hab, CF_TEXT, &usfInfo)
                                             ? 0 : MIA_DISABLED)) ;
                         return 0 ;
                    }
               break ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_CUT:
                    case IDM_COPY:
                         if (!fValidBlock)
                              return 0 ;
                                                       // Copy text to
                                                       // malloc'ed memory

                         sLen = (cxVioSize + 2) * (yEnd - yBeg + 1) + 1 ;
                         pchText = malloc (sLen) ;

                         for (s = 0, y = yBeg ; y <= yEnd ; y++)
                              {
                              x    = (y == yBeg ? xBeg : 0) ;
                              sLen = (y == yEnd ? xEnd + 1 : cxVioSize) - x ;

                              VioReadCharStr (pchText + s, &sLen,
                                              y, x, hvps) ;

                              s += sLen ;
                              while (--s >= 0 && pchText[s] == ' ') ;

                              s++ ;
                              pchText[s++] = '\r' ;
                              pchText[s++] = '\n' ;
                              }
                         pchText[s++] = '\0' ;         // s is string length

                                                       // Allocate memory block

                         DosAllocSeg (s, &selClipText, SEG_GIVEABLE) ;
                         pchClipText = MAKEP (selClipText, 0) ;

                                                       // Copy to giveable seg

                         for (s = 0 ; pchClipText[s] = pchText[s] ; s++) ;
                         free (pchText) ;
                                                       // Set clipboard data
                         WinOpenClipbrd (hab) ;
                         WinEmptyClipbrd (hab) ;
                         WinSetClipbrdData (hab, (ULONG) selClipText,
                                                 CF_TEXT, CFI_SELECTOR) ;
                         WinCloseClipbrd (hab) ;
                                                       // Clear blockout

                         if (COMMANDMSG(&msg)->cmd == IDM_COPY)
                              {
                              WinSendMsg (hwnd, WM_CLEAR_BLOCKOUT, NULL, NULL);
                              return 0 ;
                              }
                                             // fall through for IDM_CUT
                    case IDM_CLEAR:
                         if (!fValidBlock)
                              return 0 ;
                                                       // Clear selection

                         VioWrtNCell (" \x07",  cxVioSize * yEnd + xEnd -
                                               (cxVioSize * yBeg + xBeg) + 1,
                                               yBeg, xBeg, hvps) ;
                         fValidBlock = FALSE ;
                         return 0 ;

                    case IDM_PASTE:
                                        // Get text selector from clipboard

                         WinOpenClipbrd (hab) ;
                         selClipText = (SEL) WinQueryClipbrdData (hab,
                                                                  CF_TEXT) ;
                                        // Display to screen

                         if (selClipText != 0)
                              {
                              pchClipText = MAKEP (selClipText, 0) ;

                              for (sLen = 0 ; pchClipText[sLen] ; sLen++) ;

                              VioWrtTTY (pchClipText, sLen, hvps) ;
                              }
                         WinCloseClipbrd (hab) ;

                         WinSendMsg (hwnd, WM_ADJUST_ORG, NULL, NULL) ;
                         return 0 ;
                    }
               break ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
