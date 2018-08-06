/*
 * GetWindowLong
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  The WinMain proceeds to execute the GetWindowLong function.
 *  If the function call is successful, two message boxes are created which
 *  show if the window is tiled and the pointer to the window procedure.
 *
 *  Windows Version 2.0 function demonstration application
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
  HDC hDC;	       /* Handle to the Display Context.	     */
  LONG lLong;	    /* LONG returned by GetWindowLong.	     */
  char szbuf[80];  /* String buffer used in Message Boxes. */

  if (!hPrevInstance)  {

     WNDCLASS rClass;

     rClass.lpszClassName = (LPSTR)"getwnlng";
     rClass.hInstance	  = hInstance;
     rClass.lpfnWndProc   = DefWindowProc;
     rClass.hCursor  	  = LoadCursor(NULL, IDC_ARROW);
     rClass.hIcon    	  = LoadIcon(hInstance, IDI_APPLICATION);
     rClass.lpszMenuName  = (LPSTR)NULL;
     rClass.hbrBackground = GetStockObject(WHITE_BRUSH);
     rClass.style    	  = CS_HREDRAW | CS_VREDRAW;
     rClass.cbClsExtra	  = 0;
     rClass.cbWndExtra	  = 0;

     RegisterClass((LPWNDCLASS)&rClass);

     }
   else
      ;

   hInst = hInstance;

   hWnd = CreateWindow((LPSTR) "getwnlng",
		       (LPSTR) "GetWindowLong",
		       WS_OVERLAPPEDWINDOW,            /* Use overlapped window.   */
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

   /* Get the style of the Window */

   lLong = GetWindowLong(hWnd, GWL_STYLE);

   /*  If the Window is tiled, say so; otherwise, report that it is not */

   if ((lLong & WS_OVERLAPPEDWINDOW) == WS_OVERLAPPEDWINDOW)
   {  sprintf(szbuf,"%s","The Window has the Overlapped Style!");
      MessageBox(NULL, (LPSTR)szbuf, (LPSTR)"GWL_STYLE", MB_OK);
   }
   else
   {  sprintf(szbuf, "%s", "The Window is not Overlapped!!!");
      MessageBox(NULL, (LPSTR)szbuf, (LPSTR)"GWL_STYLE", MB_OK);
   }

   /*  Now, get the pointer to the function's window procedure.         */

   lLong = GetWindowLong(hWnd, GWL_WNDPROC);

   /*  Print out the address of the window procedure in a message box.	*/

   sprintf(szbuf,"%s%lx","The Pointer to the Window Procedure is ",lLong);
   MessageBox(NULL, (LPSTR)szbuf, (LPSTR)"GWL_WNDPROC", MB_OK);

} /* WinMain */
