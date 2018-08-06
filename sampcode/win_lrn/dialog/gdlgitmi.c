/*
 *   GetDlgItemInt
 *
 *   This program demonstrates the use of the GetDlgItemInt function.
 *
 */

#include "windows.h"
#include <stdio.h>
#include "gdlgitmi.h"

long FAR PASCAL ItemIWndProc(HWND, unsigned, WORD, LONG);

HANDLE  hInst;        /* Instance handle for dialog box.   */
FARPROC lpprocDialog; /* Long pointer to dialog procedure. */
char    szBuff[80];   /* Buffer for edit control text.     */

BOOL FAR PASCAL DialogBoxProc(hDlg, message, wParam, lParam)
HWND     hDlg;
unsigned message;
WORD     wParam;
LONG     lParam;
{
  int  nNumber;      /* Integer from the edit control. */
  BOOL bTranslated;  /* Translated flag pointer.       */

  switch (message)
    {
    case WM_INITDIALOG:
      SetFocus(GetDlgItem(hDlg, EDIT_CONTROL));
      return FALSE;

    case WM_COMMAND:
      if (wParam == ID_OK)
        {
         /* Get the integer value of text entered. */
        nNumber = (int)GetDlgItemInt(hDlg, EDIT_CONTROL,
                                     (BOOL FAR *)&bTranslated,
                                     TRUE);
        if (bTranslated != FALSE)
          {  /* Display that value in a message box if valid. */
          sprintf(szBuff, "%s%d", "The number from the edit field: ", nNumber);
          MessageBox(GetFocus(), (LPSTR)szBuff,
                     (LPSTR)"GetDlgItemInt()", MB_OK);
          }
        else /* Or display an error. */
          MessageBox(GetFocus(),
                     (LPSTR)"Non-numeric character or exceeded max!",
                     (LPSTR)"GetDlgItemInt() Error!",
                     MB_OK | MB_ICONEXCLAMATION);
        /* Quit the dialog box. */
        EndDialog(hDlg, TRUE);
        return TRUE;
        }
      else
        return FALSE;

    default:
      return FALSE;
    }
}

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  MSG         msg;         /* Messages.               */
  HWND        hWnd;        /* Window handle.          */
  PWNDCLASS   pItemIClass; /* Window class structure. */

  pItemIClass->hCursor        = LoadCursor(NULL, IDC_ARROW);
  pItemIClass->hIcon          = LoadIcon(hInstance,NULL);
  pItemIClass->lpszMenuName   = (LPSTR)"getintmenu";
  pItemIClass->lpszClassName  = (LPSTR)"gdlgitmi";
  pItemIClass->hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
  pItemIClass->hInstance      = hInstance;
  pItemIClass->style          = CS_HREDRAW | CS_VREDRAW;
  pItemIClass->lpfnWndProc    = ItemIWndProc;

  if (!RegisterClass((LPWNDCLASS)pItemIClass))
    return FALSE;
  LocalFree((HANDLE)pItemIClass);

  hWnd = CreateWindow((LPSTR)"gdlgitmi", (LPSTR)"GetDlgItemInt",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT,  CW_USEDEFAULT,
                      (HWND)NULL, (HMENU)NULL, (HANDLE)hInstance,
                      (LPSTR)NULL);
  hInst = hInstance;

  ShowWindow(hWnd, cmdShow);
  UpdateWindow(hWnd);

  while (GetMessage((LPMSG)&msg, NULL, 0, 0))
    {
    TranslateMessage((LPMSG)&msg);
    DispatchMessage((LPMSG)&msg);
    }

  return (int)msg.wParam;
}


/* Procedures which make up the window class. */
long FAR PASCAL ItemIWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
  switch (message)
    {
    case WM_COMMAND:
      switch (wParam)
        {
        case ID_INTBOX:
          lpprocDialog = MakeProcInstance((FARPROC)DialogBoxProc, hInst);
          DialogBox(hInst, MAKEINTRESOURCE(INTBOX), hWnd, lpprocDialog);
          FreeProcInstance((FARPROC)lpprocDialog);
          break;

        default:
          return DefWindowProc(hWnd, message, wParam, lParam);
          break;
        }
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }
  return(0L);
}

