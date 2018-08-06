/*
 *  This program demonstrates the use of CountClipboardFormats function.
 *  This function retieves a count of the number of formats that the clip-
 *  board can render.
 */

#include <windows.h>
#include <stdio.h>

static char	szAppName[] = "clipcoun";
static char	szFuncName[] = "CountClipboardFormats";

long    FAR PASCAL WndProc (HANDLE, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
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
    rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = (LPSTR) NULL;
    rClass.lpszClassName = (LPSTR) szAppName;

    if (!RegisterClass (&rClass))
      return FALSE;
    }

  hWnd = CreateWindow (szAppName,
                      szFuncName,
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      0,
                      CW_USEDEFAULT,
                      0,
                      NULL,
                      NULL,
                      hInstance,
                      NULL);

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
  int	nCount;
  char	szBuff[80];

  switch (iMessage)
    {
    case WM_CREATE:
      hMenu = CreateMenu ();
      ChangeMenu (hMenu, NULL, (LPSTR)"Clipboard Formats", 100, MF_APPEND);
      SetMenu (hWnd, hMenu);
      break;

    case WM_COMMAND:
      if (wParam == 100)
        {
        MessageBox (NULL, (LPSTR)"Counting clipboard formats",
                   (LPSTR)szFuncName, MB_OK);
        nCount = CountClipboardFormats ();
        sprintf (szBuff, "There is %d clipboard formats right now.", nCount);
        MessageBox (NULL, (LPSTR)szBuff, (LPSTR)szFuncName, MB_OK);
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
