#define ID_ABOUT     100
#define ID_ADD       102
#define ID_CANCEL    104
#define ID_EDIT      108
#define ID_LISTBOX   107
#define ID_OK        105
#define IDM_DIALOG   200

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL SendDlgInit(HANDLE);
long FAR PASCAL SendDlgWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL LineList(HWND, unsigned, WORD, LONG);
