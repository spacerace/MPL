#define ID_ABOUT 100
#define ID_DRAW   104

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL MapDlgRectInit(HANDLE);
long FAR PASCAL MapDlgRectWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
