/*
 *
 *  GetMessage
 *
 *  This program demonstrates the use of the GetMessage function.  The
 *   program registers and creates a window.  Then, it goes into the
 *   GetMessage loop.  On the screen is the instruction to double click
 *   the right mouse button.  This conducts the test of GetMessage by
 *   displaying a message box from within the WM_RBOTTONDBLCLK section
 *   of the window procedure .
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

     rClass.lpszClassName = (LPSTR)"getmsg";
     rClass.hInstance	  = hInstance;
     rClass.lpfnWndProc   = WindowProc;
     rClass.hCursor	     = LoadCursor(NULL, IDC_ARROW);
     rClass.hIcon	        = LoadIcon(hInstance, IDI_APPLICATION);
     rClass.lpszMenuName  = (LPSTR)NULL;
     rClass.hbrBackground = GetStockObject(WHITE_BRUSH);

     /* Permit the window class to accept double click messages by
        using the CS_DBLCLKS style.                                */

     rClass.style	        = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
     rClass.cbClsExtra	  = 0;
     rClass.cbWndExtra	  = 0;

     RegisterClass((LPWNDCLASS)&rClass);

     }
   else
      ;

   hInst = hInstance;

   hWnd = CreateWindow((LPSTR) "getmsg",
		       (LPSTR) "GetMessage",
		       WS_OVERLAPPEDWINDOW,          /* Use an overlapping window. */
		       CW_USEDEFAULT,                /* Use default coordinates.   */
		       CW_USEDEFAULT,                /* Use default coordinates.   */
		       CW_USEDEFAULT,                /* Use default coordinates.   */
		       CW_USEDEFAULT,                /* Use default coordinates.   */
		       (HWND)NULL,
		       (HMENU)NULL,
		       (HANDLE)hInstance,
		       (LPSTR)NULL
		     );

   ShowWindow(hWnd, cmdShow);

   UpdateWindow(hWnd);

   /* Go into the GetMessage loop at this time.  The user can then
      double click on the right mouse button when the cursor is in
      the client area of this application.                         */

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
      	RECT	    rRect;
	      PAINTSTRUCT ps;
      	HDC	    hDC;

      	hDC=BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
	   	SetMapMode(hDC, MM_ANISOTROPIC);
	   	SetWindowOrg(hDC, 0, 0);
	   	SetWindowExt(hDC, 110, 110);
	   	GetClientRect(hWnd, (LPRECT)&rRect);
	   	SetViewportOrg(hDC, 0, 0);
	   	SetViewportExt(hDC, rRect.right, rRect.bottom);

         /* Display the instructions in the client area for the
            user to conduct the test.                            */
         
	   	DrawText(hDC,
                  (LPSTR)"Double Click Right Mouse Button to Conduct Test.",
                  48,
                  (LPRECT)&rRect,
                  DT_SINGLELINE);
	   	EndPaint(hWnd, (LPPAINTSTRUCT)&ps);

	  }
	  break;

     /* Check to see if the user has double clicked the right
        mouse button.                                          */

    case WM_RBUTTONDBLCLK:        /* If so, display a success message box. */
      MessageBox(hWnd,
                 (LPSTR)"GetMessage Retrieved a WM_RBUTTONDBLCLK Message",
                 (LPSTR)"GetMessage",
                 MB_OK);
      break;                    /* And return for more message processing. */

    case WM_CLOSE:
   	DestroyWindow(hWnd);
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
