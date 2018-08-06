/*-------------------------------------------------------------------
   CHECKER2.C -- Mouse Hit-Test Demo Program with Keyboard Interface
  -------------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include <stdlib.h>

#define DIVISIONS 5

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "Checker2" ;
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

VOID DrawLine (HPS hps, LONG x1, LONG y1, LONG x2, LONG y2)
     {
     POINTL ptl ;

     ptl.x = x1 ;  ptl.y = y1 ;  GpiMove (hps, &ptl) ;
     ptl.x = x2 ;  ptl.y = y2 ;  GpiLine (hps, &ptl) ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOOL  fBlockState [DIVISIONS] [DIVISIONS] ;
     static SHORT xBlock, yBlock ;
     HPS          hps ;
     POINTL       ptl ;
     RECTL        rcl ;
     SHORT        x, y ;

     switch (msg)
          {
          case WM_SIZE:
               xBlock = SHORT1FROMMP (mp2) / DIVISIONS ;
               yBlock = SHORT2FROMMP (mp2) / DIVISIONS ;
               return 0 ;

          case WM_BUTTON1DOWN:
          case WM_BUTTON1DBLCLK:
               if (xBlock > 0 && yBlock > 0)
                    {
                    x = MOUSEMSG(&msg)->x / xBlock ;
                    y = MOUSEMSG(&msg)->y / yBlock ;

                    if (x < DIVISIONS && y < DIVISIONS)
                         {
                         fBlockState [x][y] = !fBlockState [x][y] ;

                         rcl.xRight = xBlock + (rcl.xLeft   = x * xBlock) ;
                         rcl.yTop   = yBlock + (rcl.yBottom = y * yBlock) ;

                         WinInvalidateRect (hwnd, &rcl, FALSE) ;
                         }
                    else
                         WinAlarm (HWND_DESKTOP, WA_WARNING) ;
                    }
               else
                    WinAlarm (HWND_DESKTOP, WA_WARNING) ;

               break ;                       // do default processing

          case WM_SETFOCUS:
               if (WinQuerySysValue (HWND_DESKTOP, SV_MOUSEPRESENT) == 0)
                    WinShowPointer (HWND_DESKTOP,
                                    SHORT1FROMMP (mp2) ? TRUE : FALSE) ;
               return 0 ;

          case WM_CHAR:
               if (xBlock == 0 || yBlock == 0)
                    break ;

               if (CHARMSG(&msg)->fs & KC_KEYUP)
                    break ;

               if (!(CHARMSG(&msg)->fs & KC_VIRTUALKEY))
                    break ;

               WinQueryPointerPos (HWND_DESKTOP, &ptl) ;
               WinMapWindowPoints (HWND_DESKTOP, hwnd, &ptl, 1) ;

               x = max (0, min (DIVISIONS - 1, (SHORT) ptl.x / xBlock)) ;
               y = max (0, min (DIVISIONS - 1, (SHORT) ptl.y / yBlock)) ;

               switch (CHARMSG(&msg)->vkey)
                    {
                    case VK_LEFT:
                         x-- ;
                         break ;

                    case VK_RIGHT:
                         x++ ;
                         break ;

                    case VK_DOWN:
                         y-- ;
                         break ;

                    case VK_UP:
                         y++ ;
                         break ;

                    case VK_HOME:
                         x = 0 ;
                         y = DIVISIONS - 1 ;
                         break ;

                    case VK_END:
                         x = DIVISIONS - 1 ;
                         y = 0 ;
                         break ;

                    case VK_NEWLINE:
		    case VK_ENTER:
                    case VK_SPACE:
                         WinSendMsg (hwnd, WM_BUTTON1DOWN, 
                              MPFROM2SHORT (x * xBlock, y * yBlock), NULL) ;
                         break ;

                    default:
                         return 0 ;
                    }
               x = (x + DIVISIONS) % DIVISIONS ;
               y = (y + DIVISIONS) % DIVISIONS ;

               ptl.x = x * xBlock + xBlock / 2 ;
               ptl.y = y * yBlock + yBlock / 2 ;

               WinMapWindowPoints (hwnd, HWND_DESKTOP, &ptl, 1) ;
               WinSetPointerPos (HWND_DESKTOP, (SHORT) ptl.x, (SHORT) ptl.y) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               if (xBlock > 0 && yBlock > 0)
                    for (x = 0 ; x < DIVISIONS ; x++)
                         for (y = 0 ; y < DIVISIONS ; y++)
                              {
                              rcl.xRight = xBlock + (rcl.xLeft   = x * xBlock);
                              rcl.yTop   = yBlock + (rcl.yBottom = y * yBlock);

                              WinDrawBorder (hps, &rcl, 1, 1,
                                             CLR_NEUTRAL, CLR_BACKGROUND,
                                             DB_STANDARD | DB_INTERIOR) ;

                              if (fBlockState [x][y])
                                   {
                                   DrawLine (hps, rcl.xLeft,  rcl.yBottom,
                                                  rcl.xRight, rcl.yTop) ;

                                   DrawLine (hps, rcl.xLeft,  rcl.yTop,
                                                  rcl.xRight, rcl.yBottom) ;
                                   }
                              }
               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
