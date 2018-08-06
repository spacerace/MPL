/*
 *  Function (s) demonstrated in this program: GetClipbox
 *  Description: This function retrieves the dimensions of the tightest 
 *               bounding rectangle around the current clipping boundary.  
 *               The dimensions are copied to the buffer pointed to by the
 *               last parameter.
 */

#include <windows.h>
#include <stdio.h>

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

HDC hDC;

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE     hInstance, hPrevInstance;
LPSTR      lpszCmdLine;
int        cmdShow;
  {
  HWND      hWnd;
  WNDCLASS  rClass;
  MSG       msg;

  if (!hPrevInstance)
    {
    rClass.style         = CS_HREDRAW | CS_VREDRAW;
    rClass.lpfnWndProc   = WndProc;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;
    rClass.hInstance     = hInstance;
    rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = NULL;
    rClass.lpszClassName = "CLIPBOX";

    if (!RegisterClass (&rClass))
      return FALSE;
    }

  hWnd = CreateWindow ("CLIPBOX",
      "GetClipBox",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      0,
      CW_USEDEFAULT,
      0,
      NULL,
      NULL,
      hInstance,
      NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);
  hDC = GetDC (hWnd);

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
  HMENU  hMenu;
  HRGN   hRgnBox;
  HRGN   hRgnCircle;
  HRGN   hRgnBoth;
  RECT   lpRect;
  short  nRgnType;

  switch (iMessage)
    {
    case WM_CREATE:
      hMenu = CreateMenu ();
      ChangeMenu (hMenu, NULL, (LPSTR)"CheckType", 100, MF_APPEND);
      ChangeMenu (hMenu, NULL, (LPSTR)"Set Complex", 200, MF_APPEND);
      ChangeMenu (hMenu, NULL, (LPSTR)"Set Simple", 300, MF_APPEND);
      ChangeMenu (hMenu, NULL, (LPSTR)"Set Null", 400, MF_APPEND);
      SetMenu (hWnd, hMenu);
      break;

  case WM_COMMAND:
    switch (wParam)
      {
      case 100:
        nRgnType = GetClipBox (hDC, (LPRECT) & lpRect);
        switch (nRgnType)
          {
          case ERROR:
            MessageBox (hWnd, (LPSTR)"Device context is not valid",
                (LPSTR)"GetClipBox", MB_OK | MB_ICONEXCLAMATION);
            break;
          case NULLREGION:
            MessageBox (hWnd, (LPSTR)"Clipping region is empty",
                (LPSTR)"GetClipBox", MB_OK);
            break;
          case SIMPLEREGION:
            MessageBox (hWnd, (LPSTR)"Clipping region is simple",
                (LPSTR)"GetClipBox", MB_OK);
            break;
          case COMPLEXREGION:
            MessageBox (hWnd, (LPSTR)"Clipping region is complexed",
                (LPSTR)"GetClipBox", MB_OK);
            break;
          }
        InvalidateRect (hWnd, (LPRECT)NULL, TRUE);
        break;

      case 200:
        hRgnBox = CreateRectRgn (50, 75, 125, 150);
        hRgnCircle = CreateEllipticRgn (50, 25, 150, 125);
        hRgnBoth = CreateRectRgn (0, 0, 0, 0);
        CombineRgn (hRgnBoth, hRgnBox, hRgnCircle, RGN_XOR);
        SelectClipRgn (hDC, hRgnBoth);
        break;

      case 300:
        hRgnBox = CreateRectRgn (200, 225, 225, 250);
        SelectClipRgn (hDC, hRgnBox);
        break;

      case 400:
        hRgnBox = CreateRectRgn (0, 0, 0, 0);
        SelectClipRgn (hDC, hRgnBox);
        break;
      }
    break;

    case WM_DESTROY:
      ReleaseDC (hWnd, hDC);
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam);
    }
  return (0L);
  }
