/*--------------------------------------
   CKRSTRAT.H header file, Version 0.40
  --------------------------------------*/

#ifndef CKR_BOARD_DEFINED

typedef struct
     {
     ULONG ulBlack ;
     ULONG ulWhite ;
     ULONG ulKing  ;
     }
     BOARD ;

#define CKR_BOARD_DEFINED
#endif

typedef struct
     {
     BOARD brd ;
     SHORT iBeg, iEnd, iJmp ;
     }
     SUBMOVE ;

typedef struct
     {
     SHORT   sColor ;
     SHORT   sKing ;
     SHORT   cSubMoves ;
     BOOL    fNewKing ;
     SUBMOVE asubmove [10] ;
     }
     MOVE ;

typedef struct
     {
     SHORT   sColor ;
     SHORT   sKing ;
     SHORT   cSubMoves ;
     BOOL    fNewKing ;
     SUBMOVE asubmove [10] ;
     VOID    *pmoveblk ;
     }
     MOVEP ;

typedef struct
     {
     SHORT cMoves ;
     MOVEP amovep[1] ;
     }
     MOVEBLOCK ;
