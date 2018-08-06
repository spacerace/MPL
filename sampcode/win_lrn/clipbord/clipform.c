/*
 *   This program demonstrates the use of the IsClipboardFormatAvailable
 *   function. IsClipboardFormatAvailable checks to see if the given
 *   data format is available in the clipboard. In this sample application,
 *   the format checked for in the clipboard is CF_BITMAP. IsClipboardFormat-
 *   Available is called from WinMain in this sample application. To test
 *   the function, bring up Paint and this application at the same time.
 *   If you cut anything from Paint, it goes to the clipboard and IsClipboard-
 *   FormatAvailable should return TRUE. If there is no bitmap in the clipboard
 *   then IsClipBoardFormatAvailable should return FALSE. Using Paint, once
 *   you have put a bitmap in the clipboard, there will be a bitmap format
 *   in the clipboard until your windows session is ended.
 */

#include <windows.h>

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  HWND      hWnd;
  WNDCLASS  wndclass;
  MSG       msg;
  HMENU     hMenu;

  if (!hPrevInstance)
    {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = "CLIPFORM";

    if (!RegisterClass (&wndclass))
      return FALSE;
    }
  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, (LPSTR)"Format", 100, MF_APPEND);

  hWnd = CreateWindow ("CLIPFORM",
                      "IsClipboardFormatAvailable",
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
  return (int)msg.wParam;
  }

long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  BOOL  bFormat;

  switch (iMessage)
    {
    case WM_COMMAND:
      if (wParam == 100)
        {
        bFormat = IsClipboardFormatAvailable (CF_BITMAP);
        if (bFormat)
          MessageBox (hWnd, (LPSTR)"There IS a bitmap format available",
                     (LPSTR)"IsClipboardFormatAvailable", MB_OK);
        else
          MessageBox (hWnd, (LPSTR)"There is NOT a bitmap format available",
                     (LPSTR)"IsClipboardFormatAvailable", MB_OK);
        }
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam);
    }
  return (0L);
  }
