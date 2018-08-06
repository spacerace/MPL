/*
 *
 * GetMenuState
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  When the user executes the "Test" menu selection, the
 *  state of the popup menu selection "Item 2", in the "GetMenuState"
 *  menu, is verified as "CHECKED" in a message box.  Otherwise, an
 *  error message is issued, also in the form of a message box.
 *
 */

#include "windows.h"
#define  IDM_TEST       100  /* ID of the Test menu.                */
#define  IDM_ITEM2      102  /* ID of Item 2.                       */
#define  IDM_GETMENUPOS   1  /* Position of the GetMenuState Popup. */
#define  SIZE_OUTBUFF    80  /* Output Buffer size.                 */

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
  HDC hDC;     /* Handle to the Display Context. */

  if (!hPrevInstance)  {

     WNDCLASS rClass;

     rClass.lpszClassName = (LPSTR)"gmnustat";
     rClass.hInstance	  = hInstance;
     rClass.lpfnWndProc   = WindowProc;
     rClass.hCursor	     = LoadCursor(NULL, IDC_ARROW);
     rClass.hIcon	        = LoadIcon(hInstance, IDI_APPLICATION);
     rClass.lpszMenuName  = (LPSTR)"TestMenu";            /* Load the menu. */
     rClass.hbrBackground = GetStockObject(WHITE_BRUSH);
     rClass.style	        = CS_HREDRAW | CS_VREDRAW;
     rClass.cbClsExtra	  = 0;
     rClass.cbWndExtra	  = 0;

     RegisterClass((LPWNDCLASS)&rClass);

     }
   else
      ;

   hInst = hInstance;

   hWnd = CreateWindow((LPSTR) "gmnustat",
		       (LPSTR) "GetMenuState",
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

   while (GetMessage((LPMSG)&msg, NULL, 0,0)) {
       TranslateMessage(&msg);
       DispatchMessage(&msg);
   } /* while */


} /* WinMain */

/* WINDOWPROC */

long FAR PASCAL WindowProc(hWnd, identifier, wParam, lParam)
HWND	hWnd;
unsigned identifier;
WORD	 wParam;
LONG	 lParam;

{
   char szoutbuf[SIZE_OUTBUFF]; /* Formatted output buffer for message Box */
   WORD wMState;                /* The WORD state of the menu selection.   */
   
   switch (identifier) {                              /*******************/
    case WM_COMMAND:                                  /* Process the     */
       switch (wParam) {                              /* menu selections */
   	  case IDM_TEST:                                /* here.           */
           wMState = GetMenuState(GetSubMenu(GetMenu(hWnd),
                                             IDM_GETMENUPOS),
                                  IDM_ITEM2,          /* Get the number  */
                                  MF_BYCOMMAND);      /* of the menu     */
                                                      /* a temp buffer.  */
           if (wMState == NULL)                       /* If the menu    */
             {                                          /* does not      */
             MessageBox(hWnd,                           /* exist, issue  */
                        (LPSTR)"Menu Does Not Exist!",  /* an error in   */
                        (LPSTR)"ERROR!!!",              /* the form of a */
                        MB_OK | MB_ICONEXCLAMATION);    /* message box.  */
             break;
             }
           else                                       /* Otherwise, put  */
             {                                        /* a more meaning- */
             if (wMState && MF_CHECKED != 0)
                {
                sprintf(szoutbuf,                        /* ful message in  */
                        "%s%s",                       /* a buffer for    */
                        "The State of GetMenuState ", /* the user to     */
                        "(Item 2) is CHECKED.");      /* recognize.      */
                MessageBox(hWnd,                      /* Then...         */
                           (LPSTR)szoutbuf,           /* show the result */
                           (LPSTR)"GetMenuState",     /* in a message    */
                           MB_OK);                    /* box.            */
                }
             else
                {
                sprintf(szoutbuf,                        
                        "%s%s",                          
                        "The State of GetMenuState ",    
                        "(Item 2) is UNCHECKED.");         
                MessageBox(hWnd,                         
                           (LPSTR)szoutbuf,              
                           (LPSTR)"GetMenuState",        
                           MB_OK);
                }
	          break;
             }
       }

    default:
	     return(DefWindowProc(hWnd, identifier, wParam, lParam));
	     break;

   }

   return(0L);

}
