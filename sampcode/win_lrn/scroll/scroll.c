/* SCROLL.C							 */
/* This program illistrates the use of ScrollDC and ScrollWindow */
/* as well as many other Windows functions. ScrollDC is used to  */
/* "Bounce" individual balls, one at a time, from the top of the */
/* display to the bottom of the display. ScrollWindow is used to */
/* "Raise" all the balls from the bottom of the display to the	 */
/* top of the display. This program gets the resolution of the	 */
/* screen to determine what would be a good shape for the ball	 */
/* as well as to deternime how fast to scroll the ball. We also  */
/* get the size of the client area so that we can deternime how  */
/* many balls to draw on the screen. I decided the height of the */
/* ball should be 1/10 the height of the client area and the it  */
/* should be drawn 1/10 of the way down the client area and the  */
/* same distance over on the client area. The other balls are	 */
/* drawn similarly. Enjoy .....................................  */

#include <windows.h>
#include "scroll.h"


long FAR PASCAL WndProc (HWND, unsigned, WORD, LONG) ;

static char szAppName [] = "Scroll" ;

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE      hInstance, hPrevInstance ;
     LPSTR       lpszCmdLine ;
     int         nCmdShow ;
     {

     HWND        hWnd ;
     MSG         msg ;
     WNDCLASS	 wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
	  wndclass.hIcon	 = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
	  wndclass.lpszMenuName  = szAppName;
          wndclass.lpszClassName = szAppName ;

          if (!RegisterClass (&wndclass))
               return FALSE ;
          }

     hWnd = CreateWindow (szAppName,	     /* window class		*/
		    "Scroll",	 /* window caption	    */
                    WS_OVERLAPPEDWINDOW,     /* window style            */
                    CW_USEDEFAULT,           /* initial x position      */
                    0,                       /* initial y position      */
                    CW_USEDEFAULT,           /* initial x size          */
                    0,                       /* initial y size          */
                    NULL,                    /* parent window handle    */
                    NULL,                    /* window menu handle      */
                    hInstance,               /* program instance handle */
                    NULL) ;                  /* create parameters       */

     ShowWindow (hWnd, nCmdShow) ;
     UpdateWindow (hWnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

void FAR PASCAL StopOne(){
    return;
    }


long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
     HWND	    hWnd ;
     unsigned	    iMessage ;
     WORD	    wParam ;
     LONG	    lParam ;
     {
     static int   xClient, yClient ;
     static int   xScreen, yScreen ;
     static short   numBalls , bounced ;
     static short   ScrollRate ;
     HDC	    hDC ;
     PAINTSTRUCT    ps ;
     RECT	    ClientRect ;
     RECT	    DcRect ;
     RECT	    Ball ;
     RECT	    NewBall ;
     RECT	    ScrollBox ;
     short	    i, j, up, down ;
     char szBuffer[80] ;

     switch (iMessage)
	  {
	  case WM_CREATE:

/* Get the resolution of the Screen */

	       xScreen = GetSystemMetrics (SM_CXSCREEN) ;
	       yScreen = GetSystemMetrics (SM_CYSCREEN) ;

/* Get the size of the client area initially */

	       GetClientRect (hWnd, &ClientRect) ;

/* and save it in static variables */

	       yClient = ClientRect.bottom - ClientRect.top ;
	       xClient = ClientRect.right - ClientRect.left  ;

/* Set scroll speed for different resolutions */

	       if (yClient <= 50) ScrollRate = 1 ;
	       if (yClient > 100)  ScrollRate = 3 ;
	       if (yClient > 175)  ScrollRate = 5 ;

	       break ;


	  case WM_SIZE:

/* Update the size of the client area */

	       yClient = HIWORD (lParam) ;
	       xClient = LOWORD (lParam) ;

/* Set scroll speed for different resolutions */

	       if (yClient <= 50) ScrollRate = 1 ;
	       if (yClient > 100)  ScrollRate = 3 ;
	       if (yClient > 175)  ScrollRate = 5 ;

	       break ;


	       break ;

	  case WM_PAINT:

/* Begin to paint the client area and redraw the balls */

	       BeginPaint (hWnd, &ps);

/* Initialize the size of the ball */

	       Ball.top    = ( yClient / 10) ;
	       Ball.bottom = ( yClient / 5) ;
	       Ball.left   = ((Ball.bottom - Ball.top) * (xScreen / yScreen)) ;
	       Ball.right  = 2 * Ball.left ;

/* Determine the number of balls that can fit on the screen */

	       numBalls = ( xClient - (Ball.right - Ball.left) ) /
			      ( 2 * (Ball.right - Ball.left)) ;

/* And use this as a standard for drawing the balls */

	       NewBall = Ball ;

	       for (i = 0 ; i < numBalls; i ++ ) {

		 Ellipse (ps.hdc, NewBall.left, NewBall.top,
				NewBall.right, NewBall.bottom);

/* Update the position of the new ball */

		 NewBall.left = NewBall.left +	2 * (Ball.right - Ball.left);
		 NewBall.right = NewBall.right + 2 * (Ball.right - Ball.left);

	       }

	       EndPaint (hWnd, &ps);

/* Set bounced to false, signifing that the balls are on the
		  top of the display */

	       bounced = 0 ;

	       break ;

	  case WM_COMMAND:

	       switch (wParam)
		     {
		     case IDM_DC:

			  if (!bounced) {

/* Initialize the size of the ball */

			     Ball.top	 = ( yClient / 10) ;
			     Ball.bottom = ( yClient / 5) ;
			     Ball.left	 = ((Ball.bottom - Ball.top) * (xScreen / yScreen)) ;
			     Ball.right  = 2 * Ball.left ;

/* Initialize the scrolling area for one ball */

			     ScrollBox.top = 1 ;
			     ScrollBox.left = Ball.left / 2 ;
			     ScrollBox.bottom = yClient ;
			     ScrollBox.right = ScrollBox.left + (2 * Ball.left) ;

			     hDC = GetDC (hWnd) ;
			     for (i = 0 ; i < numBalls; i ++ ) {

			       up = Ball.bottom ;
			       down = yClient - (2 * ScrollRate) ;

/* Until the ball is done bouncing */

			       while (up < down ) {

/* It goes down */

				 for (j = up ; j <= down ; j += ScrollRate ) {
				   ScrollDC (hDC,0, ScrollRate,
					       &ScrollBox,&ScrollBox,
					       NULL,NULL);
				 }

/* and won't go up quit so high */
				 up = up + ((down - up) / 3) + 1 ;
			     /*  MessageBeep (0); */


/* and now it bounces up */
				 for (j = up ; j <= down ; j += ScrollRate ) {
				   ScrollDC (hDC,0, - ScrollRate,
					       &ScrollBox,&ScrollBox,
					       NULL,NULL);
				 }
			       }

/* and bounces down for the last time */

			       for (j = up ; j <= down ; j += ScrollRate ) {
				 ScrollDC (hDC,0, 1,&ScrollBox,&ScrollBox,
					     NULL,NULL);
			       }

/* Move to the next scrolling area */

			       ScrollBox.left = ScrollBox.left + ( 2 * Ball.left) ;
			       ScrollBox.right = ScrollBox.right + (2 * Ball.left);
			     }

			     ReleaseDC (hWnd, hDC) ;
			     bounced = 1 ;
			  }

			  break ;

		     case IDM_WIN:

/* If the balls are at the bottom of the display then */

			  if (bounced) {

/* Initialize how far they need to rise */

			    down = yClient - (2 * ScrollRate);
			    up = (yClient / 5) ;

/* And raise them to the top */

			    for (j = up ; j <= down ; j += ScrollRate ) {
			      ScrollWindow (hWnd, 0,- ScrollRate,
					       NULL, NULL) ;
			    }

			    MessageBeep (0);

/* Set bounced to false, signifing the balls are at the top of the display */

			    bounced = 0 ;

			  }

			  break ;

		     case IDM_EXIT:

			  SendMessage (hWnd, WM_CLOSE, 0, 0L);
			  break;

		     case IDM_HELP:

			  MessageBox (hWnd, "Try Scrolling the DC ...",
			       szAppName, MB_ICONASTERISK | MB_OK) ;
			  break;
		     case IDM_ABOUT:

			  MessageBox (hWnd, "SCROLL DEMO.",
			       szAppName, MB_ICONASTERISK | MB_OK) ;
		     default:
			  break;
		     }
	       break;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               break ;

          default:
               return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
          }
     return 0L ;
     }
    
