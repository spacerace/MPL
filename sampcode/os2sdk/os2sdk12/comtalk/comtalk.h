/*
    comtalk.h -- Contains resource identifiers
    Created by Microsoft Corporation, 1989
*/
#include "global.h"
/* Resource ID -- Main Program */
#define		ID_RESOURCE		1

/* Dialog boxes */
#define		IDD_ABOUT		2	/* About... */
#define		IDD_SET			3	/* Settings... */
#define		IDD_MAINHELPBOX		4	/* F1 = Help, Main */
#define		IDD_SETHELPBOX		5	/* F1 = Help, Settings... */

/* Main menus */
#define 	IDM_ABOUT		10	/* on the System menu */

#define 	IDM_SESSIONMENU 	2	/* Session Menu */
#define		IDM_SETTINGS		0x200	/* 	Settings... */
#define		IDM_CONNECT		0x201	/*	Open Port   */
#define		IDM_CLOSE		0x202	/*	Close Port  */

#define 	IDM_COMMANDMENU 	3	/* Command menu */
#define 	IDM_PAGING		0x300	/*	Page Mode   */
#define		IDM_BREAK		0x301	/*	Send Break  */
#define		IDM_UP			0x302	/*	Page Up     */
#define 	IDM_PAGE		0x303	/*	Page Down   */

#define		IDM_ERRORS		4	/* Errors! hotspot */

#define		IDM_HELP		5	/* F1 = Help */

/* Settings... subitems */
#define		IDD_NOP			0	/* Cheater parity settings */
#define		IDD_ODDP		1	/* Just perfect for 1/42H  */
#define		IDD_EVENP		2
#define		IDD_MARKP		3
#define		IDD_SPACEP		4

#define		IDD_FIVE		5	/* Cheater databits settings */
#define		IDD_SIX			6
#define		IDD_SEVEN		7
#define		IDD_EIGHT		8

#define		IDD_ONESTOP		20	/* Stop bit settings */
#define		IDD_ONEFIVE		21	/* Hardcoded into program! */
#define		IDD_TWOSTOP		22

#define		IDD_PORT		30	/* Port */
#define		IDD_BAUD		31	/* Baud rate */
#define		IDD_WRAP		32	/* Word Wrap */
#define		IDD_HW			33	/* Hardware handshake */
#define		IDD_SW			34	/* Software handshake */

#define		IDD_SAVE		40	/* Big SAVE button */
#define		IDD_SETHELP		41	/* F1 = Help button in dialog */

/* Global Routines */
MRESULT EXPENTRY ClientWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY AboutDlgProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY SetDlgProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY NewFrameWndProc(HWND, USHORT, MPARAM, MPARAM);
