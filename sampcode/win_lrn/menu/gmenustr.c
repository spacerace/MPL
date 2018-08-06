/*
 *
 * GetMenuString
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  The WinMain proceeds to execute the GetMenuString function
 *  if the user selects the "GetMenuString" selection from the main menu bar.
 *  If the function call is successful, a message box is created which
 *  copies the menu string into a buffer.  The function is duplicated for
 *  use by position of the menu selection and by the resource ID.
 *
 */

#include "windows.h"
#define  IDM_TEST      100     /* ID of the Test menu.    */
#define  N_MAX_COUNT   14      /* Temporary buffer count. */
#define  SIZE_OUTBUFF  36      /* Output Buffer size.     */

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

     rClass.lpszClassName = (LPSTR)"gmenustr";
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

   hWnd = CreateWindow((LPSTR) "gmenustr",
		       (LPSTR) "GetMenuString",
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
   char szbuf[N_MAX_COUNT];     /* Temp Buffer, stores results of function */
   char szoutbuf[SIZE_OUTBUFF]; /* Formatted output buffer for message Box */
   short int nCopied;           /* Number of characters copied, error chk  */
   
   switch (identifier) {                              /*******************/
    case WM_COMMAND:                                  /* Process the     */
       switch (wParam) {                              /* menu selections */
   	  case IDM_TEST:                                /* here.           */
           nCopied = GetMenuString(GetMenu(hWnd),     /* Get the string  */
                                   wParam,            /* of the menu and */
                                   (LPSTR) szbuf,     /* put it into a   */
                                   N_MAX_COUNT,       /* temporary buf-  */
                                   MF_BYCOMMAND);     /* fer by command. */
           if (nCopied == 0)                            /* If no charac- */
             {                                          /* ters were     */
             MessageBox(hWnd,                           /* copied, issue */
                        (LPSTR)"GetMenuString Failed!", /* an error in   */
                        (LPSTR)"ERROR!!!",              /* the form of a */
                        MB_OK | MB_ICONEXCLAMATION);    /* message box.  */
             break;
             }
           else                                       /* Otherwise, put  */
             {                                        /* a more meaning- */
             sprintf(szoutbuf,                        /* ful message in  */
                     "%s%s%s",                        /* a buffer for    */
                     "The Menu String is '",          /* the user to     */
                     szbuf,                           /* recognize.      */
                     "'");                            /* Then...         */
             MessageBox(hWnd,                         /* show the result */
                        (LPSTR)szoutbuf,              /* in a message    */
                        (LPSTR)"MF_BYCOMMAND",        /* box.            */
                        MB_OK);
             nCopied = GetMenuString(GetMenu(hWnd),   /* Now, get the    */
                                     0,               /* same string     */
                                     (LPSTR)szbuf,    /* from the same   */
                                     N_MAX_COUNT,     /* menu by using   */
                                     MF_BYPOSITION);  /* the position of */
             if (nCopied == 0)                        /* the menu.       */
                {                                     /* If no charac-   */
                MessageBox(hWnd,                      /* ters were       */
                           (LPSTR)"GetMenuString Failed!", /* found,     */
                           (LPSTR)"MF_BYPOSITION",         /* issue an   */
                           MB_OK | MB_ICONEXCLAMATION);    /* error in a */
                break;                                /* message box.    */
                }
             else                                     /* Finally,        */
                {                                     /* display the     */
                sprintf(szoutbuf,                     /* results of the  */
                        "%s%s%s",                     /* GetMenuString   */
                        "The Menu String is '",       /* using position  */
                        szbuf,                        /* rather than ID. */
                        "'");                         /* But put it in a */
                MessageBox(hWnd,                      /* form which is   */
                           (LPSTR)szoutbuf,           /* more pleasing   */
                           (LPSTR)"MF_BYPOSITION",    /* to the user.    */
                           MB_OK);                    /*******************/
                };
	          break;
             }
       }

    default:
	     return(DefWindowProc(hWnd, identifier, wParam, lParam));
	     break;

   }

   return(0L);

}
