/*
 *
 *  GetCapture
 *  
 *  This program demonstrates the use of the function GetCapture.
 *  This function retrieves a handle that identifies the window that
 *  has the mouse capture.
 *  
 */

#include "windows.h"

static HWND hWnd;
char szBuff [70];

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  HWND hWndPrev;
  HWND hWndGetCap;
  int  i;
  
  if (!hPrevInstance)
    {
    WNDCLASS rClass;

    rClass.lpszClassName = (LPSTR)"getcap";
    rClass.hInstance     = hInstance;
    rClass.lpfnWndProc   = DefWindowProc;
    rClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    rClass.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
    rClass.lpszMenuName  = (LPSTR)NULL;
    rClass.hbrBackground = GetStockObject(WHITE_BRUSH);
    rClass.style         = CS_HREDRAW | CS_VREDRAW;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;
   
    RegisterClass((LPWNDCLASS)&rClass);
    }

  hWnd = CreateWindow((LPSTR)"getcap", (LPSTR)"GetCapture",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      (HWND)NULL, (HMENU)NULL,
                      (HANDLE)hInstance, (LPSTR)NULL);
 
  ShowWindow(hWnd, cmdShow);

  MessageBox(GetFocus(), (LPSTR)"Getting window with mouse capture.",
             (LPSTR)"GetCapture()",
              MB_OK);
  SetCapture(hWnd);
  hWndGetCap = GetCapture();

  for (i = 0; i < 5; i++)
    {
    MessageBeep(NULL);
    FlashWindow(hWndGetCap, NULL);
    }

  MessageBox(GetFocus(), (LPSTR)"The capture window has been flashed.",
             (LPSTR)"GetCapture()",
             MB_OK);
  ReleaseCapture();
  return 0;
}
