/*
 *  SetCursorPos
 *  setcsrps.c
 *
 *  This program demonstrates the use of the function SetCursorPos.
 *  It moves the system cursor to the coordinates specified by the
 *  X and Y parameters.
 *
 */

#include "windows.h"

/*************************** GLOBAL VARIABLES *************************/
static	HWND	hWnd;
static	RECT	rect;
/******************** WINMAIN -- Main Windows Procedure ***************/
int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE		hInstance;
HANDLE		hPrevInstance;
LPSTR		lpszCmdLine;
int		cmdShow;
{
		MSG	msg;
		int	xCoord;
		int	yCoord;

	if (!hPrevInstance) {
		/* ensure that windows know where to find parts of this
		 * task on disk by Registering a window class.  Registering
		 * a class binds an executable name to an internal name,
		 * known to Windows. */
		WNDCLASS	rClass;

		rClass.lpszClassName	= (LPSTR)"SetCursorPos";
		rClass.hInstance	= hInstance;
		rClass.lpfnWndProc	= DefWindowProc;
		rClass.hCursor		= LoadCursor (NULL, IDC_ARROW);
		rClass.hIcon		= LoadIcon (hInstance, (LPSTR)"SetCursorPos");
		rClass.lpszMenuName	= (LPSTR) NULL;
		rClass.hbrBackground	= GetStockObject (WHITE_BRUSH);
		rClass.style		= CS_HREDRAW | CS_VREDRAW;
		rClass.cbClsExtra	= 0;
		rClass.cbWndExtra	= 0;

		RegisterClass ((LPWNDCLASS) &rClass);
		} /* end if this is the 1st task/instance of this program */

	hWnd = CreateWindow (	(LPSTR) "SetCursorPos", /* Window class name */
				(LPSTR) "SetCursorPos", /* Window title */
				WS_OVERLAPPEDWINDOW,
					/* stlye -- WIN 2.x or later */
				CW_USEDEFAULT,	/* x -  WIN 2.x or later */
				CW_USEDEFAULT,	/* y -  WIN 2.x or later */
				CW_USEDEFAULT,	/* cx - WIN 2.x or later */
				CW_USEDEFAULT,	/* cy - WIN 2.x or later */
				(HWND)NULL,	/* No parent */
				(HMENU)NULL,	/* Use the class menu */
				(HANDLE)hInstance, /* .EXE file for Class */
				(LPSTR)NULL	/* No Params */
			     );
	ShowWindow (hWnd, cmdShow);     /* Allocate room for window     */
	UpdateWindow (hWnd);            /* Paint the client area        */

	MessageBox (hWnd, (LPSTR) "Moving the Cursor to the Top-Left Corner", (LPSTR) " ",MB_OK);
	GetClientRect (hWnd, (LPRECT)&rect);
	xCoord = rect.left;
	yCoord = rect.top;

	/* SetCursorPos is used here to move the cursor to the top-left
	 * corner of the screen.	*/
	SetCursorPos (xCoord, yCoord);

	MessageBox (hWnd, (LPSTR) "Demonstration Finished", (LPSTR) " ",MB_OK);
	exit (0);
} /* WINMAIN */
