/*
 *   IntersectRect
 *
 *   This example application demonstrates the use of the IntersectRect
 *   function. The IntersectRect function finds the largest area common
 *   to two RECT structures and puts the representation of the intersection
 *   in the RECT structure defined by the first parameter in the call to
 *   IntersectRect. IntersectRect is called twice in this application. The
 *   first time it is called, there is no area common to the two rectangles,
 *   the second time it is called, there is a common area.
 */

#include "windows.h"

long FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pHelloClass;

    pHelloClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pHelloClass->hIcon          = LoadIcon( hInstance,NULL);
    pHelloClass->lpszMenuName   = (LPSTR)NULL;
    pHelloClass->lpszClassName  = (LPSTR)"Sample Application";
    pHelloClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pHelloClass->hInstance      = hInstance;
    pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
    pHelloClass->lpfnWndProc    = HelloWndProc;

    if (!RegisterClass( (LPWNDCLASS)pHelloClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pHelloClass );
    return TRUE;        /* Initialization succeeded */
}

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;
    HMENU hMenu;

    HDC hDC;		   /** handle to display context	 **/
    RECT lpRect1;	   /** rectangle coordinates structure	 **/
    RECT lpRect2;	   /** rectangle coordinates structure	 **/
    RECT lpRect3;	   /** rectangle coordinates structure	 **/
    RECT destRect;	   /** target rectangle coord structure  **/

    HelloInit( hInstance );
    hWnd = CreateWindow((LPSTR)"Sample Application",
                        (LPSTR)"Sample Application",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			(HWND)NULL,	   /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

/** get handle to display context **/
   hDC = GetDC(hWnd);

/** fill lpRect1 with coordinate info for rectangle **/
   lpRect1.left = 10;
   lpRect1.top	= 10;
   lpRect1.right= 150;
   lpRect1.bottom=150;

/** fill lpRect2 with coordinate info for rectangle **/
   lpRect2.left = 160;
   lpRect2.top	= 10;
   lpRect2.right= 210;
   lpRect2.bottom=50;

/** fill lpRect3 with coordinate info for rectangle **/
   lpRect3.left = 30;
   lpRect3.top	= 30;
   lpRect3.right= 200;
   lpRect3.bottom=200;

   MessageBox(hWnd,(LPSTR)"three rectangles",
	      (LPSTR)"I am about to InvertRect...",MB_OK);

/** invert pixels in lpRect1 **/
   InvertRect (hDC,(LPRECT)&lpRect1);

/** invert pixels in lpRect2 **/
   InvertRect (hDC,(LPRECT)&lpRect2);

/** invert pixels in lpRect3 **/
   InvertRect (hDC,(LPRECT)&lpRect3);

   TextOut(hDC,60,0,(LPSTR)"RECT 1",(int)6);
   TextOut(hDC,160,0,(LPSTR)"RECT 2",(int)6);
   TextOut(hDC,90,210,(LPSTR)"RECT 3",(int)6);

   MessageBox(hWnd,(LPSTR)"the intersection of rects 1 and 2",
	      (LPSTR)"I am about to InvertRect...",MB_OK);

/** find intersection of lpRect1 and lpRect2 and place in destRect  **/
   IntersectRect((LPRECT)&destRect,(LPRECT)&lpRect1,(LPRECT)&lpRect2);

/** invert the intersection of lpRect1 and lpRect2 (should be empty) **/
   InvertRect (hDC,(LPRECT)&destRect);

   MessageBox(hWnd,(LPSTR)"the intersection of rects 1 and 3",
	      (LPSTR)"I am about to InvertRect...",MB_OK);

/** find intersection of lpRect1 and lpRect3 and place in destRect  **/
   IntersectRect((LPRECT)&destRect,(LPRECT)&lpRect1,(LPRECT)&lpRect3);

/** invert the intersection of lpRect1 and lpRect2		    **/
   InvertRect (hDC,(LPRECT)&destRect);

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}


/* Procedures which make up the window class. */
long FAR PASCAL HelloWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_SYSCOMMAND:
	return DefWindowProc( hWnd, message, wParam, lParam );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
