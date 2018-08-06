/*===========================================================================*/
/*                                                                           */
/* File    : DB.C                                                            */
/*                                                                           */
/* Purpose : Simple database routines for the stock charting application.    */
/*                                                                           */
/* History :                                                                 */
/*                                                                           */
/* (C) Copyright 1989,1990 Marc Adler/Magma Systems     All Rights Reserved  */
/*===========================================================================*/

#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>

#include "windows.h"
#include "stock.h"


/****************************************************************************/
/*                                                                          */
/* Function : StockFIleRead()                                               */
/*                                                                          */
/* Purpose  : Reads a stock file in from the disk and allocates a stock     */
/*            information structure for it.                                 */
/*                                                                          */
/* Returns  : TRUE if the stock file was read, FALSE if not.                */
/*                                                                          */
/****************************************************************************/
int PASCAL StockFileRead(LPSTR szFileName)
{
  char         sz[128];
  int          fd;
  int          rc = FALSE;
  HANDLE       hStockInfo;
  LPSTOCKINFO  lpStockInfo;
  LPTICK       lpTick;
  HWND         hWnd;

  /*
    Make sure it has the .STO extension if the user didn't specify one
  */
  lstrcpy((LPSTR) sz, szFileName);
  if (!strchr(sz, '.'))
    strcat(sz, ".sto");

  /*
    Open the file for reading
  */
  if ((fd = _lopen(sz, O_RDONLY | O_BINARY)) < 0)
  {
    MessageBox(hWndMain, "Can't open the stock file for reading", "Error", MB_OK);
    return FALSE;
  }

  /*
    Allocate space for the stock-file header
  */
  if ((hStockInfo = GlobalAlloc(GMEM_MOVEABLE, (DWORD) sizeof(STOCKINFO))) == NULL)
    goto bye;
  if ((lpStockInfo = (LPSTOCKINFO) GlobalLock(hStockInfo)) == NULL)
  {
    MessageBox(hWndMain, "Can't allocate the stock info structure", "Error", MB_OK);
    GlobalFree(hStockInfo);
    goto bye;
  }

  /*
    Save the absolute path name of the file
  */
  lstrcpy(lpStockInfo->szFileName, (LPSTR) sz);

  /*
    Read the header
  */
  if (_lread(fd, (LPSTR) &lpStockInfo->StockFile, sizeof(STOCKFILE)) != sizeof(STOCKFILE))
  {
    MessageBox(hWndMain, "Bad stock header format", "Error", MB_OK);
    GlobalUnlock(hStockInfo);
    GlobalFree(hStockInfo);
    goto bye;
  }

  /*
    Check the magic number to see if it's really a stock db file
  */
  if (lpStockInfo->StockFile.dwMagic != MAGIC_COOKIE)
  {
    MessageBox(hWndMain, "The file does not have the right signature", "Error", MB_OK);
    GlobalUnlock(hStockInfo);
    GlobalFree(hStockInfo);
    goto bye;
  }

  /*
    Allocate global memory to hold the ticks
  */
  if ((lpStockInfo->hTicks = GlobalAlloc(GMEM_MOVEABLE,
              (DWORD) sizeof(TICK) * lpStockInfo->StockFile.nTicks)) == NULL)
  {
    MessageBox(hWndMain, "Can't allocate the ticker array", "Error", MB_OK);
    goto bye;
  }
  lpStockInfo->nTicksAllocated = lpStockInfo->StockFile.nTicks;
  if ((lpTick = (LPTICK) GlobalLock(lpStockInfo->hTicks)) == NULL)
  {
    GlobalFree(lpStockInfo->hTicks);
    goto bye;
  }

  /*
    Read the ticks into the global memory buffer
  */
  if (_lread(fd, (LPSTR)lpTick, sizeof(TICK)*lpStockInfo->StockFile.nTicks) != 
      sizeof(TICK) * lpStockInfo->StockFile.nTicks)
  {
    MessageBox(hWndMain, "Error reading the ticker array", "Error", MB_OK);
    GlobalUnlock(lpStockInfo->hTicks);
    GlobalFree(lpStockInfo->hTicks);
    goto bye;
  }

  /*
    Create a window for this guy
  */
  hWnd = lpStockInfo->hWnd = GraphCreateWindow((LPSTR) sz);
  SetWindowWord(hWnd, 0, hStockInfo);
  lpStockInfo->dwFlags |= (STATE_HAS_VGRID | STATE_HAS_HGRID);

  /*
    Link this entry onto the stock info list
  */
  hCurrStockInfo = hStockInfo;

  GlobalUnlock(lpStockInfo->hTicks);
  GlobalUnlock(hStockInfo);
  rc = TRUE;

  /*
    Done reading .... close the file
  */
bye:
  close(fd);

  InvalidateRect(hWnd, (LPRECT) NULL, TRUE);
  UpdateWindow(hWnd);
  return rc;
}


int PASCAL StockFileSave(HANDLE hStockInfo)
{
  int    fd;
  char   sz[80];
  LPTICK lpTicks;
  LPSTOCKINFO lpStockInfo;

  lpStockInfo = (LPSTOCKINFO) GlobalLock(hStockInfo);
  lpTicks = (LPTICK) GlobalLock(lpStockInfo->hTicks);

  lstrcpy((LPSTR) sz, lpStockInfo->szFileName);
  strlwr(sz);
  if ((fd = open(sz, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IREAD|S_IWRITE)) < 0)
  {
    MessageBox(hWndMain, "Cannot open file", sz, MB_OK);
    return FALSE;
  }
  _lwrite(fd, (LPSTR) &lpStockInfo->StockFile, sizeof(STOCKFILE));
  _lwrite(fd, (LPSTR) lpTicks, sizeof(TICK) * lpStockInfo->StockFile.nTicks);
  close(fd);

  lpStockInfo->dwFlags &= ~STATE_DIRTY;

  MessageBox(hWndMain, "The file has been saved", sz, MB_OK);
  return TRUE;
}


/****************************************************************************/
/*                                                                          */
/* Function : MakeDummyStockFile()                                          */
/*                                                                          */
/* Purpose  : Creates a dummy stock file for testing purposes.              */
/*                                                                          */
/* Returns  :                                                               */
/*                                                                          */
/****************************************************************************/
#define TEST

#ifdef TEST

STOCKFILE DummyStockInfo =
{
  MAGIC_COOKIE,
  "IBM",
  "Intl Business Machines",

  {
   795, /* dwMinPrice     */
   810, /* dwMaxPrice     */
    1,  /* dwScaleFactor  */
    1,  /* dwTickInterval */
    1,  /* iDenominator   */
    2   /* iGridPen       */
  },

  10   /* nTicks */
};

TICK DummyTicks[10] =
{
  { 800, 20000L, { 1, 1, 90 } },
  { 802, 20500L, { 1, 2, 90 } },
  { 799, 30000L, { 1, 3, 90 } },
  { 805, 32000L, { 1, 4, 90 } },
  { 802, 28900L, { 1, 5, 90 } },
  { 804, 23500L, { 1, 8, 90 } },
  { 800, 31000L, { 1, 9, 90 } },
  { 801, 23000L, { 1,10, 90 } },
  { 798, 29500L, { 1,11, 90 } },
  { 800, 30000L, { 1,12, 90 } },
};

int MakeDummyStockFile(LPSTR szFileName)
{
  int fd;
  int i;
  char sz[80];

  lstrcpy((LPSTR) sz, szFileName);

  if ((fd = open(sz, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IREAD|S_IWRITE)) < 0)
    return FALSE;

  write(fd, (char *) &DummyStockInfo, sizeof(DummyStockInfo));
  for (i = 0;  i < 10;  i++)
    write(fd, (char *) &DummyTicks[i], sizeof(DummyTicks[0]));

  close(fd);
}

#endif /* TEST */
