/*
 * Function (s) demonstrated in this program: FrameRect
 * Compiler version: C 5.1
 *
 * Description:
 *    FrameRect draws a border around a specified rectangle using
 *    a specified brush.
 */

#include "windows.h"

int     PASCAL WinMain (HANDLE, HANDLE, LPSTR, int);
BOOL FrameRectInit (HANDLE);
long    FAR PASCAL FrameRectWndProc (HWND, unsigned, WORD, LONG);

HANDLE hInst;

/**************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;           /* current instance             */
HANDLE hPrevInstance;       /* previous instance            */
LPSTR lpCmdLine;            /* command line                 */
int     nCmdShow;               /* show-window type (open/icon) */
  {
  HWND hWnd;              /* window handle                */
  MSG msg;                /* message                      */

  if (!hPrevInstance)     /* Has application been initialized? */
    if (!FrameRectInit (hInstance))
      return (NULL);  /* Exits if unable to initialize     */

  hInst = hInstance;      /* Saves the current instance        */

  hWnd = CreateWindow ("FrameRect",   /* window class            */
                      "FrameRect Sample Application", /* window name             */
                      WS_OVERLAPPEDWINDOW,           /* window style            */
                      CW_USEDEFAULT,                 /* x position              */
                      CW_USEDEFAULT,                 /* y position              */
                      CW_USEDEFAULT,                 /* width                   */
                      CW_USEDEFAULT,                 /* height                  */
                      NULL,                          /* parent handle           */
                      NULL,                          /* menu or child ID        */
                      hInstance,                     /* instance                */
                      NULL);                         /* additional info         */

  if (!hWnd)
    return (NULL);

  ShowWindow (hWnd, nCmdShow);        /* Shows the window        */
  UpdateWindow (hWnd);                /* Sends WM_PAINT message  */

  while (GetMessage (&msg, NULL, NULL, NULL))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return (msg.wParam);
  }


/*************************************************************************/

BOOL FrameRectInit (hInstance)
HANDLE hInstance;
  {
  HANDLE hMemory;
  PWNDCLASS pWndClass;
  BOOL bSuccess;

  hMemory = LocalAlloc (LPTR, sizeof (WNDCLASS));
  pWndClass = (PWNDCLASS) LocalLock (hMemory);

  pWndClass->style = NULL;
  pWndClass->lpfnWndProc = FrameRectWndProc;
  pWndClass->hInstance = hInstance;
  pWndClass->hIcon = NULL;
  pWndClass->hCursor = LoadCursor (NULL, IDC_ARROW);
  pWndClass->hbrBackground = GetStockObject (WHITE_BRUSH);
  pWndClass->lpszMenuName = (LPSTR) "FrameRectMenu";
  pWndClass->lpszClassName = (LPSTR) "FrameRect";

  bSuccess = RegisterClass (pWndClass);

  LocalUnlock (hMemory);
  LocalFree (hMemory);

  return (bSuccess);
  }


/**************************************************************************/

long    FAR PASCAL FrameRectWndProc (hWnd, message, wParam, lParam)
HWND      hWnd;
unsigned  message;
WORD      wParam;
LONG      lParam;
  {
  FARPROC lpProcAbout;        /* pointer to the "About" function */
  RECT  rRect;                /* Will hold client rectangle  */
  HANDLE hDC;                 /* Handle to the display context  */
  PAINTSTRUCT ps;             /* Paint Structure  */
  HBRUSH hPaintBrush;         /* Brush to draw border with  */

  switch (message)
    {
    case WM_SIZE:
    case WM_PAINT:
      GetClientRect (hWnd, (LPRECT) & rRect); /*  Structure holding  */
      InvalidateRect (hWnd, (LPRECT) & rRect, TRUE);
        /*  Erase the background  */
      hDC = BeginPaint (hWnd, &ps);   /*  Get the display context  */
        /*  Make a border inside of the window  */
      rRect.right -= 10;
      rRect.left += 10;
      rRect.top += 10;
      rRect.bottom -= 10;
      hPaintBrush = GetStockObject (BLACK_BRUSH);
        /* Get a dark gray brush */
      FrameRect (hDC, (LPRECT) & rRect, hPaintBrush);
        /* Draw the border with a BLACK_BRUSH */
      EndPaint (hWnd, &ps);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return (DefWindowProc (hWnd, message, wParam, lParam));
    }
  return (NULL);
  }
