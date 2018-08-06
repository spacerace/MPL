/*

Function(s) demonstrated in this program: SetRectEmpty, UnionRect, OffsetRect

Description:  The SetRectEmpty sets all the fields of a rectangle structure
    to 0.  The UnionRect function takes two rectangle structures, and 
    places in a third rectangle structure the coordinates of the smallest
    rectangle which contains both of the other rectangles.  The OffsetRect
    function adjusts the coordinates of the rectangle structrure by the 
    offsets supplied.

*/

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "Rect.h"

#define PI 3.14159265


char    szRadius  [15];
HANDLE  hInstMain;
FARPROC lpProcGetRadius;
char     szOutputBuffer1 [70];
char     szOutputBuffer2 [500];
HWND     hWndMain, hX, hY, hOK2;
FARPROC  lpProcEnterPoint;
FARPROC  lpProcNewEnterPoint;
FARPROC  lpProcOldX;
FARPROC  lpProcOldY;
char     szXValue [40];
char     szYValue [40];


/****************************************************************************/
/************************    Message Structure      *************************/
/****************************************************************************/

struct { char *szMessage; }
       Messages [] = {
"About\0",
"     This is a sample application to demonstrate the\n\
use of the SetRectEmpty, UnionRect, and OffsetRect\n\
Windows functions.",

"Help Message",
"     This program uses the Windows functions\n\
SetRectEmpty, UnionRect, and OffsetRect.  There are\n\
three rectangles displayed in the window.  Two of\n\
these windows are adjustable, and the third is\n\
a union of the first two.  Use the menu to\n\
adjust the size and position of the two rectangles\n\
in pixels.",

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
    static char szAppName[] = "Rect";
    HWND	hWnd;
    MSG 	msg;
    WNDCLASS	wndclass;

    if (!hPrevInstance)	{
        wndclass.style          = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc    = WndProc;
        wndclass.cbClsExtra     = 0;
        wndclass.cbWndExtra     = 0;
        wndclass.hInstance      = hInstance;
        wndclass.hIcon          = NULL; /*LoadIcon (NULL, IDI_ASTERISK);*/
        wndclass.hCursor        = LoadCursor (NULL, IDC_CROSS);
        wndclass.hbrBackground  = GetStockObject (WHITE_BRUSH);
        wndclass.lpszMenuName   = szAppName;
        wndclass.lpszClassName  = szAppName;

        if (!RegisterClass (&wndclass) )
            return FALSE;
    }


    hWnd = CreateWindow (szAppName, "Rect",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
			CW_USEDEFAULT, 0, NULL, NULL,
			hInstance, NULL);

    hInstMain = hInstance;
    hWndMain  = hWnd;

    ShowWindow (hWnd, nCmdShow);
    UpdateWindow (hWnd);

    lpProcEnterPoint    = MakeProcInstance (EnterPointDlgProc, hInstance);
    lpProcNewEnterPoint = MakeProcInstance (NewEnterPointDlgProc,hInstance);

    while (GetMessage (&msg, NULL, 0, 0))	{
       TranslateMessage (&msg);
       DispatchMessage (&msg);
    }
    return msg.wParam;
}

/****************************************************************************/

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
	HWND	    hWnd;
	unsigned    iMessage;
	WORD	    wParam;
	LONG	    lParam;
	{
   HMENU         hMenu;
static   RECT    Rect1, Rect2, Rect3;
	static short  xClient, yClient;
	HDC	        hDC;
	PAINTSTRUCT   ps;
   int           xOffset, yOffset;

	switch (iMessage) {
       case WM_CREATE:
            hMenu = GetSystemMenu (hWnd, FALSE);
     
            ChangeMenu (hMenu, NULL, "&About", IDM_ABOUT, 
                        MF_APPEND | MF_STRING);
            SetRectEmpty (&Rect1);
            SetRectEmpty (&Rect2);
            SetRectEmpty (&Rect3);

            Rect1.top = 50;
            Rect1.left = 150;
            Rect1.right = 250;
            Rect1.bottom = 110;

            Rect2.top = 160;
            Rect2.left = 400;
            Rect2.right = 500;
            Rect2.bottom = 220;
            UnionRect (&Rect3, &Rect2, &Rect1);
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

	    case WM_SIZE:
            xClient=LOWORD (lParam);
            yClient=HIWORD (lParam);
            break;


       case WM_COMMAND:
            switch (wParam) {
               case IDM_CHANGERECT1:
                    SetRectEmpty (&Rect1);
                    MessageBox (hWnd, "Rectangle 1 has been emptied", 
                                "SetRectEmpty", MB_OK);
                    sprintf (szYValue, "Enter Coordinates");
                    sprintf (szXValue, "rect1.top,  rect1.left");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    Rect1.top  = atoi (szXValue);
                    Rect1.left = atoi (szYValue);                      

                    sprintf (szYValue, "Enter Coordinates");
                    sprintf (szXValue, "rect1.bottom, rect1.right");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    Rect1.bottom = atoi (szXValue);
                    Rect1.right  = atoi (szYValue);                      

                    UnionRect (&Rect3, &Rect2, &Rect1);

                    InvalidateRect (hWnd, NULL, TRUE);
                    UpdateWindow (hWnd);
                    break;

               case IDM_CHANGERECT2:
                    SetRectEmpty (&Rect2);
                    MessageBox (hWnd, "Rectangle 2 has been emptied", 
                                "SetRectEmpty", MB_OK);
                    sprintf (szYValue, "Enter Coordinates");
                    sprintf (szXValue, "rect2.top,  rect2.left");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    Rect2.top  = atoi (szXValue);
                    Rect2.left = atoi (szYValue);                      

                    sprintf (szYValue, "Enter Coordinates");
                    sprintf (szXValue, "rect2.bottom, rect2.right");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    Rect2.bottom = atoi (szXValue);
                    Rect2.right  = atoi (szYValue);                      

                    UnionRect (&Rect3, &Rect2, &Rect1);

                    InvalidateRect (hWnd, NULL, TRUE);
                    UpdateWindow (hWnd);
                    break;

               case IDM_OFFSETRECT1:
                    sprintf (szYValue, "Enter Offsets");
                    sprintf (szXValue, "X Offset, Y Offset");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    xOffset = atoi (szXValue);
                    yOffset = atoi (szYValue);                      

                    OffsetRect (&Rect1, xOffset, yOffset);
                    UnionRect (&Rect3, &Rect2, &Rect1);

                    InvalidateRect (hWnd, NULL, TRUE);
                    UpdateWindow (hWnd);
                    break;

               case IDM_OFFSETRECT2:
                    sprintf (szYValue, "Enter Offsets");
                    sprintf (szXValue, "X Offset, Y Offset");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    xOffset = atoi (szXValue);
                    yOffset = atoi (szYValue);                      

                    OffsetRect (&Rect2, xOffset, yOffset);
                    UnionRect (&Rect3, &Rect2, &Rect1);

                    InvalidateRect (hWnd, NULL, TRUE);
                    UpdateWindow (hWnd);
                    break;

               case IDM_HELP:
                    ProcessMessage (hWnd, 2);
                    break;
            }
            break;

	    case WM_PAINT:
            hDC=BeginPaint(hWnd, &ps);
            DrawSquare (hDC, Rect1);
            DrawSquare (hDC, Rect2);
            DrawSquare (hDC, Rect3);

            EndPaint (hWnd, &ps);
            break;

       case WM_DESTROY:
            PostQuitMessage (0);
            break;

	    default:
            return DefWindowProc( hWnd, iMessage, wParam, lParam );
       }
	return 0L;
}

/***************************************************************************/

BOOL FAR PASCAL GetRadiusDlgProc (hDlg, iMessage, wParam, lParam)
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
             SendDlgItemMessage (hDlg, IDD_RADIUS, EM_LIMITTEXT,
                                 (WORD)80, 0L);
             SetDlgItemText (hDlg, IDD_RADIUS, "1.0");
             return TRUE;
             break;

        case WM_COMMAND:
             switch (wParam)
             {
               case IDD_RADIUS:
                    if (HIWORD (lParam) == EN_CHANGE)
                        EnableWindow (GetDlgItem(hDlg,IDOK),
                                     (BOOL)SendMessage(LOWORD (lParam),
                                      WM_GETTEXTLENGTH, 0, 0L)) ;
                    break;
   		 
               case IDOK:                 
                    GetDlgItemText (hDlg, IDD_RADIUS, szRadius, 80) ;
                    OemToAnsi (szRadius, szRadius);
                    EndDialog (hDlg, TRUE);
                    break;
   
               case IDCANCEL:
         	     EndDialog (hDlg, FALSE) ;
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
       SetDlgItemText (hDlg, IDD_TEXT1, szXValue);
       SetDlgItemText (hDlg, IDD_TEXT2, szYValue);

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

/****************************************************************************/

void DrawSquare (hDC, rect)
     HDC       hDC;
     RECT      rect;
{
     MoveTo (hDC, rect.left,  rect.top);
     LineTo (hDC, rect.right, rect.top);
     LineTo (hDC, rect.right, rect.bottom);
     LineTo (hDC, rect.left,  rect.bottom);
     LineTo (hDC, rect.left,  rect.top);
}

