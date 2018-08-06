/*
 *   ValidateRgn
 *   valrgn.c
 *
 *   This program demonstrates the use of the ValidateRgn function. The
 *   ValidateRgn function subtracts the given region from the update
 *   region, which marks that region for painting. ValidateRgn is called 
 *   once in this program to cancel the effects of the InvalidateRgn
 *   function.  In effect, I first set a dirty bit using the InvalidateRgn
 *   function.  This dirty bit tells Windows that the area containing the
 *   specified region identified by the hRegion parameter needs to be
 *   repainted.  I clear the dirty bit with the ValidateRgn function so
 *   that the area containing the region is not repainted.  InvalidateRgn
 *   is called a second time to demonstrate the effects of the first
 *   ValidateRgn function.
 *
 */

#include "windows.h"

long FAR PASCAL ValrgnWndProc(HWND, unsigned, WORD, LONG);

/***************************************************************************/

/* Procedure called when the application is loaded for the first time */
BOOL ValrgnInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pValrgnClass;

    pValrgnClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pValrgnClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pValrgnClass->hIcon          = LoadIcon( hInstance,NULL);
    pValrgnClass->lpszMenuName   = (LPSTR)NULL;
    pValrgnClass->lpszClassName  = (LPSTR)"ValidateRgn";
    pValrgnClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pValrgnClass->hInstance      = hInstance;
    pValrgnClass->style          = CS_HREDRAW | CS_VREDRAW;
    pValrgnClass->lpfnWndProc    = ValrgnWndProc;

    if (!RegisterClass( (LPWNDCLASS)pValrgnClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pValrgnClass );
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
    HRGN  hRegion;      /* handle to elliptical region */
    HDC hDC;            /* handle to display context */
    HBRUSH hMyBrush;	   /* handle to brush */
    RECT Ellipse,       /* structure to hold ellipse coordinates */
         ClearRect;     /* structure to hold client area coordinates */

    ValrgnInit( hInstance );
    hWnd = CreateWindow((LPSTR)"ValidateRgn",
                        (LPSTR)"ValidateRgn",
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
/* Begginning of the ValidateRgn section */

/* get handle to display context */
   hDC = GetDC(hWnd);

/* fill Ellipse with coordinate information for a small ellipse */
   Ellipse.left = 10;
   Ellipse.top  = 10;
   Ellipse.right= 50;
   Ellipse.bottom=50;

   GetClientRect(hWnd,(LPRECT)&ClearRect);
   InvertRect(hDC,(LPRECT)&ClearRect);
   hRegion = CreateEllipticRgnIndirect((LPRECT)&Ellipse);

   MessageBox(hWnd,(LPSTR)"a small ellipse, then ValidateRgn the same \
small ellipse. InvalidateRgn sets a dirty bit to redraw the ellipse \
region.  ValidateRgn clears the dirty bit so that a call to UpdateWindow \
will not draw the small ellipse.",
	          (LPSTR)"I am about to InvalidateRgn...",MB_OK);


/* invalidate the ellipse identified by hRegion (TRUE param. causes erase) */
   InvalidateRgn(hWnd, (HRGN)hRegion, (BOOL)TRUE);

/* Don't draw the ellipse (ie. remove it from the update region) */
   ValidateRgn(hWnd, (HRGN)hRegion);  

/* call UpdateWindow so that draw will take place immediately */
   UpdateWindow(hWnd);


   MessageBox(hWnd,(LPSTR)"Notice that the small ellipse was not drawn",
	          (LPSTR)"Done",MB_OK);

   MessageBox(hWnd,(LPSTR)"the small ellipse again, but this time I'm not \
going to cancel the effects of InvalidateRgn with a call to ValidateRgn.  \
This time my call to UpdateWindow will draw the ellipse.",
	          (LPSTR)"I am about to InvalidateRgn...",MB_OK);

   InvalidateRgn(hWnd, (HRGN)hRegion, (BOOL)TRUE);

   UpdateWindow(hWnd);

   ReleaseDC(hWnd, hDC);

/* End of the ValidateRgn section */
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
long FAR PASCAL ValrgnWndProc( hWnd, message, wParam, lParam )
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
