/*
 *
 * GetSubMenu
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  The WinMain proceeds to execute the GetSubMenu function
 *  if the user selects the "GetSubMenu" selection from the main menu bar.
 *  If the function call is successful, a message box is created which
 *  copies the menu string into a buffer.
 *
 */

#include "windows.h"
#define  IDM_SUB      100   /* ID of the submenu.                          */
#define  N_MAX_COUNT   11   /* Maximum count of the characters of submenu. */
#define  SIZE_OUTBUFF  33   /* Size of the output buffer.                  */

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

     rClass.lpszClassName = (LPSTR)"gsubmenu";
     rClass.hInstance	  = hInstance;
     rClass.lpfnWndProc   = WindowProc;
     rClass.hCursor  	  = LoadCursor(NULL, IDC_ARROW);
     rClass.hIcon	        = LoadIcon(hInstance, IDI_APPLICATION);
     rClass.lpszMenuName  = (LPSTR)"TestSubMenu"; /* Load menu name. */
     rClass.hbrBackground = GetStockObject(WHITE_BRUSH);
     rClass.style	        = CS_HREDRAW | CS_VREDRAW;
     rClass.cbClsExtra	  = 0;
     rClass.cbWndExtra	  = 0;

     RegisterClass((LPWNDCLASS)&rClass);

     }
   else
      ;

   hInst = hInstance;

   hWnd = CreateWindow((LPSTR) "gsubmenu",
		       (LPSTR) "GetSubMenu",
		       WS_OVERLAPPEDWINDOW,           /* Use overlapped window.   */
		       CW_USEDEFAULT,                 /* Use default coordinates. */
		       CW_USEDEFAULT,                 /* Use default coordinates. */
		       CW_USEDEFAULT,                 /* Use default coordinates. */
		       CW_USEDEFAULT,                 /* Use default coordinates. */
		       (HWND)NULL,
		       LoadMenu(hInst, (LPSTR)"TestSubMenu"),  /* Load submenu.   */
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
   char szbuf[N_MAX_COUNT];        /* Buffer for submenu string.   */
   char szoutbuf[SIZE_OUTBUFF];    /* Buffer for output.           */
   short int nCopied;              /* Number of characters copied. */
   HMENU hSubMenu;                 /* Handle to the submenu.       */

   switch (identifier) {
    case WM_COMMAND:
       switch (wParam) {
   	  case IDM_SUB:              /* If the submenu is selected,         */
             hSubMenu = GetSubMenu(GetMenu(hWnd), 0); /* Get the submenu. */
             if (hSubMenu == NULL)
                {                  /* If the submenu is invalid,          */
                MessageBox(hWnd,   /*  display a error message box.       */
                           (LPSTR)"No Popup Menu Exists Here!",
                           (LPSTR)"ERROR!!!",
                           MB_OK | MB_ICONEXCLAMATION);
                break;             /* And stop here.                      */
                };                 /* Otherwise, get the menu string by position. */
             nCopied = GetMenuString(hSubMenu, 
                                     0,
                                     (LPSTR)szbuf,
                                     N_MAX_COUNT,
                                     MF_BYPOSITION);
             if (nCopied == 0)   /* and issue an error message if fails.   */
                {
                MessageBox(hWnd,
                           (LPSTR)"GetMenuString Failed!",
                           (LPSTR)"MF_BYPOSITION",
                           MB_OK | MB_ICONEXCLAMATION);
                break;
                }
             else       /* Otherwise, display success through message box. */
                {
                sprintf(szoutbuf,
                        "%s%s%s",
                        "The SubMenu is '",
                        szbuf,
                        "'");
                MessageBox(hWnd,
                           (LPSTR)szoutbuf,
                           (LPSTR)"GetSubMenu",
                           MB_OK);
                };
	          break;
       }

    default:
	     return(DefWindowProc(hWnd, identifier, wParam, lParam));
	     break;

   }

   return(0L);

}
