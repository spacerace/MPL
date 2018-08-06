/*
 *  This program demonstrates the use of ChangeClipboardChain function.
 *  This funtion removes the window specified by a handle from the chain
 *  of clipboard viewers and makes the window next in the viewer chain the
 *  descendent of the removing window's ancestor.
 */

#include <windows.h>

long    FAR PASCAL WndProc (HANDLE, unsigned, WORD, LONG);

static char	szAppName[] = "clipchan";
static char	szFuncName[] = "ChangeClipboardChain";

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
  {
  WNDCLASS rClass;
  HWND     hWnd;
  MSG      msg;
  HMENU    hMenu;

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

  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, (LPSTR)"Clipboard Chain", 100, MF_APPEND);

  hWnd = CreateWindow (szAppName,            /* window class name       */
                      szFuncName,                 /* window caption          */
                      WS_OVERLAPPEDWINDOW,        /* window style            */
                      CW_USEDEFAULT,              /* initial x position      */
                      0,                          /* initial y position      */
                      CW_USEDEFAULT,              /* initial x size          */
                      0,                          /* initial y size          */
                      NULL,                       /* parent window handle    */
                      hMenu,                      /* window menu handle      */
                      hInstance,                  /* program instance handle */
                      NULL);                     /* create parameters       */

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
HWND        hWnd;
unsigned    iMessage;
WORD        wParam;
LONG        lParam;
  {
  HMENU  hMenu;
  HWND   hWndStatic;
  BOOL   bRemoved;

  switch (iMessage)
    {
    case WM_COMMAND:
      if (wParam == 100)
        {
        hWndStatic = SetClipboardViewer (hWnd);
        MessageBox (NULL,
            (LPSTR)"Removing current window from the viewer chain",
            (LPSTR)szFuncName, MB_OK);
        bRemoved = ChangeClipboardChain (hWnd, hWndStatic);
        if (bRemoved != 0)
          MessageBox (NULL,
              (LPSTR)"Current window removed from the viewer chain",
              (LPSTR)szFuncName, MB_OK);
        else
          MessageBox (NULL,
              (LPSTR)"Current window not removed from viewer chain",
              (LPSTR)szFuncName, MB_OK);
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
