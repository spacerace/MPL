/*
 *   This program demonstrates use of the CreateDialog () function.
 *   CreateDialog () creates a modeless dialog box. CreateDialog () is
 *   called in response to a WM_COMMAND message in HelloWndProc ();
 */

#include "windows.h"
#include "creatdlg.h"

long    FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);
BOOL    FAR PASCAL ShakeOptions (HWND, unsigned, WORD, LONG);
HWND    hYourTurnDlg = NULL;
HANDLE  hInst;
HWND    hWnd;
FARPROC lpprocSHAKE;

BOOL FAR PASCAL ShakeOptions (hDlg, message, wParam, lParam)
HWND     hDlg;
unsigned message;
WORD     wParam;
LONG     lParam;
  {
  HWND hCtl;
  int	i;
  BOOL OKAY;
  HDC hDC;
  RECT ClearRect;

  switch (message)
    {
    case WM_INITDIALOG:
      return TRUE;
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case IDOK:
          DestroyWindow (hYourTurnDlg);
          hYourTurnDlg = NULL;
          break;

        default:
          return FALSE;
          break;
        } /** end CASE WM_COMMAND **/
      break;

    case WM_DESTROY:
      return TRUE;
      break;

    default:
      return FALSE;
    }
  }

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit (hInstance)
HANDLE hInstance;
  {
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc (LPTR, sizeof (WNDCLASS));
  pHelloClass->hCursor        = LoadCursor (NULL, IDC_ARROW);
  pHelloClass->hIcon          = NULL;
  pHelloClass->lpszMenuName   = (LPSTR)"hello";
  pHelloClass->lpszClassName  = (LPSTR)"CreateDialog";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  pHelloClass->lpfnWndProc    = HelloWndProc;

  if (!RegisterClass ( (LPWNDCLASS)pHelloClass))
    return FALSE;	 

  LocalFree ( (HANDLE)pHelloClass);

  return TRUE;        /* Initialization succeeded */
  }


int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  MSG	msg;

  int   i;

  if (!hPrevInstance)
    if (!HelloInit (hInstance))
      return FALSE;

  hWnd = CreateWindow ( (LPSTR)"CreateDialog",
                      (LPSTR)"CreateDialog ()",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      (HWND)NULL,        /* no parent */
                      (HMENU)NULL,       /* use class menu */
                      (HANDLE)hInstance, /* handle to window instance */
                      (LPSTR)NULL);      /* no params to pass on */

  hInst = hInstance;

  lpprocSHAKE  = MakeProcInstance ( (FARPROC)ShakeOptions, hInstance);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    if (hYourTurnDlg == NULL || !IsDialogMessage (hYourTurnDlg, &msg))
      {
      TranslateMessage ( (LPMSG) & msg);
      DispatchMessage ( (LPMSG) & msg);
      }
    }

  return (int)msg.wParam;
  }


/* Procedures which make up the window class. */
long    FAR PASCAL HelloWndProc (hWnd, message, wParam, lParam)
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
  {
  switch (message)
    {
    case WM_COMMAND:
         /*** if the dialog box does not currently exist ***/
      if (!hYourTurnDlg)
        hYourTurnDlg = CreateDialog (hInst, MAKEINTRESOURCE (5), hWnd, lpprocSHAKE);
         /*** create the dialog box and assign "hYourTurnDlg" to its handle ***/
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }
  return (0L);
  }
