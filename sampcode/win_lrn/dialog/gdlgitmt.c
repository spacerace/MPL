/*
 *   GetDlgItemText
 *
 *   This program demonstrates the use of the GetDlgItemTxt function.
 *   GetDlgItem text retrieves the caption of or the text associated
 *   with a control in a dialog box. GetDlgItemTxt is called from
 *   DialogBoxProc in this application.
 *
 */

#include "windows.h"
#include "gdlgitmt.h"

long FAR PASCAL ItemTWndProc(HWND, unsigned, WORD, LONG);

HANDLE  hInst;        /* Instance handle.              */
FARPROC lpprocDialog; /* Dialog procedure pointer.     */

BOOL FAR PASCAL DialogBoxProc(hDlg, message, wParam, lParam)
HWND     hDlg;
unsigned message;
WORD     wParam;
LONG     lParam;
{
  int  nNumChars;  /* Number of characters entered in the edit field. */
  char szBuff[80]; /* Buffer for edit control text.                   */

  switch (message)
    {
    case WM_INITDIALOG:
      SetFocus(GetDlgItem(hDlg, EDIT_CONTROL));
      return FALSE;

    case WM_COMMAND:
      if (wParam == ID_OK)
        {
        /* Get the text entered in the dialog box control. */
        nNumChars = GetDlgItemText(hDlg, EDIT_CONTROL,
                                   (LPSTR)szBuff, (int)80);
        if (nNumChars == 0) /* If no text, say it. */
          MessageBox(GetFocus(),
                     (LPSTR)"No characters entered in edit field",
                     (LPSTR)"GetDlgItemText()",
                     MB_OK);
        else /* Otherwise, show the text. */
          MessageBox(GetFocus(),(LPSTR)szBuff,
                     (LPSTR)"GetDlgItemText() Obtained:",
                     MB_OK);
        EndDialog(hDlg,TRUE);
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
  MSG       msg;         /* Message structure.      */
  HWND      hWnd;        /* Window handle.          */
  PWNDCLASS pItemTClass; /* Window class structure. */

  pItemTClass = (PWNDCLASS)LocalAlloc(LPTR, sizeof(WNDCLASS));

  pItemTClass->hCursor        = LoadCursor(NULL, IDC_ARROW);
  pItemTClass->hIcon          = LoadIcon(hInstance,NULL);
  pItemTClass->lpszMenuName   = (LPSTR)"textmenu";
  pItemTClass->lpszClassName  = (LPSTR)"gdlgitmt";
  pItemTClass->hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
  pItemTClass->hInstance      = hInstance;
  pItemTClass->style          = CS_HREDRAW | CS_VREDRAW;
  pItemTClass->lpfnWndProc    = ItemTWndProc;

  if (!RegisterClass((LPWNDCLASS)pItemTClass))
    return FALSE;
  LocalFree((HANDLE)pItemTClass);

  hWnd = CreateWindow((LPSTR)"gdlgitmt",
                      (LPSTR)"GetDlgItemText",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      (HWND)NULL,
                      (HMENU)NULL,
                      (HANDLE)hInstance,
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

long FAR PASCAL ItemTWndProc(hWnd, message, wParam, lParam)
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
        case ID_TEXTBOX:
          lpprocDialog = MakeProcInstance((FARPROC)DialogBoxProc, hInst);
          DialogBox(hInst, MAKEINTRESOURCE(TEXTBOX), hWnd, lpprocDialog);
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
