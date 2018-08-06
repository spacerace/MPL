#define ID_ABOUT    100
#define IDM_HIGH    100
#define IDM_LOW     110

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL SetPriInit(HANDLE);
long FAR PASCAL SetPriWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
