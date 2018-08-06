/*--------------------------------------------------------
   CKRDRAW.C -- Ckd Board Drawing Functions, Version 0.40
                (c) 1990, Charles Petzold
  --------------------------------------------------------*/

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

static HDC     hdcMemory, hdcMemory2 ;
static HPS     hpsMemory, hpsMemory2 ;
static HBITMAP ahbmPiece[2][2], ahbmMask[2], ahbmSave[2], ahbmMove[2] ;
static SIZEL   sizlPiece[2], sizlMove[2] ;

     /*-------------------------------------------------------------
        CkdQueryBoardDimensions: Obtains size of board with margins
       -------------------------------------------------------------*/

VOID CkdQueryBoardDimensions (SIZEL *psizlPage)
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

     if (x < 0 || x >= 8 || y < 0 || y >= 8)
          {
          GpiSetColor (hps, clrBackground) ;
          }
     else
          {
          lbnd.lColor = CLR_BLACK ;
          GpiSetAttrs (hps, PRIM_LINE, LBB_COLOR, 0L, &lbnd) ;

          abnd.lColor = (x + y) & 1 ? clrWhiteSquare : clrBlackSquare ;
          GpiSetAttrs (hps, PRIM_AREA, LBB_COLOR, 0L, &abnd) ;
          }

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

     /*------------------------------------------
        CkdRenderPiece: Draws piece PS at (0, 0)
       ------------------------------------------*/

static VOID CkdRenderPiece (HPS hps, LONG lColor, LONG lColorLine, SHORT sKing)
     {
     ARCPARAMS  arcp ;
     AREABUNDLE abnd ;
     LINEBUNDLE lbnd ;
     POINTL     ptl, aptlArc[2] ;
     SHORT      s ;

     GpiSavePS (hps) ;

               // Set colors for areas and outlines

     abnd.lColor = lColor ;
     GpiSetAttrs (hps, PRIM_AREA, ABB_COLOR, 0L, &abnd) ;

     lbnd.lColor = lColorLine ;
     GpiSetAttrs (hps, PRIM_LINE, LBB_COLOR, 0L, &lbnd) ;

               // Set arc parameters

     arcp.lP = PIECE_XAXIS / 2 ;
     arcp.lQ = PIECE_YAXIS / 2 ;
     arcp.lR = 0 ;
     arcp.lS = 0 ;
     GpiSetArcParams (hps, &arcp) ;

               // draw the piece

     for (s = 0 ; s <= sKing ; s++)
          {
          GpiBeginArea (hps, BA_ALTERNATE | BA_BOUNDARY) ;

          ptl.x = 0 ;
          ptl.y = PIECE_YAXIS / 2 + (s + 1) * PIECE_HEIGHT ;
          GpiMove (hps, &ptl) ;

          ptl.y -= PIECE_HEIGHT ;
          GpiLine (hps, &ptl) ;

          aptlArc[0].x = PIECE_XAXIS / 2 ;
          aptlArc[0].y = s * PIECE_HEIGHT ;
          aptlArc[1].x = PIECE_XAXIS ;
          aptlArc[1].y = PIECE_YAXIS / 2 + s * PIECE_HEIGHT ;

          GpiPointArc (hps, aptlArc) ;

          ptl.x = PIECE_XAXIS ;
          ptl.y = PIECE_YAXIS / 2 + (s + 1) * PIECE_HEIGHT ;
          GpiLine (hps, &ptl) ;

          GpiEndArea (hps) ;
          }

     ptl.x = PIECE_XAXIS / 2 ;
     ptl.y = PIECE_YAXIS / 2 + (sKing ? 2 : 1) * PIECE_HEIGHT ;

     GpiMove (hps, &ptl) ;
     GpiFullArc (hps, DRO_OUTLINEFILL, MAKEFIXED (1,0)) ;

     GpiRestorePS (hps, -1L) ;
     }

     /*----------------------------------------------------------
        CkdRenderPieceBitmap: Draws piece on bitmap in memory PS
       ----------------------------------------------------------*/

static VOID CkdRenderPieceBitmap (HPS hpsMemory, LONG lColorBack, LONG lColor,
                                  LONG lColorLine, SHORT sKing)
     {
     POINTL ptl ;

     GpiSavePS (hpsMemory) ;

               // Draw background of bitmap

     GpiSetColor (hpsMemory, lColorBack) ;

     ptl.x = 0 ;
     ptl.y = 0 ;
     GpiMove (hpsMemory, &ptl) ;

     ptl.x = PIECE_XAXIS ;
     ptl.y = PIECE_YAXIS + (sKing + 1) * PIECE_HEIGHT ;
     GpiBox (hpsMemory, DRO_FILL, &ptl, 0L, 0L) ;

     CkdRenderPiece (hpsMemory, lColor, lColorLine, sKing) ;

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

     /*-----------------------------------------------------------------
        CkdPieceOriginFromCenter: Same as above, but device coordinates
       -----------------------------------------------------------------*/

static VOID CkdPieceOriginFromCenterDevice (POINTL *pptlOrg, POINTL *pptlCntr)
     {
     pptlOrg->x = pptlCntr->x - sizlPiece[0].cx / 2 ;
     pptlOrg->y = pptlCntr->y - sizlPiece[0].cy / 2 ;
     }

     /*----------------------------------------------------------------
        CkdQuerySquarePieceOrigin: Obtains origin of piece on a square
       ----------------------------------------------------------------*/

static VOID CkdQuerySquarePieceOrigin (SHORT x, SHORT y, POINTL *pptlOrigin)
     {
     CkdQuerySquareCenter (x, y, pptlOrigin) ;
     CkdPieceOriginFromCenter (pptlOrigin) ;
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

     /*------------------------------------------------------------------
        CkdPointInsideTrapezoid: Determines if point is inside trapezoid
       ------------------------------------------------------------------*/

static BOOL CkdPointInsideTrapezoid (POINTL *pptl, POINTL aptl[])
     {
     if (pptl->x < aptl[0].x || pptl->x >= aptl[1].x)
          return FALSE ;

     if (pptl->y < aptl[0].y || pptl->y >= aptl[2].y)
          return FALSE ;

     if (pptl->x < (aptl[0].x * (aptl[3].y - pptl->y) +
                    aptl[3].x * (pptl->y - aptl[0].y)) /
                   (aptl[3].y - aptl[0].y))
          return FALSE ;

     if (pptl->x >= (aptl[1].x * (aptl[3].y - pptl->y) +
                     aptl[2].x * (pptl->y - aptl[0].y)) /
                    (aptl[3].y - aptl[0].y))
          return FALSE ;

     return TRUE ;
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

               return MRFROMSHORT (1) ;

          case WM_CONTROL:
               sColor = SHORT1FROMMP (mp1) - IDD_COLOR ;
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

               // Create PS and save original viewport

     hdc = WinOpenWindowDC (hwnd) ;
     hps = GpiCreatePS (hab, hdc, &sizlPage,
                        PU_ARBITRARY | GPIF_DEFAULT |
                        GPIT_MICRO   | GPIA_ASSOC) ;

     GpiQueryPageViewport (hps, &rclOrigViewport) ;

               // Create memory DC's and PS's

     hdcMemory  = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;
     hpsMemory  = GpiCreatePS (hab, hdcMemory, &sizlPage,
                               PU_ARBITRARY | GPIF_DEFAULT |
                               GPIT_MICRO   | GPIA_ASSOC) ;

     hdcMemory2 = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;
     hpsMemory2 = GpiCreatePS (hab, hdcMemory2, &sizlPage,
                               PU_ARBITRARY | GPIF_DEFAULT |
                               GPIT_MICRO   | GPIA_ASSOC) ;

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

               // Adjust page viewport of window PS

     rclViewport.xLeft   = 0 ;
     rclViewport.yBottom = 0 ;
     rclViewport.xRight  = lScale * rclOrigViewport.xRight / 65536L ;
     rclViewport.yTop    = lScale * rclOrigViewport.yTop   / 65536L ;

     rclViewport.xLeft   = (rclWindow.xRight - rclViewport.xRight) / 2 ;
     rclViewport.yBottom = (rclWindow.yTop   - rclViewport.yTop)   / 2 ;
     rclViewport.xRight += rclViewport.xLeft ;
     rclViewport.yTop   += rclViewport.yBottom ;

     GpiSetPageViewport (hps, &rclViewport) ;

               // Similarly for memory PS's

     rclViewport.xRight -= rclViewport.xLeft ;
     rclViewport.yTop   -= rclViewport.yBottom ;
     rclViewport.xLeft   = 0 ;
     rclViewport.yBottom = 0 ;

     GpiSetPageViewport (hpsMemory,  &rclViewport) ;
     GpiSetPageViewport (hpsMemory2, &rclViewport) ;
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

               // Destroy memory PS's and close DC's

     GpiDestroyPS (hpsMemory) ;
     GpiDestroyPS (hpsMemory2) ;
     DevCloseDC (hdcMemory) ;
     DevCloseDC (hdcMemory2) ;

               // Destroy screen PS

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
     SHORT            sColor, sKing ;

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
          sizlMove[sKing].cx = 2 * sizlPiece[sKing].cx ;
          sizlMove[sKing].cy = 2 * sizlPiece[sKing].cy ;

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
               CkdRenderPieceBitmap (hpsMemory, CLR_FALSE,
                               sColor ? clrWhitePiece : clrBlackPiece,
                               CLR_BLACK, sKing) ;
               }

                    // Create ahbmMask bitmaps

          ahbmMask[sKing] = GpiCreateBitmap (hps, &bmp, 0L, 0L, NULL) ;
          GpiSetBitmap (hpsMemory, ahbmMask[sKing]) ;
          CkdRenderPieceBitmap (hpsMemory, CLR_TRUE, CLR_FALSE, CLR_FALSE, sKing) ;

                    // Create ahbmSave bitmaps

          ahbmSave[sKing] = GpiCreateBitmap (hps, &bmp, 0L, 0L, NULL) ;

                    // Create ahbmMove bitmaps

          bmp.cbFix     = sizeof bmp ;
          bmp.cx        = (SHORT) sizlMove[sKing].cx ;
          bmp.cy        = (SHORT) sizlMove[sKing].cy ;
          bmp.cPlanes   = (SHORT) alBitmapFormat[0] ;
          bmp.cBitCount = (SHORT) alBitmapFormat[1] ;

          ahbmMove[sKing] = GpiCreateBitmap (hps, &bmp, 0L, 0L, NULL) ;
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

          if (ahbmSave[sKing] != NULL)
               GpiDeleteBitmap (ahbmSave[sKing]) ;

          if (ahbmMove[sKing] != NULL)
               GpiDeleteBitmap (ahbmMove[sKing]) ;
          }
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

     /*---------------------------------------------------------------------
        CkdErasePiece: Erases piece from board by drawing the board squares
       ---------------------------------------------------------------------*/

VOID CkdErasePiece (HPS hps, SHORT x, SHORT y)
     {
     CkdDrawBoardSquare (hps, x, y + 1) ;
     CkdDrawBoardSquare (hps, x, y) ;
     }

     /*---------------------------------------------------------------
        CkdQueryHitCoords: Obtains coords from mouse pointer position
       ---------------------------------------------------------------*/

VOID CkdQueryHitCoords (HPS hps, POINTL ptlMouse, SHORT *px, SHORT *py)
     {
     POINTL aptl[4] ;
     SHORT  x, y ;

     GpiConvert (hps, CVTC_DEVICE, CVTC_PAGE, 1L, &ptlMouse) ;

     for (y = 0 ; y < 8 ; y++)
          for (x = 0 ; x < 8 ; x++)
               {
               CkdQuerySquareCoords (x, y, aptl) ;
               if (CkdPointInsideTrapezoid (&ptlMouse, aptl))
                    {
                    *px = x ;
                    *py = y ;
                    return ;
                    }
               }

     *px = -1 ;
     *py = -1 ;
     }

     /*-----------------------------------------------------------------------
        CkdConvertCoordsToIndex: Obtains index (0-31) from square coordinates
       -----------------------------------------------------------------------*/

SHORT CkdConvertCoordsToIndex (SHORT x, SHORT y, SHORT sBottom)
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

     /*-----------------------------------------------------------------------
        CkdConvertIndexToCoords: Obtains square coordinates from index (0-31)
       -----------------------------------------------------------------------*/

VOID CkdConvertIndexToCoords (SHORT i, SHORT *px, SHORT *py, SHORT sBottom)
     {
     if (i <= 0 || i >= 32)
          {
          *px = -1 ;
          *py = -1 ;
          }

     *py = i / 4 ;
     *px = 2 * ((i ^ 3) % 4) + (*py & 1) ;

     if (sBottom == WHITE)
          {
          *px = 7 - *px ;
          *py = 7 - *py ;
          }
     }

     /*----------------------------------------------------
        CkdDragSave: Saves screen area when dragging piece
       ----------------------------------------------------*/

VOID CkdDragSave (HPS hps, POINTL *pptlMouse, SHORT sKing)
     {
     POINTL ptlOrigin, aptl[3] ;

     CkdPieceOriginFromCenterDevice (&ptlOrigin, pptlMouse) ;

     aptl[0].x = 0 ;
     aptl[0].y = 0 ;
     aptl[1].x = sizlPiece[sKing].cx ;
     aptl[1].y = sizlPiece[sKing].cy ;
     aptl[2]   = ptlOrigin ;

     GpiSetBitmap (hpsMemory, ahbmSave[sKing]) ;
     GpiBitBlt    (hpsMemory, hps, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE) ;
     GpiSetBitmap (hpsMemory, NULL) ;
     }

     /*----------------------------------------------------------
        CkdDragRestore: Restores screen area when dragging piece
       ----------------------------------------------------------*/

VOID CkdDragRestore (HPS hps, POINTL *pptlMouse, SHORT sKing)
     {
     POINTL ptlOrigin, aptl[3] ;

     CkdPieceOriginFromCenterDevice (&ptlOrigin, pptlMouse) ;

     aptl[0]   = ptlOrigin ;
     aptl[1].x = ptlOrigin.x + sizlPiece[sKing].cx ;
     aptl[1].y = ptlOrigin.y + sizlPiece[sKing].cy ;
     aptl[2].x = 0 ;
     aptl[2].y = 0 ;

     GpiSetBitmap (hpsMemory, ahbmSave[sKing]) ;
     GpiBitBlt    (hps, hpsMemory, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE) ;
     GpiSetBitmap (hpsMemory, NULL) ;
     }

     /*-------------------------------------------------------------
        CkdDragShow: Shows piece in new position when being dragged
       -------------------------------------------------------------*/

VOID CkdDragShow (HPS hps, POINTL *pptlMouse, SHORT sColor, SHORT sKing)
     {
     POINTL ptlOrigin, aptl[3] ;

     CkdPieceOriginFromCenterDevice (&ptlOrigin, pptlMouse) ;

               // Write out mask with bitwise AND

     aptl[0]   = ptlOrigin ;
     aptl[1].x = ptlOrigin.x + sizlPiece[sKing].cx ;
     aptl[1].y = ptlOrigin.y + sizlPiece[sKing].cy ;
     aptl[2].x = 0 ;
     aptl[2].y = 0 ;

     GpiSetBitmap (hpsMemory, ahbmMask[sKing]) ;
     GpiBitBlt    (hps, hpsMemory, 3L, aptl, ROP_SRCAND, BBO_IGNORE) ;

               // Write out piece with bitwise OR

     aptl[0]   = ptlOrigin ;
     aptl[1].x = ptlOrigin.x + sizlPiece[sKing].cx ;
     aptl[1].y = ptlOrigin.y + sizlPiece[sKing].cy ;
     aptl[2].x = 0 ;
     aptl[2].y = 0 ;

     GpiSetBitmap (hpsMemory, ahbmPiece[sColor][sKing]) ;
     GpiBitBlt    (hps, hpsMemory, 3L, aptl, ROP_SRCPAINT, BBO_IGNORE) ;
     GpiSetBitmap (hpsMemory, NULL) ;
     }

     /*-------------------------------------------------------------
        CkdDragMove: Moves piece from one part of screen to another
       -------------------------------------------------------------*/

VOID CkdDragMove (HPS hps, POINTL *pptlFrom, POINTL *pptlTo,
                           SHORT sColor, SHORT sKing)
     {
     POINTL ptlCenter, ptlOrigin, aptl[3], ptlFrom, ptlTo ;

     if ((labs (pptlFrom->x - pptlTo->x) > sizlPiece[sKing].cx) ||
         (labs (pptlFrom->y - pptlTo->y) > sizlPiece[sKing].cy))
          {
          CkdDragRestore (hps, pptlFrom, sKing) ;
          CkdDragSave    (hps, pptlTo,   sKing) ;
          CkdDragShow    (hps, pptlTo,   sColor, sKing) ;

          return ;
          }

     ptlCenter.x = min (pptlFrom->x, pptlTo->x) ;
     ptlCenter.y = min (pptlFrom->y, pptlTo->y) ;
     CkdPieceOriginFromCenterDevice (&ptlOrigin, &ptlCenter) ;

               // Copy screen into ahbmMove bitmap in hpsMemory2

     aptl[0].x = 0 ;
     aptl[0].y = 0 ;
     aptl[1].x = sizlMove[sKing].cx ;
     aptl[1].y = sizlMove[sKing].cy ;
     aptl[2]   = ptlOrigin ;

     GpiSetBitmap (hpsMemory2, ahbmMove[sKing]) ;
     GpiBitBlt (hpsMemory2, hps, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE) ;

               // Do restore, save, & show to hpsMemory2

     ptlFrom.x = pptlFrom->x - ptlOrigin.x ;
     ptlFrom.y = pptlFrom->y - ptlOrigin.y ;
     ptlTo.x   = pptlTo->x   - ptlOrigin.x ;
     ptlTo.y   = pptlTo->y   - ptlOrigin.y ;

     CkdDragRestore (hpsMemory2, &ptlFrom, sKing) ;
     CkdDragSave    (hpsMemory2, &ptlTo,   sKing) ;
     CkdDragShow    (hpsMemory2, &ptlTo,   sColor, sKing) ;

               // Copy ahbmMove bitmap in hpsMemory2 to screen

     aptl[0]   = ptlOrigin ;
     aptl[1].x = ptlOrigin.x + sizlMove[sKing].cx ;
     aptl[1].y = ptlOrigin.y + sizlMove[sKing].cy ;
     aptl[2].x = 0 ;
     aptl[2].y = 0 ;

     GpiBitBlt (hps, hpsMemory2, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE) ;
     GpiSetBitmap (hpsMemory2, NULL) ;
     }

     /*-----------------------------------------------
        CkdDragDeposit: Displays piece at end of drag
       -----------------------------------------------*/

VOID CkdDragDeposit (HPS hps, SHORT x, SHORT y, SHORT sColor, SHORT sKing)
     {
     POINTL ptlOrigin ;

     CkdQuerySquarePieceOrigin (x, y, &ptlOrigin) ;
     GpiConvert (hps, CVTC_PAGE, CVTC_DEVICE, 1L, &ptlOrigin) ;
     CkdShowPiece (hps, &ptlOrigin, sColor, sKing) ;
     }

     /*----------------------------------------------------
        CkdQueryNearestXYFromPoint: For keyboard interface
       ----------------------------------------------------*/

VOID CkdQueryNearestXYFromPoint (HPS hps, POINTL *pptlMouse, SHORT *px,
                                                             SHORT *py)
     {
     POINTL ptl, aptlSquare[4] ;
     SHORT  x, y ;

     ptl = *pptlMouse ;
     GpiConvert (hps, CVTC_DEVICE, CVTC_PAGE, 1L, &ptl) ;

     for (y = 0 ; y < 7 ; y++)
          {
          CkdQuerySquareCoords (0, y, aptlSquare) ;
          if (aptlSquare[3].y > ptl.y)
               break ;
          }

     for (x = 0 ; x < 7 ; x++)
          {
          CkdQuerySquareCoords (x, y, aptlSquare) ;
          if ((aptlSquare[0].x + aptlSquare[1].x) / 2 > ptl.x)
               break ;
          }
     *px = x ;
     *py = y ;
     }

     /*----------------------------------------------------
        CkdSlightOffsetFromXY: For keyboard interface
       ----------------------------------------------------*/

VOID CkdQuerySlightOffsetFromXY (HPS hps, SHORT x, SHORT y, POINTL *pptl)
     {
     POINTL ptlCenter, ptlOrigin ;

     CkdQuerySquareCenter (x, y, &ptlCenter) ;
     CkdQuerySquareOrigin (x, y, &ptlOrigin) ;

     pptl->x = (3 * ptlCenter.x + ptlOrigin.x) / 4 ;
     pptl->y = (3 * ptlCenter.y + ptlOrigin.y) / 4 ;

     GpiConvert (hps, CVTC_PAGE, CVTC_DEVICE, 1L, pptl) ;
     }

     /*--------------------------------------------------
        CkdExternalSave:  Save area (for external moves)
       --------------------------------------------------*/

VOID CkdExternalSave (HPS hps, SHORT x, SHORT y, SHORT sKing)
     {
     POINTL ptl ;

     CkdQuerySquareCenter (x, y, &ptl) ;
     GpiConvert (hps, CVTC_PAGE, CVTC_DEVICE, 1L, &ptl) ;
     CkdDragSave (hps, &ptl, sKing) ;
     }

     /*----------------------------------------------------------
        CkdExternalShow:  Show piece bitmap (for external moves)
       ----------------------------------------------------------*/

VOID CkdExternalShow (HPS hps, SHORT x, SHORT y, SHORT sColor, SHORT sKing)
     {
     POINTL ptl ;

     CkdQuerySquareCenter (x, y, &ptl) ;
     GpiConvert (hps, CVTC_PAGE, CVTC_DEVICE, 1L, &ptl) ;
     CkdDragShow (hps, &ptl, sColor, sKing) ;
     }

     /*--------------------------------------------------------
        CkdExternalRestore:  Restore area (for external moves)
       --------------------------------------------------------*/

VOID CkdExternalRestore (HPS hps, SHORT x, SHORT y, SHORT sKing)
     {
     POINTL ptl ;

     CkdQuerySquareCenter (x, y, &ptl) ;
     GpiConvert (hps, CVTC_PAGE, CVTC_DEVICE, 1L, &ptl) ;
     CkdDragRestore (hps, &ptl, sKing) ;
     }

     /*---------------------------------------------------
        CkdExternalMove:  Move piece (for external moves)
       ---------------------------------------------------*/

VOID CkdExternalMove (HPS hps, SHORT sTimerBeg,   SHORT sTimerEnd,
                               SHORT sTimerColor, SHORT sTimerKing,
                               SHORT sBottom,     SHORT sTimerInc,
                               SHORT sTimerSteps)
     {
     POINTL ptlBeg, ptlEnd, ptlFrom, ptlTo ;
     SHORT  x, y ;

     CkdConvertIndexToCoords (sTimerBeg, &x, &y, sBottom) ;
     CkdQuerySquareCenter (x, y, &ptlBeg) ;
     GpiConvert (hps, CVTC_PAGE, CVTC_DEVICE, 1L, &ptlBeg) ;

     CkdConvertIndexToCoords (sTimerEnd, &x, &y, sBottom) ;
     CkdQuerySquareCenter (x, y, &ptlEnd) ;
     GpiConvert (hps, CVTC_PAGE, CVTC_DEVICE, 1L, &ptlEnd) ;

     ptlFrom.x = ( sTimerInc                * ptlEnd.x +
                  (sTimerSteps - sTimerInc) * ptlBeg.x) / sTimerSteps ;

     ptlFrom.y = ( sTimerInc                * ptlEnd.y +
                  (sTimerSteps - sTimerInc) * ptlBeg.y) / sTimerSteps ;

     ptlTo.x   = ((sTimerInc + 1)               * ptlEnd.x +
                  (sTimerSteps - sTimerInc - 1) * ptlBeg.x) / sTimerSteps ;

     ptlTo.y   = ((sTimerInc + 1)               * ptlEnd.y +
                  (sTimerSteps - sTimerInc - 1) * ptlBeg.y) / sTimerSteps ;

     CkdDragMove (hps, &ptlFrom, &ptlTo, sTimerColor, sTimerKing) ;
     }

     /*------------------------------------------------------
        CkdDrawOnePieceDirect:  Draw piece (for setup board)
       ------------------------------------------------------*/

VOID CkdDrawOnePieceDirect (HPS hps, SHORT x, SHORT y, BOARD *pbrd,
                            SHORT sBottom)
     {
     MATRIXLF matlf ;
     POINTL   ptlOrigin ;
     SHORT    i, sPiece, sKing ;
     ULONG    ulBit ;

     i = CkdConvertCoordsToIndex (x, y, sBottom) ;

     if (i == -1)
          return ;

     CkdQuerySquarePieceOrigin (x, y, &ptlOrigin) ;

     ulBit = 1L <<  i ;

     if (pbrd->ulBlack & ulBit)
          sPiece = BLACK ;

     else if (pbrd->ulWhite & ulBit)
          sPiece = WHITE ;

     else
          return ;

     sKing = pbrd->ulKing & ulBit ? 1 : 0 ;

               // Move origin of PS

     GpiTranslate (hps, &matlf, TRANSFORM_REPLACE, &ptlOrigin) ;
     GpiSetModelTransformMatrix (hps, 9L, &matlf, TRANSFORM_REPLACE) ;

               // Render the piece on the board

     CkdRenderPiece (hps, sPiece ? clrWhitePiece : clrBlackPiece,
                     CLR_BLACK, sKing) ;

               // Restore origin of PS

     GpiSetModelTransformMatrix (hps, 0L, NULL, TRANSFORM_REPLACE) ;
     }

     /*------------------------------------------------------------
        CkdDrawAllPiecesDirect:  Draw all pieces (for setup board)
       ------------------------------------------------------------*/

VOID CkdDrawAllPiecesDirect (HPS hps, BOARD *pbrd, SHORT sBottom)
     {
     SHORT x, y ;

     for (y = 0 ; y < 8 ; y++)
          for (x = 0 ; x < 8 ; x++)
               CkdDrawOnePieceDirect (hps, x, y, pbrd, sBottom) ;
     }
