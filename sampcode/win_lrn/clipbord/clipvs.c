/*
 *
 *  SetClipboardViewer
 *
 *  clipvs.c
 *  
 *  This program demonstrates the use of the SetClipboardViewer function.
 *  This function adds the window specified by handle to the window to the
 *  chain of windows that are notifed whenever the contents of the clipboard
 *  have changed.
 *  
 */

#include <windows.h>

long    FAR PASCAL WndProc (HANDLE, unsigned, WORD, LONG);

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
    rClass.lpszClassName = (LPSTR) "clipvs";

    if (!RegisterClass (&rClass))
      return FALSE;
    }
  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, (LPSTR)"Add", 100, MF_APPEND);

  hWnd = CreateWindow ("clipvs",
                      "SetClipBoardViewer",
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
  HWND  hWndNext;

  switch (iMessage)
    {
    case WM_COMMAND:
      if (wParam == 100)
        {
        MessageBox (NULL, (LPSTR)"Add window to clipboard message chain",
                   (LPSTR)"SetClipBoardViewer", MB_OK);
        hWndNext = SetClipboardViewer (hWnd);
        if (hWnd == GetClipboardViewer ())
          MessageBox (NULL, (LPSTR)"Window added the message chain",
                     (LPSTR)"SetClipBoardViewer", MB_OK);
        else
          MessageBox (NULL, (LPSTR)"Window not added the message chain",
                     (LPSTR)"SetClipBoardViewer", MB_OK);
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
