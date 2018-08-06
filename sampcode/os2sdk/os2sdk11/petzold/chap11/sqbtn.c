/*----------------------------------------------------------------
   SQBTN.C -- Contains window procedure for square 3D push button
  ----------------------------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <malloc.h>
#include <string.h>

#define LCID_ITALIC 1L

               /*--------------------------------------------------
                  Structure for storing data unique to each window
		 --------------------------------------------------*/
typedef struct
     {
     PSZ  pszText ;
     BOOL fHaveCapture ;
     BOOL fHaveFocus ;
     BOOL fInsideRect ;
     BOOL fSpaceDown ;
     }
     SQBTN ;

typedef SQBTN FAR *PSQBTN ;

MRESULT EXPENTRY SqBtnWndProc (HWND, USHORT, MPARAM, MPARAM) ;
VOID             DrawButton   (HWND, HPS, PSQBTN) ;

HAB  hab ;

          /*--------------------------------------------------------
             RegisterSqBtnClass function available to other modules
            --------------------------------------------------------*/

BOOL RegisterSqBtnClass (HAB habIn)
     {
     hab = habIn ;

     return WinRegisterClass (hab, "SqBtn", SqBtnWndProc,
                              CS_SIZEREDRAW, sizeof (PSQBTN)) ;
     }

          /*-------------------------------------------
             String functions that accept far pointers
            -------------------------------------------*/

USHORT fstrlen (PCHAR pch)
     {
     USHORT usLen ;
     for (usLen = 0 ; pch[usLen] ; usLen++) ;
     return usLen ;
     }

PCHAR fstrcpy (PCHAR pchDst, PCHAR pchSrc)
     {
     USHORT usIndex ;
     for (usIndex = 0 ; pchDst[usIndex] = pchSrc[usIndex] ; usIndex++) ;
     return pchDst ;
     }

          /*-------------------------------
             SqBtnWndProc window procedure
            -------------------------------*/

MRESULT EXPENTRY SqBtnWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     BOOL          fTestInsideRect ;
     HPS           hps ;
     PCREATESTRUCT pcrst ;
     POINTL        ptl ;
     PSQBTN        pSqBtn ;
     PWNDPARAMS    pwprm ;
     RECTL         rcl ;

     pSqBtn = WinQueryWindowPtr (hwnd, 0) ;

     switch (msg)
          {
          case WM_CREATE:
               pSqBtn = _fmalloc (sizeof (SQBTN)) ;

                         // Initialize structure

               pSqBtn->fHaveCapture = FALSE ;
               pSqBtn->fHaveFocus   = FALSE ;
               pSqBtn->fInsideRect  = FALSE ;
               pSqBtn->fSpaceDown   = FALSE ;

                         // Get window text from creation structure

               pcrst = (PCREATESTRUCT) PVOIDFROMMP (mp2) ;

               pSqBtn->pszText = _fmalloc (1 + fstrlen (pcrst->pszText)) ;
               fstrcpy (pSqBtn->pszText, pcrst->pszText) ;

               WinSetWindowPtr (hwnd, 0, pSqBtn) ;
               return 0 ;

          case WM_SETWINDOWPARAMS:
               pwprm = (PWNDPARAMS) PVOIDFROMMP (mp1) ;

                         // Get window text from window parameter structure

               if (pwprm->fsStatus & WPM_TEXT)
                    {
                    _ffree (pSqBtn->pszText) ;
                    pSqBtn->pszText = _fmalloc (1 + pwprm->cchText) ;
                    fstrcpy (pSqBtn->pszText, pwprm->pszText) ;
                    }
               return 1 ;

          case WM_QUERYWINDOWPARAMS:
               pwprm == (PWNDPARAMS) PVOIDFROMMP (mp1) ;

                         // Set window parameter structure fields

               if (pwprm->fsStatus & WPM_CCHTEXT)
                    pwprm->cchText = fstrlen (pSqBtn->pszText) ;

               if (pwprm->fsStatus & WPM_TEXT)
                    fstrcpy (pwprm->pszText, pSqBtn->pszText) ;

               if (pwprm->fsStatus & WPM_CBPRESPARAMS)
                    pwprm->cbPresParams = 0 ;

               if (pwprm->fsStatus & WPM_PRESPARAMS)
                    pwprm->pPresParams = NULL ;

               if (pwprm->fsStatus & WPM_CBCTLDATA)
                    pwprm->cbCtlData = 0 ;

               if (pwprm->fsStatus & WPM_CTLDATA)
                    pwprm->pCtlData = NULL ;

               return 1 ;

          case WM_BUTTON1DOWN:
               WinSetFocus (HWND_DESKTOP, hwnd) ;
               WinSetCapture (HWND_DESKTOP, hwnd) ;
               pSqBtn->fHaveCapture = TRUE ;
               pSqBtn->fInsideRect  = TRUE ;
               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_MOUSEMOVE:
               if (!pSqBtn->fHaveCapture)
                    break ;

               WinQueryWindowRect (hwnd, &rcl) ;
               ptl.x = MOUSEMSG(&msg)->x ;
               ptl.y = MOUSEMSG(&msg)->y ;

                         // Test if mouse pointer is still in window

               fTestInsideRect = WinPtInRect (hab, &rcl, &ptl) ;

               if (pSqBtn->fInsideRect != fTestInsideRect)
                    {
                    pSqBtn->fInsideRect = fTestInsideRect ;
                    WinInvalidateRect (hwnd, NULL, FALSE) ;
                    }
               break ;

          case WM_BUTTON1UP:
               if (!pSqBtn->fHaveCapture)
                    break ;

               WinSetCapture (HWND_DESKTOP, NULL) ;
               pSqBtn->fHaveCapture = FALSE ;
               pSqBtn->fInsideRect  = FALSE ;

               WinQueryWindowRect (hwnd, &rcl) ;
               ptl.x = MOUSEMSG(&msg)->x ;
               ptl.y = MOUSEMSG(&msg)->y ;

                         // Post WM_COMMAND if mouse pointer is in window

               if (WinPtInRect (hab, &rcl, &ptl))
                    WinPostMsg (WinQueryWindow (hwnd, QW_OWNER, FALSE),
                         WM_COMMAND,
                         MPFROMSHORT (WinQueryWindowUShort (hwnd, QWS_ID)),
                         MPFROM2SHORT (CMDSRC_OTHER, TRUE)) ;

               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_ENABLE:
               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_SETFOCUS:
               pSqBtn->fHaveFocus = SHORT1FROMMP (mp2) ;
               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_CHAR:
               if (!(CHARMSG(&msg)->fs & KC_VIRTUALKEY) ||
                     CHARMSG(&msg)->vkey != VK_SPACE    ||
                     CHARMSG(&msg)->fs & KC_PREVDOWN)
                    break ;

                         // Post WM_COMMAND when space bar is released

               if (!(CHARMSG(&msg)->fs & KC_KEYUP))
                    pSqBtn->fSpaceDown = TRUE ;
               else
                    {
                    pSqBtn->fSpaceDown = FALSE ;
                    WinPostMsg (WinQueryWindow (hwnd, QW_OWNER, FALSE),
                         WM_COMMAND,
                         MPFROMSHORT (WinQueryWindowUShort (hwnd, QWS_ID)),
                         MPFROM2SHORT (CMDSRC_OTHER, FALSE)) ;
                    }
               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               DrawButton (hwnd, hps, pSqBtn) ;
               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               _ffree (pSqBtn->pszText) ;
               _ffree (pSqBtn) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

          /*--------------------------------------------------------
             Draws filled and outlined polygon (used by DrawButton)
            --------------------------------------------------------*/

VOID Polygon (HPS hps, LONG lPoints, POINTL aptl[], LONG lColor)
     {
               // Draw interior in specified color

     GpiSavePS (hps) ;
     GpiSetColor (hps, lColor) ;

     GpiBeginArea (hps, BA_NOBOUNDARY | BA_ALTERNATE) ;
     GpiMove (hps, aptl) ;
     GpiPolyLine (hps, lPoints - 1, aptl + 1) ;
     GpiEndArea (hps) ;

     GpiRestorePS (hps, -1L) ;

               // Draw boundary in default color

     GpiMove (hps, aptl + lPoints - 1) ;
     GpiPolyLine (hps, lPoints, aptl) ;
     }

          /*---------------------
             Draws Square Button
            ---------------------*/

VOID DrawButton (HWND hwnd, HPS hps, PSQBTN pSqBtn)
     {
     FATTRS      fat ;
     FONTMETRICS fm ;
     HDC         hdc ;
     LONG        lColor, lHorzRes, lVertRes, cxEdge, cyEdge ;
     POINTL      aptl[10], aptlTextBox[TXTBOX_COUNT], ptlShadow, ptlText ;
     RECTL       rcl ;

               // Find 2 millimeter edge width in pixels

     hdc = GpiQueryDevice (hps) ;
     DevQueryCaps (hdc, CAPS_HORIZONTAL_RESOLUTION, 1L, &lHorzRes) ;
     DevQueryCaps (hdc, CAPS_VERTICAL_RESOLUTION,   1L, &lVertRes) ;

     cxEdge = lHorzRes / 500 ;
     cyEdge = lVertRes / 500 ;

               // Set up coordinates for drawing the button

     WinQueryWindowRect (hwnd, &rcl) ;

     aptl[0].x = 0 ;                    aptl[0].y = 0 ;
     aptl[1].x = cxEdge ;               aptl[1].y = cyEdge ;
     aptl[2].x = rcl.xRight - cxEdge ;  aptl[2].y = cyEdge ;
     aptl[3].x = rcl.xRight - 1 ;       aptl[3].y = 0 ;
     aptl[4].x = rcl.xRight - 1 ;       aptl[4].y = rcl.yTop - 1 ;
     aptl[5].x = rcl.xRight - cxEdge ;  aptl[5].y = rcl.yTop - cyEdge ;
     aptl[6].x = cxEdge ;               aptl[6].y = rcl.yTop - cyEdge ;
     aptl[7].x = 0 ;                    aptl[7].y = rcl.yTop - 1 ;
     aptl[8].x = 0 ;                    aptl[8].y = 0 ;
     aptl[9].x = cxEdge ;               aptl[9].y = cyEdge ;

               // Paint edges at bottom and right side

     GpiSetColor (hps, CLR_BLACK) ;
     lColor = (pSqBtn->fInsideRect || pSqBtn->fSpaceDown) ?
                              CLR_PALEGRAY : CLR_DARKGRAY ;
     Polygon (hps, 4L, aptl + 0, lColor) ;
     Polygon (hps, 4L, aptl + 2, lColor) ;

               // Paint edges at top and left side

     lColor = (pSqBtn->fInsideRect || pSqBtn->fSpaceDown) ?
                              CLR_DARKGRAY : CLR_WHITE ;
     Polygon (hps, 4L, aptl + 4, lColor) ;
     Polygon (hps, 4L, aptl + 6, lColor) ;

               // Paint interior area

     GpiSavePS (hps) ;
     GpiSetColor (hps, (pSqBtn->fInsideRect || pSqBtn->fSpaceDown) ?
                              CLR_DARKGRAY : CLR_PALEGRAY) ;
     GpiMove (hps, aptl + 1) ;
     GpiBox (hps, DRO_FILL, aptl + 5, 0L, 0L) ;
     GpiRestorePS (hps, -1L) ;
     GpiBox (hps, DRO_OUTLINE, aptl + 5, 0L, 0L) ;

               // If button has focus, use italic font

     GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;

     if (pSqBtn->fHaveFocus)
          {
          fat.usRecordLength  = sizeof fat ;
          fat.fsSelection     = FATTR_SEL_ITALIC ;
          fat.lMatch          = 0 ;
          fat.idRegistry      = fm.idRegistry ;
          fat.usCodePage      = fm.usCodePage ;
          fat.lMaxBaselineExt = fm.lMaxBaselineExt ;
          fat.lAveCharWidth   = fm.lAveCharWidth ;
          fat.fsType          = 0 ;
          fat.fsFontUse       = 0 ;
          strcpy (fat.szFacename, fm.szFacename) ;

          GpiCreateLogFont (hps, NULL, LCID_ITALIC, &fat) ;
          GpiSetCharSet (hps, LCID_ITALIC) ;
          }
               // Calculate text position

     GpiQueryTextBox (hps, (LONG) fstrlen (pSqBtn->pszText), pSqBtn->pszText,
                           TXTBOX_COUNT, aptlTextBox) ;

     ptlText.x = (rcl.xRight - aptlTextBox[TXTBOX_CONCAT].x) / 2 ;
     ptlText.y = (rcl.yTop   - aptlTextBox[TXTBOX_TOPLEFT].y -
                               aptlTextBox[TXTBOX_BOTTOMLEFT].y) / 2 ;

     ptlShadow.x = ptlText.x + fm.lAveCharWidth   / 3 ;
     ptlShadow.y = ptlText.y - fm.lMaxBaselineExt / 8 ;

               // Display text shadow in black, and text in white

     GpiSetColor (hps, CLR_BLACK) ;
     GpiCharStringAt (hps, &ptlShadow, (LONG) fstrlen (pSqBtn->pszText),
                                       pSqBtn->pszText) ;
     GpiSetColor (hps, CLR_WHITE) ;
     GpiCharStringAt (hps, &ptlText, (LONG) fstrlen (pSqBtn->pszText),
                                     pSqBtn->pszText) ;

               // X out button if the window is not enabled

     if (!WinIsWindowEnabled (hwnd))
          {
          GpiMove (hps, aptl + 1) ;
          GpiLine (hps, aptl + 5) ;
          GpiMove (hps, aptl + 2) ;
          GpiLine (hps, aptl + 6) ;
          }
               // Clean up

     if (pSqBtn->fHaveFocus)
          {
          GpiSetCharSet (hps, LCID_DEFAULT) ;
          GpiDeleteSetId (hps, LCID_ITALIC) ;
          }
     }
