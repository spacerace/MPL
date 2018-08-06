/*
 *
 * GetWindowRect
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  The WinMain proceeds to execute the GetWindowRect function.
 *  If the function call is successful, message boxes are created which
 *  display the rectangle coordinates.
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
  RECT *rect;		     /* Pointer to Window Rectangle. */
  char szbuf[31];	     /* Buffer for Message Box.	    */

  if (!hPrevInstance)  {

     WNDCLASS rClass;

     rClass.lpszClassName = (LPSTR)"getwnrct";
     rClass.hInstance	  = hInstance;
     rClass.lpfnWndProc   = DefWindowProc;
     rClass.hCursor	     = LoadCursor(NULL, IDC_ARROW);
     rClass.hIcon	        = LoadIcon(NULL, IDI_APPLICATION);
     rClass.lpszMenuName  = (LPSTR)NULL;
     rClass.hbrBackground = GetStockObject(WHITE_BRUSH);
     rClass.style	        = CS_HREDRAW | CS_VREDRAW;
     rClass.cbClsExtra	  = 0;
     rClass.cbWndExtra	  = 0;

     RegisterClass((LPWNDCLASS)&rClass);

     }
   else
      ;

   hInst = hInstance;

   hWnd = CreateWindow((LPSTR) "getwnrct",
		       (LPSTR) "GetWindowRect",
		       WS_OVERLAPPEDWINDOW,           /* Use overlapped window.   */
		       CW_USEDEFAULT,                 /* Use default coordinates. */
		       CW_USEDEFAULT,                 /* Use default coordinates. */
		       CW_USEDEFAULT,                 /* Use default coordinates. */
		       CW_USEDEFAULT,                 /* Use default coordinates. */
		       (HWND)NULL,
		       (HMENU)NULL,
		       (HANDLE)hInstance,
		       (LPSTR)NULL
		     );

   ShowWindow(hWnd, cmdShow);

   UpdateWindow(hWnd);

   GetWindowRect(hWnd, (LPRECT)rect);  /* Get the Window Rectangle.    */

   /* Report the top left and the bottom right of the window rectangle */
   /* in separate message boxes.				                             */

   sprintf(szbuf,"%s%d%s%d%s","The top left is (", rect->top,
	   ",", rect->left, ").");
   MessageBox(NULL, (LPSTR)szbuf, (LPSTR)"GetWindowRect", MB_OK);

   sprintf(szbuf,"%s%d%s%d%s","The bottom right is (", rect->bottom,
	   ",", rect->right, ").");
   MessageBox(NULL, (LPSTR)szbuf, (LPSTR)"GetWindowRect", MB_OK);

} /* WinMain */

/* WINDOWPROC */
