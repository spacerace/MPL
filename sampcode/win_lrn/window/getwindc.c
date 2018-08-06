/*
 * GetWindowDC
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  The WinMain proceeds to execute the GetWinDC function.
 *  If the function call is successful, a message box is created which
 *  reflects success.  Otherwise, failure is reported via message box.
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
  MSG  msg;
  HDC  hDC;     /* Handle to the Display Context. */
  BOOL bResult; /* Result from using the DC.      */

  if (!hPrevInstance)  {

     WNDCLASS rClass;

     rClass.lpszClassName = (LPSTR)"getwindc";  /* Unique class name. */
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

   hWnd = CreateWindow((LPSTR) "getwindc", /* Create the window.        */
		       (LPSTR) "GetWindowDC",        /* Title: GetWindowDC.       */
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

   if ((hDC = GetWindowDC(hWnd)) != NULL)     /* If successful at getting */
      MessageBox(GetFocus(),	                /*  the DC, send success	  */
		 (LPSTR)"Got The Window DC",            /*  message via message	  */
		 (LPSTR)"GetWindowDC",	                /*  box.         			  */
		 MB_OK);
   else 				                            /* Otherwise,		           */
      MessageBox(GetFocus(),	                /*  send the failure    	  */
		 (LPSTR)"Do Not Have the DC",           /*  message via message	  */
		 (LPSTR)"GetWindowDC",	                /*  box.         			  */
		 MB_OK);

   bResult = Rectangle(hDC, 100, 100, 200, 200);

   if (bResult != NULL)
      MessageBox(GetFocus(), (LPSTR)"Used DC Successfully",
                 (LPSTR)"Rectangle()", MB_OK);
   else
      MessageBox(GetFocus(), (LPSTR)"Error Using DC",
                 (LPSTR)"Rectangle()", MB_OK);

   ReleaseDC(hWnd, hDC);

} /* WinMain */
