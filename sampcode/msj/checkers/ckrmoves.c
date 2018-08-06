/*----------------------------------------------------------------------
   CKRMOVES.C -- Ckm routines for determining valid moves, Version 0.40
                 (c) 1990, Charles Petzold
  ----------------------------------------------------------------------*/

#include <os2.h>
#include "checkers.h"
#include "ckrmoves.h"
                              // Some handy constants
#define B   pbrd->ulBlack
#define W   pbrd->ulWhite
#define K   pbrd->ulKing
#define E   (~B & ~W)                   // empty squares
#define MP  valnojmp[0][i].ulGrid       // valid no-jump moves (positive)
#define MN  valnojmp[1][i].ulGrid       // valid no-jump moves (negative)
#define IP  valnojmp[0][i].incr         // valid no-jump increments (pos)
#define IN  valnojmp[1][i].incr         // valid no-jump increments (neg)
#define JP  valjumps[0][i].ulGrid       // valid jumps (positive)
#define JN  valjumps[1][i].ulGrid       // valid jumps (negative)
#define IP1 valjumps[0][i].incr1        // valid jumps increment 1 (pos)
#define IN1 valjumps[1][i].incr1        // valid jumps increment 1 (neg)
#define IP2 valjumps[0][i].incr2        // valid jumps increment 2 (pos)
#define IN2 valjumps[1][i].incr2        // valid jumps increment 2 (neg)

static struct                 // Valid No-Jump Moves
     {
     SHORT incr ;
     ULONG ulGrid ;
     }
     valnojmp[2][3] = { 3, 0x00E0E0E0, 4, 0x0FFFFFFF, 5, 0x07070707,
                        3, 0x07070700, 4, 0xFFFFFFF0, 5, 0xE0E0E0E0 } ;

static struct                 // Valid Jump Moves
     {
     SHORT incr1, incr2 ;
     ULONG ulGrid ;
     }
     valjumps[2][4] = { 3, 4, 0x00E0E0E0, 4, 3, 0x000E0E0E,
                        4, 5, 0x00707070, 5, 4, 0x00070707,
                        3, 4, 0x07070700, 4, 3, 0x70707000,
                        4, 5, 0x0E0E0E00, 5, 4, 0xE0E0E000 } ;

     /*---------------------------------------------------------------------
        Obtain 32-bit ULONG of all pieces that can be moved without jumping
       ---------------------------------------------------------------------*/

ULONG CkmQueryAllMoveablePieces (BOARD *pbrd, SHORT sColor)
     {
     SHORT i ;
     ULONG S = 0 ;       // stands for "source"

     for (i = 0 ; i < 3 ; i++)
          {
          if (sColor == BLACK)
               {
               S |= (((B &     MP) << IP) & E) >> IP ;
               S |= (((B & K & MN) >> IN) & E) << IN ;
               }
          else
               {
               S |= (((W &     MN) >> IN) & E) << IN ;
               S |= (((W & K & MP) << IP) & E) >> IP ;
               }
          }
     return S ;
     }

     /*-------------------------------------------------------
        Obtain 32-bit ULONG of all pieces that can make jumps
       -------------------------------------------------------*/

ULONG CkmQueryAllJumpablePieces (BOARD *pbrd, SHORT sColor)
     {
     SHORT i ;
     ULONG S = 0 ;

     for (i = 0 ; i < 4 ; i++)
          {
          if (sColor == BLACK)
               {
               S |= ((((B &     JP) <<  IP1       ) & W) >>  IP1       ) &
                    ((((B &     JP) << (IP1 + IP2)) & E) >> (IP1 + IP2)) ;

               S |= ((((B & K & JN) >>  IN1       ) & W) <<  IN1       ) &
                    ((((B & K & JN) >> (IN1 + IN2)) & E) << (IN1 + IN2)) ;
               }
          else
               {
               S |= ((((W &     JN) >>  IN1       ) & B) <<  IN1       ) &
                    ((((W &     JN) >> (IN1 + IN2)) & E) << (IN1 + IN2)) ;

               S |= ((((W & K & JP) <<  IP1       ) & B) >>  IP1       ) &
                    ((((W & K & JP) << (IP1 + IP2)) & E) >> (IP1 + IP2)) ;
               }
          }
     return S ;
     }

     /*-----------------------------------------------------------------
        Obtain all destinations of a particular move-without-jump piece
       -----------------------------------------------------------------*/

ULONG CkmQueryMoveDestinations (BOARD *pbrd, SHORT sColor, ULONG ulPiece)
     {
     SHORT i ;
     ULONG P = ulPiece, D = 0 ;

     for (i = 0 ; i < 3 ; i++)
          {
          if (sColor == BLACK)
               {
               D |= ((P & B &     MP) << IP) & E ;
               D |= ((P & B & K & MN) >> IN) & E ;
               }
          else
               {
               D |= ((P & W &     MN) >> IN) & E ;
               D |= ((P & W & K & MP) << IP) & E ;
               }
          }
     return D ;
     }

     /*------------------------------------------------------
        Obtain all destination of a particular jumping piece
       ------------------------------------------------------*/

ULONG CkmQueryJumpDestinations (BOARD *pbrd, SHORT sColor, ULONG ulPiece)
     {
     SHORT i ;
     ULONG P = ulPiece, D = 0 ;

     for (i = 0 ; i < 4 ; i++)
          {
          if (sColor == BLACK)
               {
               D |= ((((P & B &     JP) <<  IP1       ) & W) << IP2) &
                    ((((P & B &     JP) << (IP1 + IP2)) & E)       ) ;

               D |= ((((P & B & K & JN) >>  IN1       ) & W) >> IN2) &
                    ((((P & B & K & JN) >> (IN1 + IN2)) & E)       ) ;
               }
          else
               {
               D |= ((((P & W &     JN) >>  IN1       ) & B) >> IN2) &
                    ((((P & W &     JN) >> (IN1 + IN2)) & E)       ) ;

               D |= ((((P & W & K & JP) <<  IP1       ) & B) << IP2) &
                    ((((P & W & K & JP) << (IP1 + IP2)) & E)       ) ;
               }
          }
     return D ;
     }

     /*----------------------------------------------------------
        Obtain index of a jumped piece based on jumper's indices
       ----------------------------------------------------------*/

SHORT CkmQueryJumpedPiece (SHORT sBeg, SHORT sEnd)
     {
     return (sBeg + sEnd) / 2 + (sBeg & 4 ? 0 : 1) ;
     }
