#define IDM_ABOUT         45
#define IDM_CHOOSERANDOM  46
#define IDM_HELP          47
#define IDM_CHOOSE        48
#define IDM_CLOCK         49
#define IDM_DOSDIRSORT    50
#define IDM_SPAWNIT       51
#define IDM_NOTEPAD       52
#define IDM_DOSTYPE       53
#define IDM_SPAWNALL      54
#define IDD_X             55
#define IDD_Y             56
#define IDM_REPLY         57
#define IDM_REPLYREGULAR  58
#define IDM_SEND          WM_USER + 101
#define IDM_REGULAR       WM_USER + 102


long FAR PASCAL WndProc                (HWND, unsigned, WORD, LONG) ;
BOOL FAR PASCAL EnterPointDlgProc      (HWND, unsigned, WORD, LONG) ;
BOOL FAR PASCAL NewEnterPointDlgProc   (HWND, unsigned, WORD, LONG);
HWND            FindNextWindow         (HWND) ;
BOOL            FindPeriod             (char *);
extern int far pascal Int21Function4B  (BYTE, LPSTR, LPSTR);

