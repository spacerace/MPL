/*
 *
 *  MakeProcInstance
 *  
 *  This program demonstrates the use of the functions MakeProcInstance.
 *  MakeProcInstance binds the segment of the model instance to the
 *  function pointed to by lpProc.
 *
 *  Other references: DlgOpen.c, DlgSave.c, Print.c
 *
 */

#include <windows.h>

static char	szAppName[] = "makeproc";
static char	szFuncName[] = "MakeProcInstance";
HANDLE hInst;

long	FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);
long	FAR PASCAL TestFarFunc (HWND, unsigned, WORD, LONG);

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
{
  HWND     hWnd;
  WNDCLASS rClass;
  MSG      msg;

  if (!hPrevInstance)
  {
    rClass.style         = CS_HREDRAW | CS_VREDRAW;
    rClass.lpfnWndProc   = WndProc;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;
    rClass.hInstance     = hInstance;
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = NULL;
    rClass.lpszClassName = szAppName;

    if (!RegisterClass (&rClass))
      return FALSE;
  }

  hInst = hInstance;

hWnd = CreateWindow (szAppName,            /* window class name       */
szFuncName,                 /* window caption          */
WS_OVERLAPPEDWINDOW,        /* window style            */
CW_USEDEFAULT,              /* initial x position      */
0,                          /* initial y position      */
CW_USEDEFAULT,              /* initial x size          */
0,                          /* initial y size          */
NULL,                       /* parent window handle    */
NULL,                       /* window menu handle      */
hInstance,                  /* program instance handle */
  NULL);                      /* create parameters       */

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage ((LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage ((LPMSG) & msg);
    DispatchMessage ((LPMSG) & msg);
  }
  return (msg.wParam);
}


long	FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned	iMessage;
WORD     wParam;
LONG     lParam;
{
  HMENU   hMenu;
  FARPROC lpProc;

  switch (iMessage)
  {
  case WM_CREATE:
    hMenu = CreateMenu ();
    ChangeMenu (hMenu, NULL, (LPSTR)"Make", 100, MF_APPEND);
    SetMenu (hWnd, hMenu);
    break;
  case WM_COMMAND:
    if (wParam == 100)
    {
      MessageBox (GetFocus(), (LPSTR)"Making instance proc.",
          (LPSTR)szFuncName, MB_OK);
      lpProc = MakeProcInstance ((FARPROC)TestFarFunc, hInst);
      if (lpProc != NULL)
	MessageBox (GetFocus(), (LPSTR)"Instance proc. made",
	    (LPSTR)szFuncName, MB_OK);
      else
	MessageBox (GetFocus(), (LPSTR)"Instance proc. not made",
	    (LPSTR)szFuncName, MB_OK);
      FreeProcInstance (lpProc);
    }
    break;
  case WM_DESTROY:
    PostQuitMessage (0);
    break;
  default:
    return (DefWindowProc (hWnd, iMessage, wParam, lParam));
  }
  return (0L);
}


long	FAR PASCAL TestFarFunc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned	iMessage;
WORD     wParam;
LONG     lParam;
{
  return TRUE;
}


