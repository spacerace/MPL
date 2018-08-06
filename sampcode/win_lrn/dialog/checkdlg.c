/*
 *
 *   checkdlg.c
 *
 *   This program demonstrates the use of the CheckDlgButton() function.
 *   CheckDlgButton() checks, unchecks, or disables dlg controls (where
 *   applicable). CheckDlgButton() is called from DialogBoxProc() in
 *   this sample application.
 *
 */

#include "windows.h"
#include "checkdlg.h"

HANDLE hInst;

FARPROC lpprocDialog;
long    FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);

BOOL FAR PASCAL DialogBoxProc(hDlg, message, wParam, lParam)
HWND      hDlg;
unsigned  message;
WORD      wParam;
LONG      lParam;
  {
  switch (message)
    {
    case WM_INITDIALOG:
      return TRUE;
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case ID_BUTTON1:
          break;

        case ID_BUTTON2:
          /* gray button with ID_BUTTON2 id */
          CheckDlgButton(hDlg, ID_BUTTON1, 2);
          break;

        case ID_BUTTON3:
          /* uncheck button with ID_BUTTON2 id */
          CheckDlgButton(hDlg, ID_BUTTON1, 0);
          break;

        case ID_BUTTON4:
          /* check button with ID_BUTTON2 id */
          CheckDlgButton(hDlg, ID_BUTTON1, 1);
          break;

        case ID_OK:
          EndDialog(hDlg, TRUE);
          break;
        }
      break;

    default:
      return FALSE;
    }
  }

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit(hInstance)
HANDLE hInstance;
  {
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc(LPTR, sizeof(WNDCLASS));

  pHelloClass->hCursor        = LoadCursor(NULL, IDC_ARROW);
  pHelloClass->hIcon          = LoadIcon(hInstance, NULL);
  pHelloClass->lpszMenuName   = (LPSTR)"min";
  pHelloClass->lpszClassName  = (LPSTR)"CheckDlgButton";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc    = HelloWndProc;

  if (!RegisterClass((LPWNDCLASS)pHelloClass))
    return FALSE;     /* Initialization failed */

  LocalFree((HANDLE)pHelloClass);
  return TRUE;        /* Initialization succeeded */
  }


int     PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  MSG   msg;
  HWND  hWnd;

  HelloInit(hInstance);
  hWnd = CreateWindow((LPSTR)"CheckDlgButton",
                      (LPSTR)"CheckDlgButton()",
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

  ShowWindow(hWnd, cmdShow);
  UpdateWindow(hWnd);

  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
    }
  return (int)msg.wParam;
  }


/* Procedures which make up the window class. */
long    FAR PASCAL HelloWndProc(hWnd, message, wParam, lParam)
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
  {
  switch (message)
    {
    case WM_COMMAND:
      lpprocDialog = MakeProcInstance((FARPROC)DialogBoxProc, hInst);
      DialogBox(hInst, MAKEINTRESOURCE(2), hWnd, lpprocDialog);
      FreeProcInstance((FARPROC)lpprocDialog);
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
