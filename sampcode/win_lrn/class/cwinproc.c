/*
 *   This program demonstrates the use of the CallWindowProc () function.
 *   CallWindowProc () passes message information contained in the 3rd
 *   parameter to the window identified by the 2nd parameter. CallWindow-
 *   Proc is used in subclassing. This is how it is used in this program.
 *   No real subclass is performed in this program; it is just set up.
 *   Messages destined for the listbox in the dialog box are intercepted
 *   by SubClassProc () and then passed on to the default procedure for
 *   listboxes. CallWindowProc () is called in SubClassProc () after the
 *   subclass is performed.
 */

#include "windows.h"
#include "cwinproc.h"

BOOL FAR PASCAL DialogBoxProc (HWND, unsigned, WORD, LONG);
long	FAR PASCAL HelloWndProc (HWND, unsigned, WORD, LONG);
long	FAR PASCAL SubClassProc (HWND, unsigned, WORD, LONG);

HANDLE hInst;

FARPROC lpprocDlgProc;
FARPROC lpprocSubClassProc;
FARPROC lpprocListBox;

char	szLBOutput[50];

BOOL FAR PASCAL DialogBoxProc (hDlg, message, wParam, lParam)
HWND hDlg;
unsigned	message;
WORD wParam;
LONG lParam;
{
  HWND hCtl;
  LONG lListBoxSel;
  int	i;
  char	buffer[15];

  switch (message)
  {
  case WM_INITDIALOG:
    lpprocSubClassProc = MakeProcInstance ( (FARPROC) SubClassProc, hInst);

    lpprocListBox = (FARPROC) SetWindowLong (GetDlgItem (hDlg, IDLISTBOX),
        GWL_WNDPROC,
        (long)lpprocSubClassProc);

    hCtl = GetDlgItem (hDlg, IDLISTBOX);
    i =  ID_FEET;
    while (LoadString (hInst, i, (LPSTR)buffer, MAXSTRLEN) != 0)
    {
      SendMessage (hCtl, LB_ADDSTRING, 0, (LONG) (LPSTR)buffer);
      i++;
    }

    SetFocus (hCtl);
    return (TRUE);

  case WM_COMMAND:
    switch (wParam)
    {
    case IDLISTBOX:
      if ( (HIWORD (lParam)) == 2)
      {
	hCtl = GetDlgItem (hDlg, IDLISTBOX);

	lListBoxSel = SendMessage (hCtl, LB_GETCURSEL, 0,
	    (LONG) (LPSTR)"");
	SendMessage (hCtl, LB_GETTEXT, (WORD) lListBoxSel,
	    (LONG) (LPSTR)szLBOutput);
	MessageBox (NULL, (LPSTR)szLBOutput, (LPSTR)"Chose from listbox:",
	    MB_OK);
	return (TRUE);
      }
      else
	return (FALSE);
      break;

    case IDOK:
      hCtl = GetDlgItem (hDlg, IDLISTBOX);
      lListBoxSel = SendMessage (hCtl, LB_GETCURSEL, 0, (LONG) (LPSTR)"");
      if (lListBoxSel == -1L)
	i = 1;
      else
	SendMessage (hCtl, LB_GETTEXT, (WORD)lListBoxSel,
	    (LONG) (LPSTR)szLBOutput);
      EndDialog (hDlg, TRUE);
      return (TRUE);

    case IDCANCEL:
      EndDialog (hDlg, TRUE);
      return (TRUE);

    default:
      return (FALSE);
    }
    break;

  default:
    return (FALSE);
    break;
  }
}


long	FAR PASCAL SubClassProc (hWnd, message, wParam, lParam)
HWND      hWnd;
unsigned	message;
WORD      wParam;
LONG      lParam;
{
  CallWindowProc (lpprocListBox, hWnd, message, wParam, lParam);
/*  Pass Info onto the default control procedure  */
  return 0L;
}


/* Procedure called when the application is loaded for the first time */
BOOL HelloInit (hInstance)
HANDLE hInstance;
{
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc (LPTR, sizeof (WNDCLASS));

  pHelloClass->hCursor        = LoadCursor (NULL, IDC_ARROW);
  pHelloClass->hIcon          = LoadIcon (hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)"fonttest";
  pHelloClass->lpszClassName  = (LPSTR)"CallWindowProc";
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
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
{
  MSG   msg;
  HWND  hWnd;

  HelloInit (hInstance);
  hWnd = CreateWindow ( (LPSTR)"CallWindowProc",
      (LPSTR)"CallWindowProc ()",
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

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
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
  switch (message)
  {
  case WM_COMMAND:
    lpprocDlgProc = MakeProcInstance ( (FARPROC)DialogBoxProc, hInst);
    DialogBox (hInst, MAKEINTRESOURCE (5), hWnd, lpprocDlgProc);
    FreeProcInstance ( (FARPROC)lpprocDlgProc);
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


