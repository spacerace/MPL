/*
 *
 *  Function (s) demonstrated in this program: FindResource
 *  Compiler version: C 5.1
 *
 *  Description:
 *     This program demonstrates the use of the function FindResource. 
 *     This function determines the location of a resource in the specified
 *     resource file.
 * 
 *  Other references: Clock.c
 *  
 */

#include <windows.h>
#include "findres.h"

static HANDLE hInst;
static char	szFileName[] = "FindRes";
static char	szFuncName[] = "FindResource";

long FAR PASCAL WindowProc (HANDLE, unsigned, WORD, LONG);
BOOL FAR PASCAL FunctionDemonstrated (HWND);

/***************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
  {
  MSG  msg;

  WindowInit (hInstance, hPrevInstance, cmdShow);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  exit (msg.wParam);
  }

/***************************************************************************/

BOOL WindowInit (hInstance, hPrevInstance, cmdShow)
HANDLE hInstance, hPrevInstance;
int	cmdShow;
  {
  HWND  hWnd;

  if (!hPrevInstance)
    {
    WNDCLASS rClass;

    rClass.style	    = CS_HREDRAW | CS_VREDRAW;
    rClass.lpfnWndProc   = WindowProc;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;
    rClass.hInstance     = hInstance;
    rClass.hCursor          = LoadCursor (NULL, IDC_ARROW);
    rClass.hIcon            = LoadIcon (hInstance, szFileName);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = szFileName;
    rClass.lpszClassName = szFileName;

    RegisterClass ( (LPWNDCLASS) & rClass);
    }

  hInst = hInstance;

  hWnd = CreateWindow (szFileName, szFuncName,
                      WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance,
                      NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  return TRUE;
  }

/***************************************************************************/

long    FAR PASCAL WindowProc (hWnd, iMessage, wParam, lParam)
HWND	    hWnd;
unsigned    iMessage;
WORD	    wParam;
LONG	    lParam;
  {
  HMENU  hMenu;

  switch (iMessage)
    {
    case WM_CREATE:
      hMenu = CreateMenu ();     /*  Add a menu item */
      ChangeMenu (hMenu, NULL, (LPSTR)"FindResource", 100, MF_APPEND);
      SetMenu (hWnd, hMenu);
      break;

    case WM_COMMAND:
      if (wParam == 100)  /*  Our Menu item  */
        FunctionDemonstrated (hWnd);
      else
        return (DefWindowProc (hWnd, iMessage, wParam, lParam));
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return (DefWindowProc (hWnd, iMessage, wParam, lParam));
      break;
    }
  return (0L);
  }

/***************************************************************************/

BOOL FAR PASCAL FunctionDemonstrated (hWnd)
HWND hWnd;
  {
  MessageBox (NULL, "Looking for resource FindRes", szFuncName, MB_OK);
  if (FindResource (hInst, "FindRes", RT_ICON))
    MessageBox (NULL, "Resource found", szFuncName, MB_OK);
  else
    MessageBox (NULL, "Resource not found", szFuncName, MB_OK);
  return (TRUE);
  }
