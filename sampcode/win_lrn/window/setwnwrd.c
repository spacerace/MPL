/*
 *   SetWindowWord
 *   setwnwrd.c
 *
 *   This program demonstrates the use of the function SetWindowWord.
 *   SetWindowWord changes an attribute of a window.  The attribute
 *   can be one of the following options: the instance handle of the
 *   module that owns the window, the window handle of the parent window
 *   if one exists, the control ID of the child window.
 *
 */

#include "windows.h"

/* Forward references */

BOOL FAR PASCAL SampleInit( HANDLE );

long FAR PASCAL SampleWndProc(HWND, unsigned, WORD, LONG);

/* ---------------------------------------------------------------------- */

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, CmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int CmdShow;
{
    MSG   msg;
    HWND  hParent1, hParent2, hPopup;
    HMENU hMenu;

    SampleInit( hInstance );

/* Create the first parent window */

    hParent1 = CreateWindow((LPSTR)"SetWindowWord",
			(LPSTR)"SetWindowWord - Parent1",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			400,
		   20,
         (HWND)NULL,        /* no parent */
         (HMENU)NULL,       /* use class menu */
         (HANDLE)hInstance, /* handle to window instance */
         (LPSTR)NULL        /* no params to pass on */
         );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hParent1, CmdShow );
    UpdateWindow( hParent1 );

    MessageBox (hParent1, (LPSTR)"create a popup window for parent1",
               (LPSTR)"I'm about to ...", MB_OK);

/* Create the popup window */

    hPopup = CreateWindow((LPSTR)"SetWindowWord",
                  		 (LPSTR)"Popup",
	                      WS_POPUP|WS_CAPTION|WS_VISIBLE,
			                10,
			                50,
			                200,
			                100,
                         (HWND)hParent1,     /* parent */
                         (HMENU)NULL,       /* use class menu */
                         (HANDLE)hInstance, /* handle to window instance */
                         (LPSTR)NULL        /* no params to pass on */
                        );


    MessageBox (hParent1, (LPSTR)"create a second window called parent2",
               (LPSTR)"I'm about to ...", MB_OK);

/* Create the second parent window */

    hParent2 = CreateWindow((LPSTR)"SetWindowWord",
			(LPSTR)"SetWindowWord - Parent2",
			WS_OVERLAPPEDWINDOW,
			100,
			100,
			400,
			20,
         (HWND)NULL,        /* no parent */
         (HMENU)NULL,       /* use class menu */
         (HANDLE)hInstance, /* handle to window instance */
         (LPSTR)NULL        /* no params to pass on */
         );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hParent2, CmdShow );
    UpdateWindow( hParent2 );

/****************************************************************************/
/* Begin SetWindowWord */
/* I have set up this function to change the parent of the popup window */

    MessageBox (hParent1, (LPSTR)"make the Parent2 the parent of Popup, and \
iconize Parent1.  Notice that Parent2 is currently in front of Popup .",
               (LPSTR)"I'm about to ...", MB_OK);

    SetWindowWord(hPopup, GWW_HWNDPARENT, hParent2);
    ShowWindow( hParent1, SW_SHOWMINIMIZED );

    MessageBox (hParent1, (LPSTR)"move Parent2 in front of the Popup.  \
You can't do it because Parent2 is now the parent of Popup!",
               (LPSTR)"Now try to ...", MB_OK);

/* End of SetWindowWord section */
/****************************************************************************/

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}

/* ---------------------------------------------------------------------- */
/* Procedure called when the application is loaded for the first time */

BOOL FAR PASCAL SampleInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pSampleClass;

    pSampleClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pSampleClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pSampleClass->hIcon		      = LoadIcon( hInstance,NULL);
    pSampleClass->lpszMenuName   = (LPSTR)NULL;
    pSampleClass->lpszClassName  = (LPSTR)"SetWindowWord";
    pSampleClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pSampleClass->hInstance      = hInstance;
    pSampleClass->style          = CS_HREDRAW | CS_VREDRAW;
    pSampleClass->lpfnWndProc    = SampleWndProc;

    if (!RegisterClass( (LPWNDCLASS)pSampleClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pSampleClass );
    return TRUE;        /* Initialization succeeded */
}


/* ---------------------------------------------------------------------- */
/* Every message for this window will be delevered right here.         */

long FAR PASCAL SampleWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

/*    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break; */

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
