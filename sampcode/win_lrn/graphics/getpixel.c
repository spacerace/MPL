/*

Function(s) demonstrated in this program: ChildWindowFromPoint

Windows version:  2.03

Windows SDK version:  2.00

Compiler version:  C 5.10

Description:  This function retrieves the RGB color value of the pixel at
   the point specified by the X and Y parameters.  If the point is not in
   the clipping region, the function is ignored.

Additional Comments:  This program also illustrates the subclassing of 
    windows.

*/

#define NOMINMAX
#include <stdlib.h>
#include <windows.h>
#include "GetPixel.h"
#include "string.h"
#include "stdio.h"

       char         szAppName             [] = "GetPixel"          ;
       HANDLE       hInstMain                                      ;
       HWND         hWndMain                                       ;
       char         szOutputBuffer1       [70]                     ;
       char         szOutputBuffer2       [500]                    ;
       HBITMAP      hBitmapHelp                                    ;

       HWND         hWndChild1, hWndChild2, hX, hY, hOK2;
       FARPROC      lpProcEnterPoint;
       FARPROC      lpProcNewEnterPoint;
       FARPROC      lpProcOldX;
       FARPROC      lpProcOldY;
       char         szXValue [40];
       char         szYValue [40];


/****************************************************************************/
/************************    Message Structure      *************************/
/****************************************************************************/

struct { char *szMessage; }
       Messages [] = {
"About",
"     Sample Application to demonstrate the use of\n\
the GetPixel Windows function.",

"Help Message",
"     Select a pixel, and the program will use the\n\
GetPixel Windows function to display the attributes\n\
of that pixel; ffff = White, 0 = Black.  Select a\n\
pixel by clicking the left mouse button, entering\n\
the pixel coordinates through the keyboard, or let\n\
the computer choose random pixel coordinates."
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

    hWnd = CreateWindow (szAppName, "GetPixel",
           WS_OVERLAPPEDWINDOW, xScreen / 7, yScreen / 58,
           xScreen * 3 / 4, yScreen * 49 / 50, NULL, NULL,
           hInstance, NULL);

    hInstMain = hInstance;
    hWndMain  = hWnd;

    ShowWindow (hWnd, nCmdShow);
    UpdateWindow (hWnd);

    lpProcEnterPoint    = MakeProcInstance (EnterPointDlgProc, hInstance);
    lpProcNewEnterPoint = MakeProcInstance (NewEnterPointDlgProc,hInstance);


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
	short         foo;

   HMENU         hMenu;
   PAINTSTRUCT   ps;
   POINT         pt;
   static int    xClient, yClient;
   static int    xDevisor, yDevisor, randNum;
   DWORD         Attribute;


   switch (iMessage)
       {
       case WM_CREATE:
            hMenu = GetSystemMenu (hWnd, FALSE);

            ChangeMenu (hMenu, NULL, "&About", IDM_ABOUT, 
                        MF_APPEND | MF_STRING);
            break;

       case WM_SIZE:
            xClient = LOWORD (lParam);
            yClient = HIWORD (lParam);

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

       case WM_LBUTTONDOWN:
            pt = MAKEPOINT (lParam) ;
            hDC = GetDC (hWnd);
            Attribute = GetPixel (hDC, pt.x, pt.y);
            sprintf (szOutputBuffer1, "The Point [%i, %i]", pt.x, pt.y);
            sprintf (szOutputBuffer2, "Has Attribute %x", Attribute);
            MessageBox (hWnd, szOutputBuffer2, szOutputBuffer1,MB_OK);
            ReleaseDC(hWnd, hDC);
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
               case IDM_RANDOM:
                    pt.x = pt.y = -1;
                    while (((pt.x < 0) || (pt.x > xClient)) || ((pt.y < 0) || 
                            (pt.y > yClient))) {
                       pt.x = (rand () / 47);
                       pt.y = (rand () / 102);
                    }
                    hDC = GetDC (hWnd);
                    Attribute = GetPixel (hDC, pt.x, pt.y);
                    sprintf (szOutputBuffer1, "The Point [%i, %i]", pt.x, pt.y);
                    sprintf (szOutputBuffer2, "Has Attribute %x", Attribute);
                    MessageBox (hWnd, szOutputBuffer2, szOutputBuffer1,MB_OK);
                    ReleaseDC(hWnd, hDC);
                    break;

               case IDM_ENTER:
                    pt.x = pt.y = -1;
                    while (((pt.x < 0) || (pt.x > xClient)) || ((pt.y < 0) || 
                            (pt.y > yClient))) {
                      DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                             lpProcEnterPoint);
                      pt.x = atoi (szXValue);
                      pt.y = atoi (szYValue);                      
                    }					  
                    hDC = GetDC (hWnd);
                    Attribute = GetPixel (hDC, pt.x, pt.y);
                    sprintf (szOutputBuffer1, "The Point [%i, %i]", pt.x, pt.y);
                    sprintf (szOutputBuffer2, "Has Attribute %x", Attribute);
                    MessageBox (hWnd, szOutputBuffer2, szOutputBuffer1,MB_OK);
                    ReleaseDC(hWnd, hDC);
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

BOOL FAR PASCAL EnterPointDlgProc (hDlg, iMessage, wParam, lParam)
HWND hDlg;
unsigned iMessage;
WORD wParam;
LONG lParam;
{
     int Index;
     char szChange [10];
     long lReturn;

     switch (iMessage) {
     case WM_INITDIALOG:
       SendDlgItemMessage (hDlg, IDD_X, EM_LIMITTEXT,
                           (WORD)40, 0L);
       SendDlgItemMessage (hDlg, IDD_Y, EM_LIMITTEXT,
                           (WORD)40, 0L);

       hX = GetDlgItem (hDlg, IDD_X);
         lpProcOldX = (FARPROC) GetWindowLong (hX, GWL_WNDPROC);
         SetWindowLong (hX, GWL_WNDPROC, (LONG) lpProcNewEnterPoint);
         SendMessage (hX, EM_SETSEL, 0, MAKELONG (0,32767));
       hY = GetDlgItem (hDlg, IDD_Y);
         lpProcOldY = (FARPROC) GetWindowLong (hY, GWL_WNDPROC);
         SetWindowLong (hY, GWL_WNDPROC, (LONG) lpProcNewEnterPoint);
         SendMessage (hY, EM_SETSEL, 0, MAKELONG (0,32767));
       hOK2 = GetDlgItem (hDlg, IDOK);
     
       return TRUE;
       break;

     case WM_COMMAND:
       switch (wParam) {
         case IDD_X:
              break;

         case IDD_Y:
              break;
		 
         case IDOK:            
              GetDlgItemText (hDlg, IDD_X, szXValue, 10) ;
              GetDlgItemText (hDlg, IDD_Y, szYValue, 10) ;
              EndDialog (hDlg, TRUE);
              break;

         default:
              return FALSE;
      }
    default:
      return FALSE;
  }
  return TRUE;
}

/****************************************************************************/

BOOL FAR PASCAL NewEnterPointDlgProc  (hWnd, iMessage, wParam, lParam) 
     HWND     hWnd;
     unsigned iMessage; 
     WORD     wParam;
     LONG     lParam;
{
     switch (iMessage) {
       case WM_GETDLGCODE:
            return (DLGC_WANTALLKEYS);

       case WM_CHAR:
            if ((wParam == VK_TAB) || (wParam == VK_RETURN)) {
                SendMessage (hWndMain, WM_USER, 0, 0L);
                SetFocus (FindNextWindow (hWnd));
                return TRUE;
            }
            else {
              if (hWnd == hX) 
                return ((BOOL)CallWindowProc (lpProcOldX, hWnd, 
                        iMessage, wParam, lParam));
              if (hWnd == hY) 
                return ((BOOL)CallWindowProc (lpProcOldY, hWnd, 
                        iMessage, wParam, lParam));
            }
            break;

       default:
         if (hWnd == hX)
            return ((BOOL)CallWindowProc (lpProcOldX, hWnd,
                    iMessage, wParam, lParam));
         if (hWnd == hY)
            return ((BOOL)CallWindowProc (lpProcOldY, hWnd, 
                    iMessage, wParam, lParam));
     }
}

/****************************************************************************/

HWND FindNextWindow (hWnd)
     HWND   hWnd;
{
     if (hWnd == hX)      return hY;
     if (hWnd == hY)      return hOK2;
     return NULL;
}

