/*
 *   ValidateRect
 *   valrect.c
 *
 *   This program demonstrates the use of the ValidateRect function. The
 *   ValidateRect function subtracts the given rectangle from the update
 *   region, which marks that rectangle for painting. ValidateRect is called 
 *   once in this program to cancel the effects of the InvalidateRect
 *   function.  In effect I first set a dirty bit using the InvalidateRect
 *   function.  This dirty bit tells windows that the area containing the
 *   specified rectangle identified by the Rect parameter needs to be
 *   repainted.  I clear the dirty bit with the ValidateRect function so
 *   that the area containing the rectangle is not repainted.  InvalidateRect
 *   is called a second time to demonstrate the effects of the first
 *   ValidateRect function.
 *
 */

#include "windows.h"

long FAR PASCAL ValrectWndProc(HWND, unsigned, WORD, LONG);

/***************************************************************************/

/* Procedure called when the application is loaded for the first time */
BOOL ValrectInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pValrectClass;

    pValrectClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pValrectClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pValrectClass->hIcon          = LoadIcon( hInstance,NULL);
    pValrectClass->lpszMenuName   = (LPSTR)NULL;
    pValrectClass->lpszClassName  = (LPSTR)"ValidateRect";
    pValrectClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pValrectClass->hInstance      = hInstance;
    pValrectClass->style          = CS_HREDRAW | CS_VREDRAW;
    pValrectClass->lpfnWndProc    = ValrectWndProc;

    if (!RegisterClass( (LPWNDCLASS)pValrectClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pValrectClass );
    return TRUE;        /* Initialization succeeded */
}
/***************************************************************************/

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;
    WORD  rectcolor = RGB(0x022,0x022,0x022);
    HDC hDC;		     /* handle to display context */
    HBRUSH hMyBrush;	  /* handle to brush */
    RECT Rect;         /* structure to hold rectangle coordinates */


         ValrectInit( hInstance );
    hWnd = CreateWindow((LPSTR)"ValidateRect",
                        (LPSTR)"ValidateRect",
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

/* ----------------------------------------------------------------------- */
/* Begginning of the ValidateRect section */

/* create brush to fill rectangle with */
   hMyBrush = CreateSolidBrush(rectcolor);

/* get handle to display context */
   hDC = GetDC(hWnd);

/* select brush into display context */
   SelectObject(hDC,hMyBrush);

/* draw a big rectangle so we can see InvalidateRect's effects */
   Rectangle(hDC,5,5,400,200);

/* fill Rect with coordinate information for a small rectangle */
   Rect.left = 10;
   Rect.top  = 10;
   Rect.right= 50;
   Rect.bottom=50;

   MessageBox(hWnd,(LPSTR)"a small rectangle then ValidateRect the same \
small rectangle.  InvalidateRect sets a dirty bit to redraw the rectangle \
region.  ValidateRect clears the dirty bit so that a call to UpdateWindow \
will not draw the small rectangle.",
	          (LPSTR)"I am about to InvalidateRect...",MB_OK);


/* invalidate the rect.identified by Rect (TRUE param. causes erase) */
   InvalidateRect(hWnd, (LPRECT)&Rect, (BOOL)TRUE);

/* Don't draw the rectangle (ie. remove it from the update region) */
   ValidateRect(hWnd, (LPRECT)&Rect);  

/* call UpdateWindow so that draw will take place immediately */
   UpdateWindow(hWnd);


   MessageBox(hWnd,(LPSTR)"Notice that the small rectangle was not drawn",
	          (LPSTR)"Done",MB_OK);

   MessageBox(hWnd,(LPSTR)"the small rectangle again, but this time I'm not \
going to cancel the effects of InvalidateRect with a call to ValidateRect.  \
This time my call to UpdateWindow will draw the rectangle.",
	          (LPSTR)"I am about to InvalidateRect...",MB_OK);

   InvalidateRect(hWnd, (LPRECT)&Rect, (BOOL)TRUE);

   UpdateWindow(hWnd);

   ReleaseDC(hWnd, hDC);

/* End of the ValidateRect section */
/* ----------------------------------------------------------------------- */

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}
/***************************************************************************/

/* Procedures which make up the window class. */
long FAR PASCAL ValrectWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_SYSCOMMAND:
        switch (wParam)
        {
        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
        }
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
