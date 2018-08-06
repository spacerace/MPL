/*
 *  Function (s) demonstrated in this program: MapDialogRect
 *  Description:
 *     When the About dialog box is brought up, it has two buttons.  The OK
 *  button to end the dialog box, and the DRAW button which draws an
 *  ellipse in the upper left hand corner.  Coordinates for the ellipse
 *  are converted from dialog coordinates to screen coordinates using 
 *  the MapDialogRect function.
 */

#include "windows.h"	
#include "mapdlgr.h"	

HANDLE hInst;

/**************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE    hInstance;
HANDLE    hPrevInstance;
LPSTR     lpCmdLine;
int       nCmdShow;
  {
  HWND hWnd;
  MSG msg;
  WNDCLASS WndClass;

  WndClass.style = NULL;
  WndClass.lpfnWndProc = MapDlgRectWndProc;
  WndClass.hInstance = hInstance;
  WndClass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  WndClass.hCursor = LoadCursor (NULL, IDC_ARROW);
  WndClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  WndClass.lpszMenuName = (LPSTR) NULL;
  WndClass.lpszClassName = (LPSTR) "MapDlgRect";

  if (!RegisterClass ((PWNDCLASS)&WndClass))
    return FALSE;

  hInst = hInstance;

  hWnd = CreateWindow ("MapDlgRect", (LPSTR)"MapDlgRect Sample Application",               /* window name             */
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, 0,
                       CW_USEDEFAULT, 0,
                       NULL, NULL, hInstance, NULL);
  if (!hWnd)
    return (NULL);

  ShowWindow (hWnd, nCmdShow);
  UpdateWindow (hWnd);

  while (GetMessage (&msg, NULL, NULL, NULL))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return (msg.wParam);
  }

long    FAR PASCAL MapDlgRectWndProc (hWnd, message, wParam, lParam)
HWND      hWnd;
unsigned  message;
WORD      wParam;
LONG      lParam;
  {
  FARPROC lpProcAbout;
  HMENU hMenu;

  switch (message)
    {
    case WM_SYSCOMMAND:           /* message: command from system menu */
      if (wParam == ID_ABOUT)
        {
        lpProcAbout = MakeProcInstance (About, hInst);
        DialogBox (hInst, "AboutBox", hWnd,        lpProcAbout);
        FreeProcInstance (lpProcAbout);
        break;
        }
      else
        return (DefWindowProc (hWnd, message, wParam, lParam));

    case WM_CREATE:
      hMenu = GetSystemMenu (hWnd, FALSE);
      ChangeMenu (hMenu, NULL, NULL, NULL, MF_APPEND | MF_SEPARATOR);
      ChangeMenu (hMenu, NULL, "A&bout MapDlgRect...", ID_ABOUT,
                  MF_APPEND | MF_STRING);
      break;

    case WM_DESTROY:                /* message: window being destroyed */
      PostQuitMessage (0);
      break;

    default:                        /* Passes it on if unproccessed    */
      return (DefWindowProc (hWnd, message, wParam, lParam));
    }
  return (NULL);
  }

BOOL FAR PASCAL About (hDlg, message, wParam, lParam)
HWND       hDlg;
unsigned   message;
WORD       wParam;
LONG       lParam;
  {
  RECT      rRect;     /*  Rectangle to draw elipse onto Dialog box  */
  HDC       hDC;       /*  Handle to display context  */

  switch (message)
    {
    case WM_INITDIALOG:                /* message: initialize dialog box */
      return (TRUE);

    case WM_COMMAND:                    /* message: received a command */
      if (wParam == IDOK)
        {         /* "OK" box selected?          */
        EndDialog (hDlg, NULL);          /* Exits the dialog box        */
        return (TRUE);
        }
      if (wParam == ID_DRAW)  /*  If they hit the Draw button, draw
                               *  an ellipse in the dialog box  */
        {
        hDC = GetDC (hDlg);      /* get DC for the dialog box */
        rRect.left = 0;          /* These coordinates are in dialog
                                  * box units.  They will be converted
                                  * to screen coordinates by the
                                  * MapDialogRect function  */
        rRect.top = 0;
        rRect.right = 20;
        rRect.bottom = 10;
        MapDialogRect (hDlg, (LPRECT) & rRect);
        Ellipse (hDC, rRect.left, rRect.top, rRect.right, rRect.bottom);
        ReleaseDC (hDlg, hDC);
        return (TRUE);
        }
      break;
    }
  return (FALSE);
  }
