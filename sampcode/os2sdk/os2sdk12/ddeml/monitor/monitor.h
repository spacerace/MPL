
/*** monitor.c function declarations */

MRESULT EXPENTRY MonWndProc(HWND, USHORT, MPARAM, MPARAM);
VOID MonitorPaint(HWND hwnd, HPS hps, RECTL* prcl);

/*** testsubs.c function declarations */

BOOL InitTestSubs(VOID);
VOID DrawString(HWND hwnd, char *sz);
MRESULT FAR PASCAL StrWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
BOOL StrWndCreate(HWND hwnd, int cchLine, int cLine);
VOID PaintStrWnd(HWND hwnd, HPS hps, PRECTL prcl);
VOID StartTime(VOID);
VOID StopTime(HWND hwndStrWnd, char* szFmt);

