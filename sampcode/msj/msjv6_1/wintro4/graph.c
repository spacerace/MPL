#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "stock.h"

void FAR PASCAL HandleSelectionState(LPDRAWITEMSTRUCT lpdis, int inflate);
void FAR PASCAL HandleFocusState(LPDRAWITEMSTRUCT lpdis, int inflate);
void FAR PASCAL DrawEntireItem(LPDRAWITEMSTRUCT lpdis, int inflate);


BOOL FAR PASCAL
GraphOptionsDlgProc(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  int         iPen; 
  char        szBuf[80];
  static HANDLE      hStockInfo = NULL;
  static LPSTOCKINFO lpStockInfo = (LPSTOCKINFO) NULL;
  LPGRAPHINFO lpGraphInfo;
  HWND        hWnd;
  BOOL        bTranslated;


  switch (message)
  {
    case WM_INITDIALOG :
      if ((hStockInfo = (HANDLE) lParam) == NULL)
      {
        EndDialog(hDlg, IDCANCEL);
        return TRUE;
      }

      if ((lpStockInfo = (LPSTOCKINFO) GlobalLock(hStockInfo)) == NULL)
      {
        EndDialog(hDlg, IDCANCEL);
        return TRUE;
      }
  
      lpGraphInfo = &lpStockInfo->StockFile.graphinfo;

      /*
        If the ticks array has some memory allocated to it, it means
        that we are dealing with an existing stock. If not, then we
        are creating a new stock.
      */
      if (lpStockInfo->hTicks != NULL)
      {
        SetDlgItemText(hDlg, ID_SYMBOL, lpStockInfo->StockFile.szStock);
        SetDlgItemInt(hDlg, ID_MINPRICE, (int) lpGraphInfo->dwMinPrice, FALSE);
        SetDlgItemInt(hDlg, ID_MAXPRICE, (int) lpGraphInfo->dwMaxPrice, FALSE);
        SetDlgItemInt(hDlg, ID_FACTOR, (int) lpGraphInfo->dwScaleFactor, FALSE);
        SetDlgItemInt(hDlg, ID_TICKINT, (int) lpGraphInfo->dwTickInterval, FALSE);
        SetDlgItemInt(hDlg, ID_DENOMINATOR, (int) lpGraphInfo->iDenominator, FALSE);

        if (lpStockInfo->dwFlags & STATE_HAS_VGRID)
          CheckDlgButton(hDlg, ID_VERTGRID, TRUE);
        if (lpStockInfo->dwFlags & STATE_HAS_HGRID)
          CheckDlgButton(hDlg, ID_HORZGRID, TRUE);

        EnableWindow(GetDlgItem(hDlg, ID_SYMBOL), FALSE);
      }
      else
      {
        lpGraphInfo->iGridPen = 0;
      }


      /* 
        Fill the listbox with pen ids 
      */ 
      for (iPen = 0;  iPen < PS_NULL;  iPen++) 
        SendDlgItemMessage(hDlg, ID_GRIDSTYLE, CB_ADDSTRING, 0, (LONG) iPen); 
      SendDlgItemMessage(hDlg,ID_GRIDSTYLE,CB_SETCURSEL,lpGraphInfo->iGridPen,0L);
      return TRUE;


    case WM_MEASUREITEM : 
    { 
      LPMEASUREITEMSTRUCT lpMI = (LPMEASUREITEMSTRUCT) (LPSTR) lParam; 
      lpMI->itemWidth  = 42; 
      lpMI->itemHeight = 20; 
      break; 
    } 

    case WM_DRAWITEM : 
    { 
      LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) (LPSTR) lParam; 
 
      if (lpdis->itemID == -1)
      {
        HandleFocusState(lpdis, -5);
        break;
      }

      switch (lpdis->itemAction) 
      { 
        case ODA_DRAWENTIRE: 
          DrawEntireItem(lpdis, -4); 
          break; 
 
        case ODA_SELECT: 
          HandleSelectionState(lpdis, 0); 
          break; 
 
        case ODA_FOCUS: 
          HandleFocusState(lpdis, -2); 
          break; 
      } 
 
      break; 
    } 
 

    case WM_COMMAND :
      switch (wParam)
      {
        case IDOK :
          lpGraphInfo = &lpStockInfo->StockFile.graphinfo;

          if (lpStockInfo->hTicks == NULL)
          {
            GetDlgItemText(hDlg, ID_SYMBOL, szBuf, sizeof(szBuf));
            if (*szBuf == '\0')
            {
              MessageBox(hWndMain, "You must fill in the name of the stock.", "Error", MB_OK);
              return TRUE;
            }
            strcat(szBuf, ".STO");
            lstrcpy(lpStockInfo->szFileName, szBuf);

            lpStockInfo->StockFile.dwMagic = MAGIC_COOKIE;
            if ((lpStockInfo->hTicks = GlobalAlloc(GMEM_MOVEABLE, (DWORD) sizeof(TICK) * 64)) == NULL)
            {
              MessageBox(hWndMain, "Can't allocate the ticker array", "Error", MB_OK);
              goto bye;
            }
            lpStockInfo->nTicksAllocated = 64;
            lpStockInfo->StockFile.nTicks = 0;

            /*
              Create a window for this guy
            */
            hWnd = lpStockInfo->hWnd = GraphCreateWindow((LPSTR) szBuf);
            SetWindowWord(lpStockInfo->hWnd, 0, hStockInfo);
            lpStockInfo->dwFlags |= STATE_DIRTY;
          }

          GetDlgItemText(hDlg, ID_SYMBOL, lpStockInfo->StockFile.szStock, 
                                      sizeof(lpStockInfo->StockFile.szStock));
          lpGraphInfo->dwMinPrice = GetDlgItemLong(hDlg, ID_MINPRICE,
                                      (BOOL FAR *) &bTranslated, FALSE);
          lpGraphInfo->dwMaxPrice = GetDlgItemLong(hDlg, ID_MAXPRICE,
                                      (BOOL FAR *) &bTranslated, FALSE);
          lpGraphInfo->dwScaleFactor = GetDlgItemLong(hDlg, ID_FACTOR,
                                      (BOOL FAR *) &bTranslated, FALSE);
          lpGraphInfo->dwTickInterval = GetDlgItemLong(hDlg, ID_TICKINT,
                                      (BOOL FAR *) &bTranslated, FALSE);
          lpGraphInfo->iDenominator = GetDlgItemInt(hDlg, ID_DENOMINATOR,
                                      (BOOL FAR *) &bTranslated, FALSE);
          lpGraphInfo->iGridPen = (WORD)
                  SendDlgItemMessage(hDlg, ID_GRIDSTYLE, CB_GETCURSEL, 0, 0L);

          lpStockInfo->dwFlags &= ~(STATE_HAS_VGRID | STATE_HAS_HGRID);
          if (IsDlgButtonChecked(hDlg, ID_VERTGRID))
            lpStockInfo->dwFlags |= STATE_HAS_VGRID;
          if (IsDlgButtonChecked(hDlg, ID_HORZGRID))
            lpStockInfo->dwFlags |= STATE_HAS_HGRID;

bye:
          InvalidateRect(lpStockInfo->hWnd, (LPRECT) NULL, TRUE);
          GlobalUnlock(hStockInfo);
          EndDialog(hDlg, IDOK);
          break;

        case IDCANCEL:
          GlobalUnlock(hStockInfo);
          EndDialog(hDlg, IDCANCEL);
          break;
      }
      return TRUE;

  } /* end switch (message) */

  return FALSE;
}


/**************************************************************************** 
 *                                                                          * 
 *  FUNCTION   : HandleSelectionState(LPDRAWITEMSTRUCT, int)                * 
 *                                                                          * 
 *  PURPOSE    : Handles a change in an item selection state. If an item is * 
 *               selected, a black rectangular frame is drawn around that   * 
 *               item; if an item is de-selected, the frame is removed.     * 
 *                                                                          * 
 *  COMMENT    : The black selection frame is slightly larger than the gray * 
 *               focus frame so they won't paint over each other.           * 
 *                                                                          * 
 ****************************************************************************/ 
void FAR PASCAL HandleSelectionState(lpdis, inflate) 
  LPDRAWITEMSTRUCT  lpdis; 
  int               inflate; 
{ 
  HBRUSH hbr = (lpdis->itemState & ODS_SELECTED) ? GetStockObject(BLACK_BRUSH)
                                : CreateSolidBrush(GetSysColor(COLOR_WINDOW)); 
  FrameRect(lpdis->hDC, (LPRECT) &lpdis->rcItem, hbr); 
  DeleteObject (hbr); 
} 
 
 
/**************************************************************************** 
 *                                                                          * 
 *  FUNCTION   : HandleFocusState(LPDRAWITEMSTRUCT, int)                    * 
 *                                                                          * 
 *  PURPOSE    : Handle a change in item focus state. If an item gains the  * 
 *               input focus, a gray rectangular frame is drawn around that * 
 *               item; if an item loses the input focus, the gray frame is  * 
 *               removed.                                                   * 
 *                                                                          * 
 *  COMMENT    : The gray focus frame is slightly smaller than the black    * 
 *               selection frame so they won't paint over each other.       * 
 *                                                                          * 
 ****************************************************************************/ 
void FAR PASCAL HandleFocusState(lpdis, inflate) 
  LPDRAWITEMSTRUCT  lpdis; 
  int      inflate; 
{ 
  RECT  rc; 
  HBRUSH  hbr; 
 
  /* Resize rectangle to place focus frame between the selection 
   * frame and the item. 
   */ 
  CopyRect ((LPRECT)&rc, (LPRECT)&lpdis->rcItem); 
  InflateRect ((LPRECT)&rc, inflate, inflate); 
 
  if (lpdis->itemState & ODS_FOCUS) 
    /* gaining input focus -- paint a gray frame */ 
    hbr = GetStockObject(GRAY_BRUSH); 
  else 
    /* losing input focus -- remove (paint over) frame */ 
    hbr = CreateSolidBrush(GetSysColor(COLOR_WINDOW)); 

  FrameRect(lpdis->hDC, (LPRECT)&rc, hbr); 
  DeleteObject (hbr); 
} 
 
/**************************************************************************** 
 *                                                                          * 
 *  FUNCTION   : DrawEntireItem(LPDRAWITEMSTRUCT, int)                      * 
 *                                                                          * 
 *  PURPOSE    : Draws an item and frames it with a selection frame and/or  * 
 *               a focus frame when appropriate.                            * 
 *                                                                          * 
 ****************************************************************************/ 
void FAR PASCAL DrawEntireItem(lpdis, inflate) 
  LPDRAWITEMSTRUCT  lpdis; 
  int      inflate; 
{ 
  RECT   rc; 
  HANDLE hOldPen; 
  HPEN   hPen; 
 
  if (lpdis->itemData < 0)
    return;

  /* Resize rectangle to leave space for frames */ 
  CopyRect ((LPRECT)&rc, (LPRECT)&lpdis->rcItem); 
  InflateRect ((LPRECT)&rc, inflate, inflate); 
 
  if (lpdis->itemState & ODS_FOCUS) 
    hPen = CreatePen((int) lpdis->itemData, 1, RGB(0xFF, 0xFF, 0xFF)); 
  else
    hPen = CreatePen((int) lpdis->itemData, 1, RGB(0, 0, 0)); 
  hOldPen = SelectObject(lpdis->hDC, hPen); 

#if 0
{
  char szMsg[80];
  sprintf(szMsg,
  "rc:[%d %d %d %d]  itemID:%d  itemAction:%x  itemState:%x  itemData:%ld",
   rc.left, rc.top, rc.right, rc.bottom, lpdis->itemID, lpdis->itemAction,
   lpdis->itemState, lpdis->itemData);
  MessageBox(hWndMain, szMsg, "DrawEntireItem", MB_OK);
}
#endif

  MoveTo(lpdis->hDC, rc.left,  rc.top + (rc.bottom - rc.top) / 2); 
  LineTo(lpdis->hDC, rc.right, rc.top + (rc.bottom - rc.top) / 2); 

  SelectObject(lpdis->hDC, hOldPen); 
  DeleteObject(hPen); 
 
  /* Draw or erase appropriate frames */ 
  HandleSelectionState(lpdis, inflate + 4); 
  HandleFocusState(lpdis, inflate + 2); 
} 


int PASCAL GraphWndPaint(HWND hWnd, HDC hDC, LPSTOCKINFO lpStockInfo)
{
  RECT         r;
  int          i, y;
  int          xGraph, yGraph;
  LPTICK       lpTick;
  int          rangeHigh,
               rangeLow,
               xPixelsPerDate;
  int          yAxisStart, xAxisStart;
  HANDLE       hOldPen;
  HPEN         hPen;
  TEXTMETRIC   tm;
  char         szBuf[80];

  if (lpStockInfo->StockFile.nTicks == 0)
    return TRUE;

  /*
    Get the client dimensions of the current stock window and figure out the
    width and height of the graph.
  */
  GetClientRect(hWnd, (LPRECT) &r);
  yGraph = r.bottom - r.top;
  xGraph = r.right - r.left;

  /*
    Get the low and high range into local variables.
  */
  rangeHigh = (int) lpStockInfo->StockFile.graphinfo.dwMaxPrice;
  rangeLow  = (int) lpStockInfo->StockFile.graphinfo.dwMinPrice;

  /*
    Get the metrics of the current font.
  */
  GetTextMetrics(hDC, (LPTEXTMETRIC) &tm);

  if ((lpTick = (LPTICK) GlobalLock(lpStockInfo->hTicks)) == NULL)
    return FALSE;


  /*
    This code is for WINTRO 4
  */
  y = 0;
  for (i = 0;  i < lpStockInfo->StockFile.nTicks;  i++)
  {
    sprintf(szBuf, "%d) Price %ld", i+1, lpTick[i].price);
    TextOut(hDC, 0, y, szBuf, strlen(szBuf));
    y += tm.tmHeight;
  }
  goto bye;



  /*
    Set up the mapping mode. Y-axis points up and X-axis points right.
  */
  SetMapMode(hDC, MM_ANISOTROPIC);
  SetWindowOrg(hDC, 0, 0);
  SetViewportOrg(hDC, 0, yGraph);
  SetWindowExt(hDC,  xGraph, yGraph);
  SetViewportExt(hDC, xGraph, -yGraph);

  sprintf(szBuf, "%d", rangeHigh);
  yAxisStart = strlen(szBuf) * (tm.tmAveCharWidth + 1);
  xAxisStart = tm.tmHeight + (tm.tmHeight/2);
  xPixelsPerDate = xGraph / lpStockInfo->StockFile.nTicks;

  /*
    Draw the axises. First the Y-axis, then the X-axis.
  */
  MoveTo(hDC, yAxisStart, xAxisStart);
  LineTo(hDC, yAxisStart, yGraph);
  MoveTo(hDC, yAxisStart, xAxisStart);
  LineTo(hDC, xGraph,     xAxisStart);

  /*
    Draw the hash marks for the x-axis
  */
  for (i = 1;  i < lpStockInfo->StockFile.nTicks;  i++)
  {
    int x = i * xPixelsPerDate + yAxisStart;
    int iTextLen;
    MoveTo(hDC, x, xAxisStart-2);
    LineTo(hDC, x, xAxisStart+2);
    sprintf(szBuf, "%d", i+1);
    iTextLen = strlen(szBuf) * tm.tmAveCharWidth;
    /*
      Output the x-axis text centered over the hash mark
    */
    TextOut(hDC, x - (iTextLen >> 1), xAxisStart-2, szBuf, strlen(szBuf));
  }

  /*
    Draw the hash marks for the y-axis
  */
  for (i = rangeLow;  i < rangeHigh;  i++)
  {
    int y = (i - rangeLow) * yGraph / (rangeHigh - rangeLow) + xAxisStart;
    MoveTo(hDC, yAxisStart-4, y);
    LineTo(hDC, yAxisStart+4, y);
    sprintf(szBuf, "%d", i);
    TextOut(hDC, 0, y + (tm.tmHeight >> 1), szBuf, strlen(szBuf));
  }


  /*
    Figure out how many pixels wide each entry should be. This is equal
    to the width of the graph divided by the number of ticks to be plotted.
  */
  MoveTo(hDC, yAxisStart, xAxisStart + 
          ((int) lpTick[0].price - rangeLow) * yGraph / (rangeHigh - rangeLow));
  if (lpStockInfo->StockFile.nTicks == 1)
  {
    int x = i * xPixelsPerDate;
    int y = ((int)lpTick[i].price - rangeLow) * yGraph / (rangeHigh-rangeLow);
    Ellipse(hDC, x-2, y-2, x+2, y+2);
  }
  else
  {
    for (i = 1;  i < lpStockInfo->StockFile.nTicks;  i++)
    {
      int x = i * xPixelsPerDate;
      int y = ((int) lpTick[i].price - rangeLow) * yGraph / 
              (rangeHigh - rangeLow);
      LineTo(hDC, x + yAxisStart, y + xAxisStart);
    }
  }

  /*
    Now draw the horizontal and vertical grid lines.
  */
  hPen = CreatePen(lpStockInfo->StockFile.graphinfo.iGridPen, 1, RGB(0, 0, 0));
  hOldPen = SelectObject(hDC, hPen);

  if (lpStockInfo->dwFlags & STATE_HAS_VGRID)
  {
    for (i = 0;  i < lpStockInfo->StockFile.nTicks;  i++)
    {
      int x = (i+1) * xPixelsPerDate + yAxisStart;
      MoveTo(hDC, x, xAxisStart);
      LineTo(hDC, x, yGraph);
    }
  }

  if (lpStockInfo->dwFlags & STATE_HAS_HGRID)
  {
    for (i = rangeLow+1;  i < rangeHigh;  i++)
    {
      int y = (i - rangeLow) * yGraph / (rangeHigh - rangeLow);
      MoveTo(hDC, yAxisStart, y + xAxisStart);
      LineTo(hDC, xGraph, y + xAxisStart);
    }
  }

  SelectObject(hDC, hOldPen);
  DeleteObject(hPen);

bye:
  GlobalUnlock(lpStockInfo->hTicks);
  return TRUE;
}

