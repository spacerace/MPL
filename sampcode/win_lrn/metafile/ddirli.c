/*
 *  Function Name:   DlgDirList
 *  Program Name:    ddirli.c
 *  Special Notes:
 *
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.0
 *
 *  Description:
 *   The program below allows the display of filenames and directories from
 *   the disk.  It will display the current directory and
 *   allow the user to select the filenames.
 *
 *   Windows Version 2.0 function demonstration application
 *
 */

#include <windows.h>
#include "ddirli.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL DialogProc(HWND, unsigned, WORD, LONG);

HANDLE   hInst;

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE	 hInstance, hPrevInstance;
LPSTR	 lpszCmdLine;
int	 cmdShow;
  {
  MSG	     msg;
  HWND	     hWnd;
  HMENU      hMenu;
  WNDCLASS   wcClass;

  if (!hPrevInstance)
    {
    wcClass.style	   = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = WndProc;
    wcClass.cbClsExtra	   = 0;
    wcClass.cbWndExtra	   = 0;
    wcClass.hInstance	   = hInstance;
    wcClass.hIcon	   = LoadIcon(hInstance, NULL);
    wcClass.hCursor	   = LoadCursor(NULL, IDC_ARROW);
    wcClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcClass.lpszMenuName   = (LPSTR)"MenuName";
    wcClass.lpszClassName  = (LPSTR)"DlgDirList";

    if (!RegisterClass(&wcClass))
      return FALSE;
    }

  hWnd = CreateWindow("DlgDirList",
		     "DlgDirList()",
		     WS_OVERLAPPEDWINDOW,
		     CW_USEDEFAULT,
		     CW_USEDEFAULT,
		     CW_USEDEFAULT,
		     CW_USEDEFAULT,
		     NULL,
		     NULL,
		     hInstance,
		     NULL);

  hInst = hInstance;
  ShowWindow(hWnd, cmdShow);
  UpdateWindow(hWnd);
  while (GetMessage(&msg, NULL, 0, 0))
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    }
  return msg.wParam;
  }

long FAR PASCAL WndProc(hWnd, message, wParam, lParam)
HWND	  hWnd;
unsigned  message;
WORD	  wParam;
LONG	  lParam;
  {
  switch (message)
    {
    case WM_COMMAND:
      if (wParam == IDM_1)
	{
	FARPROC lpprocDialogBox;
	lpprocDialogBox = MakeProcInstance ((FARPROC) DialogProc, hInst);
	DialogBox (hInst, MAKEINTRESOURCE(DIALOGID), hWnd, lpprocDialogBox);
	FreeProcInstance ((FARPROC) lpprocDialogBox);
	}
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
  return(0L);
  }

/* Window procedure for the dialog box */
BOOL FAR PASCAL DialogProc (hDlg, message, wParam, lParam)
HWND	   hDlg;
unsigned   message;
WORD	   wParam;
LONG	   lParam;
  {
  BOOL	   bListed;
  char	   str[128];

  switch (message)
    {
    case WM_INITDIALOG:
      {
      bListed = DlgDirList(hDlg, "*.*", ID_LISTBOX, NULL, 0x4010);
      if (!bListed)
	{
	MessageBox(NULL, (LPSTR)"DlgDirList failed", (LPSTR)"ERROR", MB_ICONHAND);
	return FALSE;
	}
      SetDlgItemText(hDlg, ID_EDIT, (LPSTR)"");
      return TRUE;
      break;
      }
    case WM_COMMAND:
      {
      switch (wParam)
	{
	case ID_LISTBOX:
	  if ((HIWORD(lParam)) == LBN_SELCHANGE)
	    {
	    DlgDirSelect(hDlg, (LPSTR)str, ID_LISTBOX);
	    SetDlgItemText(hDlg, ID_EDIT, (LPSTR)str);
	    return TRUE;
	    }
	  break;
	case IDCANCEL:
	  EndDialog(hDlg, TRUE);
	  break;
	default:
	  return FALSE;
	}
      }
    default:
      return FALSE;
    }
  return TRUE;
  }
