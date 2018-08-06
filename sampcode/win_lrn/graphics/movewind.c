/*
 *  MoveWindow
 *
 *  This function demonstrates the use of the MoveWindow function.  It will
 *  create a window, draw a pie slice in it, display a message box, and then
 *  move the window.
 */

#include <windows.h>

/* Forward Declarations  */

BOOL FAR PASCAL InitPie ( HANDLE , HANDLE , int );
long	FAR PASCAL PieWindowProc ( HANDLE , unsigned , WORD , LONG );

/*
 *  MAIN PROCEDURE
 */

int	PASCAL WinMain  (hInstance , hPrevInstance , lpszCmdLine , cmdShow )

HANDLE hInstance , hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
{
  MSG  msg;

  InitPie (hInstance, hPrevInstance, cmdShow );  /*  Init Routine  */

  while ( GetMessage((LPMSG) & msg, NULL, 0 , 0 ))
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }

  exit(msg.wParam);
}


BOOL FAR PASCAL InitPie (hInstance , hPrevInstance , cmdShow)

HANDLE hInstance;
HANDLE hPrevInstance;
int	cmdShow;

{
  WNDCLASS  wcPieClass;
  HWND	hWnd;

  wcPieClass.lpszClassName = (LPSTR) "Pie";
  wcPieClass.hInstance	   = hInstance;
  wcPieClass.lpfnWndProc   = PieWindowProc;
  wcPieClass.hCursor	   = LoadCursor ( NULL , IDC_ARROW );
  wcPieClass.hIcon	   = NULL;
  wcPieClass.lpszMenuName  = (LPSTR) NULL;
  wcPieClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcPieClass.style	   = CS_HREDRAW | CS_VREDRAW;
  wcPieClass.cbClsExtra    = 0;
  wcPieClass.cbWndExtra    = 0;

  RegisterClass ((LPWNDCLASS) & wcPieClass);

  hWnd = CreateWindow((LPSTR) "Pie", (LPSTR) "MoveWindow",
      WS_OVERLAPPEDWINDOW,  
      CW_USEDEFAULT, 	0,    
      CW_USEDEFAULT, 	0,    
      NULL, 	NULL, 	hInstance, NULL);

  ShowWindow (hWnd , cmdShow);
  UpdateWindow (hWnd);

  return TRUE;
}


/*
 *  THE WINDOW PROCEDURE - Process messages
 */

long	FAR PASCAL PieWindowProc (hWnd , message , wParam , lParam)

HWND	    hWnd;
unsigned	message;
WORD	    wParam;
LONG	    lParam;
{
  static BOOL bFlag = FALSE;	    /*	Tell us whether or not	*/
/*	we have moved the	*/
/*	Window			*/

  switch (message)
  {
  case WM_PAINT:
    PaintPieWindow (hWnd);
    if ( bFlag == FALSE )
    {
      bFlag = TRUE;
      MessageBox( GetFocus() , (LPSTR)"Press the OK Button to move the window",
          (LPSTR)"MOVE", MB_OK );

      MoveWindow( hWnd , 0 , 0 , 400 , 300 , TRUE );
/*  Change the size as well as the position using the
	       *  MoveWindow function.	We also tell the Window to
	       *  re-paint the client area.  We use the bFlag variable
	       *  to make sure we only call MoveWindow once.
	       */
    }
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

PaintPieWindow (hWnd)

HWND	hWnd;			    /*	Handle of the window  */
{
  PAINTSTRUCT	ps;
  HDC		hDC;

  BeginPaint (hWnd , (LPPAINTSTRUCT) & ps);  /*	Prepare the client area */
  hDC = ps.hdc; 		       /*  Get the Display Context  */

  Pie ( hDC , 100 , 100 , 300 , 300 , 300 , 200 , 200 , 100 );
/*  Draw the Pie
	 *  Upper Left of box holding pie slice  = 100 , 100
	 *  Lower Right of box holding pie slice = 300 , 300
	 *  Start the arc at 300 , 300
	 *  End the arc at 200 , 100
	 *  The arc will be drawn in a counter clockwise direction
	 */

  ValidateRect (hWnd , (LPRECT) NULL);	 /*  Disable any more paint messages  */
  EndPaint (hWnd, (LPPAINTSTRUCT) & ps );

  return TRUE;
}
