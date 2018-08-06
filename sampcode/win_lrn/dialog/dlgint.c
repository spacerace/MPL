/* 
 *
 *  SetDlgItemInt
 *  This program demonstrates the use of the function SetDlgItemInt.
 *  It sets the text of a dialog item (control) to the string representation
 *  of an integer value.
 */

#include "windows.h"
#include "dlgint.h"

HANDLE	 hInst;
FARPROC  lpprocEdit;

long    FAR PASCAL SampleWndProc (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL EditProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MSG	     msg;
  HWND	     hWnd;
  WNDCLASS   wcClass;

  if (!hPrevInstance)
    {
    wcClass.style	   = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = SampleWndProc;
    wcClass.cbClsExtra	   = 0;
    wcClass.cbWndExtra	   = 0;
    wcClass.hInstance	   = hInstance;
    wcClass.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
    wcClass.hIcon          = LoadIcon (hInstance, "WindowIcon");
    wcClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    wcClass.lpszMenuName   = MAKEINTRESOURCE (DIALOGMENU);
    wcClass.lpszClassName  = "dlgint";

    if (!RegisterClass (&wcClass))
      return FALSE;
    }
  hWnd = CreateWindow ("dlgint",
                      "Sending Integers",
                      WS_OVERLAPPEDWINDOW,
                      50,
                      50,
                      600,
                      250,
                      NULL,
                      NULL,
                      hInstance,
                      NULL);
  hInst = hInstance;
  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);
  while (GetMessage (&msg, NULL, 0, 0))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return msg.wParam;
  }

long    FAR PASCAL SampleWndProc (hWnd, message, wParam, lParam)
HWND	   hWnd;
unsigned   message;
WORD	   wParam;
LONG	   lParam;
  {
  switch (message)
    {
    case WM_COMMAND:
      if (wParam == ID_DIALOG)
        {
        lpprocEdit = MakeProcInstance ( (FARPROC) EditProc, hInst);
        DialogBox (hInst, MAKEINTRESOURCE (EDITBOX), hWnd, lpprocEdit);
        FreeProcInstance ( (FARPROC) lpprocEdit);
        }
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
    }
  return (0L);
  }


BOOL FAR PASCAL EditProc (hDlg, message, wParam, lParam)
HWND	   hDlg;
unsigned   message;
WORD	   wParam;
LONG	   lParam;
  {
  switch (message)
    {
    case WM_INITDIALOG:
      SetDlgItemInt (hDlg, ID_EDITBOX, 12345, (BOOL)FALSE);
      return TRUE;
      break;

    case WM_COMMAND:
      if (wParam == IDOK)
        {
        EndDialog (hDlg, TRUE);
	return TRUE;
        }
      break;

    default:
      return FALSE;
    }
  return TRUE;
  }
