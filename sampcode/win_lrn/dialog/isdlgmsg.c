/*
 *   IsDialogMessage
 *
 *   This program demonstrates the use of the IsDialogMessage function.
 *   The IsDialogMessage function is usually used in a window's message
 *   loop to determine if messages are intended for modeless dialog boxes.
 *   If a message it intended for the given dialog box, IsDialogMessage
 *   processes the mesage for the dialog box. IsDialogMessage is called
 *   from WinMain in this sample application
 */

#include "windows.h"
#include "isdlgmsg.h"

HANDLE hInst;
HWND hToTheDialog = NULL;     /*** handle to the modeless dialog box ***/

FARPROC lpprocDialog;
long	FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);


BOOL FAR PASCAL DialogBoxProc (hDlg, message, wParam, lParam)
HWND hDlg;
unsigned	message;
WORD wParam;
LONG lParam;
{
  switch (message)
  {
  case WM_INITDIALOG:
    return TRUE;
    break;

  case WM_COMMAND:
    switch (wParam)
    {
    case TOP_CONTROL:
      MessageBox (NULL, (LPSTR)"The message was processed",
          (LPSTR)" ", MB_OK);
      break;

    case ID_OK:
      EndDialog (hDlg, TRUE);
      break;
    }
    break;

  case WM_DESTROY:
    hToTheDialog = NULL;
    return TRUE;
    break;

  default:
    return FALSE;
    break;
  }
}


/* Procedure called when the application is loaded for the first time */
BOOL HelloInit (hInstance)
HANDLE hInstance;
{
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc (LPTR, sizeof (WNDCLASS));

  pHelloClass->hCursor        = LoadCursor (NULL, IDC_ARROW);
  pHelloClass->hIcon          = LoadIcon (hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)NULL;
  pHelloClass->lpszClassName  = (LPSTR)"Sample Application";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc    = HelloWndProc;

  if (!RegisterClass ( (LPWNDCLASS)pHelloClass))
    return FALSE;

  LocalFree ( (HANDLE)pHelloClass);
  return TRUE;        /* Initialization succeeded */
}


int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int	cmdShow;
{
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;

  HelloInit (hInstance);
  hMenu = LoadMenu (hInstance, "isdlgmsg");

  hWnd = CreateWindow ( (LPSTR)"Sample Application", (LPSTR)"IsDialogMessage",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, hMenu, hInstance, NULL);
  hInst = hInstance;

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
  {
    if (hToTheDialog == NULL || !IsDialogMessage (hToTheDialog, &msg))
    {/*  Make sure the message is meant for this window...  */

      TranslateMessage ( (LPMSG) & msg);
      DispatchMessage ( (LPMSG) & msg);
    }
  }
  return (int)msg.wParam;
}


/* Procedures which make up the window class. */
long	FAR PASCAL HelloWndProc (hWnd, message, wParam, lParam)
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
{
  PAINTSTRUCT ps;

  switch (message)
  {
  case WM_COMMAND:
    if (wParam == 80)
    {
      lpprocDialog = MakeProcInstance ( (FARPROC)DialogBoxProc, hInst);
      hToTheDialog = CreateDialog (hInst, MAKEINTRESOURCE (2), hWnd,
          lpprocDialog);
    }
    break;

  case WM_DESTROY:
    FreeProcInstance ( (FARPROC)lpprocDialog);
    PostQuitMessage (0);
    break;

  default:
    return DefWindowProc (hWnd, message, wParam, lParam);
    break;
  }
  return (0L);
}


