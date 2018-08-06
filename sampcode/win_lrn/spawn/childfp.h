#define IDM_ABOUT         45
#define IDM_CHOOSERANDOM  46
#define IDM_HELP          47
#define IDM_CHOOSE        48
#define IDD_X             49
#define IDD_Y             50

long FAR PASCAL WndProc                (HWND, unsigned, WORD, LONG) ;
long FAR PASCAL ChildWndProc           (HWND, unsigned, WORD, LONG) ;
BOOL FAR PASCAL EnterPointDlgProc      (HWND, unsigned, WORD, LONG) ;
BOOL FAR PASCAL NewEnterPointDlgProc   (HWND, unsigned, WORD, LONG);
HWND            FindNextWindow         (HWND) ;

