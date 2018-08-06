/*
 *  PostMessage
 *
 *  This example program will use the PostMessage function to send a
 *  WM_SYSCOMMAND message to the main window to make it iconic.  The
 *  wParam parameter will be set to SC_MINIMIZE to accomplish this.
 *
 */

#include "windows.h" 
#include "postmsg.h" 

HANDLE hInst;

int	PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
int	nCmdShow;
{
  HWND hWnd;
  MSG msg;
  HANDLE hMenu;

  if (!hPrevInstance)
    if (!PostMsgInit(hInstance))
      return (NULL);

  hInst = hInstance;

  hMenu = LoadMenu(hInst, (LPSTR)"PostMsgMenu");


  hWnd = CreateWindow((LPSTR)"PostMsg", (LPSTR)"PostMessage Sample Application",  /* window name       */
  WS_OVERLAPPEDWINDOW,  
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, hMenu, hInstance, NULL);

  if (!hWnd)
    return (NULL);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  while (GetMessage(&msg,     
      NULL,               
      NULL,               
      NULL))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return (msg.wParam);
}


BOOL PostMsgInit(hInstance)
HANDLE hInstance;
{
  HANDLE hMemory;
  PWNDCLASS pWndClass;
  BOOL bSuccess;

  hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
  pWndClass = (PWNDCLASS) LocalLock(hMemory);

  pWndClass->style = NULL;
  pWndClass->lpfnWndProc = PostMsgWndProc;
  pWndClass->hInstance = hInstance;
  pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
  pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
  pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
  pWndClass->lpszMenuName = (LPSTR) "PostMsgMenu";
  pWndClass->lpszClassName = (LPSTR) "PostMsg";

  bSuccess = RegisterClass(pWndClass);

  LocalUnlock(hMemory);
  LocalFree(hMemory);

  return (bSuccess);
}


long	FAR PASCAL PostMsgWndProc(hWnd, message, wParam, lParam)
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

    ChangeMenu(hMenu, NULL, "A&bout PostMsg...", ID_ABOUT,           
        MF_APPEND | MF_STRING);
    break;

  case WM_COMMAND:
    if (wParam == IDM_ICONIZE)
    {
      PostMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, (LONG) NULL);
    }
    break;

/*  PostMessage places the message into the applications message queue and
 *  does not wait for a response.  This is different from the SendMessage
 *  function in that SendMessage does wait.  The Window Procedure is called
 *  immediately when SendMessage is used.  For more information see
 *  PostMessage and SendMessage in the Programmer's Reference.
 */
  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return (DefWindowProc(hWnd, message, wParam, lParam));
  }
  return (NULL);
}


BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned	message;
WORD wParam;
LONG lParam;
{
  switch (message)
  {
  case WM_INITDIALOG:
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


