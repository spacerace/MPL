/*
 *  PeekMessage
 *
 *  This program will demonstrate the use of PeekMessage by catching all
 *  of the WM_PAINT messages and beeping, using MessageBeep, when there
 *  is one in the queue.
 *
 */

#include "windows.h" 
#include "peekmsg.h" 

HANDLE hInst;

int	PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
int	nCmdShow;
{
  BOOL bStatus;
  HWND hWnd;
  MSG msg;

  if (!hPrevInstance)
    if (!PeekMessageInit(hInstance))
      return (NULL);

  hInst = hInstance;   /* Saves the current instance      */

  hWnd = CreateWindow((LPSTR)"PeekMessage", (LPSTR)"PeekMessage Sample Application",  
      WS_OVERLAPPEDWINDOW,  
      CW_USEDEFAULT,  0,
      CW_USEDEFAULT,  0,
      NULL,  NULL,  hInstance, NULL);

  if (!hWnd)
    return (NULL);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  while (GetMessage(&msg, NULL, NULL, NULL))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    bStatus = PeekMessage(&msg,   /*  Message Structure  */
                          hWnd,   /*  Handle of window receiving the message  */
                          WM_PAINT,  /*  Lowest Message to examine  */
                          WM_PAINT,  /*  Highest Message to examine  */
                          PM_NOREMOVE);  /*  Don't remove the message  */

/*  To get a WM_PAINT message to be sent, size the window in different ways.
 *  Using the Diagonal sizing seems to give the best results  */

    if (bStatus)
      MessageBeep(MB_OK);  /*  We got a message, beep!!!  */
  }
  return (msg.wParam);    /* Returns the value from PostQuitMessage */
}


BOOL PeekMessageInit(hInstance)
HANDLE hInstance;
{
  HANDLE hMemory;          /* handle to allocated memory */
  PWNDCLASS pWndClass;
  BOOL bSuccess;          /* RegisterClass() result     */

  hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
  pWndClass = (PWNDCLASS) LocalLock(hMemory);

  pWndClass->style = NULL;
  pWndClass->lpfnWndProc = PeekMessageWndProc;
  pWndClass->hInstance = hInstance;
  pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
  pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
  pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
  pWndClass->lpszMenuName = (LPSTR) NULL;
  pWndClass->lpszClassName = (LPSTR) "PeekMessage";

  bSuccess = RegisterClass(pWndClass);

  LocalUnlock(hMemory);       /* Unlocks the memory    */
  LocalFree(hMemory);        /* Returns it to Windows */

  return (bSuccess);   /* Returns result of registering the window */
}


long	FAR PASCAL PeekMessageWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
{
  FARPROC lpProcAbout;
  HMENU hMenu;

  switch (message)
  {
  case WM_SYSCOMMAND:
    if (wParam == ID_ABOUT)
    {
      lpProcAbout = MakeProcInstance(About, hInst);

      DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);

      FreeProcInstance(lpProcAbout);
      break;
    }

    else
      return (DefWindowProc(hWnd, message, wParam, lParam));

  case WM_CREATE:

    hMenu = GetSystemMenu(hWnd, FALSE);

/* Add a separator to the menu */

    ChangeMenu(hMenu, NULL, NULL, NULL, MF_APPEND | MF_SEPARATOR);

/* Add new menu item to the System menu */

    ChangeMenu(hMenu, NULL, "A&bout PeekMessage...", ID_ABOUT,  
        MF_APPEND | MF_STRING);
    break;

  case WM_DESTROY:    /* message: window being destroyed */
    PostQuitMessage(0);
    break;

  default:
    return (DefWindowProc(hWnd, message, wParam, lParam));
  }
  return (0L);
}


BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned	message;
WORD wParam;
LONG lParam;
{
  switch (message)
  {
  case WM_INITDIALOG:     /* message: initialize dialog box */
    return (TRUE);

  case WM_COMMAND:
    if (wParam == IDOK)
    {
      EndDialog(hDlg, NULL);
      return (TRUE);
    }
    break;
  }
  return (FALSE);
}


