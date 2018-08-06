/*
 *  
 *  This program demonstrates the use of the function PlayMetaFile.
 *  This function plays the contents of the specified metafile on the
 *  given context.
 *
 */

#include <windows.h>
#include "pmetafil.h"

static HANDLE hWnd;
static char	szResName [] = "ResMenu";
static char	szFileName[] = "pmetafil";
static char	szFuncName[] = "PlayMetaFile";

long	FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

int	PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
{
  MSG msg;
  if (!hPrevInstance)
  {
    WNDCLASS rClass;

    rClass.lpszClassName = (LPSTR)szFileName;
    rClass.hInstance     = hInstance;
    rClass.lpfnWndProc   = WndProc;
    rClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    rClass.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
    rClass.lpszMenuName  = NULL;
    rClass.hbrBackground = GetStockObject(WHITE_BRUSH);
    rClass.style         = CS_HREDRAW | CS_VREDRAW;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;

    RegisterClass((LPWNDCLASS) & rClass);
  }

  hWnd = CreateWindow((LPSTR)szFileName, (LPSTR)szFuncName,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, NULL, hInstance, NULL);

  ShowWindow(hWnd, cmdShow);
  UpdateWindow (hWnd) ;

  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return (msg.wParam) ;
}


long	FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned	iMessage ;
WORD     wParam ;
LONG     lParam ;
{
  static HANDLE hInstance;
  HMENU hMenu;
  HDC hDC;
  HANDLE hMF;
  BOOL bPlayed;
  switch (iMessage)
  {
  case WM_CREATE:
    {
      hInstance = GetWindowWord(hWnd, GWW_HINSTANCE);
      hMenu = LoadMenu(hInstance, "ResMenu");
      SetMenu(hWnd, hMenu);
      DrawMenuBar(hWnd);
      break;
    }
  case WM_COMMAND:
    {
      switch (wParam)
      {
      case IDM_EXECUTE:
	{
	  hDC = GetDC(hWnd);
	  hMF = GetMetaFile((LPSTR)"PMETAFIL.MET");

	  MessageBox(GetFocus(), (LPSTR)"About To Play Metafile",
	      (LPSTR)szFuncName, MB_OK);

	  bPlayed = PlayMetaFile(hDC, hMF);

	  if (bPlayed != 0)
	    MessageBox(GetFocus(),
	        (LPSTR)"Metafile Played Successfully (Rectangle Generated)",
	        (LPSTR)szFuncName, MB_OK);
	  else
	    MessageBox (GetFocus(), (LPSTR)"Metafile Play Error", (LPSTR)szFuncName,
	        MB_OK);

	  ReleaseDC(hWnd, hDC);
	}
      }
      break;
    }
  case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }
  default:
    {
      return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
    }
  }
  return (0L);
}


