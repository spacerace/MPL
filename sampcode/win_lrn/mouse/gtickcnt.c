/*
 *
 * GetTickCount
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  If the user couble clicks the right mouse button, the
 *  GetTickCount function is executed.  The time since system startup
 *  is returned and displayed in a message box.
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

     rClass.lpszClassName = (LPSTR)"gtickcnt";
     rClass.hInstance	  = hInstance;
     rClass.lpfnWndProc   = WindowProc;
     rClass.hCursor   	  = LoadCursor(NULL, IDC_ARROW);
     rClass.hIcon    	  = LoadIcon(hInstance, IDI_APPLICATION);
     rClass.lpszMenuName  = (LPSTR)NULL;
     rClass.hbrBackground = GetStockObject(WHITE_BRUSH);
     rClass.style	        = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
     rClass.cbClsExtra	  = 0;       /* Add double click capabilities. */
     rClass.cbWndExtra	  = 0;

     RegisterClass((LPWNDCLASS)&rClass);

     }
   else
      ;

   hInst = hInstance;

   hWnd = CreateWindow((LPSTR) "gtickcnt",
		       (LPSTR) "GetTickCount",         /* Create a window.         */
		       WS_OVERLAPPEDWINDOW,            /* Make it overlapped.      */
		       CW_USEDEFAULT,                  /* Use default coordinates. */
		       CW_USEDEFAULT,                  /* Use default coordinates. */
		       CW_USEDEFAULT,                  /* Use default coordinates. */
		       CW_USEDEFAULT,                  /* Use default coordinates. */
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

	     hDC=BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
   	  SetMapMode(hDC, MM_ANISOTROPIC);
	     SetWindowOrg(hDC, 0, 0);
	     SetWindowExt(hDC, 110, 110);
	     GetClientRect(hWnd, (LPRECT)&rRect);
	     SetViewportOrg(hDC, 0, 0);
   	  SetViewportExt(hDC, rRect.right, rRect.bottom);
	     DrawText(hDC,
                (LPSTR)"Double Click Right Mouse Button To Conduct Test.",
                48, (LPRECT)&rRect, DT_SINGLELINE);  /* Place text to    */
	     EndPaint(hWnd, (LPPAINTSTRUCT)&ps);          /* prompt for test. */

	  }
  	  break;
	
     case WM_RBUTTONDBLCLK:                    /* If the user double   */
       {                                       /* clicks on the right  */
       char szbuf[80];   /* Output buffer       | mouse button, then   */
       DWORD dwCnt;  /* No. of ms since startup | establish needed     */
                                               /* variables and call   */
       dwCnt = GetTickCount();                 /* the GetTickCount     */
                                               /* function.  Capture   */
	    sprintf(szbuf,                          /* the time information */
          "%s%s%lu%s\0",                        /* in a zero terminated */
          "The No. of Milliseconds since",     /* buffer.              */
	       " system startup is ", dwCnt, ".");  

       MessageBox(hWnd,                        /* Output the buffer in */
                  (LPSTR)szbuf,                /* a message box format */
                  (LPSTR)"GetTickCount",       /* so that the user can */
	               MB_OK);                      /* have a readable and  */
       }                                       /* useful format.       */
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
