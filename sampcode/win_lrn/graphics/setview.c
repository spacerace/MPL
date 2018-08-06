/*
 *  SetViewportExt
 *  setview.c
 *
 *  This function demonstrates the use of the SetViewportExt function.	It will
 *  create a window, and procede to draw a triangle in the window in
 *  the MM_ANISOTROPIC mapping mode.
 *
 */

#include <windows.h>

/* Forward Declarations  */

BOOL FAR PASCAL InitSetViewportExt ( HANDLE , HANDLE , int );
long FAR PASCAL SetViewportExtWindowProc ( HANDLE , unsigned , WORD , LONG );

/*
 *  MAIN PROCEDURE
 */

int PASCAL WinMain  (hInstance , hPrevInstance , lpszCmdLine , cmdShow )

HANDLE hInstance , hPrevInstance;
LPSTR  lpszCmdLine;
int cmdShow;
  {
  MSG  msg;			    /*	Temp buffer to hold message  */

  InitSetViewportExt (hInstance, hPrevInstance, cmdShow );  /*	Init Routine  */

  while ( GetMessage((LPMSG)&msg, NULL, 0 , 0 ))
    {
    TranslateMessage((LPMSG)&msg);
    DispatchMessage((LPMSG)&msg);     /*  Give Your windowproc the  */
    }				      /*  message		    */

  exit(msg.wParam);
  }

BOOL FAR PASCAL InitSetViewportExt (hInstance , hPrevInstance , cmdShow)

HANDLE hInstance;
HANDLE hPrevInstance;
int cmdShow;

  {
  WNDCLASS  wcSetViewportExtClass;
  HWND	hWnd;

  wcSetViewportExtClass.lpszClassName = (LPSTR) "SetViewportExt";
  wcSetViewportExtClass.hInstance     = hInstance;
  wcSetViewportExtClass.lpfnWndProc   = SetViewportExtWindowProc;
  wcSetViewportExtClass.hCursor       = LoadCursor ( NULL , IDC_ARROW );
  wcSetViewportExtClass.hIcon	      = NULL;
  wcSetViewportExtClass.lpszMenuName  = (LPSTR) NULL;
  wcSetViewportExtClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcSetViewportExtClass.style	      = CS_HREDRAW | CS_VREDRAW;
  wcSetViewportExtClass.cbClsExtra    = 0;
  wcSetViewportExtClass.cbWndExtra    = 0;

  RegisterClass ((LPWNDCLASS) &wcSetViewportExtClass);

  hWnd = CreateWindow((LPSTR) "SetViewportExt",   /*  Window class name       */
		      (LPSTR) "SetViewportExt",   /*  Window title	      */
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

long FAR PASCAL SetViewportExtWindowProc (hWnd , message , wParam , lParam)

HWND	    hWnd;		    /*	Handle of the window	*/
unsigned    message;		    /*	Message type		*/
WORD	    wParam;		    /*	Message 16-bit param	*/
LONG	    lParam;		    /*	Message 32-bit param	*/
  {
  switch (message)		    /*	Check the message type	*/
    {
    case WM_PAINT:		    /*	Process the Paint	*/
	PaintSetViewportExtWindow (hWnd); /*  message		      */
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

PaintSetViewportExtWindow (hWnd)

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
