/*
**  demo.h     header file for demo of Virtual List Box
**
**  Author: Robert A. Wood
**          Executive Micro Systems
**          1716 Azurite Trail
**          Plano, TX 75075
**
**    Date: 03/19/90
*/

#define IDD_LISTBOX  100
#define IDD_TEXT   101
#define IDM_DIALOG 100

BOOL MainInit( HANDLE );
LONG FAR PASCAL MainWndProc( HWND, unsigned, WORD, LONG );
BOOL FAR PASCAL DlgProc( HWND, unsigned, WORD, LONG );
LONG FAR PASCAL VLBfile( WORD, unsigned, LONG FAR *, LPSTR );

/*** END OF DEMO.H *************************************************/

