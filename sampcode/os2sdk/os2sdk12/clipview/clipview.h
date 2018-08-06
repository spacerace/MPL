/*
    CLIPVIEW header file
    Created by Microsoft Corporation, 1989
*/
#define ID_RESOURCE	1
/*
    Menu ids
*/
#define IDM_FILE	1
#define	IDM_RENDER	10
#define	IDM_SAVE	11
#define IDM_ABOUT	12
/*
    Dialog ids
*/
#define IDD_ABOUT	1
#define	IDD_RENDER	2
#define	IDL_RENDER	0x20
/*
    Supplementary string ids
*/
#define	IDS_ABOUTMENU	0x10
#define	IDS_ALREADY	0x11
#define	IDS_APPNAME	0x12
#define	IDS_CLIPCLASS	0x13
#define	IDS_INST	0x14
#define IDS_NODISPLAY	0x15
#define	IDS_NOTPRES	0x16
#define IDS_NOTSAVED	0x17
#define IDS_OPENFAIL	0x18
#define IDS_SAVETITLE	0x19
#define CF_EMPTY	0
#define	CF_UNKNOWN	0x21
#define	LINE		10		/* Amount of "LINE" scrolling	   */
#define	MAXLEN		30		/* Longest length of any string    */
#define	MAXTITLELEN	62		/* (MAXLEN * 2) + 2		   */
#define	MAXFORMATS	20		/* Maximum number of diff. formats */
/*
    Procedure declarations
*/
/* Wndprocs */
MRESULT EXPENTRY ClipWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY AboutDlgProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY RenderDlgProc(HWND, USHORT, MPARAM, MPARAM);

/* ClipView-specific routines */
VOID ChangeSystemMenu(HWND hwnd);
VOID ReadSelector(HWND hwndMLE, PSZ pszText);
VOID FixFrame(VOID);
VOID NeedScrollBars(BOOL fNeed);
BOOL RenderFormat(HWND hwnd, USHORT usFormat);
BOOL UpdateScreen(HWND hwnd, USHORT usFormat);
VOID GetAllFormats(VOID);
VOID GetFormatName(USHORT usFormat, UCHAR szFormat[]);
BOOL SendOwnerMsg(USHORT msg, MPARAM mp1, MPARAM mp2);
BOOL DoScrolling(HWND hwnd, BOOL fHorz, USHORT sbCmd);

/* ClipFile-specific routines */
BOOL SaveClipboard(HWND hwnd, USHORT usFormat);

int cdecl main(void);
