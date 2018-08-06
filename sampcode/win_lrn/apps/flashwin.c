/*
 *
 *  Function demonstrated in this program: FlashWindow
 *  Compiler version: C 5.1
 *
 *  This program demonstrates the use of the function FlashWin. This function    
 *  "flashes" the given window once. Flashing window means changing the 
 *  appearance of its caption bar or icon as if the window were changing form 
 *  inactive to active status, or vice versa. (A grey caption bar or unframed
 *  incon changes to a black caption bar or framed icon; a black caption bar 
 *  or framed icon changes to a grey caption bar or unframed icon.)
 *
 */

#include <windows.h>

LONG FAR PASCAL WindowProc (HANDLE, unsigned, WORD, LONG);

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
  {
  MSG  msg;
  HWND hWnd;
  HMENU hMenu;

  if (!hPrevInstance)
    {
    WNDCLASS rClass;

    rClass.style         = CS_HREDRAW | CS_VREDRAW;
    rClass.lpfnWndProc   = WindowProc;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;
    rClass.hInstance     = hInstance;
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = (LPSTR) NULL;
    rClass.lpszClassName = (LPSTR) "flashwin";

    if (!RegisterClass ( ( LPWNDCLASS) &rClass))
      return FALSE;
    }

  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, "Flash!", 100, MF_APPEND);

  hWnd = CreateWindow ( (LPSTR) "flashwin", (LPSTR) "FlashWindow",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      (HWND) NULL, (HMENU) hMenu,
                      (HANDLE) hInstance, (LPSTR) NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);
  
  while (GetMessage ( (LPMSG)&msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG)&msg);
    DispatchMessage ( (LPMSG)&msg);
    }
  exit (msg.wParam);
  }

long FAR PASCAL WindowProc (hWnd, message, wParam, lParam)
HWND        hWnd;
unsigned    message;
WORD        wParam;
LONG        lParam;
  {
  static BOOL bInverted = TRUE;  /* To keep track of Active State of Window */

  switch (message)
    {
    case WM_COMMAND:
      if (wParam == 100)
        FlashWindow (hWnd, TRUE);
      else
        return (DefWindowProc (hWnd, message, wParam, lParam));
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return (DefWindowProc (hWnd, message, wParam, lParam));
      break;
    }
  return (0L);
  }
