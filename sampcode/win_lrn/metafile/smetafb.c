/*
 * This application demonstrates the SetMetaFile() Windows function.
 * The result is demonstrated on the screen.
 *
 */
#include "windows.h"

static HANDLE hInst;
HANDLE hMF;
HANDLE hMem;
HANDLE hMetaDC;
HANDLE hSetMF;

long FAR PASCAL SetMetaWndProc(HWND, unsigned, WORD, LONG);

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;
    HMENU hMenu;

    if (!hPrevInstance) {
       PWNDCLASS   pSetMetaClass;

       pSetMetaClass = (PWNDCLASS)LocalAlloc(LPTR, sizeof(WNDCLASS));

       pSetMetaClass->hCursor        = LoadCursor(NULL, IDC_ARROW);
       pSetMetaClass->hIcon          = NULL;
       pSetMetaClass->lpszMenuName   = (LPSTR)NULL;
       pSetMetaClass->lpszClassName  = (LPSTR)"SetMetaFileBits";
       pSetMetaClass->hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
       pSetMetaClass->hInstance      = hInstance;
       pSetMetaClass->style          = CS_HREDRAW | CS_VREDRAW;
       pSetMetaClass->lpfnWndProc    = SetMetaWndProc;

       if (!RegisterClass((LPWNDCLASS)pSetMetaClass))
           return FALSE;

       LocalFree((HANDLE)pSetMetaClass);
       }

    hWnd = CreateWindow((LPSTR)"SetMetaFileBits",
                        (LPSTR)"SetMetaFileBits() Demo",
                        WS_TILEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        (HWND)NULL,        
                        (HMENU)NULL,       
                        (HANDLE)hInstance, 
                        (LPSTR)NULL        
                        );

    hInst = hInstance;

    hMetaDC = CreateMetaFile(NULL);
    if (hMetaDC == NULL)
       MessageBox(GetFocus(),
                  (LPSTR)"Error in Creating Memory Metafile",
                  (LPSTR)"CreateMetaFile() Error!",
                  MB_OK | MB_ICONEXCLAMATION);
    else {
       Rectangle(hMetaDC, 10, 10, 200, 200);
       Rectangle(hMetaDC, 201, 10, 401, 200);
       LineTo(hMetaDC, 100, 250);
       Ellipse(hMetaDC, 0, 0, 100, 100);
       hMF = CloseMetaFile(hMetaDC);
       if (hMF == NULL)
          MessageBox(GetFocus(),
                     (LPSTR)"Error Closing Meta File.",
                     (LPSTR)"CloseMetaFile() Error!",
                     MB_OK | MB_ICONEXCLAMATION);
       else {
          hMem = GetMetaFileBits(hMF);
          if (hMem == NULL)
             MessageBox (GetFocus(),
                         (LPSTR)"Error in Obtaining Metafile Bit Handle",
                         (LPSTR)"GetMetaFileBits() Error!!!",
                         MB_OK | MB_ICONEXCLAMATION);
          else {
             hSetMF = SetMetaFileBits(hMem);
             if (hSetMF == NULL)
                MessageBox(GetFocus(),
                           (LPSTR)"Error in Setting Meta File Bits!",
                           (LPSTR)"SetMetaFileBits() Error!!!",
                           MB_OK | MB_ICONEXCLAMATION);
             }
          }
       }

    ShowWindow(hWnd, cmdShow);
    UpdateWindow(hWnd);

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}

long FAR PASCAL SetMetaWndProc(hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
{
   PAINTSTRUCT ps;
   HDC         hDC;

   switch (iMessage) {
      case WM_PAINT:
         hDC = BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
         PlayMetaFile(hDC, hSetMF);
         EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
         break;

      default:
         return DefWindowProc(hWnd, iMessage, wParam, lParam);
      }
   return(0L);
}

