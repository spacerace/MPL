/*
 *  MoveTo
 *
 *  This program will demonstrate the use of the MoveTo function.  It will
 *  draw a triangle using MoveTo and LineTo.
 */

#include <windows.h>

BOOL FAR PASCAL InitMoveTo (HANDLE, HANDLE, int);
long    FAR PASCAL MoveToWindowProc (HANDLE, unsigned, WORD, LONG);

int     PASCAL WinMain  (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE   hInstance, hPrevInstance;
LPSTR    lpszCmdLine;
int      cmdShow;
  {
  MSG  msg;

  InitMoveTo (hInstance, hPrevInstance, cmdShow);  /*  Init Routine  */
  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  exit (msg.wParam);
  }

BOOL FAR PASCAL InitMoveTo (hInstance, hPrevInstance, cmdShow)
HANDLE   hInstance;
HANDLE   hPrevInstance;
int      cmdShow;
  {
  WNDCLASS  wcMoveToClass;
  HWND      hWnd;

  wcMoveToClass.lpszClassName = (LPSTR) "MoveTo";
  wcMoveToClass.hInstance     = hInstance;
  wcMoveToClass.lpfnWndProc   = MoveToWindowProc;
  wcMoveToClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
  wcMoveToClass.hIcon	      = NULL;
  wcMoveToClass.lpszMenuName  = (LPSTR) NULL;
  wcMoveToClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcMoveToClass.style	      = CS_HREDRAW | CS_VREDRAW;
  wcMoveToClass.cbClsExtra    = 0;
  wcMoveToClass.cbWndExtra    = 0;

  RegisterClass ( (LPWNDCLASS) & wcMoveToClass);

  hWnd = CreateWindow ( (LPSTR) "MoveTo", (LPSTR) "MoveTo",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,    0,
                      CW_USEDEFAULT,    0,
                      NULL,     NULL,   hInstance, NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  return TRUE;
  }

long    FAR PASCAL MoveToWindowProc (hWnd, message, wParam, lParam)
HWND	    hWnd;
unsigned    message;
WORD	    wParam;
LONG	    lParam;
  {
  switch (message)
    {
    case WM_PAINT:
      PaintMoveToWindow (hWnd);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return (DefWindowProc (hWnd, message, wParam, lParam));
      break;
    }
  return (0L);
  }

PaintMoveToWindow (hWnd)
HWND	hWnd;
  {
  int   cX1, cY1, cX2, cY2;
  int   xinc, yinc;

  PAINTSTRUCT	ps;
  HDC		hDC;
  RECT		rRect;

  BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);  /* Prepare the client area */
  hDC = ps.hdc; 		       /*  Get the Display Context  */

  GetClientRect (hWnd, (LPRECT) & rRect);
/*	Get the size of the client area  */

  cX1 = rRect.left;	     /*  Get the left side of the client area  */
  cY1 = rRect.top;	     /*  Get the top of the client area  */
  cX2 = rRect.right;	     /*  Get the right side of the client area	*/
  cY2 = rRect.bottom;	     /*  Get the bottom of the client area  */

  xinc = (cX2 - cX1) / 25;   /*  Figure out how much to increment  */
  yinc = (cY2 - cY1) / 25;  /*  x and y, evenly  */

  while (cX1 < rRect.right)
    {
    MoveTo (hDC, cX1, rRect.top);
/*  Move the pen to the desired location  */
    LineTo (hDC, rRect.right, cY1);
/*  Draw the line to the other side of the client area  */
    MoveTo (hDC, cX2, rRect.bottom);
/*  Move the pen to the desired location  */
    LineTo (hDC, rRect.left, cY2);
/*  Draw the line to the other side of the client area  */

    MoveTo (hDC, rRect.right, cY1);
/*  Move the pen to the desired location  */
    LineTo (hDC, cX2, rRect.bottom);
/*  Draw the line to the other side of the client area  */
    MoveTo (hDC, rRect.left, cY2);
/*  Move the pen to the desired location  */
    LineTo (hDC, cX1, rRect.top);
/*  Draw the line to the other side of the client area  */

    cX1 += xinc;
    cY1 += yinc;
    cX2 -= xinc;
    cY2 -= yinc;
    }
  ValidateRect (hWnd, (LPRECT) NULL);   /*  Disable any more paint messages  */
  EndPaint (hWnd, (LPPAINTSTRUCT) & ps);

  return TRUE;
  }
