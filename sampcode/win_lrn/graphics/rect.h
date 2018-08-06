#define IDD_RADIUS         52
#define IDM_CHANGE         53
#define IDM_HELP           54
#define IDM_ABOUT          55
#define IDM_CHANGERECT1    56
#define IDM_CHANGERECT2    57
#define IDM_OFFSETRECT1    58
#define IDM_OFFSETRECT2    59
#define IDD_X              104
#define IDD_Y              105
#define IDD_TEXT1          106
#define IDD_TEXT2          107


long FAR PASCAL WndProc                (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL GetRadiusDlgProc       (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL EnterPointDlgProc      (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL NewEnterPointDlgProc   (HWND, unsigned, WORD, LONG);
HWND            FindNextWindow         (HWND) ;
void            DrawSquare             (HDC, RECT);

