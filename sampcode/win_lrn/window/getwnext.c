/*
 * GetWindowExt()
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  The WinMain proceeds to execute the GetWindowExt function.
 *  Two message boxes are created which display the coordinates of the
 *  window extentions.
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
  MSG   msg;
  HDC   hDC;             /* Handle to the Display Context.  */
  WORD  xExt;            /* X extent.                       */
  WORD  yExt;            /* Y extent.                       */
  DWORD dwExtent;        /* Long pointer to the extent.     */
  char szbuf[50];        /* Output buffer, zero terminated. */

  if (!hPrevInstance)  {

     WNDCLASS rClass;

     rClass.lpszClassName = (LPSTR)"getwnext"; /* Unique class name. */
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

   hInst = hInstance;

   hWnd = CreateWindow((LPSTR) "getwnext", /* Create the Window.        */
		       (LPSTR) "GetWindowExt",       /* Title: GetWindowExt.      */
		       WS_OVERLAPPEDWINDOW,          /* Set to overlapped window. */
		       CW_USEDEFAULT,                /* Use default coordinates.  */
		       CW_USEDEFAULT,                /* Use default coordinates.  */
		       CW_USEDEFAULT,                /* Use default coordinates.  */
		       CW_USEDEFAULT,                /* Use default coordinates.  */
		       (HWND)NULL,
		       (HMENU)NULL,
		       (HANDLE)hInstance,
		       (LPSTR)NULL
		     );

   ShowWindow(hWnd, cmdShow);
   UpdateWindow(hWnd);

   hDC = GetDC(hWnd);               /* Get the handle to the DC. */
   dwExtent = GetWindowExt(hDC);    /* Get the window extents.   */

   xExt = LOWORD(dwExtent);         /* Isolate the X extent.     */
   yExt = HIWORD(dwExtent);         /* Isolate the Y extent.     */

   sprintf(szbuf,                   /* Place X extent, with      */
           "%s%hu",                 /*  message, into the output */
           "The Original x Extent is ",  /*  buffer for display  */
           xExt);                   /* in a message box.         */
   MessageBox(GetFocus(),           /* Output the X extent in a  */
              (LPSTR)szbuf,         /*  message box.             */
              (LPSTR)"GetWindowExt() - MM_TEXT",
              MB_OK);

   sprintf(szbuf,                   /* Place Y extent, with      */
           "%s%hu",                 /*  message, into the output */
           "The Original y Extent is ", /*  buffer for display   */
           yExt);                   /* in a message box.         */
   MessageBox(GetFocus(),           /* Output the Y extent in a  */
              (LPSTR)szbuf,         /*  message box.             */
              (LPSTR)"GetWindowExt() - MM_TEXT",
              MB_OK);

   MessageBox(GetFocus(), (LPSTR)"Settting Extents to (2,4)",
              (LPSTR)"SetWindowExt() - MM_ISOTROPIC", MB_OK);
   SetMapMode(hDC, MM_ISOTROPIC);   /* Need this or SetWindowExt */
   SetWindowExt(hDC, 2, 4);         /* will be ignored.          */

   dwExtent = GetWindowExt(hDC);    /* Get the window extents.   */

   xExt = LOWORD(dwExtent);         /* Isolate the X extent.     */
   yExt = HIWORD(dwExtent);         /* Isolate the Y extent.     */

   sprintf(szbuf,                   /* Place X extent, with      */
           "%s%hu",                 /*  message, into the output */
           "The New x Extent is ",  /*  buffer for display       */
           xExt);                   /* in a message box.         */
   MessageBox(GetFocus(),           /* Output the X extent in a  */
              (LPSTR)szbuf,         /*  message box.             */
              (LPSTR)"GetWindowExt() - MM_ISOTROPIC",
              MB_OK);

   sprintf(szbuf,                   /* Place Y extent, with      */
           "%s%hu",                 /*  message, into the output */
           "The New y Extent is ",  /*  buffer for display       */
           yExt);                   /* in a message box.         */
   MessageBox(GetFocus(),           /* Output the Y extent in a  */
              (LPSTR)szbuf,         /*  message box.             */
              (LPSTR)"GetWindowExt() - MM_ISOTROPIC",
              MB_OK);

   ReleaseDC(hWnd, hDC);            /* Release the DC.           */

} /* WinMain */
