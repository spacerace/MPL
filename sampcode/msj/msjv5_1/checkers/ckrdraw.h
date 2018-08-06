/*------------------------------------
   CHDRAW.H header file, version 0.10
  ------------------------------------*/

MRESULT EXPENTRY ColorDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2) ;

HPS  CkdCreatePS  (HWND hwnd) ;
VOID CkdResizePS  (HPS hps, HWND hwnd) ;
BOOL CkdDestroyPS (HPS hps) ;

VOID CkdSetStandardColors (VOID) ;
VOID CkdCreatePieces  (HPS hps) ;
VOID CkdDestroyPieces (VOID) ;

VOID CkdDrawWindowBackground (HPS hps, HWND hwnd) ;
VOID CkdDrawWholeBoard (HPS hps) ;
VOID CkdDrawAllPieces (HPS hps, BOARD *pbrd, SHORT sBottom) ;
