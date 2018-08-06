/*
 *
 *   Functions (s) demonstrated in this program: FreeResource
 *   Compiler version: C 5.1
 *
 *   Description:
 *      This program demonstrates the use of the function FreeResource.
 *      FreeResource removes a loaded resource from memory by freeing the
 *      allocated memory occupied by that resource.
 */

#include "windows.h"
#include "freeres.h"
#include "string.h"

char	szAppName[10];
static HANDLE hInst;
static short    xChar, yChar;

long    FAR PASCAL FreeresWndProc (HWND, unsigned, WORD, LONG);
void FreeresPaint (HWND hWnd, HDC hDC);
BOOL FreeresInit (HANDLE hInstance);

/***************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
  {
  MSG   msg;
  HWND  hWnd;

  if (!hPrevInstance)
    {
    if (!FreeresInit (hInstance))
      return FALSE;
    }
  else 
    {
         /* Copy data from previous instance */
    GetInstanceData (hPrevInstance, (PSTR)szAppName, 10);
    }
  hWnd = CreateWindow (szAppName, "FreeResource",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      NULL, NULL, hInstance, NULL);
  hInst = hInstance;

    /* Make window visible according to the way the app is activated */
  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  return msg.wParam;
  }

/* Procedures which make up the window class. */
long    FAR PASCAL FreeresWndProc (hWnd, message, wParam, lParam)
HWND      hWnd;
unsigned  message;
WORD      wParam;
LONG      lParam;
  {
  PAINTSTRUCT  ps;
  TEXTMETRIC   tm;
  HDC          hDC;

  switch (message)
    {
    case WM_CREATE:
        {
        hDC = GetDC (hWnd);
        GetTextMetrics (hDC, &tm);
        ReleaseDC (hWnd, hDC);
        xChar = tm.tmAveCharWidth;
        yChar = tm.tmHeight + tm.tmExternalLeading;
        break;
        }
    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    case WM_PAINT:
      BeginPaint (hWnd, (LPPAINTSTRUCT) & ps);
      FreeresPaint (hWnd, ps.hdc);
      EndPaint (hWnd, (LPPAINTSTRUCT) & ps);
      break;

    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
    }
  return (0L);
  }

/* Procedure called when the application is loaded for the first time */
BOOL FreeresInit (hInstance)
HANDLE hInstance;
  {
  PWNDCLASS	pFreeresClass;

/* Load strings from resource */
  LoadString (hInstance, IDSNAME, szAppName, 10);

  pFreeresClass = (PWNDCLASS)LocalAlloc (LPTR, sizeof (WNDCLASS));
  pFreeresClass->hCursor       = LoadCursor (NULL, IDC_ARROW);
  pFreeresClass->hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
  pFreeresClass->lpszMenuName  = NULL;
  pFreeresClass->lpszClassName = szAppName;
  pFreeresClass->hbrBackground = GetStockObject (WHITE_BRUSH);
  pFreeresClass->hInstance     = hInstance;
  pFreeresClass->style         = CS_HREDRAW | CS_VREDRAW;
  pFreeresClass->lpfnWndProc   = FreeresWndProc;

  if (!RegisterClass ( (LPWNDCLASS)pFreeresClass))
    return FALSE;

  LocalFree ( (HANDLE)pFreeresClass);
  return TRUE;
  }

/***************************************************************************/
void FreeresPaint (hWnd, hDC)
HWND hWnd;
HDC  hDC;
  {
  GLOBALHANDLE hBitmap1Data; /* Handle to global memory containing bitmap */
  HBITMAP hBitmap1Info;
  BOOL nResult;

/* This section demonstrates the use of FreeResource */

/* Locate the bitmap in the resource file and identify it with a handle */
  hBitmap1Info = FindResource (hInst, "face1", RT_BITMAP);

/* Identify the global memory block to receive the bitmap data */
  hBitmap1Data = LoadResource (hInst, hBitmap1Info);

  nResult = FreeResource (hBitmap1Info);         /* Free allocated memory */

  if (!nResult)
    TextOut (hDC, xChar, yChar, "FreeResource did not work", 25);
  else
    TextOut (hDC, xChar, yChar, "FreeResource worked", 19);
  }
