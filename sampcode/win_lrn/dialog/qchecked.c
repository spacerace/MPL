/*
 *
 *   qchecked.c
 *
 *   This program demonstrates the use of the IsDlgButtonChecked function.
 *   IsDlgButtonChecked checks to see if the given button control or 3 state
 *   button is checked. That is not the same as seeing if the control has
 *   been chosen. To enact the dialog box, choose the "DIALOG" menu
 *   selection. To invoke a call to IsDlgButtonChecked,choose one of the two
 *   check boxes in the dialog box.
 *
 */

#include "windows.h"
#include "qchecked.h"

HANDLE hInst;

FARPROC lpprocDialog;
long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL DialogBoxProc(HWND, unsigned, WORD, LONG);


int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE	 hInstance, hPrevInstance;
LPSTR	 lpszCmdLine;
int	 cmdShow;
  {
  MSG	    msg;
  HWND	    hWnd;
  HMENU     hMenu;
  WNDCLASS  wndclass;

  if (!hPrevInstance)
    {
    wndclass.style	    = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc    = WndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = 0;
    wndclass.hInstance	    = hInstance;
    wndclass.hIcon	    = NULL;
    wndclass.hCursor	    = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground  = GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName   = NULL;
    wndclass.lpszClassName  = "Sample Application";

    if (!RegisterClass(&wndclass))
      return FALSE;
    }

  hWnd = CreateWindow("Sample Application",
		      "IsDlgItemChecked",
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
  lpprocDialog = MakeProcInstance((FARPROC)DialogBoxProc, hInst);
  ShowWindow(hWnd, cmdShow);
  UpdateWindow(hWnd);
  hMenu = LoadMenu(hInstance, "qchecked");
  SetMenu(hWnd, hMenu);

  while (GetMessage(&msg, NULL, 0, 0))
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    }
  FreeProcInstance (lpprocDialog);
  return msg.wParam;
  }


long FAR PASCAL WndProc(hWnd, message, wParam, lParam)
HWND	   hWnd;
unsigned   message;
WORD	   wParam;
LONG	   lParam;
  {
  switch (message)
    {
    case WM_COMMAND:
      if (wParam == 80)
        {
        DialogBox(hInst, MAKEINTRESOURCE(2), hWnd, lpprocDialog);
        break;
        }
      else
        return DefWindowProc(hWnd, message, wParam, lParam);

    case WM_DESTROY:
      FreeProcInstance((FARPROC)lpprocDialog);
      PostQuitMessage(0);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
  return(0L);
  }


BOOL FAR PASCAL DialogBoxProc(hDlg, message, wParam, lParam)
HWND	  hDlg;
unsigned  message;
WORD	  wParam;
LONG	  lParam;
  {
  HWND hCtl;

  switch (message)
    {
    case WM_INITDIALOG:
      SendDlgItemMessage(hDlg, TOP_CONTROL, BM_SETCHECK, TRUE, (LONG) 0);
      return TRUE;
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case TOP_CONTROL:
	  if (IsDlgButtonChecked(hDlg, TOP_CONTROL))
            MessageBox(GetFocus(), "IS checked", "The top control:", MB_OK);
	  else
            MessageBox(GetFocus(), "is NOT checked", "The top control:", MB_OK);
          break;

	case BOTTOM_CONTROL:
	  if (IsDlgButtonChecked(hDlg, BOTTOM_CONTROL))
            MessageBox(GetFocus(), "IS checked", "The bottom control:", MB_OK);
	  else
            MessageBox(GetFocus(), "is NOT checked", "The bottom control:",
                       MB_OK);
          break;

	case ID_OK:
	  EndDialog(hDlg, TRUE);
          break;

	default:
	  MessageBox(NULL, "oops", NULL, MB_OK);
	  return FALSE;
        }
      break;

    default:
      return FALSE;
    }
  }
