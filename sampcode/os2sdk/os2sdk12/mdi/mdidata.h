/* Global data extern declarations */
extern char szSplitbarClass[];
extern USHORT usDocNumber, cDocs;
extern USHORT cxVertSplitbar, cyHorzSplitbar;
extern SHORT cxNewDoc, cyNewDoc, xCascadeInc, yCascadeInc;
extern SHORT xCascadeSlot;
extern SHORT xNextNewDoc, yNextNewDoc;
extern SHORT cyTitlebar, cxMinmaxButton;

extern PFNWP pfnFrameWndProc;
extern PFNWP pfnMainFrameWndProc;

extern HDC hdcMem;
extern HPS hpsMem;
extern HPOINTER hptrHorzSplit, hptrVertSplit, hptrHVSplit, hptrArrow;
extern HBITMAP hbmChildSysMenu, hbmAabChildSysMenu;
extern MENUITEM miAabSysMenu;
extern HWND hwndFirstMenu;


/* Procedure declarations */
MRESULT EXPENTRY SplitbarWndProc(HWND, USHORT, MPARAM, MPARAM);
VOID CloseDocument(HWND);
BOOL ArrangeWindows(USHORT);
BOOL CreateSplitbarWindows(HWND, NPDOC);
VOID FindSwp(PSWP, USHORT, USHORT, PSWP FAR *);
VOID SetSwpPos(PSWP, HWND, HWND, SHORT, SHORT, SHORT, SHORT, USHORT);
VOID HideSwp(PSWP, HWND, USHORT *);
