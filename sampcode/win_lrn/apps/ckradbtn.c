/*
 *   This program demonstrates the use of the CheckRadioButton() function.
 *   CheckRadioButton() checks one radio button and unchecks all other radio
 *   buttons with identifiers in the range specified by the 2nd and 3rd
 *   parameters. CheckRadioButton() is called from DialogBoxProc() in this
 *   sample application.
 */

#include "windows.h"
#include "ckradbtn.h"

HANDLE hInst;

FARPROC lpprocDialog;
long    FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL DialogBoxProc(HWND, unsigned, WORD, LONG);

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
        case ID_RADIO1:
        case ID_RADIO2:
        case ID_RADIO3:
        case ID_RADIO4:
          CheckRadioButton(hDlg, ID_RADIO1, ID_RADIO4, wParam);
              /* check the chosen radio button, uncheck the rest */
          break;

        case ID_OK:
          EndDialog(hDlg, TRUE);
          break;
        }
      break;

    default:
      return FALSE;
      break;
    }
  }


/* Procedure called when the application is loaded for the first time */
BOOL HelloInit(hInstance)
HANDLE hInstance;
  {
  PWNDCLASS   pHelloClass;

  pHelloClass = (PWNDCLASS)LocalAlloc(LPTR, sizeof(WNDCLASS));

  pHelloClass->hCursor        = LoadCursor(NULL, IDC_ARROW);
  pHelloClass->hIcon             = LoadIcon(hInstance, NULL);
  pHelloClass->lpszMenuName	 = (LPSTR)"min";
  pHelloClass->lpszClassName	 = (LPSTR)"CheckRadioButton";
  pHelloClass->hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
  pHelloClass->hInstance      = hInstance;
  pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
  pHelloClass->lpfnWndProc    = HelloWndProc;

  if (!RegisterClass((LPWNDCLASS)pHelloClass))
    return FALSE;

  LocalFree((HANDLE)pHelloClass);
  return TRUE;        /* Initialization succeeded */
  }


int     PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MSG   msg;
  HWND  hWnd;

  HelloInit(hInstance);
  hWnd = CreateWindow((LPSTR)"CheckRadioButton",
                      (LPSTR)"CheckRadioButton()",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      (HWND)NULL,        /* no parent */
                      (HMENU)NULL,       /* use class menu */
                      (HANDLE)hInstance, /* handle to window instance */
                      (LPSTR)NULL );     /* no params to pass on */
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
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
  {
  HMENU  hMenu;

  switch (message)
    {
    case WM_CREATE:
      hMenu = CreateMenu ();  /*  Create a Menu  */
      ChangeMenu (hMenu, NULL, "Dialog", 100, MF_APPEND);
      SetMenu (hWnd, hMenu);
      break;

    case WM_COMMAND:
      if (wParam == 100)
        {
        lpprocDialog = MakeProcInstance((FARPROC)DialogBoxProc, hInst);
        DialogBox(hInst, MAKEINTRESOURCE(2), hWnd, lpprocDialog);
        FreeProcInstance((FARPROC)lpprocDialog);
        break;
        }
      else
        return DefWindowProc(hWnd, message, wParam, lParam);
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
