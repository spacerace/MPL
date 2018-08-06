/*
 * GetWindowOrg
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  The WinMain proceeds to execute the GetWindowOrg function.
 *  Two message boxes are created which display the x and y coordinates
 *  respectively.  The x and y values here are the default values.
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
  unsigned int xOrg;  /* X origin.                      */
  unsigned int yOrg;  /* Y origin.                      */
  long ptOrigin;      /* Pointer to the origin.         */
  char szbuf[50];     /* Output buffer.                 */

  if (!hPrevInstance)  {

     WNDCLASS rClass;

     rClass.lpszClassName = (LPSTR)"getwnorg";
     rClass.hInstance	  = hInstance;
     rClass.lpfnWndProc   = DefWindowProc;
     rClass.hCursor   	  = LoadCursor(NULL, IDC_ARROW);
     rClass.hIcon    	  = LoadIcon(hInstance, IDI_APPLICATION);
     rClass.lpszMenuName  = (LPSTR)NULL;
     rClass.hbrBackground = GetStockObject(WHITE_BRUSH);
     rClass.style    	  = CS_HREDRAW | CS_VREDRAW;
     rClass.cbClsExtra	  = 0;
     rClass.cbWndExtra	  = 0;

     RegisterClass((LPWNDCLASS)&rClass);

     }

   hInst = hInstance;

   hWnd = CreateWindow((LPSTR) "getwnorg",
		       (LPSTR) "GetWindowOrg",
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

   hDC = GetDC(hWnd);                /* Get the handle to the DC.     */
   ptOrigin = GetWindowOrg(hDC);     /* Get the origin of the window. */
   ReleaseDC(hWnd, hDC);             /* Release the DC.               */
   xOrg = LOWORD(ptOrigin);          /* Isolate the x coordinate.     */
   yOrg = HIWORD(ptOrigin);          /* Isolate the y coordinate.     */

      /* Display the X coordinate in a message box. */

   sprintf(szbuf,"%s%d","The Original x Origin is ",xOrg);
   MessageBox(GetFocus(), (LPSTR)szbuf, (LPSTR)"GetWindowOrg", MB_OK);

      /* Display the Y coordinate in a message box. */

   sprintf(szbuf,"%s%d","The Original y Origin is ",yOrg);
   MessageBox(GetFocus(), (LPSTR)szbuf, (LPSTR)"GetWindowOrg", MB_OK);

   MessageBox(GetFocus(), (LPSTR)"Setting a New Origin at (5,5)",
              (LPSTR)"SetWindowOrg", MB_OK);

      /* Set a new origin and get it too.           */
   SetWindowOrg(hDC, 5, 5);          /* Set new origin of the window. */
   ptOrigin = GetWindowOrg(hDC);     /* Get new origin of the window. */
   xOrg = LOWORD(ptOrigin);          /* Isolate the x coordinate.     */
   yOrg = HIWORD(ptOrigin);          /* Isolate the y coordinate.     */

      /* Display the X coordinate in a message box. */

   sprintf(szbuf,"%s%d","The New x Origin is ",xOrg);
   MessageBox(GetFocus(), (LPSTR)szbuf, (LPSTR)"GetWindowOrg", MB_OK);

      /* Display the Y coordinate in a message box. */

   sprintf(szbuf,"%s%d","The New y Origin is ",yOrg);
   MessageBox(GetFocus(), (LPSTR)szbuf, (LPSTR)"GetWindowOrg", MB_OK);

} /* WinMain */
