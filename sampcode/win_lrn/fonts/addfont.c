/*
 * Name               : AddFont.c
 * Function(s) demonstrated in this program :
 *     AddFontResource (), RemoveFontResource (), SendMessage ().
 * Description        : AddFontResource loads an external file of fonts,
 *                      returning a short integer of fonts that have been
 *                      added.  SendMessage sends a WM_FONTCHANGE message
 *                      to notify other windows of the change.
 *                      RemoveFontResource is used to remove the resource
 *                      that was added.  The resource may be added multiple
 *                      times, and deleted once for every time it was added.
 *                      If you attempt to delete it furthermore, an error
 *                      message comes up indicating that the resource is not
 *                      available to delete.
 *
 * NOTE:                Script.fon is required to be in the same directory.
 *
 */

#include <windows.h>
#include "addfont.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);
int     sprintf (PSTR, PSTR, int);

static char	szBuffer[] = "script.fon";
static int	ErrorCheck;
static char	szResName [] = "ResMenu";

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance;
LPSTR       lpszCmdLine;
int         nCmdShow;
  {
  static char   szAppName [] = "FontResources";
  HWND      hWnd;
  WNDCLASS  wndclass;
  MSG       msg;

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
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass (&wndclass))
      return FALSE;
    }

  hWnd = CreateWindow (szAppName, "The Joy of FontResources",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, 0,
                      CW_USEDEFAULT, 0,
                      NULL, NULL, hInstance, NULL);

  ShowWindow (hWnd, nCmdShow);
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
  static HANDLE hInstance, hResource;
  int	hFile;
  char	szBuf[50];
  short	sRes;
  HMENU hMenu;
  switch (iMessage)
    {
    case WM_CREATE:
      hInstance = GetWindowWord (hWnd, GWW_HINSTANCE);
      hMenu = LoadMenu (hInstance, "ResMenu");
      SetMenu (hWnd, hMenu);
      DrawMenuBar (hWnd);
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case IDM_ADDFONT:
          ErrorCheck = AddFontResource ( (LPSTR)szBuffer);
          SendMessage (-1, WM_FONTCHANGE, 0, 0L);
          ErrorCheck = sprintf (szBuf, "%i", ErrorCheck);
          MessageBox (GetFocus (), szBuf, "Number of added Fonts", MB_OK);
          break;

        case IDM_REMOVEFONT:
          sRes = RemoveFontResource ( (LPSTR)szBuffer);
          if (sRes != NULL)
            {
            ErrorCheck = sprintf (szBuf, "%i:  indicating the FontResource was removed", sRes);
            MessageBox (GetFocus (), szBuf, "RemoveFontResource returns", MB_OK);
            }
	  else
            MessageBox (GetFocus (), "Problem removing Font!", "Error", MB_OK);
          SendMessage (-1, WM_FONTCHANGE, 0, 0L);
	  break;
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
