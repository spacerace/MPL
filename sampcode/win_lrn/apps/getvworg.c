/*
 *
 * GetViewportOrg
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  The WinMain proceeds to execute the GetViewportOrg function.
 *  Two message boxes are created which display the x and y coordinates
 *  of the viewport origin respectively.
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
  HDC hDC;            /* Handle to the Display Context. */
  unsigned int xOrg;  /* X coordinate of the origin.    */
  unsigned int yOrg;  /* Y coordinate of the origin.    */
  long ptOrigin;      /* Long pointer to the origin.    */
  char szbuf[19];     /* Output buffer.                 */

  if (!hPrevInstance)  {

     WNDCLASS rClass;

     rClass.lpszClassName = (LPSTR)"getvworg";
     rClass.hInstance	  = hInstance;
     rClass.lpfnWndProc   = DefWindowProc;
     rClass.hCursor	     = LoadCursor(NULL, IDC_ARROW);
     rClass.hIcon	        = LoadIcon(hInstance, IDI_APPLICATION);
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

   hWnd = CreateWindow((LPSTR) "getvworg",
		       (LPSTR) "GetViewportOrg",
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

   hDC = GetDC(hWnd);

   ptOrigin = GetViewportOrg(hDC);      /* Get the viewport origin.  */

   xOrg = LOWORD(ptOrigin);             /* Isolate the X coordinate. */
   yOrg = HIWORD(ptOrigin);             /* Isolate the Y coordinate. */

   sprintf(szbuf,                       /* Place the X coordinate in */
           "%s%d",                      /*  the output buffer along  */
           "The x Origin is ",          /*  with some explanatory    */
           xOrg);                       /*  text.                    */
   MessageBox(NULL,                     /* Display the information   */
              (LPSTR)szbuf,             /*  in a message box.        */
              (LPSTR)"GetViewportOrg",
              MB_OK);

   sprintf(szbuf,                       /* Place the Y origin in the */
           "%s%d",                      /*  output buffer along with */
           "The y Origin is ",          /*  some text.               */
           yOrg);
   MessageBox(NULL,                     /* Output the information in */
              (LPSTR)szbuf,             /*  another message box.     */
              (LPSTR)"GetViewportOrg",
              MB_OK);

} /* WinMain */
