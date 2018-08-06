/*---------------------------------------------------------------
   CKRBOARD.C -- BoardWndProc for user interaction, Version 0.30
                 (c) 1990, Charles Petzold
  ---------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include <stdlib.h>
#include "checkers.h"
#include "ckrdraw.h"

MRESULT EXPENTRY BoardWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOOL     fMovingPiece ;
     static HPS      hps ;
     static HPOINTER hptrUpHand, hptrDnHand, hptrArrow ;
     static HWND     hwndJudge ;
     static POINTL   ptlLast ;
     static SHORT    sBottom = BLACK, sColor = -1, sKing = 0 ;
     BOARD           brd ;
     POINTL          ptlMouse ;
     SHORT           x, y, i ;

     switch (msg)
          {
          case WM_CREATE:
               hps = CkdCreatePS (hwnd) ;

               hptrUpHand = WinLoadPointer (HWND_DESKTOP, NULL, IDP_UPHAND) ;
               hptrDnHand = WinLoadPointer (HWND_DESKTOP, NULL, IDP_DNHAND) ;
               hptrArrow  = WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW,
                                                FALSE) ;
               return 0 ;

          case WM_SIZE:
               CkdResizePS (hps, hwnd) ;
               CkdDestroyPieces () ;
               CkdCreatePieces (hps) ;
               return 0 ;

          case WM_TELL_BOARD_JUDGE_HANDLE:
               hwndJudge = HWNDFROMMP (mp1) ;
               return 0 ;

          case WM_TELL_BOARD_NEW_ORIENTATION:
               sBottom = SHORT1FROMMP (mp1) ;
               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_TELL_BOARD_COLOR_DIALOG:
               if (!WinDlgBox (HWND_DESKTOP, hwnd, ColorDlgProc,
                               NULL, IDD_COLOR_DLG, mp1))
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

          case WM_BUTTON1UP:
               if (sColor == -1)
                    return 0 ;

               WinSetActiveWindow (HWND_DESKTOP, hwnd) ;

                                        // get mouse coords and index

               ptlMouse.x = MOUSEMSG(&msg)->x ;
               ptlMouse.y = MOUSEMSG(&msg)->y ;
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

                    sKing = (SHORT) WinSendMsg (hwndJudge,
                                        WM_QUERY_JUDGE_IF_KING, NULL, NULL) ;

                              // Remove the mouse pointer

                    WinSetPointer (HWND_DESKTOP, NULL) ;

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

                    sKing = (SHORT) WinSendMsg (hwndJudge,
                                        WM_QUERY_JUDGE_IF_KING, NULL, NULL) ;

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
                         WinSetPointer (HWND_DESKTOP, hptrArrow) ;
                         WinSendMsg (hwndJudge, WM_TELL_JUDGE_BOARD_MOVE_ENDED,
                                     NULL, NULL) ;
                         }
                    }

               return 0 ;

          case WM_MOUSEMOVE:
               ptlMouse.x = MOUSEMSG(&msg)->x ;
               ptlMouse.y = MOUSEMSG(&msg)->y ;

                              // set the mouse pointer and move the piece

               if (fMovingPiece)
                    {
                    WinSetPointer (HWND_DESKTOP, NULL) ;
                    CkdDragMove (hps, &ptlLast, &ptlMouse, sColor, sKing) ;
                    ptlLast = ptlMouse ;
                    }

               else if (sColor == -1)
                    WinSetPointer (HWND_DESKTOP, hptrArrow) ;

               else
                    WinSetPointer (HWND_DESKTOP,
                         sBottom ^ sColor ? hptrDnHand : hptrUpHand) ;

               return 0 ;

          case WM_SETFOCUS:
                              // set the mouse pointer

               if (WinQuerySysValue (HWND_DESKTOP, SV_MOUSEPRESENT) == 0)
                    WinShowPointer (HWND_DESKTOP,
                                    SHORT1FROMMP (mp2) ? TRUE : FALSE) ;

               if (fMovingPiece)
                    WinSetPointer (HWND_DESKTOP, NULL) ;

               else if (sColor == -1)
                    WinSetPointer (HWND_DESKTOP, hptrArrow) ;

               else
                    WinSetPointer (HWND_DESKTOP,
                         sBottom ^ sColor ? hptrDnHand : hptrUpHand) ;
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

          case WM_JUDGE_SAYS_REMOVE_PIECE:
               i = SHORT1FROMMP (mp1) ;
               CkdConvertIndexToCoords (i, &x, &y, sBottom) ;
               CkdErasePiece (hps, x, y) ;
               return 0 ;

          case WM_JUDGE_SAYS_RESET_BOARD:
               fMovingPiece = FALSE ;
               sColor = -1 ;
               WinSetPointer (HWND_DESKTOP, hptrArrow) ;
               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_JUDGE_SAYS_MOVE_BLACK:
               sColor = BLACK ;
               WinSetPointer (HWND_DESKTOP,
                              sBottom == BLACK ? hptrUpHand : hptrDnHand) ;
               return 0 ;

          case WM_JUDGE_SAYS_MOVE_WHITE:
               sColor = WHITE ;
               WinSetPointer (HWND_DESKTOP,
                              sBottom == BLACK ? hptrDnHand : hptrUpHand) ;
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
