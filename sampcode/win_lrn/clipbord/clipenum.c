/*
 *  This program demonstrates the use of EnumClipboardFormats function.
 *  This function numbers a list of formats and returns the next format in
 *  the list.
 *  
 */

#include <windows.h>

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

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
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = NULL;
    rClass.lpszClassName = "clipenum";

    if (!RegisterClass (&rClass))
      return FALSE;
    }

  hWnd = CreateWindow ("clipenum",            /* window class name       */
                      "EnumClipboardFormats",                 /* window caption          */
                      WS_OVERLAPPEDWINDOW,        /* window style            */
                      CW_USEDEFAULT,              /* initial x position      */
                      0,                          /* initial y position      */
                      CW_USEDEFAULT,              /* initial x size          */
                      0,                          /* initial y size          */
                      NULL,                       /* parent window handle    */
                      NULL,                       /* window menu handle      */
                      hInstance,                  /* program instance handle */
                      NULL);                      /* create parameters       */

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
  int	nNextFormat;

  switch (iMessage)
    {
    case WM_CREATE:
      hMenu = CreateMenu ();
      ChangeMenu (hMenu, NULL, (LPSTR)"Number Formats", 100, MF_APPEND);
      SetMenu (hWnd, hMenu);
      break;

    case WM_COMMAND:
      if (wParam == 100)
        {
        MessageBox (NULL, (LPSTR)"Getting first format in list",
                   (LPSTR)"EnumClipboardFormats", MB_OK);
        nNextFormat = EnumClipboardFormats (0);
        if (nNextFormat == 0)
          MessageBox (NULL, (LPSTR)"First format returned",
                     (LPSTR)"EnumClipboardFormats", MB_OK);
        else
          MessageBox (NULL, (LPSTR)"First format not returned",
                     (LPSTR)"EnumClipboardFormats", MB_OK);
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
