/*
Function(s) demonstrated in this program: GlobalLock
Compiler version: 5.10
Description: When the right mouse button is clicked, this program will
	     attempt to allocate and lock a small block of global memory.
	     As a check, an arithmetic operation is performed on data
	     stored in this global memory.
*/

#include <windows.h>
#include <stdio.h>

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

typedef struct {     /* structure we are going */
          int x;     /* to allocate and lock   */
          int y;
          } MYSTRUCT;

typedef MYSTRUCT far *lpMyPtr;   /* far pointer to MYSTRUCT type */

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       nCmdShow;
{
   static char szAppName [] = "globlock";
   HWND     hWnd;
   WNDCLASS wndclass;
   MSG      msg;

   if (!hPrevInstance)
      {
      wndclass.style	        = CS_HREDRAW | CS_VREDRAW;
      wndclass.lpfnWndProc   = WndProc;
      wndclass.cbClsExtra    = 0;
      wndclass.cbWndExtra    = 0;
      wndclass.hInstance     = hInstance;
      wndclass.hIcon	        = LoadIcon (NULL, IDI_APPLICATION);
      wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
      wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
      wndclass.lpszMenuName  = NULL;
      wndclass.lpszClassName = szAppName;

      if (!RegisterClass (&wndclass))
         return FALSE;
      }

   hWnd = CreateWindow (szAppName,   /* window class name   */
                        "Using GlobalLock (Click the Right Mouse Button for Demo)",
                        WS_OVERLAPPEDWINDOW,  /* window style            */
                        CW_USEDEFAULT,        /* initial x position      */
                        0,                    /* initial y position      */
                        CW_USEDEFAULT,        /* initial x size          */
                        0,                    /* initial y size          */
                        NULL,                 /* parent window handle    */
                        NULL,                 /* window menu handle      */
                        hInstance,            /* program instance handle */
                        NULL);                /* create parameters       */

   ShowWindow (hWnd, nCmdShow);
   UpdateWindow (hWnd);

   while (GetMessage(&msg, NULL, 0, 0))
      {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      }
   return (msg.wParam);
}

long FAR PASCAL WndProc(hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
{
    HANDLE  hMemBlock;	  /* Handle to memory block */
    lpMyPtr	ThisPtr;	  /* far pointer to MYSTRUCT structure */
    char	szBuff[150];	  /* buffer for message box */

    switch(iMessage)
       {
       case WM_RBUTTONDOWN:
          {
          hMemBlock = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
                                  (long)sizeof(MYSTRUCT));

          /* if memory allocated properly */
          if (hMemBlock)
             {
             /* lock memory into current address */
             ThisPtr = (lpMyPtr)GlobalLock(hMemBlock);

             /* if lock worked */
             if (ThisPtr)
                {
                MessageBox(GetFocus(),
                           (LPSTR)"The lock worked properly",
                           (LPSTR)"GlobalLock",
                           MB_OK);
                /* use memory from global heap */
                ThisPtr->x = 4;
                ThisPtr->y = 4;
                ThisPtr->x = ThisPtr->x * ThisPtr->y;
                sprintf(szBuff, "%s%d", "The values referenced by these pointers are located\
 in the global heap. \n\nThisPtr->x * ThisPtr->y = ", ThisPtr->x);
                MessageBox(GetFocus(), (LPSTR)szBuff,
                           (LPSTR)"Info from GlobalLock'ed memory", MB_OK);
                GlobalUnlock(hMemBlock);   /* unlock memory */
                GlobalFree(hMemBlock);     /* free memory   */
                }
             else
                {
                MessageBox(GetFocus(),
                           (LPSTR)"The lock DID NOT work properly",
                           (LPSTR)"GlobalLock", MB_OK);
                GlobalFree(hMemBlock);     /* free memory   */
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
         return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
   }
   return (0L);
}
