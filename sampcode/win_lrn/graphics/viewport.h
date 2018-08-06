#define IDD_RADIUS         52
#define IDM_CHANGE         53
#define IDM_HELP           54
#define IDM_ABOUT          55
#define IDM_OFFVIEWPORT    56
#define IDM_OFFWINDOW      57
#define IDM_SETVIEWPORT    58
#define IDM_SETEXTENT      59
#define IDM_SCALEVIEW      60
#define IDM_SCALEWINDOW    61
#define IDM_GETVIEWPORTEXT 62
#define IDM_GETWINDOWEXT   63
#define IDM_SETVIEWPORTORG 64
#define IDM_SETWINDOWORG   65
#define IDD_X              104
#define IDD_Y              105
#define IDD_TEXT1          106
#define IDD_TEXT2          107


BOOL  bOffsetViewportOrg = FALSE;
BOOL  bOffsetWindowOrg = FALSE;
BOOL  bSetViewportExt = FALSE;
BOOL  bSetWindowExt = FALSE;
BOOL  bScaleViewportExt = FALSE;
BOOL  bScaleWindowExt = FALSE;
BOOL  bSetViewportOrg = FALSE;
BOOL  bSetWindowOrg = FALSE;



long FAR PASCAL WndProc                (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL GetRadiusDlgProc       (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL EnterPointDlgProc      (HWND, unsigned, WORD, LONG) ;
BOOL FAR PASCAL NewEnterPointDlgProc   (HWND, unsigned, WORD, LONG);
HWND            FindNextWindow         (HWND) ;

