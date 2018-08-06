/*

Function(s) demonstrated in this program: GetStretchBltMode
Description:   This function retrieves the current stretching mode.  The
   stretching mode defines how information is to be added or removed from
   bitmaps that are stretched or compressed by using the StretchBlt function.

Additional Comments:  Possible modes are BLACKONWHITE, WHITEONBLACK, 
   and COLORONCOLOR with values 1, 2, and 3 respectively.
*/


#include <windows.h>
#include "GetBitSM.h"
#include <string.h>
#include <stdio.h>

       char	    szAppName[] = "GetBitSM";
       HANDLE	    hInstMain;
       HWND	    hWndMain;
       char	    szOutputBuffer1[70];
       char	    szOutputBuffer2[500];
       HBITMAP	    hBitmapHelp;

struct { char *szMessage; }
       Messages [] = { "Help Message",
       "     Sample program to illustrate the use of GetStretchBltMode.\n\
       Choose the option to see the current stretch mode." };


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
    HBITMAP	hBitmapCirc, hBitmapDiam, hBitmapSqua, hBitmapXs, hBitmapTria,
		hBitmapPlus;
    HMENU	hMenu;
    short	xScreen, yScreen;

    if (!hPrevInstance)
	{
	wndclass.style		= CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS;
	wndclass.lpfnWndProc    = WndProc;
	wndclass.cbClsExtra     = 0;
	wndclass.cbWndExtra     = 0;
	wndclass.hInstance      = hInstance;
	wndclass.hIcon		= NULL;
	wndclass.hCursor	= LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground	= GetStockObject (WHITE_BRUSH);
	wndclass.lpszMenuName	= szAppName;
	wndclass.lpszClassName	= szAppName;

	if (!RegisterClass (&wndclass) )
	    return FALSE;
	}

    xScreen = GetSystemMetrics (SM_CXSCREEN);
    yScreen = GetSystemMetrics (SM_CYSCREEN);

    hWnd = CreateWindow (szAppName, "Help Window",
	   WS_OVERLAPPEDWINDOW, xScreen/7, yScreen/58,
	   xScreen*3/4, yScreen*49/50, NULL, NULL, hInstance, NULL);

    hInstMain = hInstance;
    hWndMain  = hWnd;

    ShowWindow (hWnd, nCmdShow);
    UpdateWindow (hWnd);

    while (GetMessage (&msg, NULL, 0, 0))
        {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
        }

    DeleteObject (hBitmapHelp);
    return msg.wParam;
    }
/****************************************************************************/

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
   HWND      hWnd;
   unsigned  iMessage;
   WORD      wParam;
   LONG      lParam;
   {
   int           Index;
   HANDLE        hDC;
   HDC           hMemoryDC;
   BITMAP        Bitmap;
   short	 foo;

   switch (iMessage)
       {
       case WM_SIZE:
	    hDC = GetDC(hWnd);
            hMemoryDC = CreateCompatibleDC(hDC);
            hBitmapHelp = LoadBitmap (hInstMain, "BitmapHelp");
            GetObject(hBitmapHelp, 16, (LPSTR) &Bitmap);
            SelectObject(hMemoryDC, hBitmapHelp);
            foo = GetStretchBltMode (hDC);

            SetStretchBltMode(hDC, BLACKONWHITE);
            StretchBlt(hDC, 0, 0, LOWORD (lParam), HIWORD (lParam),
				hMemoryDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, SRCCOPY);

            SetStretchBltMode(hDC, foo);
	    DeleteObject (hBitmapHelp);
	    DeleteDC(hMemoryDC);
	    ReleaseDC(hWnd, hDC);
	    break;

       case WM_COMMAND:
            switch (wParam) {
              case IDM_GETMODE:
                   hDC = GetDC(hWnd);
                   hMemoryDC = CreateCompatibleDC(hDC);
                   hBitmapHelp = LoadBitmap (hInstMain, "BitmapHelp");
                   GetObject(hBitmapHelp, 16, (LPSTR) &Bitmap);
                   SelectObject(hMemoryDC, hBitmapHelp);
                   foo = GetStretchBltMode (hDC);

                   sprintf (szOutputBuffer1, "Stretch Mode = %x, BLACKONWHITE",
                            foo);
                   MessageBox (hWnd, szOutputBuffer1, "GetStretchBltMode", 
                               MB_OK);

                   SetStretchBltMode(hDC, BLACKONWHITE);
                   StretchBlt(hDC, 0, 0, LOWORD (lParam), HIWORD (lParam),
                   hMemoryDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, SRCCOPY);
                   SetStretchBltMode(hDC, foo);
                   DeleteObject (hBitmapHelp);
                   DeleteDC(hMemoryDC);
                   ReleaseDC(hWnd, hDC);
                   break;

              case IDM_HELP:
                   ProcessMessage (hWnd, 0);
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


