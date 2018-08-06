/*
 *
 *  EmptyClipboard ()
 *
 *  This program demonstrates the use of the EmptyClipboard () function.
 *  This function empties the clipboard and frees handles to data in the
 *  clipboard. It the assigns ownership of the clipboard to the window
 *  that currently has the clipboard open. The clipboard must be open
 *  when EmptyClipboard is called.
 *  
 */

#include <windows.h>

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
    rClass.hIcon         = LoadIcon  (hInstance, IDI_APPLICATION);
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hbrBackground = GetStockObject  (WHITE_BRUSH);
    rClass.lpszMenuName  = NULL;
    rClass.lpszClassName = "clipempt";

    if (!RegisterClass (&rClass))
      return FALSE;
    }

  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, (LPSTR)"Clear Clipboard", 100, MF_APPEND);

  hWnd = CreateWindow ("clipempt",
                      "EmptyClipboard",
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
  HMENU hMenu;

  switch (iMessage)
    {
    case WM_COMMAND:
      if (wParam == 100)
        {
        OpenClipboard (hWnd);
        MessageBox (hWnd, (LPSTR)"Emptying clipboard", (LPSTR)"EmptyClipboard",
                    MB_OK);
        if (EmptyClipboard ())  /*  Empty the clipboard  */
          MessageBox (hWnd, (LPSTR)"Clipboard emptied", (LPSTR)"EmptyClipboard",
                      MB_OK);
        else
          MessageBox (hWnd, (LPSTR)"Clipboard not emptied",
                     (LPSTR)"EmptyClipboard", MB_OK);
        CloseClipboard  ();
        }
      break;

    case WM_DESTROY:
      PostQuitMessage  (0);
      break;

    default:
      return (DefWindowProc (hWnd, iMessage, wParam, lParam));
    }
  return  (0L);
  }
