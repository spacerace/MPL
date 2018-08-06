/*
 *
 *  clipown.c
 *  
 *  This program demonstrates the use of the GetClipboardOwner functions.
 *  This function retrieves the window handle of the current ower of clip-
 *  board.
 *  
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
    rClass.lpszMenuName  = NULL;
    rClass.lpszClassName = "clipown";

    if (!RegisterClass (&rClass))
      return FALSE;
    }
  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, (LPSTR)"Clipboard Owner", 100, MF_APPEND);

  hWnd = CreateWindow ("clipown",
                      "GetClipboardOwner",
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
unsigned	iMessage;
WORD     wParam;
LONG     lParam;
  {
  HWND  hClipWnd;

  switch (iMessage)
    {
    case WM_COMMAND:
      if (wParam == 100)
        {
        MessageBox (NULL, (LPSTR)"Getting clipboard owner",
                    (LPSTR)"GetClipboardOwner", MB_OK);
        hClipWnd = GetClipboardOwner ();
        if (hClipWnd)
          MessageBox (NULL, (LPSTR)"Clipboard has an owner",
                     (LPSTR)"GetClipboardOwner", MB_OK);
        else
          MessageBox (NULL, (LPSTR)"Clipboard does not has an owner",
                     (LPSTR)"GetClipboardOwner", MB_OK);
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
