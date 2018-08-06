#define ID_ABOUT  100
#define IDM_YIELD 100

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL YieldInit(HANDLE);
long FAR PASCAL YieldWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
