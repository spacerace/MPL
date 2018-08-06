/*
 * Name: allocres.c
 * Function (s) demonstrated : AllocResource (), FindResource (),
 *                             SizeofResource (), AccessResource ().
 * Windows version    : 2.03
 * Compiler version   : 5.1
 * Description        :
 *         FindResource is used first to locate and identify the resource
 *         item.  SizeofResource is used to determine how many bytes need
 *         to be read to obtain the resource item, AllocResource () is used
 *         to allocate memory for the resource, and finally AccessResource ()
 *         allows us to read the resource in directly.
 *
 */

#include <windows.h>
#include "AllocRes.h"

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);
int     FAR PASCAL _lclose (int);
int     FAR PASCAL _lread (HANDLE, LPSTR, short);

static char	szBuffer[8];
static int	ErrorCheck;
static char	szResName [] = "HotStuff";

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance;
LPSTR       lpszCmdLine;
int         nCmdShow;
  {
  static char szAppName [] = "Resources";
  HWND        hWnd;
  WNDCLASS    wndclass;
  MSG         msg;

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

  hWnd = CreateWindow (szAppName, "The Joy of Resources",
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
HWND      hWnd;
unsigned  iMessage;
WORD      wParam;
LONG      lParam;
  {
  static HANDLE hInstance, hResource, hGlobRes;
  int	hFile;
  char  far *szStuff, szSuccess[20];
  short	sRes;
  HMENU  hMenu;

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
        case IDM_EXECUTE:
      /*  FindResource is used first to locate and identify the resource
       *  item.  SizeofResource is used to determine how many bytes need
       *  to be read to obtain the resource item, AllocResource () reserves
       *  the memory to store the resource and AccessResource () permits
       *  reading of the resource.
       */
          hResource = FindResource (hInstance, "HotStuff", RT_RCDATA);
          sRes = SizeofResource (hInstance, hResource);
          hGlobRes = AllocResource (hInstance, hResource, (DWORD)sRes);
          ErrorCheck = sprintf (szStuff, "%i", hGlobRes);
          MessageBox (GetFocus (), szStuff, "hGlobRes returns...", MB_OK);
          szStuff = GlobalLock (hGlobRes);
          hFile = AccessResource (hInstance, hResource);
          ErrorCheck = _lread (hFile, (LPSTR)szStuff, sRes);
          ErrorCheck = GlobalUnlock (hGlobRes);
          hGlobRes = GlobalFree (hGlobRes);
          ErrorCheck = _lclose (hFile);
          if (hFile < 1)
            {
            ErrorCheck = sprintf (szStuff, "It failed");
            ErrorCheck = sprintf (szSuccess, "Error");
            }
	  else
            ErrorCheck = sprintf (szSuccess, "Item read");
          MessageBox (GetFocus (), szStuff, szSuccess, MB_OK);
          break;

        default:
          return DefWindowProc (hWnd, iMessage, wParam, lParam);
        }

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam);
    }
  return (0L);
  }
