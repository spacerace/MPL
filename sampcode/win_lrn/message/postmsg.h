#define ID_ABOUT    100
#define IDM_ICONIZE 100

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL PostMsgInit(HANDLE);
long FAR PASCAL PostMsgWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
