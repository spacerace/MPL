/*
 * SetTextCharacterExtra
 * gtxtchx.c
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  If the user couble clicks the right mouse button, the
 *  SetTextCharacterExtra function is executed.  The intercharacter
 *  spacing is returned and displayed in a message box.
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

     rClass.lpszClassName = (LPSTR)"stxtchx";
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

   hWnd = CreateWindow((LPSTR) "stxtchx",
		       (LPSTR) "SetTextCharacterExtra", /* Create a window.         */
		       WS_OVERLAPPEDWINDOW,             /* Make it overlapped.      */
		       CW_USEDEFAULT,                   /* Use default coordinates. */
		       CW_USEDEFAULT,                   /* Use default coordinates. */
		       CW_USEDEFAULT,                   /* Use default coordinates. */
		       CW_USEDEFAULT,                   /* Use default coordinates. */
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
                (LPSTR)"Double Click Right Mouse Button To Set Intercharacter Spacing.",
                62, (LPRECT)&rRect, DT_SINGLELINE);  /* Place text to    */
	     EndPaint(hWnd, (LPPAINTSTRUCT)&ps);          /* prompt for test. */

	  }
  	  break;
	
     case WM_RBUTTONDBLCLK:                    /* If the user double    */
       {                                       /* clicks on the right   */
       char szbuf[80];   /* Output buffer       | mouse button, then    */
       short sIntSpacing; /* Interchar spacing. | variables and call    */
       short sOldIntSpacing; /* Old spacing.  */
       HDC hDC; /* Handle to display Context. */

       hDC = GetDC(hWnd);                      /* (after getting DC)    */
       sIntSpacing = GetTextCharacterExtra(hDC); /* GetTextCharacterExtra */
                                               /* (with the display     */
                                               /* context of the top    */
                                               /* level window.         */
	    sprintf(szbuf,                          /* Capture the interchar */
          "%s%i%s\0",                          /* spacing information   */
          "Current Intercharacter Spacing = ", /* in a zero terminated  */
	       (int)sIntSpacing, ".");              /* buffer.               */   
	    MessageBox(hWnd,                        /* Output the buffer in  */
                  (LPSTR)szbuf,                /* a message box format  */
                  (LPSTR)"SetTextCharacterExtra", /* so that the user   */
	               MB_OK);                      /* can have a readable   */
                                               /* and useful format.    */

       sOldIntSpacing = SetTextCharacterExtra(hDC, 10);
                                               /* Set the new spacing   */
                                               /* to 10.                */

       if (sIntSpacing != sOldIntSpacing)
          {                                    /* Issue error if fails. */
          MessageBox(hWnd, (LPSTR)"SetTextCharacterExtra Failed!!!",
                     (LPSTR)"ERROR!!!",
                     MB_OK | MB_ICONEXCLAMATION);
          break;
          };
                                               /* Output confirmation.  */
       MessageBox(hWnd, (LPSTR)"Set the Intercharacter Spacing to 10.",
                  (LPSTR)"Setting = 10",
                  MB_OK);
                                               /* Get new spacing.      */
       sIntSpacing = GetTextCharacterExtra(hDC);

       sprintf(szbuf,                          
          "%s%i%s\0",                          
          "New Intercharacter Spacing = ", 
	       (int)sIntSpacing, ".");               
	    MessageBox(hWnd,                        /* Show that Set worked. */
                  (LPSTR)szbuf,                
                  (LPSTR)"SetTextCharacterExtra",
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
