/*---------------------------------------------------------------
   CKRJUDGE.C -- JudgeWndProc for controlling game, Version 0.30
                 (c) 1990, Charles Petzold
  ---------------------------------------------------------------*/

#include <os2.h>
#include "checkers.h"
#include "ckrmoves.h"

MRESULT EXPENTRY JudgeWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOOL  fKing, fNewKing, fMustJump ;
     static BOARD brd = { 0x00000FFF, 0xFFF00000, 0x00000000 }, brdLast ;
     static HWND  hwndBoard ;
     static SHORT sColor = BLACK, iBegin, iEnd, iJump ;
     ULONG        ulBit, ulMove, ulJump ;

     switch (msg)
          {
          case WM_TELL_JUDGE_BOARD_HANDLE:
               hwndBoard = HWNDFROMMP (mp1) ;
               return 0 ;

          case WM_TELL_JUDGE_NEW_GAME:
               brd.ulBlack = 0x00000FFF ;
               brd.ulWhite = 0xFFF00000 ;
               brd.ulKing  = 0x00000000 ;

               sColor = BLACK ;

               WinSendMsg (hwndBoard, WM_JUDGE_SAYS_RESET_BOARD, NULL, NULL) ;
               WinSendMsg (hwndBoard, WM_JUDGE_SAYS_MOVE_BLACK, NULL, NULL) ;
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

               return TRUE ;

          case WM_QUERY_JUDGE_IF_KING:
               return fKing ;

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
               return TRUE ;

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

                    return TRUE ;
                    }

               return FALSE ;

          case WM_TELL_JUDGE_BOARD_MOVE_ENDED:
               if (sColor == BLACK)
                    {
                    sColor = WHITE ;
                    WinSendMsg (hwndBoard, WM_JUDGE_SAYS_MOVE_WHITE,
                                NULL, NULL) ;
                    }
               else
                    {
                    sColor = BLACK ;
                    WinSendMsg (hwndBoard, WM_JUDGE_SAYS_MOVE_BLACK,
                                NULL, NULL) ;
                    }
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
