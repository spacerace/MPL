/*
 * Function (s) demonstrated in this program: CopyRect
 *
 * Description:  This function copies the contents of one rectangle structure
 *     into another.
 *
 */

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#include <math.h>

char	szRadius  [15];
HANDLE  hInstMain;
HWND    hWndMain;

long FAR PASCAL WndProc                (HWND, unsigned, WORD, LONG);
void            DrawSquare             (HDC, RECT);

/****************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE  hInstance, hPrevInstance;
LPSTR	lpszCmdLine;
int	nCmdShow;
  {
  static char	szAppName[] = "CopyRect";
  HWND	hWnd;
  MSG 	msg;
  WNDCLASS	wndclass;

  if (!hPrevInstance)
    {
    wndclass.style          = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc    = WndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = 0;
    wndclass.hInstance      = hInstance;
    wndclass.hIcon          = NULL; /*LoadIcon (NULL, IDI_ASTERISK);*/
    wndclass.hCursor        = LoadCursor (NULL, IDC_CROSS);
    wndclass.hbrBackground  = GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName   = NULL;
    wndclass.lpszClassName  = szAppName;

    if (!RegisterClass (&wndclass))
      return FALSE;
    }
  hWnd = CreateWindow (szAppName, "CopyRect",
                      WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
                      CW_USEDEFAULT, 0, NULL, NULL,
                      hInstance, NULL);

  hInstMain = hInstance;
  hWndMain  = hWnd;

  ShowWindow (hWnd, nCmdShow);
  UpdateWindow (hWnd);

  while (GetMessage (&msg, NULL, 0, 0))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return msg.wParam;
  }


/****************************************************************************/

long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND	    hWnd;
unsigned    iMessage;
WORD	    wParam;
LONG	    lParam;
  {
  static RECT   Rect1, Rect2;
  HDC	        hDC;
  PAINTSTRUCT   ps;
  int   xOffset, yOffset;

  switch (iMessage)
    {
    case WM_CREATE:
      SetRectEmpty (&Rect1);
      SetRectEmpty (&Rect2);

      Rect1.top = 10;
      Rect1.left = 15;
      Rect1.right = 30;
      Rect1.bottom = 20;
      break;

    case WM_PAINT:
      SetRectEmpty (&Rect1);
      SetRectEmpty (&Rect2);
      Rect1.top = 10;
      Rect1.left = 15;
      Rect1.right = 30;
      Rect1.bottom = 20;

      hDC = BeginPaint (hWnd, &ps);
      CopyRect (&Rect2, &Rect1);      /*  Copy the contents of Rect1
                                       *  to Rect2  */

      for (xOffset = 0; xOffset < 15; xOffset++)
        {
        Rect1.top += 10;
        Rect1.left += 15;
        Rect1.bottom += 11;
        Rect1.right += 17;
        DrawSquare (hDC, Rect1);
        }
      DrawSquare (hDC, Rect2);
      EndPaint (hWnd, &ps);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam);
    }
  return 0L;
  }


/***************************************************************************/

void DrawSquare (hDC, rect)
HDC       hDC;
RECT      rect;
  {
  MoveTo (hDC, rect.left,  rect.top);
  LineTo (hDC, rect.right, rect.top);
  LineTo (hDC, rect.right, rect.bottom);
  LineTo (hDC, rect.left,  rect.bottom);
  LineTo (hDC, rect.left,  rect.top);
  }
