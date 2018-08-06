/*
 * Function (s) demonstrated in this program: OffsetClipRgn
 * Compiler version: C 5.1
 * Description: This program creates a rectangle region, gets it location,
 *              frames the region, offsets the region, displays what kind
 *              of region it is, get it new loction of the region, frames
 *              the new location, and displays the difference of the offset.
 */

#include <windows.h>
#include <stdio.h>
#include "offclipr.h"

static char	szFuncName[] = "OffsetClipRgn";
static char	szAppName[] = "OFFCLIPR";

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE     hInstance, hPrevInstance;
LPSTR      lpszCmdLine;
int        nCmdShow;
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
    wndclass.lpszMenuName  = (LPSTR)"OFFCLIP";
    wndclass.lpszClassName = szFuncName;

    if (!RegisterClass (&wndclass))
      return FALSE;
    }

  hMenu = LoadMenu (hInstance, (LPSTR)"OFFCLIP");

  hWnd = CreateWindow (szFuncName, szAppName,
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
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  HDC         hDC;
  HRGN        hRgnBox;
  HBRUSH      hBlueBrush;
  HBRUSH      hRedBrush;
  RECT        rectBefore;
  RECT        rectAfter;
  short	nRgnType;
  int	nIndex;
  char	szBuffer [300];

  switch (iMessage)
    {
    case WM_INITMENU:
      InvalidateRect (hWnd, (LPRECT)NULL, TRUE);
      break;

    case WM_COMMAND:
      if (wParam == IDM_BOX)
        {
        hRgnBox = CreateRectRgn (50, 75, 125, 150);

        hBlueBrush = CreateSolidBrush (RGB (0, 0, 255));
        hRedBrush = CreateSolidBrush (RGB (255, 0, 0));
        if (hBlueBrush == NULL || hRedBrush == NULL)
          {
          MessageBox (GetFocus (), (LPSTR)"Error in Creating brushes.",
              (LPSTR)szFuncName, MB_OK | MB_ICONHAND);
          return (0L);
          }

        hDC = GetDC (hWnd);

        nRgnType = SelectClipRgn (hDC, hRgnBox);
        if (nRgnType == ERROR)
          {
          MessageBox (GetFocus (), (LPSTR)"Error in selecting clip region.",
              (LPSTR)szFuncName, MB_OK | MB_ICONHAND);
          return (0L);
          }

        nRgnType = GetClipBox (hDC, (LPRECT) & rectBefore);
        if (nRgnType == ERROR)
          {
          MessageBox (GetFocus (), (LPSTR)"Error in getting clip box.",
              (LPSTR)szFuncName, MB_OK | MB_ICONHAND);
          return (0L);
          }
        FrameRect (hDC, (LPRECT) & rectBefore, hBlueBrush);

        MessageBox (GetFocus (), (LPSTR)"Going to offset clip region.",
            (LPSTR)szFuncName, MB_OK);

        nRgnType = OffsetClipRgn (hDC, 200, 50);
        switch (nRgnType)
          {
          case COMPLEXREGION:
            MessageBox (GetFocus (), (LPSTR)"Offset clip region is complex.",
                (LPSTR)szFuncName, MB_OK);
            break;
          case SIMPLEREGION:
            MessageBox (GetFocus (), (LPSTR)"Offset clip region is simple.",
                (LPSTR)szFuncName, MB_OK);
            break;
          case NULLREGION:
            MessageBox (GetFocus (), (LPSTR)"There is no offset clip region.",
                (LPSTR)szFuncName, MB_OK);
            break;
          case ERROR:
            MessageBox (GetFocus (), (LPSTR)"Error in offset clip region.",
                (LPSTR)szFuncName, MB_OK | MB_ICONHAND);
            break;
          default :
            MessageBox (GetFocus (), (LPSTR)"Error !!!!",
                (LPSTR)szFuncName, MB_OK | MB_ICONHAND);
            break;
          }

        nRgnType = GetClipBox (hDC, (LPRECT) & rectAfter);
        if (nRgnType == ERROR)
          {
          MessageBox (GetFocus (), (LPSTR)"Error in getting clip box.",
              (LPSTR)szFuncName, MB_OK | MB_ICONHAND);
          return (0L);
          }
        FrameRect (hDC, (LPRECT) & rectAfter, hRedBrush);

        nIndex = sprintf (szBuffer,
            "%s %d %s %d\n%s %d %s %d\n%s %d %s %d\n%s %d %s %d",
            "clip box .left moved from", rectBefore.left,
            "to", rectAfter.left,
            "clip box .right moved from", rectBefore.right,
            "to", rectAfter.right,
            "clip box .top moved from", rectBefore.top,
            "to", rectAfter.top,
            "clip box .bottom moved from", rectBefore.bottom,
            "to", rectAfter.bottom);
        MessageBox (GetFocus (), (LPSTR)szBuffer, (LPSTR)szFuncName, MB_OK);

        ReleaseDC (hWnd, hDC);  /*  Clean up Memory  */
        DeleteObject (hRgnBox);
        DeleteObject (hRedBrush);
        DeleteObject (hBlueBrush);
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
