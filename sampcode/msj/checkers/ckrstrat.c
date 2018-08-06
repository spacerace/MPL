/*----------------------------------------------------------------------
   CKRSTRAT.C -- Determines best move from board position, Version 0.40
                 (c) 1990, Charles Petzold
  ----------------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include <process.h>
#include <stdlib.h>
#include "checkers.h"
#include "ckrstrat.h"
#include "ckrmoves.h"

#define USE_MALLOC  TRUE

typedef struct
     {
     MOVE  *pmove ;
     SHORT sLevel ;
     HWND  hwndJudge ;
     }
     THREADPARAMS ;

extern HAB hab ;

HWND hwndClient ;

     /*--------------------------------------------------------
        CksAllocateMem:  Allocate or reallocate a memory block
       --------------------------------------------------------*/

static VOID * CksAllocateMem (VOID * pIn, size_t iSize)
     {
     VOID * p ;

#if USE_MALLOC

     p = realloc (pIn, iSize) ;

#else

     {
     SEL sel ;

     if (pIn == NULL)
          {
          if (DosAllocSeg ((USHORT) iSize, &sel, SEG_NONSHARED))
               sel = 0 ;

          p = MAKEP (sel, 0) ;
          }
     else
          {
          if (DosReallocSeg ((USHORT) iSize, SELECTOROF (pIn)))
               p = NULL ;
          else
               p = pIn ;
          }
     }

#endif

     if (p == NULL)
          {
          WinPostMsg (hwndClient, WM_TELL_CLIENT_NO_MEMORY, NULL, NULL) ;

          _endthread () ;
          }

     return p ;
     }

     /*----------------------------------
        CksFreeMem:  Free a memory block
       ----------------------------------*/

static VOID CksFreeMem (VOID * p)
     {

#if USE_MALLOC

     free (p) ;

#else

     DosFreeSeg (SELECTOROF (p)) ;

#endif

     }

     /*-------------------------------------------------------------
        CksQueryNumBitsInLong:  Returns number of bits set in ULONG
       -------------------------------------------------------------*/

static SHORT CksQueryNumBitsInLong (ULONG l)
     {
     SHORT cBits = 0 ;

     while (l > 0)
          {
          if (l & 1)
               cBits++ ;

          l >>= 1 ;
          }
     return cBits ;
     }

     /*------------------------------------------------------------------
        CksQueryOneBitInLong: Returns ULONG with only (n-1)th bit set.

        For example, CksQueryOneBitInLong (0x00000305, 0) --> 0x00000001
                                          (0x00000305, 1) --> 0x00000004
                                          (0x00000305, 2) --> 0x00000100
                                          (0x00000305, 3) --> 0x00000200
                                          (0x00000305, 4) --> 0x00000000
       ------------------------------------------------------------------*/

static ULONG CksQueryOneBitInLong (ULONG l, SHORT n)
     {
     SHORT iCount = 0, i ;
     ULONG ulMask = 1 ;

     for (i = 0 ; i < 32 ; i++)
          {
          if (ulMask & l)
               {
               if (n == iCount)
                    return ulMask ;
               else
                    iCount++ ;
               }
          ulMask <<= 1 ;
          }
     return 0 ;
     }

     /*--------------------------------------------------------
        CksQueryBitPosInLong: Obtains position of LSB in ULONG
       --------------------------------------------------------*/

static SHORT CksQueryBitPosInLong (ULONG l)
     {
     SHORT i ;

     for (i = 0 ; i < 32 ; i++)
          {
          if (l & 1)
               return i ;

          l >>= 1 ;
          }
     return -1 ;
     }

     /*----------------------------------------------------------
        CksGetMaxValue:  Returns the maximum value from an array
                         (assumes lowest value is -10000)
       ----------------------------------------------------------*/

static SHORT CksGetMaxValue (SHORT asValue[], SHORT cValues)
     {
     SHORT i, sMax = -10000 ;

     for (i = 0 ; i < cValues ; i++)
          if (asValue[i] > sMax)
               sMax = asValue[i] ;

     return sMax ;
     }

     /*-----------------------------------------------------------------
        CksGetMaxValueIndex:  Returns the index of a maximum value from
                              an array (randomly if multiple maximums)
       -----------------------------------------------------------------*/

static SHORT CksGetMaxValueIndex (SHORT asValue[], SHORT cValues)
     {
     SHORT i, sMax, cSame, iMax, sRand, iIndex ;

     sMax  = CksGetMaxValue (asValue, cValues) ;
     cSame = 0 ;

     for (i = 0 ; i < cValues ; i++)
          if (asValue[i] == sMax)
               {
               iMax = i ;
               cSame ++ ;
               }

     if (cSame == 1)
          return iMax ;

     sRand  = rand () % cSame ;
     iIndex = 0 ;

     for (i = 0 ; i < cValues ; i++)
          if (asValue[i] == sMax)
               {
               if (iIndex == sRand)
                    {
                    iMax = i ;
                    break ;
                    }
               else
                    iIndex ++ ;
               }

     return iMax ;
     }

     /*------------------------------------------------------------------
        CksApplyChangesToBoard:  Modifies next BOARD structure in a MOVE
                                 structure based on a submove
       ------------------------------------------------------------------*/

static VOID CksApplyChangesToBoard (MOVE *pmove)
     {
     SHORT i = pmove->cSubMoves - 1 ;

     pmove->asubmove[i+1].brd = pmove->asubmove[i].brd ;

     if (pmove->sColor == BLACK)
          {
          pmove->asubmove[i+1].brd.ulBlack &= ~(1L << pmove->asubmove[i].iBeg) ;
          pmove->asubmove[i+1].brd.ulBlack |=  (1L << pmove->asubmove[i].iEnd) ;
          }
     else
          {
          pmove->asubmove[i+1].brd.ulWhite &= ~(1L << pmove->asubmove[i].iBeg) ;
          pmove->asubmove[i+1].brd.ulWhite |=  (1L << pmove->asubmove[i].iEnd) ;
          }

     if (pmove->asubmove[i].brd.ulKing & (1L << pmove->asubmove[i].iBeg))
          {
          pmove->asubmove[i+1].brd.ulKing  &= ~(1L << pmove->asubmove[i].iBeg) ;
          pmove->asubmove[i+1].brd.ulKing  |=  (1L << pmove->asubmove[i].iEnd) ;
          }

     if (pmove->asubmove[i].iJmp != -1)
          {
          pmove->asubmove[i+1].brd.ulBlack &= ~(1L << pmove->asubmove[i].iJmp) ;
          pmove->asubmove[i+1].brd.ulWhite &= ~(1L << pmove->asubmove[i].iJmp) ;
          pmove->asubmove[i+1].brd.ulKing  &= ~(1L << pmove->asubmove[i].iJmp) ;
          }

     if (pmove->asubmove[i+1].brd.ulKing & (1L << pmove->asubmove[i].iEnd))
          pmove->sKing = 1 ;

     else if ((pmove->sColor == BLACK && pmove->asubmove[i].iEnd > 27) ||
              (pmove->sColor == WHITE && pmove->asubmove[i].iEnd <  4))
          {
          pmove->fNewKing = TRUE ;
          pmove->asubmove[i+1].brd.ulKing |= (1L << pmove->asubmove[i].iEnd) ;
          }
     }

     /*---------------------------------------------------------------------
        CksGetAllJumps:  Recursive routine to fill MOVEBLOCK structure with
                         all possible jumps.
       ---------------------------------------------------------------------*/

static VOID CksGetAllJumps (MOVEBLOCK *pmoveblk, BOARD brdIn, SHORT sColor,
                            ULONG ulJumpIn, SHORT sSubMove)
     {
     BOARD brd ;
     SHORT i, iBeg, iEnd, iJmp,
            cJumps,  sJump,  cDests,  sDest ;
     ULONG ulJumps, ulJump, ulDests, ulDest ;

     ulDests = CkmQueryJumpDestinations (&brdIn, sColor, ulJumpIn) ;
     cDests  = CksQueryNumBitsInLong (ulDests) ;

     for (sDest = 0 ; sDest < cDests ; sDest++)
          {
          ulDest = CksQueryOneBitInLong (ulDests, sDest) ;
          iBeg   = CksQueryBitPosInLong (ulJumpIn) ;
          iEnd   = CksQueryBitPosInLong (ulDest) ;
          iJmp   = CkmQueryJumpedPiece (iBeg, iEnd) ;

                    // Reallocate MOVEBLOCK structure for this move

          i = pmoveblk->cMoves ;

          pmoveblk = CksAllocateMem (pmoveblk, sizeof (MOVEBLOCK) +
                                               sizeof (MOVEP) * i) ;

                    // Copy the MOVEP structure from previous move

          if (sSubMove > 0 && sDest > 0)
               pmoveblk->amovep[i] = pmoveblk->amovep[i - 1] ;

          pmoveblk->amovep[i].sColor                  = sColor ;
          pmoveblk->amovep[i].sKing                   = 0 ;
          pmoveblk->amovep[i].cSubMoves               = sSubMove + 1 ;
          pmoveblk->amovep[i].fNewKing                = FALSE ;
          pmoveblk->amovep[i].asubmove[sSubMove].brd  = brdIn ;
          pmoveblk->amovep[i].asubmove[sSubMove].iBeg = iBeg ;
          pmoveblk->amovep[i].asubmove[sSubMove].iEnd = iEnd ;
          pmoveblk->amovep[i].asubmove[sSubMove].iJmp = iJmp ;
          pmoveblk->amovep[i].pmoveblk                = NULL ;

          CksApplyChangesToBoard ((MOVE *) &pmoveblk->amovep[i]) ;

          brd     = pmoveblk->amovep[i].asubmove[sSubMove + 1].brd ;
          ulJumps = ulDest & CkmQueryAllJumpablePieces (&brd, sColor) ;

          if (ulJumps > 0 && pmoveblk->amovep[i].fNewKing == FALSE)
               {
               cJumps = CksQueryNumBitsInLong (ulJumps) ;

               for (sJump = 0 ; sJump < cJumps ; sJump++)
                    {
                    ulJump = CksQueryOneBitInLong (ulJumps, sJump) ;

                    CksGetAllJumps (pmoveblk, brd, sColor,
                                    ulJump, sSubMove + 1) ;
                    }
               }

          if (sSubMove == 0 || sDest < cDests - 1)
               pmoveblk->cMoves ++ ;
          }
     }

     /*--------------------------------------------------------------------
        CksGetAllPossibleMoves:  Allocates a MOVEBLOCK structure and fills
                                 it in with all possible moves.
       --------------------------------------------------------------------*/

static MOVEBLOCK * CksGetAllPossibleMoves (BOARD brd, SHORT sColor)
     {
     MOVEBLOCK *pmoveblk ;
     SHORT     i, iBeg, iEnd,
                cMoves,  cJumps,  sMove,  sJump,  cDests,  sDest ;
     ULONG     ulMoves, ulJumps, ulMove, ulJump, ulDests, ulDest ;

               // Allocate MOVEBLOCK structure and initialize one move
               // to indicate a "NULL" move

     pmoveblk = CksAllocateMem (NULL, sizeof (MOVEBLOCK)) ;
     pmoveblk->cMoves = 0 ;
     pmoveblk->amovep[0].sColor           = sColor ;
     pmoveblk->amovep[0].sKing            = 0 ;
     pmoveblk->amovep[0].cSubMoves        = 0 ;
     pmoveblk->amovep[0].fNewKing         = FALSE ;
     pmoveblk->amovep[0].asubmove[0].brd  = brd ;
     pmoveblk->amovep[0].pmoveblk         = NULL ;

               // Find all pieces that can make moves and jumps

     ulMoves = CkmQueryAllMoveablePieces (&brd, sColor) ;
     ulJumps = CkmQueryAllJumpablePieces (&brd, sColor) ;

               // Do the following for a move but not a jump

     if (ulMoves > 0 && ulJumps == 0)
          {
          cMoves = CksQueryNumBitsInLong (ulMoves) ;

          for (sMove = 0 ; sMove < cMoves ; sMove++)
               {
               ulMove  = CksQueryOneBitInLong (ulMoves, sMove) ;
               ulDests = CkmQueryMoveDestinations (&brd, sColor, ulMove) ;
               cDests  = CksQueryNumBitsInLong (ulDests) ;

               for (sDest = 0 ; sDest < cDests ; sDest++)
                    {
                    ulDest = CksQueryOneBitInLong (ulDests, sDest) ;
                    iBeg   = CksQueryBitPosInLong (ulMove) ;
                    iEnd   = CksQueryBitPosInLong (ulDest) ;

                              // Reallocate the MOVEBLOCK structure

                    i = pmoveblk->cMoves ;

                    pmoveblk->cMoves ++ ;

                    pmoveblk = CksAllocateMem (pmoveblk, sizeof (MOVEBLOCK) +
                                                         sizeof (MOVEP) * i) ;

                              // Fill in the information for this move

                    pmoveblk->amovep[i].sColor           = sColor ;
                    pmoveblk->amovep[i].sKing            = 0 ;
                    pmoveblk->amovep[i].cSubMoves        = 1 ;
                    pmoveblk->amovep[i].fNewKing         = FALSE ;
                    pmoveblk->amovep[i].asubmove[0].brd  = brd ;
                    pmoveblk->amovep[i].asubmove[0].iBeg = iBeg ;
                    pmoveblk->amovep[i].asubmove[0].iEnd = iEnd ;
                    pmoveblk->amovep[i].asubmove[0].iJmp = -1 ;
                    pmoveblk->amovep[i].pmoveblk         = NULL ;

                              // Apply the changes to the next asubmove BOARD

                    CksApplyChangesToBoard ((MOVE *) &pmoveblk->amovep[i]) ;
                    }
               }
          }

     else if (ulJumps > 0)
          {
          cJumps = CksQueryNumBitsInLong (ulJumps) ;

          for (sJump = 0 ; sJump < cJumps ; sJump++)
               {
               ulJump = CksQueryOneBitInLong (ulJumps, sJump) ;

               CksGetAllJumps (pmoveblk, brd, sColor, ulJump, 0) ;
               }
          }

     return pmoveblk ;
     }

     /*-----------------------------------------------------------
        CksCalculateAdvantage:  Total the pieces, weighting kings
                                75% higher than non-kings.
       -----------------------------------------------------------*/

static SHORT CksCalculateAdvantage (BOARD *pbrd, SHORT sColor)
     {
     SHORT sAdvantage ;
     ULONG B = pbrd->ulBlack,
           W = pbrd->ulWhite,
           K = pbrd->ulKing ;

     if (W == 0)
          sAdvantage = 10000 ;

     else if (B == 0)
          sAdvantage = -10000 ;

     else if (CkmQueryAllMoveablePieces (pbrd, WHITE) == 0L &&
              CkmQueryAllJumpablePieces (pbrd, WHITE) == 0L)

          sAdvantage = 10000 ;

     else if (CkmQueryAllMoveablePieces (pbrd, BLACK) == 0L &&
              CkmQueryAllJumpablePieces (pbrd, BLACK) == 0L)

          sAdvantage = -10000 ;

     else
          sAdvantage = (100 * CksQueryNumBitsInLong (B & ~K) +
                        175 * CksQueryNumBitsInLong (B &  K)) -
                       (100 * CksQueryNumBitsInLong (W & ~K) +
                        175 * CksQueryNumBitsInLong (W &  K)) ;

     return (sColor == BLACK ? sAdvantage : -sAdvantage) ;
     }

     /*--------------------------------------------------------------
        CksGetBestAdvantage:  Loop through possible moves and return
                              index of best move (if fIndex is TRUE)
                              or the value of the best move (FALSE)
       --------------------------------------------------------------*/

static SHORT CksGetBestAdvantage (MOVEBLOCK *pmoveblk, SHORT sColor,
                                                       BOOL fIndex)
     {
     BOARD brd ;
     SHORT i, cMoves, cSubMoves, sReturn ;
     SHORT *asValue ;

          // Get the number of moves in the MOVEBLOCK

     cMoves = pmoveblk->cMoves ;

          // If 0 or 1 move, and returning index, return 0

     if (fIndex && cMoves <= 1)
          return 0 ;

          // If 0 moves, and returning value, return -10000 ;

     if (!fIndex && cMoves == 0)
          return -10000 ;

          // Else allocate memory for number of moves

     asValue = CksAllocateMem (NULL, sizeof (SHORT) * cMoves) ;

          // Loop through the moves, calculating advantages

     for (i = 0 ; i < cMoves ; i++)
          {
          cSubMoves  = pmoveblk->amovep[i].cSubMoves ;
          brd        = pmoveblk->amovep[i].asubmove[cSubMoves].brd ;
          asValue[i] = CksCalculateAdvantage (&brd, sColor) ;
          }

          // Determine what to return from fIndex parameter

     if (fIndex)
          sReturn = CksGetMaxValueIndex (asValue, cMoves) ;
     else
          sReturn = CksGetMaxValue (asValue, cMoves) ;

     CksFreeMem (asValue) ;

     return sReturn ;
     }

     /*---------------------------------------------------------------------
        CksFreeMoveBlock:  Free MOVEBLOCK structure and all other MOVEBLOCK
                           structures in tree.
       ---------------------------------------------------------------------*/

static VOID CksFreeMoveBlock (MOVEBLOCK *pmoveblk)
     {
     SHORT i ;

     for (i = 0 ; i < pmoveblk->cMoves ; i++)
          {
          if (pmoveblk->amovep[i].pmoveblk != NULL)
               CksFreeMoveBlock (pmoveblk->amovep[i].pmoveblk) ;
          }

     CksFreeMem (pmoveblk) ;
     }

     /*-----------------------------------------------------------------
        CksGetCounterMoves:  Recursively get counter-moves depending on
                             sLevel setting.
       -----------------------------------------------------------------*/

static VOID CksGetCounterMoves (MOVEBLOCK *pmoveblk, SHORT sLevel)
     {
     BOARD     brd ;
     MOVEBLOCK *pmoveblk2 ;
     SHORT     i, cSubMoves, sColor ;

               // If sLevel is 0 or less, just return

     if (sLevel <= 0)
          return ;

               // If no moves on previous level, set the counter move to NULL

     if (pmoveblk->cMoves == 0)
          {
          pmoveblk2 = CksAllocateMem (NULL, sizeof (MOVEBLOCK)) ;
          *pmoveblk2 = *pmoveblk ;
          pmoveblk->amovep[0].pmoveblk = pmoveblk2 ;

          CksGetCounterMoves (pmoveblk2, sLevel - 1) ;
          }

               // Otherwise, loop through all the moves and get counter moves

     for (i = 0 ; i < pmoveblk->cMoves ; i++)
          {
          cSubMoves = pmoveblk->amovep[i].cSubMoves ;
          brd       = pmoveblk->amovep[i].asubmove[cSubMoves].brd ;
          sColor    = pmoveblk->amovep[i].sColor ^ 1 ;

          pmoveblk->amovep[i].pmoveblk = CksGetAllPossibleMoves (brd, sColor) ;

          CksGetCounterMoves (pmoveblk->amovep[i].pmoveblk, sLevel - 1) ;
          }
     }

     /*------------------------------------------------------
        CksStrategyThread:  Thread for determining best move
       ------------------------------------------------------*/

static VOID CksStrategyThread (THREADPARAMS *ptp)
     {
     BOARD     brd ;
     HWND      hwndJudge ;
     MOVE      *pmove ;
     MOVEBLOCK *pmoveblkMain,
               *pmoveblk ;
     SHORT     sLevel, i, sColor, cMoves, iAdv ;
     SHORT     *asValue ;

               // Save fields of THREADPARAMS structure

     pmove     = ptp->pmove ;
     sLevel    = ptp->sLevel ;
     hwndJudge = ptp->hwndJudge ;

               // Set random number generator

     srand ((int) WinGetCurrentTime (hab)) ;

               // Get the initial board layout and the color

     brd    = pmove->asubmove[0].brd ;
     sColor = pmove->sColor ;

               // Determine all possible moves & initial number of moves

     pmoveblkMain = CksGetAllPossibleMoves (brd, sColor) ;

     cMoves = pmoveblkMain->cMoves ;

               // If only 0 or 1 possible moves, nothing much to do

     if (cMoves <= 1)
          {
          iAdv = 0 ;
          }

               // For Simple Level, make random (but legal) move

     else if (sLevel == LEVEL_SIMPLE)
          {
          iAdv = rand () % cMoves ;
          }

               // For Beginner Level, make best immediate move

     else if (sLevel == LEVEL_BEGINNER)
          {
          iAdv = CksGetBestAdvantage (pmoveblkMain, sColor, TRUE) ;
          }

               // For Intermediate Level, choose best move for best response

     else if (sLevel == LEVEL_INTERMEDIATE)
          {
          CksGetCounterMoves (pmoveblkMain, 2) ;

          asValue = CksAllocateMem (NULL, cMoves * sizeof (SHORT)) ;

          for (i = 0 ; i < cMoves ; i++)
               {
               pmoveblk   = pmoveblkMain->amovep[i].pmoveblk ;
               iAdv       = CksGetBestAdvantage (pmoveblk, sColor ^ 1, TRUE) ;
               pmoveblk   = pmoveblk->amovep[iAdv].pmoveblk ;
               asValue[i] = CksGetBestAdvantage (pmoveblk, sColor, FALSE) ;
               }

          iAdv = CksGetMaxValueIndex (asValue, cMoves) ;

          CksFreeMem (asValue) ;
          }

               // For Advanced Level, go 5 levels deep

     else if (sLevel == LEVEL_ADVANCED)
          {
          MOVEBLOCK *pmoveblk1, *pmoveblk2, *pmoveblk3, *pmoveblk4 ;
          SHORT     i1, i2, i3 ;
          SHORT     *asValue2, *asValue3 ;

          CksGetCounterMoves (pmoveblkMain, 4) ;

          asValue = CksAllocateMem (NULL, cMoves * sizeof (SHORT)) ;

          for (i1 = 0 ; i1 < cMoves ; i1++)
               {
               asValue2 = NULL ;
               asValue3 = NULL ;
               i        = 0 ;

               pmoveblk1 = pmoveblkMain->amovep[i1].pmoveblk ;

               for (i2 = 0 ; i2 < max (1, pmoveblk1->cMoves) ; i2++)
                    {
                    pmoveblk2 = pmoveblk1->amovep[i2].pmoveblk ;

                    for (i3 = 0 ; i3 < max (1, pmoveblk2->cMoves) ; i3++)
                         {
                         pmoveblk3 = pmoveblk2->amovep[i3].pmoveblk ;

                         asValue2 = CksAllocateMem (asValue2,
                                                    (i + pmoveblk3->cMoves) *
                                                    sizeof (SHORT)) ;

                         asValue3 = CksAllocateMem (asValue3,
                                                    (i + pmoveblk3->cMoves) *
                                                    sizeof (SHORT)) ;

                         asValue2[i] = CksGetBestAdvantage (pmoveblk3,
                                                            sColor ^ 1, FALSE) ;

                         iAdv = CksGetBestAdvantage (pmoveblk3,
                                                     sColor ^ 1, TRUE) ;

                         pmoveblk4 = pmoveblk3->amovep[iAdv].pmoveblk ;

                         asValue3[i] = CksGetBestAdvantage (pmoveblk4,
                                                            sColor, FALSE) ;
                         i++ ;
                         }
                    }
               iAdv = CksGetMaxValueIndex (asValue2, i) ;

               asValue[i1] = asValue3[iAdv] ;

               CksFreeMem (asValue2) ;
               CksFreeMem (asValue3) ;
               }

          iAdv = CksGetMaxValueIndex (asValue, cMoves) ;

          CksFreeMem (asValue) ;
          }

               // Now get move from iAdv index and free MOVEBLOCK structure

     * pmove = * (MOVE *) & pmoveblkMain->amovep[iAdv] ;

     CksFreeMoveBlock (pmoveblkMain) ;

               // Notify the Judge that the move is ready

     WinPostMsg (hwndJudge, WM_TELL_JUDGE_STRAT_MOVE_ENDED,
                 MPFROMP (pmove), NULL) ;

     _endthread () ;
     }

     /*------------------------------------------------------
        StratWndProc:  Window procedure for strategy routine
       ------------------------------------------------------*/

MRESULT EXPENTRY StratWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static HWND         hwndJudge ;
     static MOVE         *pmove ;
     static SHORT        sLevel ;
     static THREADPARAMS tp ;
     NEWGAME             *pnewgame ;

     switch (msg)
          {
          case WM_NEW_GAME:
               pnewgame   = PVOIDFROMMP (mp1) ;

               hwndJudge  = pnewgame->hwndJudge ;
               hwndClient = pnewgame->hwndClient ;
               sLevel     = pnewgame->sLevel ;

               return 0 ;

          case WM_JUDGE_SAYS_MAKE_MOVE:
               pmove = PVOIDFROMMP (mp1) ;

               tp.pmove     = pmove ;
               tp.sLevel    = sLevel ;
               tp.hwndJudge = hwndJudge ;

               _beginthread (CksStrategyThread, NULL, 8192, &tp) ;

               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
