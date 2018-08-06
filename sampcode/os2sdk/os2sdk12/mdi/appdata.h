/* Global data extern declarations */
extern char szMDIClass[], szDocClass[];
extern USHORT cxBorder, cyBorder, cyHScroll, cxVScroll, cyVScrollArrow;
extern USHORT cxScreen, cyScreen, cyIcon, cxByteAlign, cyByteAlign;
extern USHORT cxSizeBorder, cySizeBorder;
extern ULONG clrNext;
extern LONG rglDevCaps[];

extern HAB hab;
extern HMQ hmqMDI;
extern HHEAP hHeap;
extern HWND hwndMDI, hwndMDIFrame, hwndActiveDoc;
extern HWND hwndSysMenu;
extern FONTMETRICS fmSystemFont;
extern NPDOC npdocFirst;


/* Procedure declarations */
MRESULT EXPENTRY MDIWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY AboutDlgProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY DocWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY DocFrameWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY MainFrameWndProc(HWND, USHORT, MPARAM, MPARAM);
