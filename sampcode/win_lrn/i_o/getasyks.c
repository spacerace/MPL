/*
 * GetAsyncKeyState
 *
 * This function determines whether a key is up or down, and
 * whether the key was pressed after a previous call to the
 * GetAsyncKeyState function.
 *
 */

#include <windows.h>

long FAR PASCAL AsyncWndProc(HWND, unsigned, WORD, LONG);

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    nCmdShow;
{
  HWND     hWnd;
  WNDCLASS wndclass;
  MSG      msg;

  if (!hPrevInstance) 
    {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = AsyncWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = "getasyks";

    if (!RegisterClass(&wndclass))
         return FALSE;
    }

  hWnd = CreateWindow ((LPSTR)"getasyks",
                       (LPSTR)"GetAsyncKeyState()",
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, 0,
                       CW_USEDEFAULT, 0,
                       NULL, NULL,
                       hInstance, NULL);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  while (GetMessage(&msg, NULL, 0, 0))
    {
     TranslateMessage(&msg);
     DispatchMessage(&msg);
    }

  return (msg.wParam);     
}

long FAR PASCAL AsyncWndProc(hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
{
 switch(iMessage)
   {
   int Keydown;
   PAINTSTRUCT ps;
   HDC hDC;

   case WM_CHAR:
     Keydown = GetAsyncKeyState(VK_F12);
     if ((Keydown & 0x8000) == 0x8000)
       MessageBox(hWnd, (LPSTR)"'F12' is down.",
                  (LPSTR)"GetAsyncKeyState()", MB_OK);
     else if ((Keydown & 0x0001) == 0x0001)
       MessageBox(hWnd,
                  (LPSTR)"'F12' pressed since last GetAsyncState() call.",
                  (LPSTR)"GetAsyncKeyState()", MB_OK);
     else
       MessageBox(hWnd,
                  (LPSTR)"'F12' not pressed since last GetAsyncState() call.",
                  (LPSTR)"GetAsyncKeyState()", MB_OK);
     break;

  case WM_PAINT:
     hDC = BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
     TextOut(hDC, (short)10, (short)10,
             (LPSTR)"Hit space bar while pressing/not pressing 'F12' to test",
             (short)55);
     EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
     break;
    
  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc (hWnd, iMessage, wParam, lParam);
    break;
  }
  return (0L); 
}

