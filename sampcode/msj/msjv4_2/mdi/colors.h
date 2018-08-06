/*
 * COLORS.H - Include for COLORS program
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
 * Resource file constants
 *
 */

/* Strings */
#define  IDS_TITLE      	1
#define	IDS_MAINCLASS		2
#define	IDS_COLORCLASS		3

/* Debugging menu choice */
#define	IDM_DEBUG			0x100

/* File Menu Choices */
#define	IDM_NEW		 		0x101
#define	IDM_OPEN		 		0x102
#define	IDM_SAVE		 		0x103
#define	IDM_SAVEAS	 		0x104
#define	IDM_PRINT	 		0x105
#define	IDM_ABOUT	 		0x106
#define	IDM_EXIT		 		0x107

/* Color Menu Choices */
#define	IDM_0					0x108
#define	IDM_25				0x109
#define	IDM_50				0x10a
#define	IDM_75				0x10b
#define	IDM_100				0x10c

/* New dialog box */
#define	DLGNEW_RED			0x100
#define	DLGNEW_GREEN		0x101
#define	DLGNEW_BLUE			0x102

/**/

/*
 *
 * Window extra constants
 *
 */

#define	WE_COLOR				0
#define	WE_SHADE				2
#define	WE_EXTRA				4

#define	COLOR_RED			0
#define	COLOR_GREEN			1
#define	COLOR_BLUE			2

/**/

/*
 *
 * Function prototypes
 *
 */

int PASCAL						WinMain( HANDLE, HANDLE, LPSTR, int );
HWND								MainInit( HANDLE, HANDLE, int );
long FAR PASCAL				MainWndProc( HWND, unsigned, WORD, LONG );
BOOL								ColorInit( HANDLE );
HWND								ColorCreate( HWND, int );
long FAR PASCAL 				ColorWndProc( HWND, unsigned, WORD, LONG );
int FAR PASCAL					MainDlgNew( HWND, unsigned, WORD, LONG );
int FAR PASCAL					MainDlgAbout( HWND, unsigned, WORD, LONG );

