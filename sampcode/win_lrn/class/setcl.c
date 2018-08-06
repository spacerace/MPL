/*

Function(s) demonstrated in this program: SetClassLong

Windows version:  2.03

Windows SDK version:  2.00

Compiler version:  C 5.10

Description:   This function replaces the long value in the window class
   structure corresponding to the window handle parameter, with the 
   respective new value.  

Additional Comments:  This program requests 4 extra bytes when it registers
   its class structure.  These 4 extra bytes are used to store a long
   value corresponding to the number of instances of windows in this class.
   This value is updated using GetClassLong and SetClassLong and an upper 
   bound of 3 windows of this class is enforced.  
   

*/

#define NOMINMAX	
#include <stdlib.h>
#include <windows.h>
#include "SetCl.h"
#include "string.h"
#include "stdio.h"

       char         szAppName             [] = "SetCl"             ;
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
use  of  the  SetClassLong  Windows function.",

"Help Message",
"     This program uses GetClassLong and\n\
SetClassLong to keep track of the number of windows\n\
of this class.  In order to do this four bytes must\n\
be reserved in the class structure using the\n\
cbClsExtra field in the class the first time it is\n\
initialized.  To test this program try to run more\n\
than three instances.  When trying to run a fourth\n\
instance, the program will state that the limit has\n\
been reached and automatically terminate.",

"Warning",
"     Only Three windows of this class are allowed\n\
at any one time.",

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
	wndclass.cbClsExtra     = 4;
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

    hWnd = CreateWindow (szAppName, "SetClassLong Main",
           WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
           NULL, NULL, hInstance, NULL);
       
    ShowWindow (hWnd, nCmdShow);
    UpdateWindow (hWnd);

    WindowCount = GetClassLong (hWnd, 0);
    sprintf (szOutputBuffer1, 
             "     Number of windows with this class currently running = %i.",
             WindowCount + 1);
    MessageBox (hWnd, szOutputBuffer1, "SetClassLong", MB_OK);

    if (WindowCount < MAXWINDOWS) {
       WindowCount += 1;
       SetClassLong (hWnd, 0, WindowCount);
    }
    else {
       ProcessMessage (hWnd, 4);
       SendMessage (hWnd, WM_SYSCOMMAND, SC_CLOSE, 0L);
    }

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