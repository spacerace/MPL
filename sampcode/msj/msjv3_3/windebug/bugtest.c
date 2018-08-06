/*
 * DEBUG TEST PROGRAM - SOURCE CODE (BUGTEST.C)
 *
 * LANGUAGE : Microsoft C 5.0
 * MODEL    : small
 * STATUS   : operational
 *
 * 12/11/87 1.00 - Kevin P. Welch - initial creation.
 *
 */

#include <windows.h>
#include "debug.h"

/* local definitions */
#define	BUG_CONTROL	201

/* function definitions */
LONG FAR PASCAL	TestWndFn( HWND, WORD, WORD, LONG );

/**/

/*
 * MAINLINE - BUG TEST PROGRAM
 *
 * This mainline initializes the test program and processes
 * and dispatches all messages relating to the debug test
 * window.
 *
 */
 
int PASCAL WinMain( hInstance, hPrevInstance, lpsCmd, wCmdShow )
   HANDLE	hInstance;
   HANDLE   	hPrevInstance;
   LPSTR    	lpsCmd;
   WORD      	wCmdShow;
{
	/* local variables */
   MSG		Msg;	/* current system message */
     
	/* initialization */
	if ( TestInit( hInstance, hPrevInstance, lpsCmd, wCmdShow ) ) {

		/* process system messages until finished */
		while ( GetMessage( (LPMSG)&Msg, NULL, 0, 0 ) ) {
			TranslateMessage( (LPMSG)&Msg );
			DispatchMessage( (LPMSG)&Msg );
		}

		/* terminate application */
		exit( Msg.wParam );
			
	} else
		exit( FALSE );

}

/**/

/*
 *	TestInit( hInstance, hPrevInstance, lpsCmd, wCmdShow ) : BOOL;
 *
 *		hInstance	current instance handle
 *		hPrevInstance	handle to previous instance
 *		lpsCmd		command line string
 *		wCmdShow	window display flag
 *
 * This utility function performs all the initialization required
 * for testing the debug utility.  Included in this program is
 * the registry and creation of the main window & the installation
 * of the debug utility code.
 *
 */
 
static BOOL TestInit( hInstance, hPrevInstance, lpsCmd, wCmdShow )
	HANDLE		hInstance;
	HANDLE		hPrevInstance;
	LPSTR		lpsCmd;
	WORD		wCmdShow;
{
	/* local variables */
	HWND		hWnd;	/* current window handle */
	BOOL		bResult;	/* result of initialization */
	WNDCLASS	WndClass;	/* window class */

	/* initialization */
	bResult = FALSE;
	
	/* register window class */
	if ( !hPrevInstance ) {
	
		/* define MAZE window class */
		memset( &WndClass, 0, sizeof(WNDCLASS) );
		WndClass.lpszClassName = (LPSTR)"TestWindow";
		WndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
		WndClass.lpszMenuName = (LPSTR)NULL;
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		WndClass.lpfnWndProc = TestWndFn;
		WndClass.hInstance = hInstance;
		WndClass.hIcon = LoadIcon( hInstance, "BugTestIcon" );
		WndClass.hbrBackground = (HBRUSH)(COLOR_MENU + 1);
	
		/* register maze window class */		
		if ( RegisterClass( (LPWNDCLASS)&WndClass ) ) {
	
			/* create window */
			hWnd = CreateWindow(
					"TestWindow",						/* class name */
					"Debug Test Window",				/* caption */
					WS_TILEDWINDOW,					/* style */
					CW_USEDEFAULT,						/* x position */
					CW_USEDEFAULT,						/* y position */
					CW_USEDEFAULT,						/* width */
					CW_USEDEFAULT,						/* height */
					(HWND)NULL,							/* parent window */
					(HMENU)NULL,						/* menu */
					hInstance,							/* application */
					(LPSTR)NULL							/* other data */
				);

			/* continue if successful */
			if ( hWnd ) {
				
				/* Here is where the debug utility is installed
				 * into the program.  A response message number
				 * is provided along with the maximum number of
				 * debug statements which will be maintained
				 * by the listbox.  The larger this number, the
				 * less global memory available for your 
				 * application.
				 */
#if DEBUG	
				DebugSetup( hWnd, BUG_CONTROL, 100 );
#endif

				/* make window visible */
				bResult = TRUE;
				ShowWindow( hWnd, wCmdShow );											

			}  
					
		} 
		
	}
	
	/* return result */
	return( bResult );
	
}

/**/

/*
 * TEST WINDOW MESSAGE PROCESSING PROCEDURE
 *
 * TestWndFn( hWnd, wMessage, wParam, lParam ) : LONG FAR PASCAL
 *
 *		hWnd		window handle
 *		wMessage	message number
 *		wParam		additional message information
 *		lParam		additional message information
 *
 * This window function processes all the messages related to
 * the debug test window.  Using the system menu the user can
 * display the debug control panel dialog box.
 *
 */
 
LONG FAR PASCAL TestWndFn( hWnd, wMessage, wParam, lParam )
	HWND		hWnd;
	WORD		wMessage;
	WORD		wParam;
	LONG		lParam;
{
	/* local variables */
	LONG		lResult;	/* result of message */

	/* initialization */
	lResult = FALSE;
	
#if DEBUG
	/* sample debugging output */
	switch( wMessage )
		{
	case WM_MOVE :
		Debug( 1, "WM_MOVE: [%u,%u]", HIWORD(lParam), 
			LOWORD(lParam) );
		break;
	case WM_SIZE :
		Debug( 1, "WM_SIZE: [%u,%u]", HIWORD(lParam), 
			LOWORD(lParam) );
		break;
	case WM_CHAR :
		Debug( 2, "WM_CHAR: [%c,%u]", wParam, wParam );
		break;
	case WM_ACTIVATE :
		Debug( 3, "WM_ACTIVATE: %s", 
			(wParam)?"activate":"inactivate" );
		break;
	case WM_ACTIVATEAPP :
		Debug( 3, "WM_ACTIVATEAPP: %s", 
			(wParam)?"activate":"inactivate" );
		break;
	case WM_PAINT :
		Debug( 4, "WM_PAINT:" );
		break;
	default : 
		break;
	}
#endif

	/* process each message */
	switch( wMessage )
		{
	case WM_SYSCOMMAND : /* system command */

		/* In here you need to handle the special case where the
		 * user asks for the debug control panel to be displayed.
		 * To do so you need to trap the control panel response
		 * message you provided when installing the debug
		 * utility.
		 */

		/* process sub-message */
		switch( wParam )
			{
#if DEBUG
		case BUG_CONTROL : /* debug control panel */
			DebugControl( hWnd );
			break;
#endif
		default :
			lResult = DefWindowProc( hWnd, wMessage, 
						wParam, lParam );
			break;
		}		
		
		break;
	case WM_DESTROY :	/* destroy window */
		PostQuitMessage( 0 );
		break;
	default : /* send to default */
		lResult = DefWindowProc( hWnd, wMessage, wParam, 
					lParam );
		break;
	}
	
	/* return normal result */
	return( lResult );

}
