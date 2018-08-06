/*
 * Function(s) demonstrated in this program: ChildWindowFromPoint
 * Compiler version:  C 5.10
 * Description:  This function determines which, if any, of the child windows
 *               belonging to the given parent window contains the specified
 *               point.
 * Additional Comments:  This program also illustrates the subclassing of
 *                       windows.
 */

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "childfp.h"

char	szMessageBuffer [150] = "    Demo Program (ChildWindowFromPoint).";

char    szHelpBuffer [200] = "     Choose to enter point coordinates, or\n\
let the computer choose random coordinates.\n\
The program will use the ChildWindowFromPoint\n\
function to compute which window the point\n\
resides in.";

HWND     hWndMain, hWndChild1, hWndChild2, hX, hY, hOK2;
HANDLE   hInstMain;
FARPROC  lpProcEnterPoint;
FARPROC  lpProcNewEnterPoint;
FARPROC  lpProcOldX;
FARPROC  lpProcOldY;

char	szXValue [40];
char	szYValue [40];

/****************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE  hInstance, hPrevInstance;
LPSTR   lpszCmdLine;
int     nCmdShow;
  {
  static char   szAppName [] = "ChildFp";
  static char   szChildClass [] = "ChildFpChild";

  HWND        hWnd;
  WNDCLASS    wndclass;
  MSG msg;
  short xScreen, yScreen;

  if (!hPrevInstance)
    {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName  = szAppName;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass (&wndclass))
      return FALSE;

    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = ChildWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = NULL;
    wndclass.hCursor       = LoadCursor (NULL, IDC_CROSS);
    wndclass.hbrBackground = GetStockObject (LTGRAY_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = szChildClass;

    if (!RegisterClass (&wndclass))
      return FALSE;
    }

  xScreen = GetSystemMetrics (SM_CXSCREEN);
  yScreen = GetSystemMetrics (SM_CYSCREEN);

  hWnd = CreateWindow (szAppName,                /* window class name       */
                      "Child Window From Point", /* window caption          */
                      WS_OVERLAPPEDWINDOW,       /* window style            */
                      CW_USEDEFAULT,             /* initial x position      */
                      0,                         /* initial y position      */
                      CW_USEDEFAULT,             /* initial x size          */
                      0,                         /* initial y size          */
                      NULL,                      /* parent window handle    */
                      NULL,                      /* window menu handle      */
                      hInstance,                 /* program instance handle */
                      NULL);                     /* create parameters       */

  ShowWindow (hWnd, nCmdShow);
  UpdateWindow (hWnd);

  hWndMain  = hWnd;
  hInstMain = hInstance;

  hWndChild1 = CreateWindow (szChildClass, "Child 1", WS_CHILD |
                             WS_CAPTION | WS_SYSMENU |
                             WS_THICKFRAME | WS_MAXIMIZEBOX |
                             WS_CLIPSIBLINGS,
                             xScreen / 9, yScreen / 8,
                             xScreen / 3, yScreen / 2,
                             hWndMain, 1, hInstance, NULL);

  ShowWindow (hWndChild1, SW_SHOWNORMAL);
  UpdateWindow (hWndChild1);

  hWndChild2 = CreateWindow (szChildClass, "Child 2", WS_CHILD |
                             WS_CAPTION | WS_SYSMENU |
                             WS_THICKFRAME | WS_MAXIMIZEBOX |
                             WS_CLIPSIBLINGS,
                             5 * xScreen / 9, yScreen / 8,
                             xScreen / 3, yScreen / 2,
                             hWndMain, 1, hInstance, NULL);

  ShowWindow (hWndChild2, SW_SHOWNORMAL);
  UpdateWindow (hWndChild2);

  lpProcEnterPoint    = MakeProcInstance (EnterPointDlgProc, hInstance);
  lpProcNewEnterPoint = MakeProcInstance (NewEnterPointDlgProc, hInstance);

  while (GetMessage(&msg, NULL, 0, 0))
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    }
  return (msg.wParam);
  }

/****************************************************************************/

long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  HMENU         hMenu;
  POINT         pt;
  static int    xClient, yClient;
  char          szOutputBuffer [40];

  switch (iMessage)
    {
    case WM_CREATE:
      hMenu = GetSystemMenu (hWnd, FALSE);
      ChangeMenu (hMenu, NULL, "&About", IDM_ABOUT, MF_APPEND | MF_STRING);
      break;

    case WM_SIZE:
      xClient = LOWORD (lParam);
      yClient = HIWORD (lParam);
      sprintf (szOutputBuffer, "Width = %i, Height = %i", xClient, yClient);
      MessageBox (hWnd, szOutputBuffer, "Client Window Dimensions", MB_OK);
      break;

    case WM_LBUTTONDBLCLK:
      break;

    case WM_SYSCOMMAND:
      switch (wParam)
        {
        case IDM_ABOUT:
          MessageBox (hWnd, szMessageBuffer, (LPSTR)"About", MB_OK);
          break;
        default:
        return DefWindowProc (hWnd, iMessage, wParam, lParam);
        }
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case IDM_CHOOSERANDOM:
          pt.x = pt.y = -1;
          while (((pt.x < 0) || (pt.x > xClient)) || ((pt.y < 0) ||
                 (pt.y > yClient)))
            {
            pt.x = (rand () / 47);
            pt.y = (rand () / 102);
            }
          hWnd = ChildWindowFromPoint (hWnd, pt);
          sprintf (szOutputBuffer, "The Point [%i, %i]", pt.x, pt.y);
          if (hWnd == hWndChild1)
            MessageBox (hWnd, "is in Child Window 1", szOutputBuffer,
                        MB_OK);
          else
            if (hWnd == hWndChild2)
              MessageBox (hWnd, "is in Child Window 2", szOutputBuffer,
                          MB_OK);
            else
              MessageBox (hWnd, "is not in a Child Window",
                          szOutputBuffer, MB_OK);
          break;

        case IDM_CHOOSE:
          pt.x = pt.y = -1;
          while (((pt.x < 0) || (pt.x > xClient)) || ((pt.y < 0) ||
                 (pt.y > yClient)))
            {
            DialogBox (hInstMain, (LPSTR)"EnterPointDlg", hWnd,
                       lpProcEnterPoint);
            pt.x = atoi (szXValue);
            pt.y = atoi (szYValue);
            }
          hWnd = ChildWindowFromPoint (hWnd, pt);
          sprintf (szOutputBuffer, "The Point [%i, %i]", pt.x, pt.y);
          if (hWnd == hWndChild1)
            MessageBox (hWnd, "is in Child Window 1", szOutputBuffer,
                        MB_OK);
          else
            if (hWnd == hWndChild2)
              MessageBox (hWnd, "is in Child Window 2", szOutputBuffer,
                          MB_OK);
          else
            MessageBox (hWnd, "is not in a Child Window",
                        szOutputBuffer, MB_OK);
          break;

        case IDM_HELP:
          MessageBox (hWnd, szHelpBuffer, "Help Message", MB_OK);
          break;
        }
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam);
    }
  return (0L);
  }

/****************************************************************************/

long    FAR PASCAL ChildWndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  return DefWindowProc (hWnd, iMessage, wParam, lParam);
  }


/****************************************************************************/

BOOL FAR PASCAL EnterPointDlgProc (hDlg, iMessage, wParam, lParam)
HWND     hDlg;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  switch (iMessage) 
    {
    case WM_INITDIALOG:
      SendDlgItemMessage (hDlg, IDD_X, EM_LIMITTEXT, (WORD)40, 0L);
      SendDlgItemMessage (hDlg, IDD_Y, EM_LIMITTEXT, (WORD)40, 0L);

      hX = GetDlgItem (hDlg, IDD_X);
      lpProcOldX = (FARPROC) GetWindowLong (hX, GWL_WNDPROC);
      SetWindowLong (hX, GWL_WNDPROC, (LONG) lpProcNewEnterPoint);
      SendMessage (hX, EM_SETSEL, 0, MAKELONG (0, 32767));
      hY = GetDlgItem (hDlg, IDD_Y);
      lpProcOldY = (FARPROC) GetWindowLong (hY, GWL_WNDPROC);
      SetWindowLong (hY, GWL_WNDPROC, (LONG) lpProcNewEnterPoint);
      SendMessage (hY, EM_SETSEL, 0, MAKELONG (0, 32767));
      hOK2 = GetDlgItem (hDlg, IDOK);

      return TRUE;
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case IDD_X:
          break;

        case IDD_Y:
          break;

        case IDOK:
          GetDlgItemText (hDlg, IDD_X, szXValue, 10);
          GetDlgItemText (hDlg, IDD_Y, szYValue, 10);
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
  switch (iMessage) 
    {
    case WM_GETDLGCODE:
      return (DLGC_WANTALLKEYS);

    case WM_CHAR:
      if ((wParam == VK_TAB) || (wParam == VK_RETURN))
        {
        SendMessage (hWndMain, WM_USER, 0, 0L);
        SetFocus (FindNextWindow (hWnd));
        return TRUE;
        }
      else
        {
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
  if (hWnd == hX)      
    return hY;
  if (hWnd == hY)      
    return hOK2;
  return NULL;
  }
