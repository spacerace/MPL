/*--------------------------------------
   CKRMOVES.H header file, Version 0.40
  --------------------------------------*/

ULONG CkmQueryAllMoveablePieces (BOARD *pbrd, SHORT sColor) ;
ULONG CkmQueryAllJumpablePieces (BOARD *pbrd, SHORT sColor) ;
ULONG CkmQueryMoveDestinations  (BOARD *pbrd, SHORT sColor, ULONG ulPiece) ;
ULONG CkmQueryJumpDestinations  (BOARD *pbrd, SHORT sColor, ULONG ulPiece) ;
SHORT CkmQueryJumpedPiece       (SHORT sBeg,  SHORT sEnd) ;
