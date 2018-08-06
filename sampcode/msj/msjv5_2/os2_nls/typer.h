//  Resource ids
//
#define	IDD_ABOUT	1	// About... dialog id
#define IDM_FILE	1	// File menu id
#define	IDM_ABOUT	0x10	// About... menu item id
#define IDR_TYPER	1	// Main resource ID
#define IDS_NAME	1	// String ID for window class name
#define IDS_TITLE	2	// String ID for title bar text

// Useful constants
//
#define	MAX_LINES	35	// Must be larger than window height in chars.
#define	MAX_LINE_LENGTH	255	// Should be larger than window width in chars.
#define	MAX_STRING	21	// Maximum length of strings to be loaded
#define	TABLE_SIZE	12	// Current maximum is 5 lead byte pairs

// "Exported" procedure declarations
//
MRESULT EXPENTRY AboutDlgProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY TyperWndProc(HWND, USHORT, MPARAM, MPARAM);
