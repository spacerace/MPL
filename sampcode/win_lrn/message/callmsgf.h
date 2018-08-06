#define IDM_ABOUT         45
#define IDM_HELP          47
#define IDM_GETHDC        48
#define IDM_CALLMSGFILTER 49
#define IDM_FROMMSGFILTER WM_USER + 101
#define IDM_FILTERMSG     WM_USER + 102


long FAR PASCAL  WndProc                (HWND, unsigned, WORD, LONG) ;
void FAR PASCAL  InitHook               (HANDLE);
BOOL FAR PASCAL  KillHook               ();

