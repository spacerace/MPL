/*===========================================================================*/
/*                                                                           */
/* File    : STOCK.H                                                         */
/*                                                                           */
/* Purpose :                                                                 */
/*                                                                           */
/* History :                                                                 */
/*                                                                           */
/* (C) Copyright 1990 Marc Adler/Magma Systems     All Rights Reserved       */
/*===========================================================================*/


extern HANDLE hThisInstance;  /* Program instance handle */
extern HWND hWndMain;         /* Handle to main window   */
extern HWND hwndMDIClient;    /* Handle to MDI client    */
extern HWND hWndActive;
extern HWND hWndStatus;
  #define Y_STATUSWINDOW   20   /* The height of the status window */
extern HANDLE hCurrStockInfo;

#define CBWNDEXTRA          sizeof(HANDLE)


/*
  Stock information
*/

#define MAXSTOCKNAME     4
#define MAXDESCRIPTION   32

typedef struct tagDate
{
  BYTE   chMonth;
  BYTE   chDay;
  BYTE   chYear;
} DATE;

typedef struct tagPrice
{
  DWORD  dwTotal;      /* dollar amt * iDenominator + fractional amount */
} PRICE;

typedef struct tagTick
{
  PRICE  price;
  DWORD  dwVolume;
  DATE   date;
} TICK, FAR *LPTICK;

/*
  Data structure describing how we draw the graph
*/
typedef struct tagGraphInfo
{
  DWORD dwMinPrice;
  DWORD dwMaxPrice;
  DWORD dwScaleFactor;
  DWORD dwTickInterval;
  WORD  iDenominator;    /* the fractional amount used for this stock */
  WORD  iGridPen;
} GRAPHINFO, FAR *LPGRAPHINFO;


typedef struct tagStockFile
{
  DWORD  dwMagic;
#define MAGIC_COOKIE             66666666L
  char   szStock[MAXSTOCKNAME];
  char   szDescription[MAXDESCRIPTION];
  GRAPHINFO graphinfo;
  WORD   nTicks;
/*
  TICK   aTicks[1];
*/
} STOCKFILE;

typedef struct tagInCoreStockInfo
{
  char      szFileName[14];  /* file name where the stock data is kept  */
  STOCKFILE StockFile;       /* a copy of the stock file header         */
  HANDLE    hTicks;
  HWND      hWnd;            /* window which the stock is shown in      */
  DWORD     dwFlags;         /* any kind of status bits we need to keep */
#define STATE_HAS_VGRID  1L
#define STATE_HAS_HGRID  2L
  HANDLE    hNextStockInfo;  /* link to next stock info structure       */
} STOCKINFO, FAR *LPSTOCKINFO;


#define ID_ABOUT            99
#define ID_STOCK            100
#define ID_GRAPH            101

#define ID_NEW              101
#define ID_OPEN             102
#define ID_SAVE             103
#define ID_CLOSE            104
#define ID_PRINT            105
#define ID_EXIT             106
#define ID_MAP              107

#define ID_TICK_ADD         200
#define ID_TICK_CHANGE      201
#define ID_GRAPH_PRICE      202
#define ID_GRAPH_VOLUME     203
#define ID_GRAPH_ZOOM       204
#define ID_GRAPH_COLORS     205
#define ID_GRAPH_OPTIONS    199
#define ID_GRAPH_GRID_HORZ  206
#define ID_GRAPH_GRID_VERT  207
#define ID_WINDOW_TILE      208
#define ID_WINDOW_CASCADE   209
#define ID_WINDOW_ICONS     210
#define ID_WINDOW_CLOSEALL  211
#define ID_WINDOW_CHILDREN  220

#define ID_TICK_DATE        100
#define ID_TICK_PRICE       101
#define ID_TICK_VOLUME      102


#define ID_RECTTEXT   120
#define ID_VXEXT      109
#define ID_VXORG      107
#define ID_VYEXT      111
#define ID_VYORG      113
#define ID_WXEXT      103
#define ID_WXORG      101
#define ID_WYEXT      105
#define ID_WYORG      115

#define IDC_FILENAME  400
#define IDC_EDIT      401
#define IDC_FILES     402
#define IDC_PATH      403
#define IDC_LISTBOX   404


/*
  Stuff for graph dialog box
*/
#define ID_SYMBOL     300
#define ID_MINPRICE   301
#define ID_MAXPRICE   302
#define ID_FACTOR     303
#define ID_TICKINT    304
#define ID_HORZGRID   305
#define ID_VERTGRID   306
#define ID_GRIDSTYLE  307
#define ID_HELP       308
#define ID_DENOMINATOR 309



int NEAR PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL FAR PASCAL InitializeApplication(VOID);
BOOL FAR PASCAL InitializeInstance(LPSTR lpCmdLine, WORD nCmdShow);
LONG FAR PASCAL MainWndProc(HWND, WORD, WORD, LONG);
LONG FAR PASCAL StatusWndProc(HWND, WORD, WORD, LONG);
LONG FAR PASCAL GraphWndProc(HWND, WORD, WORD, LONG);
BOOL FAR PASCAL AboutDlgProc(HWND, WORD, WORD, LONG);
BOOL FAR PASCAL MapDlgProc(HWND, WORD, WORD, LONG);
BOOL FAR PASCAL AddTickDlgProc(HWND, WORD, WORD, LONG);
BOOL FAR PASCAL GraphInfoDlgProc(HWND, WORD, WORD, LONG);
VOID MainPaint(HWND, HDC);
int PASCAL StockFileRead(LPSTR szFileName);
HWND PASCAL GraphCreateWindow(LPSTR lpName);
int PASCAL GraphWndPaint(HWND hWnd, HDC hDC, LPSTOCKINFO lpStockInfo);

/* FILEOPEN.C */
HANDLE FAR PASCAL OpenDlg(HWND, unsigned, WORD, LONG);
void SeparateFile(HWND, LPSTR, LPSTR, LPSTR);
void UpdateListBox(HWND);
void AddExt(PSTR, PSTR);
void ChangeDefExt(PSTR, PSTR);


