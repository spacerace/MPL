/*
 *
 *  GetClipboardViewer
 *
 *  clipvg.c
 *  
 *  This program demonstrates the use of GetClipboardViewer function.
 *  This funtion retrieves the window handle of the first window in the
 *  clipboard viewer chain.
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
    rClass.lpszClassName = "clipvg";

    if (!RegisterClass (&rClass))
      return FALSE;
    }
  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, (LPSTR)"Retrieve", 100, MF_APPEND);

  hWnd = CreateWindow ("clipvg",
                      "GetClipboardViewer",
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
HWND       hWnd;
unsigned   iMessage;
WORD       wParam;
LONG       lParam;
  {
  HWND  hWndViewChain;

  switch (iMessage)
    {
    case WM_COMMAND:
      if (wParam == 100)
        {
        SetClipboardViewer (hWnd);
        MessageBox (hWnd,
                    "Retreving frist window in clipboard viewer message chain",
                    "GetClipboardViewer", MB_OK);
        hWndViewChain = GetClipboardViewer ();
        if (hWndViewChain)
          MessageBox (hWnd, "Got the frist window in the message chain",
                      "GetClipboardViewer", MB_OK);
        else
          MessageBox (hWnd, "No window in the message chain",
                      "GetClipboardViewer", MB_OK);
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