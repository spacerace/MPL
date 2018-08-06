/*
 *  This program demonstrates the use of CloseClipboard function.
 *  This function close the clipboard. ClipClipboard should be called when
 *  a window has finished examining or changing the clipboard. It lets
 *  other application access the clipboard.
 */

#include <windows.h>

static char	szAppName[] = "clipclos";
static char	szFuncName[] = "CloseClipboard";

long    FAR PASCAL WndProc (HANDLE, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
  {
  HWND      hWnd;
  WNDCLASS  rClass;
  MSG       msg;
  HMENU     hMenu;

  if (!hPrevInstance)
    {
    rClass.style         = CS_HREDRAW | CS_VREDRAW;
    rClass.lpfnWndProc   = WndProc;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;
    rClass.hInstance     = hInstance;
    rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = (LPSTR) NULL;
    rClass.lpszClassName = (LPSTR) szAppName;

    if (!RegisterClass (&rClass))
      return FALSE;
    }

  hMenu = CreateMenu ();  /*  Add a Menu Item  */
  ChangeMenu (hMenu, NULL, (LPSTR)"Close Clipboard", 100, MF_APPEND);

  hWnd = CreateWindow (szAppName,          /* window class name       */
                      szFuncName,          /* window caption          */
                      WS_OVERLAPPEDWINDOW, /* window style            */
                      CW_USEDEFAULT,       /* initial x position      */
                      0,                   /* initial y position      */
                      CW_USEDEFAULT,       /* initial x size          */
                      0,                   /* initial y size          */
                      NULL,                /* parent window handle    */
                      hMenu,               /* window menu handle      */
                      hInstance,           /* program instance handle */
                      NULL);               /* create parameters       */

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage (&msg, NULL, 0, 0))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return (msg.wParam);
  }


long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND      hWnd;
unsigned  iMessage;
WORD      wParam;
LONG      lParam;
  {
  HMENU  hMenu;
  BOOL   bClosed;

  switch (iMessage)
    {
    case WM_COMMAND:
      if (wParam == 100)
        {
        OpenClipboard (hWnd);
        MessageBox (NULL, (LPSTR)"Closing clipboard", (LPSTR)szFuncName,
            MB_OK);
        bClosed = CloseClipboard ();
        if (bClosed != 0)
          MessageBox (NULL, (LPSTR)"Clipboard closed", (LPSTR)szFuncName,
               MB_OK);
        else
          MessageBox (NULL, (LPSTR)"Clipboard not closed", (LPSTR)szFuncName,
              MB_OK);
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
