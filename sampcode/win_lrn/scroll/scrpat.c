/*
 *  Function(s) demonstrated in this program: UnrealizeObject, SetBrushOrg,
 *                                            PatBlt
 *  Compiler version: 5.1
 *  Description:
 *     This program will set up a cross hatched pattern on the client
 *  area of a window.  It will then scroll it using the two above functions
 *  in conjunction with timer messages.
 */

#include <windows.h>

int     iTimerCount;
HBRUSH  hNewBrush;
RECT    rRect;

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance ;
LPSTR       lpszCmdLine ;
int         nCmdShow ;
  {
  static char szAppName [] = "ScrPat" ;
  HWND        hWnd ;
  WNDCLASS    wndclass ;
  MSG msg;
  HDC hDC;

  if (!hPrevInstance)
    {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
    wndclass.lpfnWndProc   = WndProc ;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = NULL;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName  = NULL ;
    wndclass.lpszClassName = szAppName ;

    if (!RegisterClass (&wndclass))
      return FALSE ;
    }

  hWnd = CreateWindow (szAppName,            /* window class name       */
                 "ScrPat Sample Application",  /* window caption          */
                 WS_OVERLAPPEDWINDOW,        /* window style            */
                 CW_USEDEFAULT,              /* initial x position      */
                 0,                          /* initial y position      */
                 CW_USEDEFAULT,              /* initial x size          */
                 0,                          /* initial y size          */
                 NULL,                       /* parent window handle    */
                 NULL,                       /* window menu handle      */
                 hInstance,                  /* program instance handle */
                 NULL) ;                     /* create parameters       */

  /* Set up the background brush. */
  hNewBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(0, 0, 255));
  hDC = GetDC(hWnd);
  GetClientRect( hWnd, (LPRECT) &rRect );
  ReleaseDC(hWnd, hDC);

  /* Start up the timer to roll the pattern. */
  SetTimer(hWnd, 100, 133, 0L);

  ShowWindow (hWnd, nCmdShow) ;
  UpdateWindow (hWnd) ;

  while (GetMessage(&msg, NULL, 0, 0))
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    } 
  return (msg.wParam) ;     
  }

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned iMessage ;
WORD     wParam ;
LONG     lParam ;
  {
  HDC         hDC;
  HMENU       hMenu;
  PAINTSTRUCT ps;

  switch(iMessage)
    {
    case WM_CREATE:
      break;

    case WM_ERASEBKGND:
        /* Don't let it paint the background. */
        return((long)TRUE);

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );

        SelectObject(ps.hdc, GetStockObject(NULL_BRUSH));
        UnrealizeObject(hNewBrush);
        SetBrushOrg(ps.hdc, 0, iTimerCount);
        SelectObject(ps.hdc, hNewBrush);

        PatBlt(ps.hdc,
               ps.rcPaint.left,
               ps.rcPaint.top,
               ps.rcPaint.right - ps.rcPaint.left,
               ps.rcPaint.bottom - ps.rcPaint.top,
               PATCOPY);

        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    case WM_TIMER:
        if (wParam == 100 ) {

            iTimerCount = (iTimerCount+1) % 64;

            hDC = GetDC(hWnd);

            SelectObject(hDC, GetStockObject(NULL_BRUSH));
            UnrealizeObject(hNewBrush);
            SetBrushOrg(hDC, 0, iTimerCount);
            SelectObject(hDC, hNewBrush);

            PatBlt(hDC,
                   rRect.left,
                   rRect.top,
                   rRect.right,
                   rRect.bottom,
                   PATCOPY);

            ReleaseDC(hWnd, hDC);

            break;
            }

    case WM_DESTROY:
      {
      PostQuitMessage(0);
      break;
      }
    default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
    }
    return (0L); 
  }
