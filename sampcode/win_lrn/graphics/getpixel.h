#define  IDM_RANDOM     101
#define  IDM_ENTER      102
#define  IDM_HELP       103
#define  IDD_X          104
#define  IDD_Y          105
#define  IDM_ABOUT      106


long FAR PASCAL WndProc                (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL EnterPointDlgProc      (HWND, unsigned, WORD, LONG) ;
BOOL FAR PASCAL NewEnterPointDlgProc   (HWND, unsigned, WORD, LONG);
HWND            FindNextWindow         (HWND) ;
