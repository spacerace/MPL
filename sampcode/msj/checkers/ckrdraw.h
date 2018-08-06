/*-------------------------------------
   CHKDRAW.H header file, Version 0.40
  -------------------------------------*/

MRESULT EXPENTRY ColorDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2) ;

VOID CkdQueryBoardDimensions (SIZEL *psizl) ;

HPS  CkdCreatePS  (HWND hwnd) ;
VOID CkdResizePS  (HPS hps, HWND hwnd) ;
BOOL CkdDestroyPS (HPS hps) ;

VOID CkdSetStandardColors (VOID) ;
VOID CkdCreatePieces  (HPS hps) ;
VOID CkdDestroyPieces (VOID) ;

VOID CkdDrawWindowBackground (HPS hps, HWND hwnd) ;
VOID CkdDrawWholeBoard (HPS hps) ;
VOID CkdDrawAllPieces (HPS hps, BOARD *pbrd, SHORT sBottom) ;
VOID CkdErasePiece (HPS hps, SHORT x, SHORT y) ;

VOID CkdQueryHitCoords (HPS hps, POINTL ptlMouse, SHORT *px, SHORT *py) ;
SHORT CkdConvertCoordsToIndex (SHORT x, SHORT y, SHORT sBottom) ;
VOID CkdConvertIndexToCoords (SHORT i, SHORT *px, SHORT *py, SHORT sBottom) ;

VOID CkdDragSave (HPS hps, POINTL *pptlMouse, SHORT sKing) ;
VOID CkdDragRestore (HPS hps, POINTL *pptlMouse, SHORT sKing) ;
VOID CkdDragShow (HPS hps, POINTL *pptlMouse, SHORT sColor, SHORT sKing) ;
VOID CkdDragMove (HPS hps, POINTL *pptlFrom, POINTL *pptlTo,
                           SHORT sColor, SHORT sKing) ;
VOID CkdDragDeposit (HPS hps, SHORT x, SHORT y, SHORT sColor, SHORT sKing) ;

VOID CkdQueryNearestXYFromPoint (HPS hps, POINTL *pptlMouse, SHORT *px,
                                                             SHORT *py) ;
VOID CkdQuerySlightOffsetFromXY (HPS hps, SHORT x, SHORT y, POINTL *pptl) ;

VOID CkdExternalSave (HPS hps, SHORT x, SHORT y, SHORT sKing) ;
VOID CkdExternalShow (HPS hps, SHORT x, SHORT y, SHORT sColor, SHORT sKing) ;
VOID CkdExternalRestore (HPS hps, SHORT x, SHORT y, SHORT sKing) ;
VOID CkdExternalMove (HPS hps, SHORT sTimerBeg,   SHORT sTimerEnd,
                               SHORT sTimerColor, SHORT sTimerKing,
                               SHORT sBottom,     SHORT sTimerInc,
                               SHORT sTimerSteps) ;

VOID CkdDrawOnePieceDirect  (HPS hps, SHORT x, SHORT y, BOARD *pbrd,
                             SHORT sBottom) ;

VOID CkdDrawAllPiecesDirect (HPS hps, BOARD *pbrd, SHORT sBottom) ;
