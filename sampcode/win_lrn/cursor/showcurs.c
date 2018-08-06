/*
Function(s) demonstrated in this program:  ShowCursor

Compiler version:  5.10

Description:  Use ShowCursor to hide the cursor each time the right mouse
	      button is pressed, and display it again when the button is
	      released.

Additional Comments:



****************************************************************************/

#include <windows.h>

long FAR PASCAL WndProc (HWND, unsigned, WORD, LONG) ;

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE      hInstance, hPrevInstance ;
     LPSTR       lpszCmdLine ;
     int         nCmdShow ;
     {
     static char szAppName [] = "SampleWindow" ;
     HWND        hWnd ;
     WNDCLASS    wndclass ;
     MSG	 msg;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	  wndclass.hCursor	 = LoadCursor (NULL, IDC_CROSS);
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          if (!RegisterClass (&wndclass))
               return FALSE ;
          }

     hWnd = CreateWindow (szAppName,            /* window class name       */
		    "Using the ShowCursor function",  /* window caption 	 */
                    WS_OVERLAPPEDWINDOW,        /* window style            */
                    CW_USEDEFAULT,              /* initial x position      */
                    0,                          /* initial y position      */
                    CW_USEDEFAULT,              /* initial x size          */
                    0,                          /* initial y size          */
                    NULL,                       /* parent window handle    */
                    NULL,                       /* window menu handle      */
                    hInstance,                  /* program instance handle */
                    NULL) ;                     /* create parameters       */

     ShowWindow (hWnd, nCmdShow) ;

     UpdateWindow (hWnd) ;

     while (GetMessage(&msg, NULL, 0, 0))
     {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
     } 
     return (msg.wParam) ;     
     }

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned iMessage ;
WORD     wParam ;
LONG	 lParam ;
    {
    PAINTSTRUCT ps;
    HDC 	hDC;
    switch(iMessage)
	{
	case WM_CREATE:
	    {
	    break;
	    }
	case WM_PAINT:
	    {
	    hDC = BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
	    EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
	    break;
	    }
	case WM_RBUTTONDOWN:
	    {
	    ShowCursor(0);
	    break;
	    }
	case WM_RBUTTONUP:
	    {
	    ShowCursor(1);
	    break;
	    }
	case WM_DESTROY:
	    {
	    PostQuitMessage(0);
	    break;
	    }
	default:
	    {
	    return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
	    }
	}
    return (0L);
    }
