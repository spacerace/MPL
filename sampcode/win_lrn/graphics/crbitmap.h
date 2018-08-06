#define ID_ABOUT 100

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
long FAR PASCAL CreateBitMapWndProc(HWND, unsigned, WORD, LONG);
BOOL CreateBitMapInit(HANDLE);
