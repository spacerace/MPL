/*
 *
 * GetTextMetrics
 *
 * This program registers a window and creates it on the screen.  The
 *  program then creates the window, shows the window, and then updates
 *  the window.  If the user couble clicks the right mouse button, the
 *  GetTextMetrics function is executed with the HDC of the top level
 *  window and the current font.  The width and height are returned in a
 *  message box.
 *
 */

#include "windows.h"

/* Global Variables */
static HANDLE hInst;         /* Handle to the Instance.            */
static HWND hWnd;            /* Handle to the Window.              */

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

     rClass.lpszClassName = (LPSTR)"gtxtmtrx";
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

   hWnd = CreateWindow((LPSTR) "gtxtmtrx",
		       (LPSTR) "GetTextMetrics",        /* Create a window.         */
		       WS_OVERLAPPEDWINDOW,            /* Make it overlapped.      */
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
                (LPSTR)"Double Click Right Mouse Button To Conduct Test.",
                48, (LPRECT)&rRect, DT_SINGLELINE);  /* Prompt text.    */
	     EndPaint(hWnd, (LPPAINTSTRUCT)&ps);          

	  }
  	  break;
	
     case WM_RBUTTONDBLCLK:                    /* If the user double   */
       {                                       /* clicks on the right  */
       char szbuf[80];   /* Output buffer.      | mouse button, then   */
       TEXTMETRIC *strTM; /* Extents - test text.| establish needed    */
       BOOL bGotem;      /* Boolean success flg | variables and call   */
       HDC hDC;      /* Display Context Handle. | the GetTextMetrics   */
       HANDLE hLMem;     /* Local Memory for the Text Metrics.         */

          /* Allocate the local memory for the Text Metrics. */

       hLMem = LocalAlloc(LMEM_DISCARDABLE | LMEM_MOVEABLE,
                          sizeof(TEXTMETRIC));
       if (hLMem == NULL)   /* If error, then complain. */
          {
          MessageBox(hWnd, (LPSTR)"Could Not allocate Local Memory!!!",
                     (LPSTR)"ERROR!!!", MB_OK | MB_ICONEXCLAMATION);
          break;
          };

          /* Lock the local memory. */

       strTM = (TEXTMETRIC *)LocalLock(hLMem);

       if (strTM == NULL)   /* If error, then complain. */
          {
          MessageBox(hWnd, (LPSTR)"Could Not lock Local Memory!!!",
                     (LPSTR)"ERROR!!!", MB_OK | MB_ICONEXCLAMATION);
          break;
          };

       hDC = GetDC(hWnd);                      /* (after getting DC)   */

          /* Get the Text Metrics of the current Font. */
       
       bGotem = GetTextMetrics(hDC, (LPTEXTMETRIC)strTM);

       if (bGotem == NULL)   /* If the GetTextMetrics fails, then complain. */
          {
          MessageBox(hWnd,
             (LPSTR)"Did Not Get The Text Metrics!!!",
             (LPSTR)"ERROR!!!", MB_OK | MB_ICONEXCLAMATION);
          break;
          }
       else
          {
	       sprintf(szbuf,                       /* Otherwise, capture   */
                  "%s%s%i%s%i%s\0",            /* the Ext information  */
                  "The Text Metrics ",         /* in a zero terminated */
                  "<height,weight> are (",     /* buffer.             */   
	               (int)strTM->tmHeight, ",",    
                  (int)strTM->tmWeight, ").");

	       MessageBox(hWnd,                     /* Output the buffer in */
                     (LPSTR)szbuf,             /* a message box format */
                     (LPSTR)"GetTextMetrics",  /* so that the user can */
	                  MB_OK);                   /* have a readable and  */
          };                                   /* useful format.       */
       LocalUnlock(hLMem);  /* Unlock the local memory. */
       LocalFree(hLMem);    /* Free the local memory.   */
       };
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

