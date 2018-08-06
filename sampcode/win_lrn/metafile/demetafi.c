/*
 *  Function Name:   DeleteMetaFile
 *  Program Name:    demetafi.c
 *  Special Notes:   Needs CRMETAFI.MET file to run.
 *
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.0
 *
 *  Description:
 *   The program below used to free up memory the metafile has been using.
 *   The metafile may be recalled from the disk into memory by using the
 *   GetMetaFile function.  The program below draws a rectangle, then
 *   releases the metafile handle.
 */

#include "windows.h"

long	FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

int	PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE	  hInstance, hPrevInstance;
LPSTR	  lpszCmdLine;
int	cmdShow;
{
  MSG	     msg;
  HWND	     hWnd;
  WNDCLASS   wcClass;

  if (!hPrevInstance)
  {
    wcClass.style	   = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = WndProc;
    wcClass.cbClsExtra	   = 0;
    wcClass.cbWndExtra	   = 0;
    wcClass.hInstance	   = hInstance;
    wcClass.hIcon	   = LoadIcon(hInstance, NULL);
    wcClass.hCursor	   = LoadCursor(NULL, IDC_ARROW);
    wcClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"DeleteMetaFile";

    if (!RegisterClass((LPWNDCLASS) & wcClass))
      return FALSE;
  }

  hWnd = CreateWindow("DeleteMetaFile",
      "DeleteMetaFile()",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      NULL,
      NULL,
      hInstance,
      NULL);

  ShowWindow(hWnd, cmdShow);
  UpdateWindow(hWnd);
  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}


long	FAR PASCAL WndProc(hWnd, message, wParam, lParam)
HWND	   hWnd;
unsigned	message;
WORD	   wParam;
LONG	   lParam;
{
  PAINTSTRUCT ps;
  HDC	      hDC;
  HANDLE      hMetaFile;

  switch (message)
  {
  case WM_PAINT:
    {
      BeginPaint(hWnd, &ps);
      hDC = ps.hdc;
      hMetaFile = GetMetaFile("demetafi.met");
      PlayMetaFile(hDC, hMetaFile);
      if (!DeleteMetaFile(hMetaFile))
	MessageBox(hWnd, "DeleteMetaFile failed", "ERROR", MB_ICONHAND);
      else
	TextOut(hDC, 20, 20, "Metafile handle released", 24);
      ValidateRect(hWnd, NULL);
      EndPaint(hWnd, &ps);
      break;
    }
  case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return(0L);
}


