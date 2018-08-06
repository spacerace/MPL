/*===========================================================================*/
/*                                                                           */
/* File    : TICK.C                                                          */
/*                                                                           */
/* Purpose : Routines pertaining to the adding and modification of tickers.  */
/*                                                                           */
/* History :                                                                 */
/*                                                                           */
/* Written by Marc Adler/Magma Systems for Microsoft Systems Journal         */
/*===========================================================================*/

#include <windows.h>
#include "stock.h"


/****************************************************************************/
/*                                                                          */
/* Function : AddTickDlgProc()                                              */
/*                                                                          */
/* Purpose  : Dialog box proc for adding tickers.                           */
/*                                                                          */
/* Returns  :                                                               */
/*                                                                          */
/****************************************************************************/
BOOL FAR PASCAL AddTickDlgProc(hDlg, msg, wParam, lParam)
  HWND hDlg;
  WORD msg;
  WORD wParam;
  LONG lParam;
{
  RECT        r;
  char        buf[80];
  LPSTOCKINFO lpStockInfo;
  LPTICK      lpTick;
  TICK        tick;
  HWND        hWnd;

  static HANDLE hStockInfo;


  switch (msg)
  {
    case WM_INITDIALOG:
      /*
         Make sure that there is a current stock information record which
         we can append the ticker onto.
      */
      if (!hCurrStockInfo)
        EndDialog(hDlg, FALSE);
      return TRUE;


    case WM_COMMAND:
      switch (wParam)
      {
        /*
          The user chose the OK button...
        */
        case IDOK:
          /*
            Get a pointer to the stock info record
          */
          lpStockInfo = (LPSTOCKINFO) GlobalLock(hCurrStockInfo);
          if (lpStockInfo == NULL)
          {
            MessageBox(hDlg, "GlobalLock returned NULL", "Error", MB_OK);
            goto byebye;
          }

          /*
            Are we entering the first ticker? If so, then allocate a
            ticker array for the stock. We allocate 64 tickers initially.
          */
          if (lpStockInfo->hTicks == NULL)
          {
            lpStockInfo->StockFile.nTicks = 0;
            lpStockInfo->nTicksAllocated  = 64;
            lpStockInfo->hTicks = GlobalAlloc(GMEM_MOVEABLE,
                                              (DWORD) sizeof(TICK) * 64);
            if (lpStockInfo->hTicks == NULL)
            {
              MessageBox(hDlg, "Can't allocate initial ticks", "Error", MB_OK);
              goto byebye;
            }
          }

          /*
            Make sure that we do not overflow the ticker array. If there
            is a chance of this, then reallocate.
          */
          if (lpStockInfo->StockFile.nTicks + 1 >= lpStockInfo->nTicksAllocated)
          {
            HANDLE h;
            lpStockInfo->nTicksAllocated *= 2;
            h = GlobalReAlloc(lpStockInfo->hTicks,
                              (DWORD) sizeof(TICK)*lpStockInfo->nTicksAllocated,
                              GMEM_MOVEABLE);
            if (h == NULL)
            {
              MessageBox(hDlg,"Could not allocate enough memory for the tickers",
                              "Error", MB_OK);
              goto byebye;
            }
            else
              lpStockInfo->hTicks = h;
          }

          /*
            Get a pointer to the ticker array
          */
          if ((lpTick = (LPTICK) GlobalLock(lpStockInfo->hTicks)) == NULL)
          {
            MessageBox(hDlg, "GlobalLock returned NULL", "Error", MB_OK);
            GlobalUnlock(hCurrStockInfo);
            goto byebye;
          }

          /*
            Get the price and volume and out it in the last element of the
            ticker array.
            KLUDGE NOTES :
              a) We really should sort the records by date...
              b) We ignore the date here.
          */
          tick.price = GetDlgItemLong(hDlg,ID_TICK_PRICE,NULL,FALSE);
          tick.dwVolume = GetDlgItemLong(hDlg,ID_TICK_VOLUME,NULL,FALSE);

          /*
            Copy the ticker structure
          */
          lpTick[lpStockInfo->StockFile.nTicks++] = tick;
          lpStockInfo->dwFlags |= STATE_DIRTY;

          /*
            Unlock the memory and get outta here...
          */
          GlobalUnlock(lpStockInfo->hTicks);
          GlobalUnlock(hCurrStockInfo);
          EndDialog(hDlg, TRUE);
          break;


        /*
          The user chose the CANCEL button....
        */
        case IDCANCEL :
byebye:
          EndDialog(hDlg, FALSE);
          break;
      }
      return TRUE;

    default:
      return FALSE;
  }
}


LONG GetDlgItemLong(HWND hDlg, WORD id, BOOL FAR *lpTranslated, BOOL bSigned)
{
  extern long atol();
  char szBuf[64];

  GetDlgItemText(hDlg, id, (LPSTR) szBuf, sizeof(szBuf));
  return atol(szBuf);
}

