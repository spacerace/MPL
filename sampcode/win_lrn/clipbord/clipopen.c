/*
 *  OpenClipboard
 *  This program demonstrates the use of the OpenClipboard function.
 *  This function opens the clipboard for examination and prevents other
 *  applications from modifying the clipboard contents.
 */

#include <windows.h>

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE     hInstance, hPrevInstance;
LPSTR      lpszCmdLine;
int        cmdShow;
  {
  HWND     hWnd;
  WNDCLASS rClass;
  MSG      msg;
  HMENU    hMenu;

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
    rClass.lpszMenuName  = (LPSTR) NULL;
    rClass.lpszClassName = (LPSTR) "clipopen";

    if (!RegisterClass (&rClass))
      return FALSE;
    }
  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, (LPSTR)"Open Clipboard", 100, MF_APPEND);

  hWnd = CreateWindow ("clipopen",
                      "OpenClipboard",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      0,
                      CW_USEDEFAULT,
                      0,
                      NULL,
                      hMenu,
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
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  BOOL  bOpened;

  switch (iMessage)
    {
    case WM_COMMAND:
      if (wParam == 100)
        {
        MessageBox (NULL, (LPSTR)"Open a clipboard", (LPSTR)"OpenClipboard",
                    MB_OK);
        bOpened = OpenClipboard (hWnd);
        if (bOpened)
          MessageBox (NULL, (LPSTR)"Clipboard is open", (LPSTR)"OpenClipboard",
                      MB_OK);
        else
          MessageBox (NULL, (LPSTR)"Clipboard is not open",
                     (LPSTR)"OpenClipboard", MB_OK);
        CloseClipboard ();
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
