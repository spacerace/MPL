#define IDM_FROMMSGFILTER WM_USER + 101

extern HANDLE LIBINST;

void FAR PASCAL   InitHook (HANDLE);
BOOL FAR PASCAL   KillHook ();
DWORD FAR PASCAL  CallMsgMessageFilter (int, WORD, LONG);

