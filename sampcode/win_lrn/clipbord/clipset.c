/*
 *
 *  SetClipboardData
 *  
 *  This program demonstrates the use of the SetClipboardData function.
 *  This function sets a data handle into the clipboard for the data
 *  specified by the second parameter. The data is assumed to have the
 *  format specified by the frist parameter. After the clipboard data
 *  handle has been assigned, this function frees the block indentified
 *  by the second parameter.
 *  
 */

#include <windows.h>

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);
int     strlen (char *);

LPSTR FAR PASCAL lstrcpy (LPSTR, LPSTR);

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
    rClass.lpszClassName = "clipset";

    if (!RegisterClass (&rClass))
      return FALSE;
    }

  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, (LPSTR)"Set Data", 100, MF_APPEND);

  hWnd = CreateWindow ("clipset",
                      "SetClipboardData",
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
  HANDLE hClipData;
  HANDLE hMem;
  LPSTR  lpText;

  switch (iMessage)
    {
    case WM_COMMAND:
      if (wParam == 100)
        {
        OpenClipboard (hWnd);
        hMem = GlobalAlloc (GMEM_MOVEABLE | GMEM_ZEROINIT,
                           (long) strlen ("SetClipboardData"));
        lpText = (LPSTR) GlobalLock (hMem);
        lstrcpy (lpText, "SetClipboardData");
        EmptyClipboard ();
        MessageBox (hWnd, (LPSTR)"Setting handle to clipboard",
                    (LPSTR)"SetClipboardData", MB_OK);
        hClipData = SetClipboardData (CF_TEXT, hMem);
        if (hClipData != NULL)
          MessageBox (hWnd, (LPSTR)"Handle set", (LPSTR)"SetClipboardData",
                      MB_OK);
        else
          MessageBox (hWnd, (LPSTR)"Handle not set", (LPSTR)"SetClipboardData",
                      MB_OK);
        CloseClipboard ();
        GlobalUnlock (hMem);
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
