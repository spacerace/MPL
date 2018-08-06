/*---------------------------------------------------------------
   CKRBOARD.C -- BoardWndProc for user interaction, Version 0.40
                 (c) 1990, Charles Petzold
  ---------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include <stdlib.h>
#include "checkers.h"
#include "ckrdraw.h"

#define TIMER_ID      1
#define TIMER_STEPS  10
#define TIMER_TIME  100

extern HAB hab ;

MRESULT EXPENTRY BoardWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOOL     fMovingPiece ;
     static HPS      hps ;
     static HPOINTER hptr, hptrUpHand, hptrDnHand, hptrArrow, hptrWait ;
     static HWND     hwndJudge ;
     static POINTL   ptlMouse, ptlLast ;
     static SHORT    sBottom = BLACK, sColor = -1, sKing = 0,
                     sTimerBeg, sTimerEnd, sTimerColor, sTimerKing, sTimerInc ;
     BOARD           brd ;
     NEWGAME         *pnewgame ;
     SHORT           x, y, i ;

     switch (msg)
          {
          case WM_CREATE:
               hps = CkdCreatePS (hwnd) ;

               hptrUpHand = WinLoadPointer (HWND_DESKTOP, 0, IDP_UPHAND) ;
               hptrDnHand = WinLoadPointer (HWND_DESKTOP, 0, IDP_DNHAND) ;
               hptrArrow  = WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW,
                                                FALSE) ;
               hptrWait   = WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT,
                                                FALSE) ;
               return 0 ;

          case WM_SIZE:
               CkdResizePS (hps, hwnd) ;
               CkdDestroyPieces () ;
               CkdCreatePieces (hps) ;
               return 0 ;

          case WM_NEW_GAME:
               pnewgame  = PVOIDFROMMP (mp1) ;
               hwndJudge = pnewgame->hwndJudge ;
               sBottom   = pnewgame->sBottom ;

               fMovingPiece = FALSE ;
               sColor       = -1 ;
               hptr         = hptrArrow ;

               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_JUDGE_SAYS_MAKE_MOVE:
               sColor = SHORT1FROMMP (mp2) ;
               hptr   = (sBottom == sColor ? hptrUpHand : hptrDnHand) ;

               WinSetPointer (HWND_DESKTOP, hptr) ;
               return 0 ;

          case WM_JUDGE_SAYS_SHOW_HOURGLASS:
               WinSetPointer (HWND_DESKTOP, hptr = hptrWait) ;
               return 0 ;

          case WM_JUDGE_SAYS_SHOW_ARROW:
               WinSetPointer (HWND_DESKTOP, hptr = hptrArrow) ;
               return 0 ;

          case WM_BUTTON1DOWN:          // save mouse coordinates

               ptlMouse.x = MOUSEMSG(&msg)->x ;
               ptlMouse.y = MOUSEMSG(&msg)->y ;
               break ;

          case WM_BUTTON1UP:
               if (sColor == -1)
                    return 0 ;
                                        // get index from saved mouse coords

               CkdQueryHitCoords (hps, ptlMouse, &x, &y) ;
               i = CkdConvertCoordsToIndex (x, y, sBottom) ;

               if (i == -1)             // didn't hit black square
                    {
                    WinAlarm (HWND_DESKTOP, WA_ERROR) ;
                    return 0 ;
                    }

               if (!fMovingPiece)       // ie, picking up piece
                    {
                    if (!WinSendMsg (hwndJudge, WM_QUERY_JUDGE_PICKUP_PIECE,
                                     MPFROMSHORT (i), NULL))
                         {
                         WinAlarm (HWND_DESKTOP, WA_ERROR) ;
                         return 0 ;
                         }

                    sKing = SHORT1FROMMR (
                              WinSendMsg (hwndJudge,
                                          WM_QUERY_JUDGE_IF_KING, NULL, NULL));

                              // Remove the mouse pointer

                    WinSetPointer (HWND_DESKTOP, hptr = NULL) ;

                              // Erase, save area, and show piece at mouse

                    CkdErasePiece (hps, x, y) ;
                    CkdDragSave (hps, &ptlMouse, sKing) ;
                    CkdDragShow (hps, &ptlMouse, sColor, sKing) ;

                              // Prepare for WM_MOUSEMOVE

                    fMovingPiece = TRUE ;
                    ptlLast = ptlMouse ;
                    }

               else           // ie, attempt to set down piece
                    {
                    if (!WinSendMsg (hwndJudge, WM_QUERY_JUDGE_PUTDOWN_PIECE,
                                     MPFROMSHORT (i), NULL))
                         {
                         WinAlarm (HWND_DESKTOP, WA_ERROR) ;
                         return 0 ;
                         }
                              // restore area

                    CkdDragRestore (hps, &ptlMouse, sKing) ;

                    sKing = SHORT1FROMMR (
                              WinSendMsg (hwndJudge,
                                          WM_QUERY_JUDGE_IF_KING, NULL, NULL));

                              // set down the piece on the square

                    CkdDragDeposit (hps, x, y, sColor, sKing) ;

                              // check for continued jumps

                    if (WinSendMsg (hwndJudge, WM_QUERY_JUDGE_CONTINUE_MOVE,
                                    MPFROMSHORT (i), NULL))
                         {
                         CkdErasePiece (hps, x, y) ;
                         CkdDragSave (hps, &ptlLast, sKing) ;
                         }
                    else           // the move is over
                         {
                         fMovingPiece = FALSE ;
                         sColor       = -1 ;

                         WinSetPointer (HWND_DESKTOP, hptr = hptrArrow) ;
                         WinSendMsg (hwndJudge, WM_TELL_JUDGE_BOARD_MOVE_ENDED,
                                     NULL, NULL) ;
                         }
                    }

               return 0 ;

          case WM_MOUSEMOVE:
               WinSetPointer (HWND_DESKTOP, hptr) ;

                              // move the piece

               if (fMovingPiece)
                    {
                    ptlMouse.x = MOUSEMSG(&msg)->x ;
                    ptlMouse.y = MOUSEMSG(&msg)->y ;

                    CkdDragMove (hps, &ptlLast, &ptlMouse, sColor, sKing) ;
                    ptlLast = ptlMouse ;
                    }

               return 0 ;

          case WM_CHAR:
               if (CHARMSG(&msg)->fs & KC_KEYUP)
                    return 0 ;

               if (!(CHARMSG(&msg)->fs & KC_VIRTUALKEY))
                    return 0 ;

                         // convert pointer position to x, y coords

               WinQueryPointerPos (HWND_DESKTOP, &ptlMouse) ;
               WinMapWindowPoints (HWND_DESKTOP, hwnd, &ptlMouse, 1) ;
               CkdQueryNearestXYFromPoint (hps, &ptlMouse, &x, &y) ;

                         // move the coordinates

               switch (CHARMSG(&msg)->vkey)
                    {
                    case VK_HOME:      x = 0 ;  y = 7 ;  break ;
                    case VK_END:       x = 0 ;  y = 0 ;  break ;
                    case VK_PAGEUP:    x = 7 ;  y = 7 ;  break ;
                    case VK_PAGEDOWN:  x = 7 ;  y = 0 ;  break ;

                    case VK_UP:        y = min (y + 1, 7) ;  break ;
                    case VK_DOWN:      y = max (y - 1, 0) ;  break ;
                    case VK_RIGHT:     x = min (x + 1, 7) ;  break ;
                    case VK_LEFT:      x = max (x - 1, 0) ;  break ;

                    case VK_SPACE:     break ;

                    default:           return 0 ;
                    }
                         // process keystrokes like mouse messages

               CkdQuerySlightOffsetFromXY (hps, x, y, &ptlMouse) ;

               switch (CHARMSG(&msg)->vkey)
                    {
                    case VK_SPACE:
                         WinSendMsg (hwnd, WM_BUTTON1DOWN,
                                     MPFROM2SHORT ((SHORT) ptlMouse.x,
                                                   (SHORT) ptlMouse.y), NULL) ;

                         WinSendMsg (hwnd, WM_BUTTON1UP,
                                     MPFROM2SHORT ((SHORT) ptlMouse.x,
                                                   (SHORT) ptlMouse.y), NULL) ;
                         break ;

                    default:
                         WinMapWindowPoints (hwnd, HWND_DESKTOP, &ptlMouse, 1) ;
                         WinSetPointerPos (HWND_DESKTOP, (SHORT) ptlMouse.x,
                                                         (SHORT) ptlMouse.y) ;
                         break ;
                    }
               return 0 ;

          case WM_JUDGE_SAYS_MOVE_PIECE:
               sTimerBeg   = SHORT1FROMMP (mp1) ;
               sTimerEnd   = SHORT2FROMMP (mp1) ;
               sTimerColor = SHORT1FROMMP (mp2) ;
               sTimerKing  = SHORT2FROMMP (mp2) ;
               sTimerInc   = 0 ;

               WinStartTimer (hab, hwnd, TIMER_ID, TIMER_TIME) ;
               return 0 ;

          case WM_TIMER:
               if (sTimerInc == 0)
                    {
                    CkdConvertIndexToCoords (sTimerBeg, &x, &y, sBottom) ;
                    CkdErasePiece (hps, x, y) ;
                    CkdExternalSave (hps, x, y, sTimerKing) ;
                    CkdExternalShow (hps, x, y, sTimerColor, sTimerKing) ;
                    }

               CkdExternalMove (hps, sTimerBeg,  sTimerEnd, sTimerColor,
                                     sTimerKing, sBottom,   sTimerInc,
                                     TIMER_STEPS) ;

               if (++sTimerInc >= TIMER_STEPS)
                    {
                    CkdConvertIndexToCoords (sTimerEnd, &x, &y, sBottom) ;
                    CkdExternalRestore (hps, x, y, sTimerKing) ;
                    CkdDragDeposit (hps, x, y, sTimerColor, sTimerKing) ;
                    WinStopTimer (hab, hwnd, TIMER_ID) ;

                    WinSendMsg (hwndJudge, WM_TELL_JUDGE_PIECE_MOVED,
                                NULL, NULL) ;
                    }
               return 0 ;
                                   // need to king piece if new king !


          case WM_JUDGE_SAYS_REMOVE_PIECE:
               i = SHORT1FROMMP (mp1) ;
               CkdConvertIndexToCoords (i, &x, &y, sBottom) ;
               CkdErasePiece (hps, x, y) ;
               return 0 ;

          case WM_JUDGE_SAYS_KING_PIECE:
               i = SHORT1FROMMP (mp1) ;
               CkdConvertIndexToCoords (i, &x, &y, sBottom) ;
               CkdErasePiece (hps, x, y) ;
               CkdDragDeposit (hps, x, y, sTimerColor, 1) ;
               return 0 ;

          case WM_TELL_BOARD_COLOR_DIALOG:
               if (!WinDlgBox (HWND_DESKTOP, hwnd, ColorDlgProc,
                               0, IDD_COLOR_DLG, mp1))
                    return 0 ;

               if (SHORT1FROMMP (mp2))
                    {
                    CkdDestroyPieces () ;
                    CkdCreatePieces (hps) ;
                    }
               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_TELL_BOARD_STANDARD_COLORS:
               CkdSetStandardColors () ;
               CkdDestroyPieces () ;
               CkdCreatePieces (hps) ;
               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_SETFOCUS:
                              // set the mouse pointer

               if (WinQuerySysValue (HWND_DESKTOP, SV_MOUSEPRESENT) == 0)
                    WinShowPointer (HWND_DESKTOP,
                                    SHORT1FROMMP (mp2) ? TRUE : FALSE) ;

               WinSetPointer (HWND_DESKTOP, hptr) ;
               return 0 ;

          case WM_PAINT:
               WinBeginPaint (hwnd, hps, NULL) ;

               WinSendMsg (hwndJudge, WM_QUERY_JUDGE_CURRENT_BOARD,
                           MPFROMP (&brd), NULL) ;

               CkdDrawWindowBackground (hps, hwnd) ;
               CkdDrawWholeBoard (hps) ;
               CkdDrawAllPieces (hps, &brd, sBottom) ;

               if (fMovingPiece)
                    {
                    WinQueryPointerPos (HWND_DESKTOP, &ptlMouse) ;
                    WinMapWindowPoints (HWND_DESKTOP, hwnd, &ptlMouse, 1) ;
                    CkdDragSave (hps, &ptlMouse, sKing) ;
                    CkdDragShow (hps, &ptlMouse, sColor, sKing) ;

                    ptlLast = ptlMouse ;
                    }

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               CkdDestroyPieces () ;
               CkdDestroyPS (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
