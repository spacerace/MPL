/*
 *  SetWindowExt
 *  setwinex.c
 *
 *  This function demonstrates the use of the SetWindowExt function.  It will
 *  create a window, and procede to draw a triangle in the window in
 *  the MM_ANISOTROPIC mapping mode.
 *
 */

#include <windows.h>

/* Forward Declarations  */

BOOL FAR PASCAL InitSetWindowExt ( HANDLE , HANDLE , int );
long FAR PASCAL SetWindowExtWindowProc ( HANDLE , unsigned , WORD , LONG );

/*
 *  MAIN PROCEDURE
 */

int PASCAL WinMain  (hInstance , hPrevInstance , lpszCmdLine , cmdShow )

HANDLE hInstance , hPrevInstance;
LPSTR  lpszCmdLine;
int cmdShow;
  {
  MSG  msg;			    /*	Temp buffer to hold message  */

  InitSetWindowExt (hInstance, hPrevInstance, cmdShow );  /*  Init Routine  */

  while ( GetMessage((LPMSG)&msg, NULL, 0 , 0 ))
    {
    TranslateMessage((LPMSG)&msg);
    DispatchMessage((LPMSG)&msg);     /*  Give Your windowproc the  */
    }				      /*  message		    */

  exit(msg.wParam);
  }

BOOL FAR PASCAL InitSetWindowExt (hInstance , hPrevInstance , cmdShow)

HANDLE hInstance;
HANDLE hPrevInstance;
int cmdShow;

  {
  WNDCLASS  wcSetWindowExtClass;
  HWND	hWnd;

  wcSetWindowExtClass.lpszClassName = (LPSTR) "SetWindowExt";
  wcSetWindowExtClass.hInstance     = hInstance;
  wcSetWindowExtClass.lpfnWndProc   = SetWindowExtWindowProc;
  wcSetWindowExtClass.hCursor	    = LoadCursor ( NULL , IDC_ARROW );
  wcSetWindowExtClass.hIcon	    = NULL;
  wcSetWindowExtClass.lpszMenuName  = (LPSTR) NULL;
  wcSetWindowExtClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcSetWindowExtClass.style	    = CS_HREDRAW | CS_VREDRAW;
  wcSetWindowExtClass.cbClsExtra    = 0;
  wcSetWindowExtClass.cbWndExtra    = 0;

  RegisterClass ((LPWNDCLASS) &wcSetWindowExtClass);

  hWnd = CreateWindow((LPSTR) "SetWindowExt",	/*  Window class name	    */
		      (LPSTR) "SetWindowExt",	/*  Window title	    */
		      WS_OVERLAPPEDWINDOW,  /*	Type of window		*/
		      CW_USEDEFAULT,	    /*	x			*/
		      CW_USEDEFAULT,	    /*	y			*/
		      CW_USEDEFAULT,	    /*	cx			*/
		      CW_USEDEFAULT,	    /*	cy			*/
		      (HWND)NULL,	    /*	No parent for this wind */
		      (HMENU)NULL,	    /*	Use the Class menu	*/
		      (HANDLE)hInstance,    /*	Who created this window */
		      (LPSTR)NULL	    /*	No params. to pass on.	*/
		     );

  ShowWindow (hWnd , cmdShow);	 /*  Display this window on the screen	*/
  UpdateWindow (hWnd);		 /*  Cause a paint message		*/

  return TRUE;
  }

/*
 *  THE WINDOW PROCEDURE - Process messages
 */

long FAR PASCAL SetWindowExtWindowProc (hWnd , message , wParam , lParam)

HWND	    hWnd;		    /*	Handle of the window	*/
unsigned    message;		    /*	Message type		*/
WORD	    wParam;		    /*	Message 16-bit param	*/
LONG	    lParam;		    /*	Message 32-bit param	*/
  {
  switch (message)		    /*	Check the message type	*/
    {
    case WM_PAINT:		    /*	Process the Paint	*/
	PaintSetWindowExtWindow (hWnd); /*  message		    */
	break;

    case WM_DESTROY:		    /*	If close requested	*/
	PostQuitMessage(0);	    /*	  send yourself a quit	*/
	break;			    /*	  message		*/

    default:
	return( DefWindowProc( hWnd , message , wParam , lParam ) );
	break;
    }
  return( 0L );
  }

/*
 *  THE PAINT PROCEDURE
 */

PaintSetWindowExtWindow (hWnd)

HWND	hWnd;			       /*  Handle of the window  */
  {
  PAINTSTRUCT	ps;
  HDC		hDC;
  POINT 	lpTriangle[4];
  HANDLE	hOldBrush , hBrush;	      /*  For loading new brushes  */
  RECT		rRect;			      /*  Will hold the client	   */
					      /*  Rectangle		   */

  BeginPaint (hWnd , (LPPAINTSTRUCT) &ps);    /*  Prepare the client area  */
  hDC = ps.hdc; 			      /*  Get the Display Context  */

  hBrush = GetStockObject ( GRAY_BRUSH );     /*  Get a gray brush	   */
  hOldBrush = SelectObject ( hDC , hBrush );  /*  Select the new brush	   */

  lpTriangle[0].x = 150;    /*	The values of the points  */
  lpTriangle[0].y = 100;
  lpTriangle[1].x = 100;
  lpTriangle[1].y = 200;
  lpTriangle[2].x = 200;
  lpTriangle[2].y = 200;

  SetMapMode ( hDC , MM_ANISOTROPIC );	 /*  Set the mapping mode	      */

  SetWindowExt ( hDC , 300 , 300 );	 /*  Set the extent of the drawing
					  *  area.  This is the area that
					  *  holds graphics that you create
					  *  with GDI functions.  Do not
					  *  confuse this function with
					  *  the actual window.  The
					  *  SetViewportExt sets the
					  *  extent of the area to be mapped
					  *  to which is the actual window
					  */

  GetClientRect ( hWnd , (LPRECT) &rRect );
					 /*  Get the size of the client area
					  *  so that we can set the viewport
					  *  extent
					  */

  SetViewportExt ( hDC , rRect.right , rRect.bottom );
					 /*  Set the Extent of the viewport   */

  Polygon ( hDC , lpTriangle , 3 );	 /*  Draw the triangle		      */

  ValidateRect (hWnd , (LPRECT) NULL);	 /*  Disable any more paint messages  */
  EndPaint (hWnd, (LPPAINTSTRUCT) &ps );

  SelectObject( hDC , hOldBrush );	 /*  Replace the old brush  */
  return TRUE;
  }
