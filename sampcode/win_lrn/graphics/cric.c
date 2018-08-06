/*
 *  Function Name:   CreateIC
 *  Program Name:    cric.c
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.0
 *
 *  Description:
 *   An information context is retrieved from the display, and the brush is
 *   extracted.  The brush information for the IC is then displayed on
 *   the screen to show that the IC has valid information.
 */

#include "windows.h"
#include "stdio.h"

long	FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_CreateIC(hWnd, hDC)
HWND hWnd;
HDC  hDC;
{
  HDC hIC;
  PLOGBRUSH pBrush;
  HBRUSH  hICBrush;
  char	szstring[80];
  int	nLength;
/*  information context created   */
  hIC = CreateIC((LPSTR)"DISPLAY", (LPSTR)NULL, (LPSTR)NULL, (LPSTR)NULL);

  if (hIC == NULL)
  {
    MessageBox(hWnd, (LPSTR)"CreateIC failed", (LPSTR)"ERROR", MB_ICONHAND);
    return;
  }

  pBrush = (PLOGBRUSH) LocalAlloc(LMEM_MOVEABLE, sizeof(LOGBRUSH));
  hICBrush = SelectObject(hIC, CreateSolidBrush((LONG)0));
  GetObject(hICBrush, 8, (LPSTR) pBrush);   /* put brush into structure */
  nLength = sprintf(szstring, "Information Context Brush Attributes are:"
      "   style=%u, color=%lx, hatch=%d ",
      pBrush->lbStyle, pBrush->lbColor, pBrush->lbHatch);
  TextOut (hDC, 10, 10, (LPSTR) szstring, nLength);

  return;
}


/**************************************************************************/

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
  WNDCLASS   wcClass;

  wcClass.style          = CS_HREDRAW | CS_VREDRAW;
  wcClass.lpfnWndProc    = WndProc;
  wcClass.cbClsExtra     = 0;
  wcClass.cbWndExtra     = 0;
  wcClass.hInstance      = hInstance;
  wcClass.hIcon          = LoadIcon( hInstance, NULL );
  wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
  wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
  wcClass.lpszMenuName   = (LPSTR)NULL;
  wcClass.lpszClassName  = (LPSTR)"CreateIC";

  if (!RegisterClass( (LPWNDCLASS) & wcClass ) )
/* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
    return FALSE;

  return TRUE;        /* Initialization succeeded */
}


int	PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
{
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;

  if (!hPrevInstance)
  {
/* Call initialization procedure if this is the first instance */
    if (!WinInit( hInstance ))
      return FALSE;
  }

  hWnd = CreateWindow((LPSTR)"CreateIC",
      (LPSTR)"CreateIC()",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      (HWND)NULL,        /* no parent */
  (HMENU)NULL,       /* use class menu */
  (HANDLE)hInstance, /* handle to window instance */
  (LPSTR)NULL        /* no params to pass on */
  );

/* Make window visible according to the way the app is activated */
  ShowWindow( hWnd, cmdShow );
  UpdateWindow( hWnd );

/* Polling messages from event queue */
  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }

  return (int)msg.wParam;
}


/* Procedures which make up the window class. */
long	FAR PASCAL WndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
{
  PAINTSTRUCT ps;

  switch (message)
  {

  case WM_PAINT:
    BeginPaint( hWnd, (LPPAINTSTRUCT) & ps );
    CALL_CreateIC(hWnd, ps.hdc);
    ValidateRect(hWnd, (LPRECT) NULL);
    EndPaint( hWnd, (LPPAINTSTRUCT) & ps );
    break;

  case WM_DESTROY:
    PostQuitMessage( 0 );
    break;

  default:
    return DefWindowProc( hWnd, message, wParam, lParam );
    break;
  }
  return(0L);
}


