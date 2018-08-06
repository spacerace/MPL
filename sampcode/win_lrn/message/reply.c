/*

Description:   This function specifies whether the current window function
    is processing a message that is passed to it through a call to the 
    SendMessage function.

*/


#include <windows.h>
#include "Reply.h"
#include "string.h"
#include "stdio.h"

       char         szAppName             [] = "Reply"             ;
       HANDLE       hInstMain                                      ;
       HWND         hWndMain                                       ;
       char         szOutputBuffer1       [70]                     ;
       char         szOutputBuffer2       [600]                    ;

/****************************************************************************/
/************************    Message Structure      *************************/
/****************************************************************************/

struct { char *szMessage; }
       Messages [] = {
"About\0",
"     This is a sample application to demonstrate the\n\
use  of  the  ReplyMessage Windows function.  ",

"Help Message",
"     Sample program to illustrate the use of ReplyMessage.\n\
Use the WINSPAWN program to spawn this program in one of\n\
two ways.  The first way, 'Reply Using ReplyMessage',\n\
will capture the input focus using the ReplyMessage\n\
Windows Function.   The second way, 'Reply Regular Spawn',\n\
will not capture the focus.",

"In Reply",
"     Message sent and received from within Reply.\n\
The Reply program has successfully captured the input\n\
focus."

};	
/****************************************************************************/

void ProcessMessage (HWND, int); 

void ProcessMessage (hWnd, MessageNumber) 
     HWND     hWnd;
     int      MessageNumber;
{
     sprintf (szOutputBuffer1, "%s", Messages [MessageNumber]);
     sprintf (szOutputBuffer2, "%s", Messages [MessageNumber + 1]);
     MessageBox (hWnd, szOutputBuffer2, szOutputBuffer1, MB_OK);
}       

/****************************************************************************/

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
    HANDLE	hInstance, hPrevInstance;
    LPSTR	lpszCmdLine;
    int 	nCmdShow;
    {
    HWND	hWnd;
    MSG 	msg;
    WNDCLASS	wndclass;
    HMENU	hMenu;
    short   xScreen, yScreen;


    if (!hPrevInstance)
	{
	wndclass.style	         = CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS;
	wndclass.lpfnWndProc    = WndProc;
	wndclass.cbClsExtra     = 0;
	wndclass.cbWndExtra     = 0;
	wndclass.hInstance      = hInstance;
	wndclass.hIcon	         = NULL; 
   wndclass.hCursor        = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground	= GetStockObject (WHITE_BRUSH);
	wndclass.lpszMenuName	= szAppName;
	wndclass.lpszClassName	= szAppName;

    if (!RegisterClass (&wndclass) )
        return FALSE;
    }

	 xScreen = GetSystemMetrics (SM_CXSCREEN);
    yScreen = GetSystemMetrics (SM_CYSCREEN);

    hWnd = CreateWindow (szAppName, "Reply",
           WS_OVERLAPPEDWINDOW, xScreen / 7, yScreen / 58,
           xScreen * 3 / 4, yScreen * 49 / 50, NULL, NULL,
           hInstance, NULL);

    hInstMain = hInstance;
    hWndMain  = hWnd;

    ShowWindow (hWnd, nCmdShow);
    UpdateWindow (hWnd);

    while (GetMessage (&msg, NULL, 0, 0))
        {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
        }

    return msg.wParam;
}
/****************************************************************************/

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
   HWND      hWnd;
   unsigned  iMessage;
   WORD      wParam;
   LONG      lParam;
   {
   HMENU         hMenu;
   int           Index;
   HANDLE        hDC;
   HDC           hMemoryDC;
   BITMAP        Bitmap;
	short         foo;
   BOOL          InSend;

   switch (iMessage)
       {
       case WM_CREATE:
            hMenu = GetSystemMenu (hWnd, FALSE);
     
            ChangeMenu (hMenu, NULL, "&About", IDM_ABOUT, 
                        MF_APPEND | MF_STRING);
            break;

       case WM_SIZE:
            break;

       case WM_SYSCOMMAND:
            switch (wParam) {
               case IDM_ABOUT:
                    ProcessMessage (hWnd, 0);
                    break;
               default:
                    return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
            }
       break;


       case WM_COMMAND:
            switch (wParam) {
              case IDM_RECEIVE:
                   ProcessMessage (hWnd, 4);
                   break;

              case IDM_SEND:
                   ReplyMessage (1L);
                   SendMessage (hWnd, WM_COMMAND, IDM_RECEIVE, 0L);
                   break;

              case IDM_REGULAR:
                   break;

              case IDM_HELP:
                   ProcessMessage (hWnd, 2);
                   break;
	         }
            break;

       case WM_DESTROY:
            PostQuitMessage (0);
            break;

	    default:
            return DefWindowProc( hWnd, iMessage, wParam, lParam );
       }
	return 0L;
}

/****************************************************************************/

