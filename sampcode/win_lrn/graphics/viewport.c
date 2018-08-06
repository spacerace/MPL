/*

Function(s) demonstrated in this program: GetViewportExt, GetWindowExt, 
    OffsetViewportOrg, OffsetWindowOrg, SetViewportExt, SetWindowExt, 
	 ScaleViewportExt, ScaleWindowExt, SetViewportOrg, SetWindowOrg

Windows version:  2.03

Windows SDK version:  2.00

Compiler version:  C 5.10

Description:  These functions modify the interface between screen pixels and
    the screen viewport.

Additional Comments:  The Scale functions (ScaleViewportExt, ScaleWindowExt),
    both take 5 parameters the first is a handle to a display context and 
    the next 4 are the numerators and denominators of the fractions that 
    will be used to scale the x and y extents.

*/

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "ViewPort.h"

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
use of ViewPort Window Extent functions.",

"Help Message",
"     This program uses the Windows functions\n\
GetViewportExt, GetWindowExt, OffsetViewportOrg,\n\
OffsetWindowOrg, SetViewportExt, SetWindowExt,\n\
ScaleViewportExt, SetViewportOrg, SetWindowOrg\n\
and ScaleWindowExt to modify the relationship\n\
between the viewport and the screen pixels.  Use\n\
the menu to request the function, then set the\n\
variables accordingly.",

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
    static char szAppName[] = "ViewPort";
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


    hWnd = CreateWindow (szAppName, "ViewPort",
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
	static HRGN   hRgnClip;
	static double xXcoord, yYcoord, fRadiusInc=(float)1, fRadius=(float)0, 
                 fAngle=(float)0, fAngleShift=(float)1, fMaxRadius, 
                 fProportion;
	static short  xClient, yClient;
	HDC	        hDC;
	HRGN	        hRGN;
	PAINTSTRUCT   ps;
   static WORD   OffViewX, OffViewY, OffWindowX, OffWindowY, SetViewX, 
                 SetViewY, SetWindowX, SetWindowY, ScaleViewXnum,
                 ScaleViewYnum, ScaleViewXdenom, ScaleViewYdenom,
                 ScaleWindowXnum, ScaleWindowYnum, ScaleWindowXdenom,
                 ScaleWindowYdenom, SetWindowOrgX, SetWindowOrgY,
                 SetViewportOrgX, SetViewportOrgY;
   DWORD         Check;

	switch (iMessage) {
       case WM_CREATE:
            hMenu = GetSystemMenu (hWnd, FALSE);
     
            ChangeMenu (hMenu, NULL, "&About", IDM_ABOUT, 
                        MF_APPEND | MF_STRING);
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
            fProportion= ((double) yClient)/ ((double) xClient);
            fMaxRadius = xClient/2.0;
            break;


       case WM_COMMAND:
            switch (wParam) {
               case IDM_CHANGE:
                    lpProcGetRadius = MakeProcInstance(GetRadiusDlgProc,
                                     hInstMain);
                    DialogBox (hInstMain, (LPSTR)"GetAngle", hWnd, 
                              lpProcGetRadius);
                    FreeProcInstance (lpProcGetRadius);
                    fAngleShift = atof (szRadius);
        
                    lpProcGetRadius = MakeProcInstance(GetRadiusDlgProc,
                                     hInstMain);
                    DialogBox (hInstMain, (LPSTR)"GetRadius", hWnd, 
                              lpProcGetRadius);
                    FreeProcInstance (lpProcGetRadius);
                    fRadiusInc = atof (szRadius);

                    InvalidateRect (hWnd, NULL, TRUE);
                    UpdateWindow (hWnd);
                    break;

               case IDM_OFFVIEWPORT:
                    bOffsetViewportOrg = TRUE;
                    sprintf (szXValue, "Enter Origin (x,y)");
                    sprintf (szYValue, "OffsetViewportOrg");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    OffViewX = atoi (szXValue);
                    OffViewY = atoi (szYValue);                      

                    InvalidateRect (hWnd, NULL, TRUE);
                    UpdateWindow (hWnd);
                    break;

               case IDM_OFFWINDOW:
                    bOffsetWindowOrg = TRUE;
                    sprintf (szXValue, "Enter Origin (x,y)");
                    sprintf (szYValue, "OffsetWindowOrg");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    OffWindowX = atoi (szXValue);
                    OffWindowY = atoi (szYValue);                      
                    				  
                    InvalidateRect (hWnd, NULL, TRUE);
                    UpdateWindow (hWnd);
                    break;

               case IDM_SETVIEWPORT:
                    bSetViewportExt = TRUE;
                    sprintf (szXValue, "Enter Extents (x,y)");
                    sprintf (szYValue, "SetViewportExt");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    SetViewX = atoi (szXValue);
                    SetViewY = atoi (szYValue);                      
                    				  
                    InvalidateRect (hWnd, NULL, TRUE);
                    UpdateWindow (hWnd);
                    break;

               case IDM_SETEXTENT:
                    bSetWindowExt = TRUE;
                    sprintf (szXValue, "Enter Extents (x,y)");
                    sprintf (szYValue, "SetWindowExt");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    SetWindowX = atoi (szXValue);
                    SetWindowY = atoi (szYValue);                      

                    InvalidateRect (hWnd, NULL, TRUE);
                    UpdateWindow (hWnd);
                    break;

               case IDM_SCALEVIEW:
                    bScaleViewportExt = TRUE;
                    sprintf (szXValue, "Enter Numerators (x,y)");
                    sprintf (szYValue, "ScaleViewportExt");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    ScaleViewXnum = atoi (szXValue);
                    ScaleViewYnum = atoi (szYValue);                      

                    sprintf (szXValue, "Enter Denominators (x,y)");
                    sprintf (szYValue, "ScaleViewportExt");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    ScaleViewXdenom = atoi (szXValue);
                    ScaleViewYdenom = atoi (szYValue);                      

                    InvalidateRect (hWnd, NULL, TRUE);
                    UpdateWindow (hWnd);
                    break;

               case IDM_SCALEWINDOW:
                    bScaleWindowExt = TRUE;
                    sprintf (szXValue, "Enter Numerators (x,y)");
                    sprintf (szYValue, "ScaleWindowExt");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    ScaleWindowXnum = atoi (szXValue);
                    ScaleWindowYnum = atoi (szYValue);                      

                    sprintf (szXValue, "Enter Denominators (x,y)");
                    sprintf (szYValue, "ScaleWindowExt");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    ScaleWindowXdenom = atoi (szXValue);
                    ScaleWindowYdenom = atoi (szYValue);                      

                    InvalidateRect (hWnd, NULL, TRUE);
                    UpdateWindow (hWnd);
                    break;

               case IDM_GETVIEWPORTEXT:
                    hDC = GetDC (hWnd);
                    SetMapMode (hDC, MM_ANISOTROPIC);
                    if (bSetViewportExt)
                        SetViewportExt    (hDC, SetViewX, SetViewY);
                    Check = GetViewportExt (hDC);
                    sprintf (szOutputBuffer1, "Viewport Extension = (%i,%i)",
                            LOWORD (Check), HIWORD (Check));
                    MessageBox (hWnd, szOutputBuffer1, "GetViewportExt", MB_OK);
                    ReleaseDC (hWnd, hDC);
                    break;

               case IDM_GETWINDOWEXT:
                    hDC = GetDC (hWnd);
                    SetMapMode (hDC, MM_ANISOTROPIC);
                    if (bSetWindowExt)       
                        SetWindowExt      (hDC, SetWindowX, SetWindowY); 
                    Check = GetWindowExt (hDC);
                    sprintf (szOutputBuffer1, "Window Extention = (%i,%i)",
                            LOWORD (Check), HIWORD (Check));
                    MessageBox (hWnd, szOutputBuffer1, "GetWindowExt", MB_OK);
                    ReleaseDC (hWnd, hDC);
                    break;

               case IDM_SETVIEWPORTORG:
                    bSetViewportOrg = TRUE;
                    sprintf (szXValue, "Enter Origin (x,y)");
                    sprintf (szYValue, "SetViewportOrg");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    SetViewportOrgX = atoi (szXValue);
                    SetViewportOrgY = atoi (szYValue);                      

                    InvalidateRect (hWnd, NULL, TRUE);
                    UpdateWindow (hWnd);
                    break;

               case IDM_SETWINDOWORG:
                    bSetWindowOrg = TRUE;
                    sprintf (szXValue, "Enter Origin (x,y)");
                    sprintf (szYValue, "SetWindowOrg");
                    DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd, 
                           lpProcEnterPoint);
                    SetWindowOrgX = atoi (szXValue);
                    SetWindowOrgY = atoi (szYValue);                      

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
            SetMapMode (hDC, MM_ANISOTROPIC);
            SetViewportOrg (hDC, xClient/2, yClient/2);

            if (bOffsetViewportOrg)
                  OffsetViewportOrg (hDC, OffViewX, OffViewY);
            if (bOffsetWindowOrg)     
                  OffsetWindowOrg   (hDC, OffWindowX, OffWindowY);
            if (bSetViewportExt)
                  SetViewportExt    (hDC, SetViewX, SetViewY);
            if (bSetWindowExt)       
                  SetWindowExt      (hDC, SetWindowX, SetWindowY); 
            if (bScaleViewportExt)
                  ScaleViewportExt  (hDC, ScaleViewXnum, ScaleViewXdenom,
                                     ScaleViewYnum, ScaleViewYdenom);
            if (bScaleWindowExt)
                  ScaleWindowExt  (hDC, ScaleWindowXnum, ScaleWindowXdenom,
                                     ScaleWindowYnum, ScaleWindowYdenom);
            if (bSetViewportOrg) 
                  SetViewportOrg (hDC, SetViewportOrgX, SetViewportOrgY);
            if (bSetWindowOrg)
                  SetWindowOrg (hDC, SetWindowOrgX, SetWindowOrgY);

            MoveTo (hDC, 0, 0); 
            ShowCursor (TRUE);
            for (fRadius=(float)0; fRadius<fMaxRadius; fRadius+=fRadiusInc) {
                 xXcoord = fRadius * cos(fAngle);
                 yYcoord = fRadius * sin(fAngle) * fProportion;
                 LineTo (hDC, (short)xXcoord, (short)yYcoord);
                 fAngle +=fAngleShift;
            }
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

