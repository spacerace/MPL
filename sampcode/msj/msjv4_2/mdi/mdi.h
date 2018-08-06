/*
 * MDI.H - Include for MDI applications
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 * Developed by:
 *   Geoffrey Nicholls
 *   Kevin P. Welch
 *
 * (C) Copyright 1988
 * Eikon Systems, Inc.
 * 989 E. Hillsdale Blvd, Suite 260
 * Foster City  CA  94404
 *
 */

/**/

/*
 *
 * Public section of file
 *
 */

/* Window class for child windows */
#define	WS_MDICHILD			WS_CAPTION	| WS_CHILD | WS_CLIPCHILDREN \
													| WS_VISIBLE | WS_THICKFRAME \
													| WS_CLIPSIBLINGS

/* How do we get our menu? */
#define	MdiGetMenu(w)		GetProp(w,PROP_CHILDMENU)
#define	MdiSetAccel(w,a)	SetProp(w,PROP_ACCEL,a)

/* Programmatic interface */
HWND	MdiMainCreateWindow(	LPSTR, LPSTR, DWORD, int, int, int, int,
									HWND, HMENU, HANDLE, LPSTR );
long	MdiMainDefWindowProc( HWND, unsigned, WORD, LONG );
HWND	MdiChildCreateWindow(	LPSTR, LPSTR, DWORD, int, int, int, int,
										HWND, HMENU, HANDLE, LPSTR );
long	MdiChildDefWindowProc( HWND, unsigned, WORD, LONG );
BOOL	MdiGetMessage( HWND, LPMSG, HWND, WORD, WORD );
int	MdiTranslateAccelerators( HWND, LPMSG );

/* Child System Menu Commands */
/* Do not use these menu choices yourself */
#define	IDM_CLOSE			0x3f00
#define	IDM_RESTORE			0x3f01
#define	IDM_NEXTWINDOW		0x3f02
#define	IDM_PREVWINDOW		0x3f03
#define	IDM_MOVE				0x3f04
#define	IDM_SIZE				0x3f05
#define	IDM_MAXIMIZE		0x3f06

/* Window Menu Choices */
#define	IDM_NEWWINDOW		0x3f07
#define	IDM_ARRANGE			0x3f08
#define	IDM_ARRANGEALL		0x3f09
#define	IDM_HIDE				0x3f0a
#define	IDM_UNHIDE			0x3f0b
#define	IDM_FIRSTWIN		0x3f0c
#define	WINDOW_POS			8

/**/

/*
 *
 * Private section of file
 *
 */

/* Window data for the MDI parent */
#define	PROP_ACTIVE			"MdiParentActiveChild"
#define	PROP_COUNT			"MdiParentChildCount"
#define	PROP_HIDDEN			"MdiParentHiddenCount"
#define	PROP_ZOOM			"MdiParentChildZoomed"
#define	PROP_MAINMENU		"MdiParentMenu"
#define	PROP_WINDOWMENU	"MdiParentWindowMenu"
#define	PROP_CTRLACCEL		"MdiParentChildSysAccel"
#define	PROP_SYSMENU		"MdiParentChildSysMenu"
#define	PROP_TITLE			"MdiParentTitle"

/* Window data for MDI children */
#define	PROP_LEFT			"MdiChildRestoreLeft"
#define	PROP_TOP				"MdiChildRestoreTop"
#define	PROP_WIDTH			"MdiChildRestoreWidth"
#define	PROP_HEIGHT			"MdiChildRestoreHeight"
#define	PROP_CHILDMENU		"MdiChildMenu"
#define	PROP_ACCEL			"MdiChildAccel"
#define	PROP_MENUID			"MdiChildId"
#define	PROP_ISMDI			"MdiChildWeAreOne"

/* Unhide dialog box */
#define	DLGUNHIDE_LB		0x100

/* Child system menu bitmap */
#define	OBM_CLOSE	32767

/* Return values for WM_MENUCHAR (missing from windows.h) */
#define	MC_ABORT				1
#define	MC_SELECT			2

/* States for the keyboard interface for the menus */
#define	POP_NONE				-3
#define	POP_MAINSYS			-2
#define	POP_CHILDSYS		-1
#define	POP_MAIN1ST			0

/**/

/*
 *
 * Procedure prototypes
 *
 */

/* mdi2.c */
HWND						MdiCreateChildWindow( int );
void						MdiDestroyChildWindow( HWND );
void						MdiActivateChild( HWND, BOOL );
void						MdiActivateNextChild( HWND );
void						MdiActivatePrevChild( HWND );
void						MdiDeactivateChild( HWND );
void						MdiZoomChild( HWND );
void						MdiRestoreChild( HWND, BOOL );
void						MdiSwitchZoom( HWND, HWND );
HWND						MdiChooseNewActiveChild( HWND );
void						MdiHideChild( HWND );
void						MdiUnhideChild( HWND );
int FAR PASCAL			MdiDlgUnhide( HWND, unsigned, WORD, LONG );

/* mdi3.c */
void						MdiZoomMenu( HWND );
void						MdiRestoreMenu( HWND );
void						MdiAppendWindowToMenu( HWND );
void						MdiReinsertWindowInMenu( HWND );
void						MdiRemoveWindowFromMenu( HWND, BOOL );
void						MdiWindowMenu( HWND, HMENU, BOOL );
void						MdiInitSystemMenu( HWND );
HMENU						MdiGetChildSysMenu( void );
HBITMAP			  		MdiCreateChildSysBitmap( HWND );
void						MdiSetMenuKeyHook( HANDLE );
void						MdiMenuMessageLoopUpdate( HWND );
LONG FAR PASCAL		MdiMsgHook( int, WORD, LONG );
void						MdiFreeMenuKeyHook( void );

/* External stuff (in windows) */
LPSTR FAR PASCAL		lstrcpy( LPSTR, LPSTR );
