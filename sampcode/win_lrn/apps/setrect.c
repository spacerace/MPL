/*
 *  SetRect
 *  setrect.c
 *
 *  This program demonstrates the use of the function SetRect.
 *  The SetRect functions creates a new RECT structure with the values
 *  given by the last four parameters.
 *
 */

#include "windows.h"

/*************************** GLOBAL VARIABLES *************************/
static  HWND    hWnd;

/************************** FORWARD REFERENCES ************************/
long FAR PASCAL WindowProc (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL SetRectTestPaint ();
RECT            NewRect ();

/******************** WINMAIN -- Main Windows Procedure ***************/
int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE          hInstance;
HANDLE          hPrevInstance;
LPSTR           lpszCmdLine;
int             cmdShow;
{
                MSG	msg;

        if (!hPrevInstance) {
                /* ensure that windows know where to find parts of this
                 * task on disk by Registering a window class.  Registering
                 * a class binds an executable name to an internal name,
                 * known to Windows. */
                WNDCLASS        rClass;

                rClass.lpszClassName    = (LPSTR)"setrect";
                rClass.hInstance        = hInstance;
                rClass.lpfnWndProc      = WindowProc;
                rClass.hCursor          = LoadCursor (NULL, IDC_ARROW);
                rClass.hIcon            = LoadIcon (hInstance, (LPSTR)"setrect");
                rClass.lpszMenuName     = (LPSTR) NULL;
		rClass.hbrBackground    = GetStockObject (WHITE_BRUSH);
                rClass.style            = CS_HREDRAW | CS_VREDRAW;
                rClass.cbClsExtra       = 0;
                rClass.cbWndExtra       = 0;

                RegisterClass ((LPWNDCLASS) &rClass);
                } /* end if this is the 1st task/instance of this program */

        hWnd = CreateWindow (   (LPSTR) "setrect", /* Window class name */
                                (LPSTR) "setrect", /* Window title */
                                WS_OVERLAPPEDWINDOW,
                                        /* stlye -- WIN 2.x or later */
                                CW_USEDEFAULT,  /* x -  WIN 2.x or later */
                                CW_USEDEFAULT,  /* y -  WIN 2.x or later */
                                CW_USEDEFAULT,  /* cx - WIN 2.x or later */
                                CW_USEDEFAULT,  /* cy - WIN 2.x or later */
                                (HWND)NULL,     /* No parent */
                                (HMENU)NULL,    /* Use the class menu */
                                (HANDLE)hInstance, /* .EXE file for Class */
                                (LPSTR)NULL     /* No Params */
                             );

	MessageBox (NULL,
		(LPSTR)"This is a demostration of the SetRect Function",
		(LPSTR)"SetRect",
		MB_OK);

        ShowWindow (hWnd, cmdShow);	/* Allocate room for the window	*/
        UpdateWindow (hWnd);		/* Paint the client area */

        while (GetMessage ((LPMSG)&msg, NULL, 0, 0)) {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
                } /* end while more messages */
        exit (msg.wParam);
} /* WINMAIN */
/***************************** WINDOWPROC *****************************/
/*      Every message dispatched to any window of type "setrect" will
 *      result in a call to this procedure.
 */
long FAR PASCAL WindowProc (hWnd, identifier, wParam, lParam)
HWND            hWnd;           /* Intended window */
unsigned        identifier;     /* Message Number */
WORD            wParam;         /* 16 bit param */
LONG            lParam;         /* 32 bit param */
{

        switch (identifier) {
		case WM_PAINT:
			SetRectTestPaint ();
			break;
                default:
                        return (DefWindowProc
                                (hWnd, identifier, wParam, lParam));
                        break;
                } /* end switch */
        return (0L);
} /* windowproc */
/************************** PAINTING PROCEDURE *****************************/
/* This function will paint the new rectangle in the client area with a
 * newly created brush. */
BOOL FAR PASCAL SetRectTestPaint ()
{
		PAINTSTRUCT     ps;
		RECT            rect;
		HDC             hDC;
		HBRUSH          hBrush;

	hDC = BeginPaint (hWnd, (LPPAINTSTRUCT) &ps);

	/* Create a new brush to paint the new rectangle with */
	hBrush = CreateSolidBrush (RGB (0, 0, 0)); /* BLACK = 0,0,0 */

	rect = NewRect ();			/* get the new rectangle */
	FillRect (hDC, (LPRECT)&rect, hBrush);	/* color rectangle	*/

	ValidateRect (hWnd, (LPRECT)NULL);	/* confirm paint	*/
	EndPaint (hWnd, (LPPAINTSTRUCT) &ps);
	return TRUE;
}
/********************** SET NEW RECTANGLE - SETRECT *********************/
/* Gets the current coordinates of the client area and returns a
 * smaller rectangle of that will be centered in the client area.
 */
RECT    NewRect ()
{
		RECT	rect;
		int     left;
		int     top;
		int     right;
		int     bottom;

	GetClientRect (hWnd, (LPRECT)&rect); /* Get Client Area Coordinates */
	
	left = rect.left + 15;		     /* values for smaller rect. */
	top = rect.top + 15;
	right = rect.right - 15;
	bottom = rect.bottom - 15;

	/* Create new rectangle with the above coordinates
	 * into the rect struct */
	SetRect ((LPRECT)&rect, left, top, right, bottom);

	return (rect);
}
