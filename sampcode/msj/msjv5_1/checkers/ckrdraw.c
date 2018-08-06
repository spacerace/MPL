/*--------------------------------------------------------------------
   CKRDRAW.C source code file for Ckd drawing functions, version 0.10
             (c) 1990, Charles Petzold
  --------------------------------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include "checkers.h"
#include "ckrdraw.h"

     /*----------------------------------------
        Defines for board and piece dimensions
       ----------------------------------------*/

#define BRD_HORZFRONT        500
#define BRD_HORZBACK         350
#define BRD_VERT             300
#define BRD_EDGE              75
#define BRD_HORZMARGIN       250
#define BRD_FRONTMARGIN      250
#define BRD_BACKMARGIN       250
#define PIECE_XAXIS          (BRD_HORZBACK - 50)
#define PIECE_YAXIS          (BRD_VERT - 50)
#define PIECE_HEIGHT          50

     /*-------------------------------------
        Global variables external to module
       -------------------------------------*/

extern HAB hab ;

     /*-------------------------------------
        Global variables internal to module
       -------------------------------------*/

               // Background, board, and piece colors

static LONG clrBackground  = CLR_CYAN ;
static LONG clrBlackSquare = CLR_DARKGREEN ;
static LONG clrWhiteSquare = CLR_PALEGRAY ;
static LONG clrBlackPiece  = CLR_RED ;
static LONG clrWhitePiece  = CLR_WHITE ;

               // Text strings for saving colors to OS2.INI

static CHAR szApplication []    = "Checkers" ;
static CHAR szClrBackground []  = "Background Color" ;
static CHAR szClrBlackSquare [] = "Black Square Color" ;
static CHAR szClrWhiteSquare [] = "White Square Color" ;
static CHAR szClrBlackPiece []  = "Black Piece Color" ;
static CHAR szClrWhitePiece []  = "White Piece Color" ;

               // Original viewport for adjusting board to window size

static RECTL rclOrigViewport ;

               // Bitmaps for drawing pieces

static HDC     hdcMemory ;
static HPS     hpsMemory ;
static HBITMAP ahbmPiece[2][2], ahbmMask[2] ;
static SIZEL   sizlPiece[2] ;

     /*-------------------------------------------------------------
        CkdQueryBoardDimensions: Obtains size of board with margins
       -------------------------------------------------------------*/

static VOID CkdQueryBoardDimensions (SIZEL *psizlPage)
     {
     psizlPage->cx = 8 * BRD_HORZFRONT + 2 * BRD_HORZMARGIN ;
     psizlPage->cy = 8 * BRD_VERT + BRD_FRONTMARGIN + BRD_BACKMARGIN ;
     }

     /*-----------------------------------------------------------
        CkdQuerySquareOrigin: Obtains lower left corner of square
       -----------------------------------------------------------*/

static VOID CkdQuerySquareOrigin (SHORT x, SHORT y, POINTL *pptl)
     {
     pptl->x = BRD_HORZMARGIN + y * (BRD_HORZFRONT - BRD_HORZBACK) / 2 +
                    x * (y * BRD_HORZBACK + (8 - y) * BRD_HORZFRONT) / 8 ;

     pptl->y = BRD_FRONTMARGIN + y * BRD_VERT ;
     }

     /*-----------------------------------------------------
        CkdQuerySquareCoords: Obtains coordinates of square
       -----------------------------------------------------*/

static VOID CkdQuerySquareCoords (SHORT x, SHORT y, POINTL aptl[])
     {
     CkdQuerySquareOrigin (x,     y,     aptl + 0) ;
     CkdQuerySquareOrigin (x + 1, y,     aptl + 1) ;
     CkdQuerySquareOrigin (x + 1, y + 1, aptl + 2) ;
     CkdQuerySquareOrigin (x,     y + 1, aptl + 3) ;
     }

     /*-----------------------------------------------
        CkdDrawBoardSquare: Draws one square of board
       -----------------------------------------------*/

static LONG CkdDrawBoardSquare (HPS hps, SHORT x, SHORT y)
     {
     AREABUNDLE abnd ;
     LINEBUNDLE lbnd ;
     LONG       lReturn ;
     POINTL     aptl[4] ;

     GpiSavePS (hps) ;

     lbnd.lColor = CLR_BLACK ;
     GpiSetAttrs (hps, PRIM_LINE, LBB_COLOR, 0L, &lbnd) ;

     abnd.lColor = (x + y) & 1 ? clrWhiteSquare : clrBlackSquare ;
     GpiSetAttrs (hps, PRIM_AREA, LBB_COLOR, 0L, &abnd) ;

     GpiBeginArea (hps, BA_ALTERNATE | BA_BOUNDARY) ;

     CkdQuerySquareCoords (x, y, aptl) ;
     GpiMove (hps, aptl + 3) ;
     GpiPolyLine (hps, 4L, aptl) ;
     lReturn = GpiEndArea (hps) ;

     GpiRestorePS (hps, -1L) ;

     return lReturn ;
     }

     /*----------------------------------------------------
        CkdDrawAllBoardSquares: Draws all squares of board
       ----------------------------------------------------*/

static LONG CkdDrawAllBoardSquares (HPS hps)
     {
     SHORT x, y ;

     for (y = 0 ; y < 8 ; y++)
          for (x = 0 ; x < 8 ; x++)
               if (CkdDrawBoardSquare (hps, x, y) == GPI_HITS)
                    return MAKELONG (x, y) ;

     return MAKELONG (-1, -1) ;
     }

     /*----------------------------------------------------
        CkdRenderPiece: Draws piece on bitmap in memory PS
       ----------------------------------------------------*/

static VOID CkdRenderPiece (HPS hpsMemory, LONG lColorBack, LONG lColor,
                            LONG lColorLine, SHORT sKing)
     {
     ARCPARAMS  arcp ;
     AREABUNDLE abnd ;
     LINEBUNDLE lbnd ;
     POINTL     ptl, aptlArc[2] ;
     SHORT      s ;

     GpiSavePS (hpsMemory) ;

               // Draw background of bitmap

     GpiSetColor (hpsMemory, lColorBack) ;

     ptl.x = 0 ;
     ptl.y = 0 ;
     GpiMove (hpsMemory, &ptl) ;

     ptl.x = PIECE_XAXIS ;
     ptl.y = PIECE_YAXIS + (sKing + 1) * PIECE_HEIGHT ;
     GpiBox (hpsMemory, DRO_FILL, &ptl, 0L, 0L) ;

               // Set colors for areas and outlines

     abnd.lColor = lColor ;
     GpiSetAttrs (hpsMemory, PRIM_AREA, ABB_COLOR, 0L, &abnd) ;

     lbnd.lColor = lColorLine ;
     GpiSetAttrs (hpsMemory, PRIM_LINE, LBB_COLOR, 0L, &lbnd) ;

               // Set arc parameters

     arcp.lP = PIECE_XAXIS / 2 ;
     arcp.lQ = PIECE_YAXIS / 2 ;
     arcp.lR = 0 ;
     arcp.lS = 0 ;
     GpiSetArcParams (hpsMemory, &arcp) ;

               // draw the piece

     for (s = 0 ; s <= sKing ; s++)
          {
          GpiBeginArea (hpsMemory, BA_ALTERNATE | BA_BOUNDARY) ;

          ptl.x = 0 ;
          ptl.y = PIECE_YAXIS / 2 + (s + 1) * PIECE_HEIGHT ;
          GpiMove (hpsMemory, &ptl) ;

          ptl.y -= PIECE_HEIGHT ;
          GpiLine (hpsMemory, &ptl) ;

          aptlArc[0].x = PIECE_XAXIS / 2 ;
          aptlArc[0].y = s * PIECE_HEIGHT ;
          aptlArc[1].x = PIECE_XAXIS ;
          aptlArc[1].y = PIECE_YAXIS / 2 + s * PIECE_HEIGHT ;

          GpiPointArc (hpsMemory, aptlArc) ;

          ptl.x = PIECE_XAXIS ;
          ptl.y = PIECE_YAXIS / 2 + (s + 1) * PIECE_HEIGHT ;
          GpiLine (hpsMemory, &ptl) ;

          GpiEndArea (hpsMemory) ;
          }

     ptl.x = PIECE_XAXIS / 2 ;
     ptl.y = PIECE_YAXIS / 2 + (sKing ? 2 : 1) * PIECE_HEIGHT ;

     GpiMove (hpsMemory, &ptl) ;
     GpiFullArc (hpsMemory, DRO_OUTLINEFILL, MAKEFIXED (1,0)) ;

     GpiRestorePS (hpsMemory, -1L) ;
     }

     /*------------------------------------------------------
        CkdQuerySquareCenter: Obtains center of board square
       ------------------------------------------------------*/

static VOID CkdQuerySquareCenter (SHORT x, SHORT y, POINTL *pptlCenter)
     {
     POINTL aptl[4] ;

     CkdQuerySquareCoords (x, y, aptl) ;

     pptlCenter->x = (aptl[0].x + aptl[1].x + aptl[2].x + aptl[3].x) / 4 ;
     pptlCenter->y = (aptl[1].y + aptl[2].y) / 2 ;
     }

     /*---------------------------------------------------------------------
        CkdPieceOriginFromCenter: Converts center of square to piece origin
       ---------------------------------------------------------------------*/

static VOID CkdPieceOriginFromCenter (POINTL *pptl)
     {
     pptl->x -= PIECE_XAXIS / 2 ;
     pptl->y -= PIECE_YAXIS / 2 ;
     }

     /*----------------------------------------------------------------
        CkdQuerySquarePieceOrigin: Obtains origin of piece on a square
       ----------------------------------------------------------------*/

static VOID CkdQuerySquarePieceOrigin (SHORT x, SHORT y, POINTL *pptlOrigin)
     {
     CkdQuerySquareCenter (x, y, pptlOrigin) ;
     CkdPieceOriginFromCenter (pptlOrigin) ;
     }

     /*-----------------------------------------------------------------------
        CkdConvertCoordsToIndex: Obtains index (0-31) from square coordinates
       -----------------------------------------------------------------------*/

static SHORT CkdConvertCoordsToIndex (SHORT x, SHORT y, SHORT sBottom)
     {
     if (x < 0 || x > 7 || y < 0 || y > 7)
          return -1 ;

     if ((x - (y & 1)) & 1)
          return -1 ;

     if (sBottom == WHITE)
          {
          x = 7 - x ;
          y = 7 - y ;
          }

     return 3 ^ (4 * y + (x - (y & 1)) / 2) ;
     }

     /*-------------------------------------------------------------
        CkdShowPiece: Draws a piece on the screen at specific point
       -------------------------------------------------------------*/

static VOID CkdShowPiece (HPS hps, POINTL *pptlOrg, SHORT sColor, SHORT sKing)
     {
     POINTL aptl[3] ;

               // Write out mask with bitwise AND

     aptl[0]   = *pptlOrg ;
     aptl[1].x = pptlOrg->x + sizlPiece[sKing].cx ;
     aptl[1].y = pptlOrg->y + sizlPiece[sKing].cy ;
     aptl[2].x = 0 ;
     aptl[2].y = 0 ;

     GpiSetBitmap (hpsMemory, ahbmMask[sKing]) ;
     GpiBitBlt    (hps, hpsMemory, 3L, aptl, ROP_SRCAND, BBO_IGNORE) ;

               // Write out piece with bitwise OR

     aptl[0]   = *pptlOrg ;
     aptl[1].x = pptlOrg->x + sizlPiece[sKing].cx ;
     aptl[1].y = pptlOrg->y + sizlPiece[sKing].cy ;
     aptl[2].x = 0 ;
     aptl[2].y = 0 ;

     GpiSetBitmap (hpsMemory, ahbmPiece[sColor][sKing]) ;
     GpiBitBlt    (hps, hpsMemory, 3L, aptl, ROP_SRCPAINT, BBO_IGNORE) ;

     GpiSetBitmap (hpsMemory, NULL) ;
     }

     /*---------------------------------------------------------------------
        CkdDrawOnePiece: Draws a piece on the board at specific coordinates
       ---------------------------------------------------------------------*/

static VOID CkdDrawOnePiece (HPS hps, SHORT x, SHORT y,
                             BOARD *pbrd, SHORT sBottom)
     {
     POINTL ptlOrigin ;
     SHORT  i, sKing ;

     i = CkdConvertCoordsToIndex (x, y, sBottom) ;

     if (i == -1)
          return ;

     CkdQuerySquarePieceOrigin (x, y, &ptlOrigin) ;
     GpiConvert (hps, CVTC_PAGE, CVTC_DEVICE, 1L, &ptlOrigin) ;

     sKing = pbrd->ulKing & 1L << i ? 1 : 0 ;

     if (pbrd->ulBlack & 1L << i)
          CkdShowPiece (hps, &ptlOrigin, BLACK, sKing) ;

     if (pbrd->ulWhite & 1L << i)
          CkdShowPiece (hps, &ptlOrigin, WHITE, sKing) ;
     }

     /*----------------------------------------------------
        ColorDlgProc: Dialog procedure for changing colors
       ----------------------------------------------------*/

MRESULT EXPENTRY ColorDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static LONG  *pclr ;
     static SHORT sColor ;
     CHAR         *pchHeading ;

     switch (msg)
          {
          case WM_INITDLG:
               switch (* (PSHORT) PVOIDFROMMP (mp2))
                    {
                    case IDM_COLOR_BACKGROUND:
                         pchHeading = "Window Background Color" ;
                         pclr = &clrBackground ;
                         break ;

                    case IDM_COLOR_BLACK_SQUARE:
                         pchHeading = "Black Square Color" ;
                         pclr = &clrBlackSquare ;
                         break ;

                    case IDM_COLOR_WHITE_SQUARE:
                         pchHeading = "White Square Color" ;
                         pclr = &clrWhiteSquare ;
                         break ;

                    case IDM_COLOR_BLACK_PIECE:
                         pchHeading = "Black Piece Color" ;
                         pclr = &clrBlackPiece ;
                         break ;

                    case IDM_COLOR_WHITE_PIECE:
                         pchHeading = "White Piece Color" ;
                         pclr = &clrWhitePiece ;
                         break ;
                    }
               WinSetDlgItemText (hwnd, IDD_HEADING, pchHeading) ;

               sColor = (SHORT) *pclr ;

               WinSendDlgItemMsg (hwnd, IDD_COLOR + sColor, BM_SETCHECK,
                                  MPFROM2SHORT (TRUE, 0), NULL) ;

               WinSetFocus (HWND_DESKTOP,
                            WinWindowFromID (hwnd, IDD_COLOR + sColor)) ;
               return 1 ;

          case WM_CONTROL:
               WinSendDlgItemMsg (hwnd, IDD_COLOR + sColor, BM_SETCHECK,
                                  MPFROM2SHORT (FALSE, 0), NULL) ;

               sColor = SHORT1FROMMP (mp1) - IDD_COLOR ;

               WinSendDlgItemMsg (hwnd, IDD_COLOR + sColor, BM_SETCHECK,
                                  MPFROM2SHORT (TRUE, 0), NULL) ;
               return 0 ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                         *pclr = (LONG) sColor ;
                         WinDismissDlg (hwnd, TRUE) ;
                         return 0 ;

                    case DID_CANCEL:
                         WinDismissDlg (hwnd, FALSE) ;
                         return 0 ;
                    }
               break ;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }

     /*-------------------------------------------------
        CkdCreatePS: Create PS for checker board window
       -------------------------------------------------*/

HPS CkdCreatePS (HWND hwnd)
     {
     HDC    hdc ;
     HPS    hps ;
     SIZEL  sizlPage ;
     USHORT sDataSize ;

     CkdQueryBoardDimensions (&sizlPage) ;

     hdc = WinOpenWindowDC (hwnd) ;
     hps = GpiCreatePS (hab, hdc, &sizlPage,
                        PU_ARBITRARY | GPIF_DEFAULT |
                        GPIT_MICRO   | GPIA_ASSOC) ;

     GpiQueryPageViewport (hps, &rclOrigViewport) ;

               // Get colors from OS2.INI

     sDataSize = sizeof (LONG) ;
     WinQueryProfileData (hab, szApplication, szClrBackground,
                          &clrBackground, &sDataSize) ;

     sDataSize = sizeof (LONG) ;
     WinQueryProfileData (hab, szApplication, szClrBlackSquare,
                          &clrBlackSquare, &sDataSize) ;

     sDataSize = sizeof (LONG) ;
     WinQueryProfileData (hab, szApplication, szClrWhiteSquare,
                          &clrWhiteSquare, &sDataSize) ;

     sDataSize = sizeof (LONG) ;
     WinQueryProfileData (hab, szApplication, szClrBlackPiece,
                          &clrBlackPiece, &sDataSize) ;

     sDataSize = sizeof (LONG) ;
     WinQueryProfileData (hab, szApplication, szClrWhitePiece,
                          &clrWhitePiece, &sDataSize) ;
     return hps ;
     }

     /*-------------------------------------------------------
        CkdResizePS: Change page viewport for new window size
       -------------------------------------------------------*/

VOID CkdResizePS (HPS hps, HWND hwnd)
     {
     LONG  lScale ;
     RECTL rclWindow, rclViewport ;

     WinQueryWindowRect (hwnd, &rclWindow) ;

               // Calculate scaling factor

     lScale = min (65536L * rclWindow.xRight / rclOrigViewport.xRight,
                   65536L * rclWindow.yTop   / rclOrigViewport.yTop) ;

               // Adjust page viewport of memory PS

     rclViewport.xLeft   = 0 ;
     rclViewport.yBottom = 0 ;
     rclViewport.xRight  = lScale * rclOrigViewport.xRight / 65536L ;
     rclViewport.yTop    = lScale * rclOrigViewport.yTop   / 65536L ;

     rclViewport.xLeft   = (rclWindow.xRight - rclViewport.xRight) / 2 ;
     rclViewport.yBottom = (rclWindow.yTop   - rclViewport.yTop)   / 2 ;
     rclViewport.xRight += rclViewport.xLeft ;
     rclViewport.yTop   += rclViewport.yBottom ;

     GpiSetPageViewport (hps, &rclViewport) ;
     }

     /*---------------------------------------------------
        CkdDestroyPS: Destroy PS for checker board window
       ---------------------------------------------------*/

BOOL CkdDestroyPS (HPS hps)
     {
               // Save colors in OS2.INI

     WinWriteProfileData (hab, szApplication, szClrBackground,
                          &clrBackground, sizeof (LONG)) ;

     WinWriteProfileData (hab, szApplication, szClrBlackSquare,
                          &clrBlackSquare, sizeof (LONG)) ;

     WinWriteProfileData (hab, szApplication, szClrWhiteSquare,
                          &clrWhiteSquare, sizeof (LONG)) ;

     WinWriteProfileData (hab, szApplication, szClrBlackPiece,
                          &clrBlackPiece, sizeof (LONG)) ;

     WinWriteProfileData (hab, szApplication, szClrWhitePiece,
                          &clrWhitePiece, sizeof (LONG)) ;

     return GpiDestroyPS (hps) ;
     }

     /*-----------------------------------------------------------
        CkdSetStandardColors: Sets colors to tournament standards
       -----------------------------------------------------------*/

VOID CkdSetStandardColors (VOID)
     {
     clrBackground  = CLR_CYAN ;
     clrBlackSquare = CLR_DARKGREEN ;
     clrWhiteSquare = CLR_PALEGRAY ;
     clrBlackPiece  = CLR_RED ;
     clrWhitePiece  = CLR_WHITE ;
     }

     /*------------------------------------------------------------
        CkdCreatePieces: Creates bitmaps to use for drawing pieces
       ------------------------------------------------------------*/

VOID CkdCreatePieces (HPS hps)
     {
     BITMAPINFOHEADER bmp ;
     LONG             alBitmapFormat[2] ;
     RECTL            rclViewport ;
     SHORT            sColor, sKing ;
     SIZEL            sizlPage ;

               // Create memory DC's and PS's

     CkdQueryBoardDimensions (&sizlPage) ;

     hdcMemory = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;
     hpsMemory = GpiCreatePS (hab, hdcMemory, &sizlPage,
                              PU_ARBITRARY | GPIF_DEFAULT |
                              GPIT_MICRO   | GPIA_ASSOC) ;

               // Set page viewport for hpsMemory

     GpiQueryPageViewport (hps, &rclViewport) ;

     rclViewport.xRight -= rclViewport.xLeft ;
     rclViewport.yTop   -= rclViewport.yBottom ;
     rclViewport.xLeft   = 0 ;
     rclViewport.yBottom = 0 ;

     GpiSetPageViewport (hpsMemory, &rclViewport) ;

               // Get bitmap format of video display

     GpiQueryDeviceBitmapFormats (hps, 2L, alBitmapFormat) ;

               // Loop through possible color and size combinations

     for (sKing = 0 ; sKing < 2 ; sKing++)
          {
                    // Determine pixel dimensions of bitmaps

          sizlPiece[sKing].cx = PIECE_XAXIS ;
          sizlPiece[sKing].cy = PIECE_YAXIS + (sKing + 1) * PIECE_HEIGHT ;

          GpiConvert (hpsMemory, CVTC_PAGE, CVTC_DEVICE, 1L,
                      (PPOINTL) &sizlPiece[sKing]) ;

          sizlPiece[sKing].cx ++ ;
          sizlPiece[sKing].cy ++ ;

                    // Set up BITMAPINFOHEADER structure

          bmp.cbFix     = sizeof bmp ;
          bmp.cx        = (SHORT) sizlPiece[sKing].cx ;
          bmp.cy        = (SHORT) sizlPiece[sKing].cy ;
          bmp.cPlanes   = (SHORT) alBitmapFormat[0] ;
          bmp.cBitCount = (SHORT) alBitmapFormat[1] ;

                    // Create ahbmPiece bitmaps

          for (sColor = BLACK ; sColor <= WHITE ; sColor++)
               {
               ahbmPiece[sColor][sKing] =
                         GpiCreateBitmap (hps, &bmp, 0L, 0L, NULL) ;

               GpiSetBitmap (hpsMemory, ahbmPiece[sColor][sKing]) ;
               CkdRenderPiece (hpsMemory, CLR_FALSE,
                               sColor ? clrWhitePiece : clrBlackPiece,
                               CLR_BLACK, sKing) ;
               }

                    // Create ahbmMask bitmaps

          ahbmMask[sKing] = GpiCreateBitmap (hps, &bmp, 0L, 0L, NULL) ;
          GpiSetBitmap (hpsMemory, ahbmMask[sKing]) ;
          CkdRenderPiece (hpsMemory, CLR_TRUE, CLR_FALSE, CLR_FALSE, sKing) ;
          }

     GpiSetBitmap (hpsMemory, NULL) ;
     }

     /*---------------------------------------------------
        CkdDestroyPieces: Destroy bitmaps used for pieces
       ---------------------------------------------------*/

VOID CkdDestroyPieces (VOID)
     {
     SHORT sColor, sKing ;

     for (sKing = 0 ; sKing < 2 ; sKing++)
          {
          for (sColor = BLACK ; sColor <= WHITE ; sColor++)
               if (ahbmPiece[sColor][sKing] != NULL)
                    GpiDeleteBitmap (ahbmPiece[sColor][sKing]) ;

          if (ahbmMask[sKing] != NULL)
               GpiDeleteBitmap (ahbmMask[sKing]) ;
          }

     GpiDestroyPS (hpsMemory) ;
     DevCloseDC (hdcMemory) ;
     }

     /*--------------------------------------------------------------------
        CkdDrawWindowBackground: Fills entire window with background color
       --------------------------------------------------------------------*/

VOID CkdDrawWindowBackground (HPS hps, HWND hwnd)
     {
     RECTL rcl ;

     WinQueryWindowRect (hwnd, &rcl) ;
     WinFillRect (hps, &rcl, clrBackground) ;
     }

     /*-----------------------------------------------------------
        CkdDrawWholeBoard: Draws the board squares and front edge
       -----------------------------------------------------------*/

VOID CkdDrawWholeBoard (HPS hps)
     {
     AREABUNDLE abnd ;
     LINEBUNDLE lbnd ;
     SHORT      x ;
     POINTL     aptl[4] ;

     CkdDrawAllBoardSquares (hps) ;

     GpiSavePS (hps) ;

     lbnd.lColor = CLR_BLACK ;
     GpiSetAttrs (hps, PRIM_LINE, LBB_COLOR, 0L, &lbnd) ;

     for (x = 0 ; x < 8 ; x++)
          {
          CkdQuerySquareCoords (x, 0, aptl) ;

          aptl[2].x = aptl[1].x ;
          aptl[2].y = aptl[1].y - BRD_EDGE ;

          aptl[3].x = aptl[0].x ;
          aptl[3].y = aptl[0].y - BRD_EDGE ;

          abnd.lColor = x & 1 ? clrWhiteSquare : clrBlackSquare ;
          GpiSetAttrs (hps, PRIM_AREA, LBB_COLOR, 0L, &abnd) ;

          GpiBeginArea (hps, BA_ALTERNATE | BA_BOUNDARY) ;

          GpiMove (hps, aptl + 3) ;
          GpiPolyLine (hps, 4L, aptl) ;

          GpiEndArea (hps) ;
          }

     GpiRestorePS (hps, -1L) ;
     }

     /*-----------------------------------------------------
        CkdDrawAllPieces: Draws all the pieces on the board
       -----------------------------------------------------*/

VOID CkdDrawAllPieces (HPS hps, BOARD *pbrd, SHORT sBottom)
     {
     SHORT x, y ;

     for (y = 0 ; y < 8 ; y++)
          for (x = 0 ; x < 8 ; x++)
               CkdDrawOnePiece (hps, x, y, pbrd, sBottom) ;
     }
