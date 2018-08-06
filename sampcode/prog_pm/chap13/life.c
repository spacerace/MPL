/*--------------------------------------
   LIFE.C -- John Conway's Game of Life
  --------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "life.h"

#define ID_TIMER    1

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

CHAR szClientClass [] = "Life" ;
HAB  hab ;

int main (void)
     {
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_MENU          | FCF_ICON ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
                                     0L, NULL, ID_RESOURCE, &hwndClient) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

VOID EnableMenuItem (HWND hwndMenu, SHORT idMenuItem, BOOL fEnable)
     {
     WinSendMsg (hwndMenu, MM_SETITEMATTR, 
                 MPFROM2SHORT (idMenuItem, TRUE),
                 MPFROM2SHORT (MIA_DISABLED, fEnable ? 0 : MIA_DISABLED)) ;
     }

VOID ErrorMsg (HWND hwnd, CHAR *szMessage)
     {
     WinMessageBox (HWND_DESKTOP, hwnd, szMessage, szClientClass, 0,
                    MB_OK | MB_ICONEXCLAMATION) ;
     }

VOID DrawCell (HPS hps, SHORT x, SHORT y, SHORT cxCell, SHORT cyCell,
               BYTE bCell)
     {
     RECTL rcl ;

     rcl.xLeft   = x * cxCell ;
     rcl.yBottom = y * cyCell ;
     rcl.xRight  = rcl.xLeft   + cxCell - 1 ;
     rcl.yTop    = rcl.yBottom + cyCell - 1 ;

     WinFillRect (hps, &rcl, bCell & 1 ? CLR_NEUTRAL : CLR_BACKGROUND) ;
     }

VOID DoGeneration (HPS hps, PBYTE pbGrid, SHORT xNumCells, SHORT yNumCells,
                   SHORT cxCell, SHORT cyCell)
     {
     SHORT x, y, sSum ;

     for (y = 0 ; y < yNumCells - 1 ; y++)
          for (x = 0 ; x < xNumCells ; x++)
               {
               if (x == 0 || x == xNumCells - 1 || y == 0)
                    *pbGrid |= *pbGrid << 4 ;
               else
                    {
                    sSum = (*(pbGrid             - 1) +    // Left
                            *(pbGrid - xNumCells - 1) +    // Lower Left
                            *(pbGrid - xNumCells    ) +    // Lower
                            *(pbGrid - xNumCells + 1))     // Lower Right
                                             >> 4 ;

                    sSum += *(pbGrid             + 1) +    // Right
                            *(pbGrid + xNumCells + 1) +    // Upper Right
                            *(pbGrid + xNumCells    ) +    // Upper
                            *(pbGrid + xNumCells - 1) ;    // Upper Left

                    sSum = (sSum | *pbGrid) & 0x0F ;

                    *pbGrid <<= 4 ;

                    if (sSum == 3)
                         *pbGrid |= 1 ;

                    if ((*pbGrid & 1) != *pbGrid >> 4)
                         DrawCell (hps, x, y, cxCell, cyCell, *pbGrid) ;
                    }
               pbGrid++ ;
               }
     }

VOID DisplayGenerationNum (HPS hps, SHORT xGen, SHORT yGen, LONG lGeneration)
     {
     static CHAR szBuffer [24] = "Generation " ;
     POINTL      ptl ;

     ptl.x = xGen ;
     ptl.y = yGen ;

     ltoa (lGeneration, szBuffer + 11, 10) ;

     GpiSavePS (hps) ;

     GpiSetBackMix (hps, BM_OVERPAINT) ;
     GpiCharStringAt (hps, &ptl, (LONG) strlen (szBuffer), szBuffer) ;

     GpiRestorePS (hps, -1L) ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOOL  fTimerGoing ;
     static HWND  hwndMenu ;
     static LONG  lGeneration ;
     static SEL   selGrid ;
     static SHORT cxChar, cyChar, cyDesc, cxClient, cyClient, xGenNum, yGenNum,
                  cxCell, cyCell, xNumCells, yNumCells, sCellScale = 1 ;
     FONTMETRICS  fm ;
     HPS          hps ;
     PBYTE        pbGrid ;
     POINTL       ptl ;
     SHORT        x, y ;

     switch (msg)
          {
          case WM_CREATE:
               hps = WinGetPS (hwnd) ;
               GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;
               cxChar = (SHORT) fm.lAveCharWidth ;
               cyChar = (SHORT) fm.lMaxBaselineExt ;
               cyDesc = (SHORT) fm.lMaxDescender ;
               WinReleasePS (hps) ;

               hwndMenu = WinWindowFromID (
                               WinQueryWindow (hwnd, QW_PARENT, FALSE),
                               FID_MENU) ;
               return 0 ;

          case WM_SIZE:
               if (selGrid)
                    {
                    DosFreeSeg (selGrid) ;
                    selGrid = 0 ;
                    }

               if (fTimerGoing)
                    {
                    WinStopTimer (hab, hwnd, ID_TIMER) ;
                    fTimerGoing = FALSE ;
                    }

               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;

               xGenNum = cxChar ;
               yGenNum = cyClient - cyChar + cyDesc ;

               cxCell = cxChar * 2 / sCellScale ;
               cyCell = cyChar / sCellScale ;

               xNumCells = cxClient / cxCell ;
               yNumCells = (cyClient - cyChar) / cyCell ;

               if (xNumCells <= 0 || yNumCells <= 0)
                    {
                    ErrorMsg (hwnd, "Not enough room for even one cell.") ;
                    }

               else if ((LONG) xNumCells * yNumCells > 65536L)
                    {
                    ErrorMsg (hwnd, "More than 64K cells not supported.") ;
                    }

               else if (DosAllocSeg (xNumCells * yNumCells, &selGrid, 0))
                    {
                    ErrorMsg (hwnd, "Not enough memory for this many cells.") ;
                    selGrid = 0 ;
                    }

               else
                    {
                    pbGrid = MAKEP (selGrid, 0) ;

                    for (y = 0 ; y < yNumCells ; y++)
                         for (x = 0 ; x < xNumCells ; x++)
                              *pbGrid++ = 0 ;
                    }

               EnableMenuItem (hwndMenu, IDM_SIZE,  TRUE) ;
               EnableMenuItem (hwndMenu, IDM_START, selGrid != 0) ;
               EnableMenuItem (hwndMenu, IDM_STOP,  FALSE) ;
               EnableMenuItem (hwndMenu, IDM_STEP,  selGrid != 0) ;
               EnableMenuItem (hwndMenu, IDM_CLEAR, selGrid != 0) ;

               lGeneration = 0 ;
               return 0 ;

          case WM_BUTTON1DOWN:
               x = MOUSEMSG(&msg)->x / cxCell ;
               y = MOUSEMSG(&msg)->y / cyCell ;

               if (selGrid && !fTimerGoing && x < xNumCells && y < yNumCells)
                    {
                    pbGrid = MAKEP (selGrid, 0) ;

                    hps = WinGetPS (hwnd) ;

                    DrawCell (hps, x, y, cxCell, cyCell,
                              *(pbGrid + y * xNumCells + x) ^= 1) ;

                    WinReleasePS (hps) ;
                    }
               else
                    WinAlarm (HWND_DESKTOP, WA_WARNING) ;
               break ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_LARGE:
                    case IDM_SMALL:
                    case IDM_TINY:
                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (sCellScale, TRUE),
                                     MPFROM2SHORT (MIA_CHECKED, 0)) ;

                         sCellScale = COMMANDMSG(&msg)->cmd ;

                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (sCellScale, TRUE),
                                     MPFROM2SHORT (MIA_CHECKED, MIA_CHECKED)) ;

                         WinSendMsg (hwnd, WM_SIZE, NULL,
                                     MPFROM2SHORT (cxClient, cyClient)) ;

                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;

                    case IDM_START:
                         if (!WinStartTimer (hab, hwnd, ID_TIMER, 1))
                              {
                              ErrorMsg (hwnd, "Too many clocks or timers.") ;
                              }
                         else
                              {
                              fTimerGoing = TRUE ;

                              EnableMenuItem (hwndMenu, IDM_SIZE,  FALSE) ;
                              EnableMenuItem (hwndMenu, IDM_START, FALSE) ;
                              EnableMenuItem (hwndMenu, IDM_STOP,  TRUE) ;
                              EnableMenuItem (hwndMenu, IDM_STEP,  FALSE) ;
                              EnableMenuItem (hwndMenu, IDM_CLEAR, FALSE) ;
                              }
                         return 0 ;

                    case IDM_STOP:
                         WinStopTimer (hab, hwnd, ID_TIMER) ;
                         fTimerGoing = FALSE ;

                         EnableMenuItem (hwndMenu, IDM_SIZE,  TRUE) ;
                         EnableMenuItem (hwndMenu, IDM_START, TRUE) ;
                         EnableMenuItem (hwndMenu, IDM_STOP,  FALSE) ;
                         EnableMenuItem (hwndMenu, IDM_STEP,  TRUE) ;
                         EnableMenuItem (hwndMenu, IDM_CLEAR, TRUE) ;
                         return 0 ;

                    case IDM_STEP:
                         WinSendMsg (hwnd, WM_TIMER, NULL, NULL) ;
                         return 0 ;

                    case IDM_CLEAR:
                         lGeneration = 0L ;

                         pbGrid = MAKEP (selGrid, 0) ;

                         for (y = 0 ; y < yNumCells ; y++)
                              for (x = 0 ; x < xNumCells ; x++)
                                   *pbGrid++ = 0 ;

                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;
                    }
               break ;

          case WM_TIMER:
               hps = WinGetPS (hwnd) ;

               DisplayGenerationNum (hps, xGenNum, yGenNum, ++lGeneration) ;

               pbGrid = MAKEP (selGrid, 0) ;

               DoGeneration (hps, pbGrid, xNumCells, yNumCells, cxCell, cyCell);

               WinReleasePS (hps) ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               if (selGrid)
                    {
                    for (x = 1 ; x <= xNumCells ; x++)
                         {
                         ptl.x = cxCell * x - 1 ;
                         ptl.y = 0 ;
                         GpiMove (hps, &ptl) ;

                         ptl.y = cyCell * yNumCells - 1 ;
                         GpiLine (hps, &ptl) ;
                         }

                    for (y = 1 ; y <= yNumCells ; y++)
                         {
                         ptl.x = 0 ;
                         ptl.y = cyCell * y - 1 ;
                         GpiMove (hps, &ptl) ;

                         ptl.x = cxCell * xNumCells - 1 ;
                         GpiLine (hps, &ptl) ;
                         }

                    pbGrid = MAKEP (selGrid, 0) ;

                    for (y = 0 ; y < yNumCells ; y++)
                         for (x = 0 ; x < xNumCells ; x++)
                              if (*pbGrid++)
                                   DrawCell (hps, x, y, cxCell, cyCell,
                                             *(pbGrid - 1)) ;

                    DisplayGenerationNum (hps, xGenNum, yGenNum, lGeneration) ;
                    }
               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               if (fTimerGoing)
                    WinStopTimer (hab, hwnd, ID_TIMER) ;

               if (selGrid)
                    DosFreeSeg (selGrid) ;

               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
