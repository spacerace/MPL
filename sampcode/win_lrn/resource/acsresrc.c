/*
 * Name: acsresrc.c
 * Function (s) demonstrated in this program: AccessResource (), FindResource (),
 *                                           SizeofResource ().
 * Description  :
 *         FindResource is used first to locate and identify the resource
 *         item.  SizeofResource is used to determine how many bytes need
 *         to be read to obtain the resource item, and finally
 *         AccessResource () is used to get the handle and set the file
 *         pointer to the beginning of the selected resource.  AccessResource ()
 *         opens the file (resource) as a result, so it must be explicitely
 *         closed.  In this case, "It works!" is displayed in a MessageBox ()
 *         upon successful completion, "It failed" displays if unsuccessful.
 * Additional Comments: Note that _lclose () must be used to close the file
 *         (resource).
 */

#include <windows.h>
#include "acsresrc.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);
int     FAR PASCAL _lclose (int);
int     FAR PASCAL _lread (HANDLE, LPSTR, short);

static char	szBuffer[8];
static int	ErrorCheck;
static char	szResName [] = "HotStuff";

int    PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE   hInstance, hPrevInstance;
LPSTR    lpszCmdLine;
int      nCmdShow;
  {
  static char  szAppName [] = "Resources";
  HWND       hWnd;
  WNDCLASS   wndclass;
  MSG        msg;
  HMENU      hMenu;

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

  hMenu = LoadMenu (hInstance, "ResMenu");

  hWnd = CreateWindow (szAppName, "AccessResource",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, 0,
                      CW_USEDEFAULT, 0,
                      NULL, hMenu, hInstance, NULL);

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
HWND      hWnd;
unsigned  iMessage;
WORD      wParam;
LONG      lParam;
  {
  static HANDLE hInstance, hResource;
  int	hFile;
  char  szStuff[20], szSuccess[20];
  short sRes;

  switch (iMessage)
    {
    case WM_CREATE:
      hInstance = GetWindowWord (hWnd, GWW_HINSTANCE);
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case IDM_EXECUTE:
/* FindResource is used first to locate and identify the resource
        item.  SizeofResource is used to determine how many bytes need
        to be read to obtain the resource item, and finally
        AccessResource () is used to get the handle and set the file
        pointer to the beginning of the selected resource.  AccessResource ()
        opens the file (resource) as a result, so it must be explicitely
        closed.
     */
          hResource = FindResource (hInstance, "HotStuff", RT_RCDATA);
          sRes = SizeofResource (hInstance, hResource);
          hFile = AccessResource (hInstance, hResource);
          ErrorCheck = _lread (hFile, (LPSTR)szStuff, sRes);
          ErrorCheck = _lclose (hFile);
          if (hFile < 1)
            {
            ErrorCheck = sprintf (szStuff, "It failed");
            ErrorCheck = sprintf (szSuccess, "Error");
            }
	  else
            ErrorCheck = sprintf (szSuccess, "Item read");
          MessageBox (GetFocus (), szStuff, szSuccess, MB_OK);
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
