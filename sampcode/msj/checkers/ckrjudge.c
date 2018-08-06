/*---------------------------------------------------------------
   CKRJUDGE.C -- JudgeWndProc for controlling game, Version 0.40
                 (c) 1990, Charles Petzold
  ---------------------------------------------------------------*/

#include <os2.h>
#include "checkers.h"
#include "ckrmoves.h"
#include "ckrstrat.h"

MRESULT EXPENTRY JudgeWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOOL  fKing, fNewKing, fMustJump ;
     static BOARD brd = { 0x00000FFF, 0xFFF00000, 0x00000000 }, brdLast ;
     static HWND  hwndPlayer [2], hwndBoard, hwndClient ;
     static MOVE  move ;
     static SHORT sColor = BLACK, iBegin, iEnd, iJump, iBoardMove ;
     NEWGAME      *pnewgame ;
     ULONG        ulBit, ulMove, ulJump ;

     switch (msg)
          {
          case WM_NEW_GAME:
               pnewgame = PVOIDFROMMP (mp1) ;

               hwndPlayer [BLACK] = pnewgame->hwndBlack ;
               hwndPlayer [WHITE] = pnewgame->hwndWhite ;
               hwndBoard          = pnewgame->hwndBoard ;
               hwndClient         = pnewgame->hwndClient ;
               brd                = pnewgame->brd ;

               WinSendMsg (hwndPlayer [BLACK], WM_NEW_GAME, mp1, mp2) ;
               WinSendMsg (hwndPlayer [WHITE], WM_NEW_GAME, mp1, mp2) ;

               if (hwndBoard != hwndPlayer [BLACK] &&
                   hwndBoard != hwndPlayer [WHITE])
                    {
                    WinSendMsg (hwndBoard, WM_NEW_GAME, mp1, mp2) ;
                    }

               sColor = BLACK ;

               move.sColor          = sColor ;
               move.sKing           = 0 ;
               move.cSubMoves       = 0 ;
               move.fNewKing        = FALSE ;
               move.asubmove[0].brd = brd ;

               WinSendMsg (hwndPlayer [sColor],
                           WM_JUDGE_SAYS_MAKE_MOVE,
                           MPFROMP (&move), MPFROMSHORT (sColor)) ;

               if (hwndPlayer [sColor] != hwndBoard)
                    WinSendMsg (hwndBoard, WM_JUDGE_SAYS_SHOW_HOURGLASS,
                                NULL, NULL) ;

               return 0 ;

          case WM_QUERY_JUDGE_CURRENT_BOARD:
               * (PBOARD) PVOIDFROMMP(mp1) = brd ;
               return 0 ;

          case WM_QUERY_JUDGE_PICKUP_PIECE:
               iBegin    = SHORT1FROMMP (mp1) ;
               ulBit     = 1L << iBegin ;
               fKing     = brd.ulKing & ulBit ? TRUE : FALSE ;
               fMustJump = FALSE ;
               fNewKing  = FALSE ;
               ulMove    = CkmQueryAllMoveablePieces (&brd, sColor) ;
               ulJump    = CkmQueryAllJumpablePieces (&brd, sColor) ;

               if (ulJump != 0)         // ie, some possible jumps
                    {
                    if (!(ulBit & ulJump))
                         return FALSE ;      // invalid piece for jumping

                    fMustJump = TRUE ;
                    }
               else                     // no possible jumps
                    {
                    if (!(ulBit & ulMove))   // invalid piece for moving
                         return FALSE ;
                    }
                                        // save the board and adjust it
               brdLast = brd ;

               if (sColor == BLACK)
                    brd.ulBlack &= ~ulBit ;
               else
                    brd.ulWhite &= ~ulBit ;

               brd.ulKing &= ~ulBit ;

               return (MRESULT) TRUE ;

          case WM_QUERY_JUDGE_IF_KING:
               return (MRESULT) fKing ;

          case WM_QUERY_JUDGE_PUTDOWN_PIECE:
               ulBit  = 1L << iBegin ;
               ulMove = CkmQueryMoveDestinations (&brdLast, sColor, ulBit) ;
               ulJump = CkmQueryJumpDestinations (&brdLast, sColor, ulBit) ;
               iEnd   = SHORT1FROMMP (mp1) ;
               ulBit  = 1L << iEnd ;

               if (fMustJump)
                    {
                    if (!(ulBit & ulJump))
                         return FALSE ;      // not a valid jump destination

                    iJump = CkmQueryJumpedPiece (iBegin, iEnd) ;
                    }
               else
                    {
                    if (!(ulBit & ulMove))
                         return FALSE ;      // not a valid move destination

                    iJump = -1 ;
                    }
                                             // adjust board
               brdLast = brd ;

               if (sColor == BLACK)
                    {
                    brd.ulBlack |= ulBit ;

                    if (iJump != -1)
                         brd.ulWhite &= ~(1L << iJump) ;

                    if (iEnd >= 28 || fKing == TRUE)
                         {
                         brd.ulKing |= ulBit ;

                         if (!fKing)
                              fNewKing = TRUE ;
                         }
                    }
               else      // (sColor == WHITE)
                    {
                    brd.ulWhite |= ulBit ;

                    if (iJump != -1)
                         brd.ulBlack &= ~(1L << iJump) ;

                    if (iEnd <= 3 || fKing == TRUE)
                         {
                         brd.ulKing |= ulBit ;

                         if (!fKing)
                              fNewKing = TRUE ;
                         }
                    }

               if (fNewKing)
                    fKing = TRUE ;
                                             // inform board of jumped piece
               if (iJump != -1)
                    WinSendMsg (hwndBoard, WM_JUDGE_SAYS_REMOVE_PIECE,
                                MPFROMSHORT (iJump), NULL) ;

               return (MRESULT) TRUE ;

          case WM_QUERY_JUDGE_CONTINUE_MOVE:
               if (fNewKing)
                    return FALSE ;

               if (!fMustJump)
                    return FALSE ;

               iBegin = SHORT1FROMMP (mp1) ;
               ulBit  = 1L << iBegin ;
               ulJump = CkmQueryAllJumpablePieces (&brd, sColor) ;

               if (ulBit & ulJump)
                    {
                    fMustJump = TRUE ;
                    brdLast = brd ;

                    if (sColor == BLACK)
                         brd.ulBlack &= ~ulBit ;
                    else
                         brd.ulWhite &= ~ulBit ;

                    brd.ulKing &= ~ulBit ;

                    return (MRESULT) TRUE ;
                    }

               return FALSE ;

          case WM_TELL_JUDGE_BOARD_MOVE_ENDED:
               sColor ^= 1 ;

               if (CkmQueryAllMoveablePieces (&brd, sColor) > 0 ||
                   CkmQueryAllJumpablePieces (&brd, sColor) > 0)
                    {
                    move.sColor          = sColor ;
                    move.sKing           = 0 ;
                    move.cSubMoves       = 0 ;
                    move.fNewKing        = FALSE ;
                    move.asubmove[0].brd = brd ;

                    if (hwndPlayer [sColor] != hwndBoard)
                         WinSendMsg (hwndBoard, WM_JUDGE_SAYS_SHOW_HOURGLASS,
                                     NULL, NULL) ;

                    WinSendMsg (hwndPlayer [sColor], WM_JUDGE_SAYS_MAKE_MOVE,
                                MPFROMP (&move), MPFROMSHORT (sColor)) ;
                    }
               else
                    {
                    WinSendMsg (hwndBoard, WM_JUDGE_SAYS_SHOW_ARROW,
                                NULL, NULL) ;

                    WinSendMsg (hwndClient, WM_TELL_CLIENT_GAME_OVER,
                                MPFROMSHORT (sColor ^ 1), NULL) ;
                    }

               return 0 ;

          case WM_TELL_JUDGE_STRAT_MOVE_ENDED:
               iBoardMove = 0 ;

               if (move.cSubMoves > 0)
                    {
                    WinSendMsg (hwndBoard, WM_JUDGE_SAYS_MOVE_PIECE,
                                MPFROM2SHORT (move.asubmove[0].iBeg,
                                              move.asubmove[0].iEnd),
                                MPFROM2SHORT (move.sColor,
                                              move.sKing)) ;

                    brd = move.asubmove[1].brd ;
                    }
               else
                    {
                    WinSendMsg (hwndClient, WM_TELL_JUDGE_BOARD_MOVE_ENDED,
                                NULL, NULL) ;
                    }

               return 0 ;

          case WM_TELL_JUDGE_PIECE_MOVED:
               if (move.asubmove[iBoardMove].iJmp != -1)
                    {
                    WinSendMsg (hwndBoard, WM_JUDGE_SAYS_REMOVE_PIECE,
                                MPFROMSHORT (move.asubmove[iBoardMove].iJmp),
                                NULL) ;
                    }

               iBoardMove ++ ;

               if (iBoardMove < move.cSubMoves)
                    {
                    WinSendMsg (hwndBoard, WM_JUDGE_SAYS_MOVE_PIECE,
                                MPFROM2SHORT (move.asubmove[iBoardMove].iBeg,
                                              move.asubmove[iBoardMove].iEnd),
                                MPFROM2SHORT (move.sColor, move.sKing)) ;

                    brd = move.asubmove[iBoardMove + 1].brd ;
                    }
               else
                    {
                    if (move.fNewKing)
                         WinSendMsg (hwndBoard, WM_JUDGE_SAYS_KING_PIECE,
                              MPFROMSHORT (move.asubmove[iBoardMove-1].iEnd),
                              NULL) ;

                    WinSendMsg (hwnd, WM_TELL_JUDGE_BOARD_MOVE_ENDED,
                                NULL, NULL) ;
                    }
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
