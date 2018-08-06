/*

Function(s) demonstrated in this program: SetWindowLong

Windows version:  2.03

Windows SDK version:  2.00

Compiler version:  C 5.10

Description:   This function replaces the long value in the window class
   structure corresponding to the window handle parameter, with the 
   respective new value.  

Additional Comments:  This program requests 4 extra bytes when it registers
   its window structure.  These 4 extra bytes are used to store a long
   value corresponding to the number of left mouse button clicks.
   This value is updated using GetWindowLong and SetWindowLong.
   

*/

#define NOMINMAX	
#include <stdlib.h>
#include <windows.h>
#include "SetWl.h"
#include "string.h"
#include "stdio.h"

       char         szAppName             [] = "SetWl"             ;
       HANDLE       hInstMain                                      ;
       HWND         hWndMain                                       ;
       char         szOutputBuffer1       [70]                     ;
       char         szOutputBuffer2       [500]                    ;
       HBITMAP      hBitmapHelp                                    ;


/****************************************************************************/
/************************    Message Structure      *************************/
/****************************************************************************/

struct { char *szMessage; }
       Messages [] = {
"About",
"     This is a sample application to demonstrate the\n\
use  of  the  SetWindowLong  Windows function.",

"Help Message",
"     This program uses GetWindowLong and\n\
SetWindowLong to keep track of the number of left\n\
button mouse click message.  A message box shows\n\
how many every time one is sent.  Click the left\n\
mouse button to test this function.",

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
    LONG    WindowCount;


    if (!hPrevInstance)
	{
	wndclass.style	         = CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS;
	wndclass.lpfnWndProc    = WndProc;
	wndclass.cbClsExtra     = 0;
	wndclass.cbWndExtra     = 4;
	wndclass.hInstance      = hInstance;
	wndclass.hIcon	         = NULL; 
   wndclass.hCursor        = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground	= GetStockObject (WHITE_BRUSH);
	wndclass.lpszMenuName	= szAppName;
	wndclass.lpszClassName	= szAppName;

    if (!RegisterClass (&wndclass) )
        return FALSE;
    }

    hWnd = CreateWindow (szAppName, "SetWindowLong",
           WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
           NULL, NULL, hInstance, NULL);
       
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
   HMENU     hMenu;

   switch (iMessage)
       {
       case WM_CREATE:
            hMenu = GetSystemMenu (hWnd, FALSE);

            ChangeMenu (hMenu, NULL, "&About", IDM_ABOUT, 
                        MF_APPEND | MF_STRING);
            break;

       case WM_LBUTTONUP:
            SetWindowLong (hWnd, 0, GetWindowLong (hWnd, 0) + 1);
            sprintf (szOutputBuffer1, 
                     "Number of left mouse button double clicks = %i",
                      GetWindowLong (hWnd, 0));
            MessageBox (hWnd, szOutputBuffer1, "SetWindowLong", MB_OK);
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

