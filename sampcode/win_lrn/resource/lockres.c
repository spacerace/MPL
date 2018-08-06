/*
 * Function (s) demonstrated in this program: LockResource
 *
 * Description:
 * This program uses a text file to store text which is brought in at
 * run time.  The functions FindResource and LoadResource are used to
 * place this text into memory.  Once it is in memory the function
 * LockResource is used to create a long pointer to the text so it can
 * be accessed.  LockResource also locks the text into the current
 * memory block so the long pointer will not become invalid.  Then the
 * text is massaged and output via a messagebox.
 *
*/

#include <windows.h>
#include "lockres.h"

static char	szFuncName [] = "LockResource";
static char	szAppName  [] = "LOCKRES";
static HANDLE   hInst;

/***************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance;
HANDLE      hPrevInstance;
LPSTR       lpszCmdLine;
int	nCmdShow;
  {
  HWND        hWnd;
  WNDCLASS    wndclass;
  MSG         msg;
  HMENU       hMenu;

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
    wndclass.lpszMenuName  = (LPSTR)"Lock";
    wndclass.lpszClassName = szFuncName;

    if (!RegisterClass (&wndclass))
      return FALSE;
    }

  hInst = hInstance;
  hMenu = LoadMenu (hInstance, (LPSTR)"Lock");

hWnd = CreateWindow (szFuncName,           /* window class name       */
szAppName,                  /* window caption          */
WS_OVERLAPPEDWINDOW,        /* window style            */
CW_USEDEFAULT,              /* initial x position      */
0,                          /* initial y position      */
CW_USEDEFAULT,              /* initial x size          */
0,                          /* initial y size          */
NULL,                       /* parent window handle    */
hMenu,                      /* window menu handle      */
hInstance,                  /* program instance handle */
  NULL);                      /* create parameters       */

  ShowWindow (hWnd, nCmdShow);
  UpdateWindow (hWnd);

  while (GetMessage (&msg, NULL, 0, 0))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return (msg.wParam);
  }


/***************************************************************************/

long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  HDC         hDC;
  HANDLE      hResource;
  LPSTR       lpText;
  LPSTR       lpTextTrunc;
  int	nIndex1;
  int	nIndex2;

  switch (iMessage)
    {
    case WM_INITMENU:
      InvalidateRect (hWnd, NULL, TRUE);
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case IDM_RES:
          hDC = GetDC (hWnd);
    /* Move the text into memory */
          hResource = LoadResource (hInst, FindResource (hInst,"Sample", "TEXT"));

    /* Locking the text and creating a long pointer to it */
          lpText = LockResource (hResource);

    /* Massaging the text */
          lpTextTrunc = lpText;
          while (*lpTextTrunc != '\0' && *lpTextTrunc++ != '~');
            if (*lpTextTrunc != '\0')
              {
              lpTextTrunc--;
              *lpTextTrunc = '\0';
              }

    /* Outputting the text */
          MessageBox (hWnd, lpText, (LPSTR)szFuncName, MB_OK);

          FreeResource (hResource);
          ReleaseDC (hWnd, hDC);
          break;

        default:
          return DefWindowProc (hWnd, iMessage, wParam, lParam);
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
