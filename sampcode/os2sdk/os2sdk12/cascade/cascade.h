#define ID_RESOURCE 1

#define IDM_FILE	   1
#define IDM_NEW 	   2
#define IDM_OPEN	   3
#define IDM_SAVE	   4
#define IDM_SAVEAS	   5
#define IDM_EXIT	   6
#define IDM_ABOUT	   7
#define IDM_EDIT	   8
#define IDM_DELETE	   9
#define IDM_CUT 	   10
#define IDM_COPY	   11
#define IDM_PASTE	   12

#define IDM_FONTS	20
#define IDM_SCREEN	IDM_FONTS+1
#define IDM_PRINTER	IDM_FONTS+300
#define IDM_FIRSTFONT	IDM_FONTS
#define IDM_LASTFONT	IDM_FONTS+600

MRESULT EXPENTRY WndProc( HWND, USHORT, MPARAM, MPARAM );
int cdecl main(void);
