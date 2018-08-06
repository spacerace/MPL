/*
 *
 *  Functions demonstrated in this program: FloodFill
 *  Compiler version: C 5.1
 *
 *  This program demonstrates the use of FloodFill function.
 *  This function fills an area of the suface with the current
 *  brush. The area is assumed to be bounded by the given 
 *  rgbColor. The function begins at the point specified by
 *  the logical coordinates X, Y and continues in all directions
 *  to the color boundary.
 *
 */

#include <windows.h>

static HANDLE hInst;
static char szFileName[] = "floodfil";
static char szFuncName[] = "FloodFill";

long FAR PASCAL WindowProc (HANDLE, unsigned, WORD, LONG);

/**************************************************************************/

int PASCAL WinMain  (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
  {
  MSG  msg;

  WindowInit (hInstance, hPrevInstance, cmdShow);
  while (GetMessage ( (LPMSG)&msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG)&msg);
    DispatchMessage ( (LPMSG)&msg);
    }
  exit (msg.wParam);
  }
/**************************************************************************/

BOOL WindowInit (hInstance, hPrevInstance, cmdShow)
HANDLE hInstance, hPrevInstance;
int cmdShow;
  {
  HWND  hWnd;

  if (!hPrevInstance)
     {
     WNDCLASS rClass;

     rClass.style         = CS_HREDRAW | CS_VREDRAW;
     rClass.lpfnWndProc   = WindowProc;
     rClass.cbClsExtra    = 0;
     rClass.cbWndExtra    = 0;
     rClass.hInstance     = hInstance;
     rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
     rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
     rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
     rClass.lpszMenuName  = (LPSTR) NULL;
     rClass.lpszClassName = (LPSTR) szFileName;
   
     if (!RegisterClass ( ( LPWNDCLASS) &rClass))
       return FALSE;
     }

  hInst = hInstance;

  hWnd = CreateWindow ( (LPSTR) szFileName, (LPSTR) szFuncName,
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      (HWND) NULL, (HMENU) NULL,
                      (HANDLE) hInstance, (LPSTR) NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  return TRUE;
  }
/**************************************************************************/

long FAR PASCAL WindowProc (hWnd, message, wParam, lParam)
HWND        hWnd;
unsigned    message;
WORD        wParam;
LONG        lParam;
  {
  PAINTSTRUCT ps;

  switch (message)
    {
    case WM_PAINT:
      BeginPaint (hWnd, (LPPAINTSTRUCT)&ps);
      FunctionDemonstrated (hWnd, (PAINTSTRUCT *)&ps);
      EndPaint (hWnd, (LPPAINTSTRUCT)&ps);
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return (DefWindowProc (hWnd, message, wParam, lParam));
      break;
    }
  return (0L);
  }

/*************************************************************************/
/*              FUNCTION DEMONSTRATED HERE - LOOK HERE                   */
/*************************************************************************/

FunctionDemonstrated (hWnd, pps)
HWND hWnd;
PAINTSTRUCT *pps;
  {
  HDC hDC = pps->hdc;
  HPEN hPen;
  HPEN hMyPen;
  HBRUSH hBrush;
  HBRUSH hMyBrush;
  BOOL bFilled;

/*  Creating a rectangle to fill */
  hMyPen = CreatePen (0, 1, RGB (0, 0, 255));
  hPen = SelectObject (hDC, hMyPen);
  Rectangle (hDC, 0, 0, 500, 200);
  hMyPen = SelectObject (hDC, hPen);
  DeleteObject (hMyPen);

/* Getting a brush to do the filling */
  hMyBrush = GetStockObject (GRAY_BRUSH);
  hBrush = SelectObject (hDC, hMyBrush);

  FloodFill (hDC, 52, 48, RGB (0, 0, 255));

  hMyBrush = SelectObject (hDC, hBrush);

  return TRUE;
  }
