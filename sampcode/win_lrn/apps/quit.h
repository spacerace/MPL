#define ID_ABOUT    100
#define IDM_QUIT    100

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL QuitInit(HANDLE);
long FAR PASCAL QuitWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
