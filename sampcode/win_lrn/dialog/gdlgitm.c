/*
 *   GetDlgItem
 *
 *   This program demonstrates the use of the GetDlgItem function.
 *
 */

#include <windows.h>
#include <stdio.h>
#include "gdlgitm.h"

long FAR PASCAL DlgItmWndProc(HWND, unsigned, WORD, LONG);

HANDLE  hInst;        /* Instance handle for dialog box. */
FARPROC lpprocDialog; /* Pointer to dialog procedure.    */

BOOL FAR PASCAL DialogBoxProc(hDlg, message, wParam, lParam)
HWND     hDlg;
unsigned message;
WORD     wParam;
LONG     lParam;
{ 
  switch (message)
    {
    case WM_INITDIALOG:
      SetFocus(GetDlgItem(hDlg, EDIT_CONTROL));
      return FALSE;

    case WM_COMMAND:
      if (wParam == ID_OK)
        { 
        char szBuff[80]; /* Output buffer from the edit control. */
        HWND hCtl;       /* Handle to the edit control, Used by  */
                         /* GetDlgItem().                        */

        /* Get the handle to the edit control window. */
        MessageBox(GetFocus(),
                   (LPSTR)"Getting handle to control in dialog box",
                   (LPSTR)"GetDlgItem", MB_OK);
        hCtl = GetDlgItem(hDlg, EDIT_CONTROL);
        if (hCtl == NULL)
          MessageBox(GetFocus(), (LPSTR)"Error In Obatining Handle!",
                     (LPSTR)"GetDlgItem() Error!",
                     MB_OK | MB_ICONEXCLAMATION);
        else
          /* If the handle is good, get and show the edit control text. */
          SendMessage(hCtl, WM_GETTEXT, (WORD)80, (LONG)szBuff);
          MessageBox(GetFocus(), (LPSTR)szBuff,
                     (LPSTR)"Edit Box Contents using GetDlgItem()",
                     MB_OK);
        /* And Quit. */
        EndDialog(hDlg,TRUE);
        return TRUE;
        }
      else return FALSE;

    default:
      return FALSE;
   } 
}

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  MSG       msg;          /* Window messages.           */
  HWND      hWnd;         /* Window handle.             */
  PWNDCLASS pDlgItmClass; /* Pointer to the class data. */

  pDlgItmClass = (PWNDCLASS)LocalAlloc(LPTR, sizeof(WNDCLASS));

  pDlgItmClass->hCursor       = LoadCursor(NULL, IDC_ARROW);
  pDlgItmClass->hIcon         = LoadIcon(hInstance, NULL);
  pDlgItmClass->lpszMenuName  = (LPSTR)"dlgmenu";
  pDlgItmClass->lpszClassName = (LPSTR)"gdlgitm";
  pDlgItmClass->hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  pDlgItmClass->hInstance     = hInstance;
  pDlgItmClass->style         = CS_HREDRAW | CS_VREDRAW;
  pDlgItmClass->lpfnWndProc   = DlgItmWndProc;

  if (!RegisterClass((LPWNDCLASS)pDlgItmClass))
    return FALSE;

  LocalFree((HANDLE)pDlgItmClass);

  hWnd = CreateWindow((LPSTR)"gdlgitm", (LPSTR)"GetDlgItem",
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
long FAR PASCAL DlgItmWndProc(hWnd, message, wParam, lParam)
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
{
  switch (message)
    {
    case WM_COMMAND:
      switch (wParam)
        {
        case ID_DLGMENU:
          /* Show the demo dialog box. */
          lpprocDialog = MakeProcInstance((FARPROC)DialogBoxProc, hInst);
          DialogBox(hInst, MAKEINTRESOURCE(ITEMBOX), hWnd, lpprocDialog);
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
