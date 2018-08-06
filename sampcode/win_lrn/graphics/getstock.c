/*
 *  GetStockObject
 *
 *  This function demonstrates the use of the GetStockObject function.	It will
 *  create a window using the CreateWindow function, show the window using
 *  the ShowWindow command and procede to draw a triangle in the window in
 *  the MM_ANISOTROPIC mapping mode.  It will use the GetStockObject function
 *  to get the handle to a new brush.
 *
 *  Windows Version 2.0 function demonstration application
 *
 */

#include <windows.h>

/* Forward Declarations  */

BOOL FAR PASCAL InitGetStockObject ( HANDLE , HANDLE , int );
long FAR PASCAL GetStockObjectWindowProc ( HANDLE , unsigned , WORD , LONG );

/*
 *  MAIN PROCEDURE
 */

int PASCAL WinMain  (hInstance , hPrevInstance , lpszCmdLine , nCmdShow )

HANDLE hInstance , hPrevInstance;
LPSTR  lpszCmdLine;
int nCmdShow;
  {
  MSG  msg;			    /*	Temp buffer to hold message  */

  InitGetStockObject (hInstance, hPrevInstance, nCmdShow );  /*  Init Routine  */

  while ( GetMessage((LPMSG)&msg, NULL, 0 , 0 ))
    {
    TranslateMessage((LPMSG)&msg);
    DispatchMessage((LPMSG)&msg);     /*  Give Your windowproc the  */
    }				      /*  message		    */

  exit(msg.wParam);
  }

BOOL FAR PASCAL InitGetStockObject (hInstance , hPrevInstance , nCmdShow)

HANDLE hInstance;
HANDLE hPrevInstance;
int nCmdShow;

  {
  WNDCLASS  wcGetStockObjectClass;
  HWND	hWnd;

  wcGetStockObjectClass.lpszClassName = (LPSTR) "GetStockObject";
  wcGetStockObjectClass.hInstance     = hInstance;
  wcGetStockObjectClass.lpfnWndProc   = GetStockObjectWindowProc;
  wcGetStockObjectClass.hCursor       = LoadCursor ( NULL , IDC_ARROW );
  wcGetStockObjectClass.hIcon	      = NULL;
  wcGetStockObjectClass.lpszMenuName  = (LPSTR) NULL;
  wcGetStockObjectClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcGetStockObjectClass.style	      = CS_HREDRAW | CS_VREDRAW;
  wcGetStockObjectClass.cbClsExtra    = 0;
  wcGetStockObjectClass.cbWndExtra    = 0;

  RegisterClass ((LPWNDCLASS) &wcGetStockObjectClass);

  hWnd = CreateWindow((LPSTR) "GetStockObject",   /*  Window class name       */
		      (LPSTR) "GetStockObject",   /*  Window title	      */
		      WS_OVERLAPPEDWINDOW,	  /*  Type of window	      */
		      CW_USEDEFAULT,		  /*  default x pos	      */
		      CW_USEDEFAULT,		  /*  default y pos	      */
		      CW_USEDEFAULT,		  /*  default change in x pos */
		      CW_USEDEFAULT,		  /*  default change in y pos */
		      (HWND)NULL,		  /*  No parent for this wind */
		      (HMENU)NULL,		  /*  Use the Class menu      */
		      (HANDLE)hInstance,	  /*  Who created this window */
		      (LPSTR)NULL		  /*  No params. to pass on.  */
		     );

  ShowWindow (hWnd , nCmdShow);   /*  Display this window on the screen
				   *  nCmdShow is passed in by WinMain, and
				   *  should only be used with ShowWindow
				   *  once during a program.  Any further
				   *  calls to ShowWindow will need to have
				   *  certain values.  See entry in manual
				   *  for ShowWindow for further details
				   */

  UpdateWindow (hWnd);		 /*  Cause a paint message		*/

  return TRUE;
  }

/*
 *  THE WINDOW PROCEDURE - Process messages
 */

long FAR PASCAL GetStockObjectWindowProc (hWnd , message , wParam , lParam)

HWND	    hWnd;		    /*	Handle of the window	*/
unsigned    message;		    /*	Message type		*/
WORD	    wParam;		    /*	Message 16-bit param	*/
LONG	    lParam;		    /*	Message 32-bit param	*/
  {
  switch (message)		    /*	Check the message type	*/
    {
    case WM_PAINT:		    /*	Process the Paint	*/
	PaintGetStockObjectWindow (hWnd); /*  message		      */
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

PaintGetStockObjectWindow (hWnd)

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

  hBrush    = GetStockObject ( GRAY_BRUSH );  /*  Get a gray brush	   */

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
