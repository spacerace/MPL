/*
 *
 * GetMessagePos
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  If the user proceeds to close the window, the GetMessagePos
 *  function is executed.  The mouse coordinates are then displayed on
 *  the screen in a message box before the window is actually closed and
 *  destroyed.
 *
 */

#include "windows.h"

/* Global Variables */
static HANDLE hInst;
static HWND hWnd;

/* FORWARD REFERENCES */
long FAR PASCAL WindowProc (HWND, unsigned, WORD, LONG);

/* WINMAIN */
int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
  MSG msg;

  if (!hPrevInstance)  {

     WNDCLASS rClass;

     rClass.lpszClassName = (LPSTR)"gmesspos";
     rClass.hInstance	  = hInstance;
     rClass.lpfnWndProc   = WindowProc;
     rClass.hCursor   	  = LoadCursor(NULL, IDC_ARROW);
     rClass.hIcon    	  = LoadIcon(hInstance, (LPSTR)"simpleton");
     rClass.lpszMenuName  = (LPSTR)NULL;
     rClass.hbrBackground = GetStockObject(WHITE_BRUSH);
     rClass.style    	  = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
     rClass.cbClsExtra	  = 0;
     rClass.cbWndExtra	  = 0;

     RegisterClass((LPWNDCLASS)&rClass);

     }
   else
      ;

   hInst = hInstance;

   hWnd = CreateWindow((LPSTR) "gmesspos",
		       (LPSTR) "GetMessagePos",
		       WS_OVERLAPPEDWINDOW,         /* Use overlapped window.   */
		       CW_USEDEFAULT,               /* Use default coordinates. */
		       CW_USEDEFAULT,               /* Use default coordinates. */
		       CW_USEDEFAULT,               /* Use default coordinates. */
		       CW_USEDEFAULT,               /* Use default coordinates. */
		       (HWND)NULL,
		       (HMENU)NULL,
		       (HANDLE)hInstance,
		       (LPSTR)NULL
		     );

   ShowWindow(hWnd, cmdShow);

   UpdateWindow(hWnd);

   while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
       TranslateMessage(&msg);
       DispatchMessage(&msg);
   }

   exit(msg.wParam);

} /* WinMain */

/* WINDOWPROC */

long FAR PASCAL WindowProc(hWnd, identifier, wParam, lParam)
HWND	hWnd;
unsigned identifier;
WORD	 wParam;
LONG	 lParam;

{
   
   switch (identifier) {

     case WM_PAINT: {

  	     PAINTSTRUCT ps;
    	  RECT	    rRect;
	     HDC	    hDC;

	     hDC=BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);       /* Paint a window  */
   	  SetMapMode(hDC, MM_ANISOTROPIC);                /* which will tell */
	     SetWindowOrg(hDC, 0, 0);                        /* the user to     */
	     SetWindowExt(hDC, 110, 110);                    /* double click    */
	     GetClientRect(hWnd, (LPRECT)&rRect);            /* the right mouse */
	     SetViewportOrg(hDC, 0, 0);                      /* button to con-  */
   	  SetViewportExt(hDC, rRect.right, rRect.bottom); /* the test.       */
	     DrawText(hDC,
                (LPSTR)"Double Click Right Mouse Button to Conduct Test.",
                48, (LPRECT)&rRect, DT_SINGLELINE);
	     EndPaint(hWnd, (LPPAINTSTRUCT)&ps);

	  }
  	  break;

    case WM_RBUTTONDBLCLK: {                  /* If the user double       */
                                              /*  clicks the right        */
       char szbuf[32];                        /*  mouse button, then      */
       unsigned long ptPos;                   /*  an output buffer is     */
       POINT point;                           /*  allocated along with    */
                                              /*  a pointer to a position */
       ptPos = GetMessagePos();               /*  and a point.  The fn    */
	    point = MAKEPOINT(ptPos);              /*  GetMessagePos is exec-  */
	    sprintf(szbuf,                         /*  uted and the mouse      */
               "%s%d%s%d%s",                  /*  position is printed in  */
               "The Mouse Position is ",      /*  a messge box.           */
	            point.x, ",", point.y, ".");
	    MessageBox(hWnd,
                  (LPSTR)szbuf,
                  (LPSTR)"GetMessagePos",
	               MB_OK);
       }
       break;

    case WM_CLOSE: {

     	 DestroyWindow(hWnd);
       }
		 break;
	
    case WM_DESTROY:
	    PostQuitMessage(0);
	    break;

    default:
	    return(DefWindowProc(hWnd, identifier, wParam, lParam));
	    break;

   }

   return(0L);

}
