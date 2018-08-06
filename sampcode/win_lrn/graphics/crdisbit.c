/*
 *  CreateDiscardableBitmap
 */

#include "windows.h"

static HWND	hWnd;
static RECT	rect;

void    ExecuteTest ();

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE		hInstance;
HANDLE		hPrevInstance;
LPSTR		lpszCmdLine;
int	cmdShow;
{
  MSG	msg;

  if (!hPrevInstance)
  {
/* ensure that windows know where to find parts of this
		 * task on disk by Registering a window class.  Registering
                 * a class binds an executable name to an internal name,
		 * known to Windows. */
    WNDCLASS	rClass;

    rClass.lpszClassName         = (LPSTR)"CreateDiscardableBitmap";
    rClass.hInstance	 = hInstance;
    rClass.lpfnWndProc	 = DefWindowProc;
    rClass.hCursor               = LoadCursor (NULL, IDC_ARROW);
    rClass.hIcon                 = LoadIcon (hInstance, (LPSTR)"CreateDiscardableBitmap");
    rClass.lpszMenuName  = (LPSTR) NULL;
    rClass.hbrBackground         = GetStockObject (WHITE_BRUSH);
    rClass.style		 = CS_HREDRAW | CS_VREDRAW;
    rClass.cbClsExtra	 = 0;
    rClass.cbWndExtra	 = 0;

    RegisterClass ( (LPWNDCLASS) & rClass);
  } /* end if this is the 1st task/instance of this program */

  hWnd = CreateWindow ( (LPSTR) "CreateDiscardableBitmap", /* Window class name */
  (LPSTR) "CreateDiscardableBitmap", /* Window title */
  WS_OVERLAPPEDWINDOW,
    /* stlye -- WIN 2.x or later */
CW_USEDEFAULT,  /* x -  WIN 2.x or later */
CW_USEDEFAULT,  /* y -  WIN 2.x or later */
CW_USEDEFAULT,  /* cx - WIN 2.x or later */
CW_USEDEFAULT,  /* cy - WIN 2.x or later */
  (HWND)NULL,   /* No parent */
  (HMENU)NULL,  /* Use the class menu */
  (HANDLE)hInstance, /* .EXE file for Class */
  (LPSTR)NULL   /* No Params */
  );
  ShowWindow (hWnd, cmdShow);     /* Allocate room for window     */
  UpdateWindow (hWnd);            /* Paint the client area        */

  MessageBox (hWnd, (LPSTR) "Beginning Test", (LPSTR) " ", MB_OK);

/* The actual test of the CreateDiscardableBitmap function	*/
  ExecuteTest (hWnd);

  MessageBox (hWnd, (LPSTR) "Demonstration Finished", (LPSTR) " ", MB_OK);
  return 0L;
} /* WINMAIN */


/****************************************************************************/
/* This is the function to test CreateDiscardableBitmap Function.				 */

void ExecuteTest (hWnd)
HWND hWnd;
{
  HDC		hDC;
  HDC		hMemoryDC;				/* handle to In-memory display Context */
  HBITMAP 	hBitmap;					/* handle to Bitmap	*/
  HBITMAP 	hOldBitmap;
  BITMAP	bm;						/* copy of bitmap		*/
  short	xStart;
  short	yStart;
  POINT		pt;						/* structure of x-y coordinates */

/* prepare and create bitmap	*/
  hDC = GetDC (hWnd);
  hMemoryDC = CreateCompatibleDC (hDC);
  hBitmap = CreateDiscardableBitmap (hDC, 64, 32);
  hOldBitmap = SelectObject (hMemoryDC, hBitmap);

/* Color with Black	*/
  PatBlt (hMemoryDC, 0, 0, 64, 32, BLACKNESS);

  GetObject (hBitmap, sizeof (BITMAP), (LPSTR) & bm);
  pt.x = bm.bmWidth;
  pt.y = bm.bmHeight;
  DPtoLP (hDC, &pt, 1);          /* convert to logical units for GDI */

  xStart = yStart = 0;

/* Copy to the Display Context	*/
  BitBlt (hDC, xStart, yStart, pt.x, pt.y, hMemoryDC, 0, 0, SRCCOPY);

/* Discard objects and free up memory */
  DeleteDC (hMemoryDC);
  ReleaseDC (hWnd, hDC);
  return;
} /* END EXECUTETEST */


/* END DISCARD.C */
