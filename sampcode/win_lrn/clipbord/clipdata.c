/*
 *  This program demonstrates the use of GetClipboardData function.
 *  This function retieves data from the clipboard in the format given by
 *  an unsigned short interger value that specifies a data format. The
 *  clipboard must have been previously opened.
 *  
 */

#include <windows.h>

static char	szAppName[] = "clipdata";
static char     szFuncName[] = "GetClipboardData";

LPSTR FAR PASCAL lstrcpy (LPSTR, LPSTR);

long    FAR PASCAL WndProc (HANDLE, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  HWND     hWnd;
  WNDCLASS rClass;
  HMENU    hMenu;
  MSG      msg;

  if (!hPrevInstance)
    {
    rClass.style         = CS_HREDRAW | CS_VREDRAW;
    rClass.lpfnWndProc   = WndProc;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;
    rClass.hInstance     = hInstance;
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = (LPSTR) NULL;
    rClass.lpszClassName = (LPSTR) szAppName;

    if (!RegisterClass (&rClass))
      return FALSE;
    }

  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, (LPSTR)"Read From Clipboard", 100, MF_APPEND);

  hWnd = CreateWindow (szAppName,            /* window class name       */
                      szFuncName,            /* window caption          */
                      WS_OVERLAPPEDWINDOW,   /* window style            */
                      CW_USEDEFAULT,         /* initial x position      */
                      0,                     /* initial y position      */
                      CW_USEDEFAULT,         /* initial x size          */
                      0,                     /* initial y size          */
                      NULL,                  /* parent window handle    */
                      hMenu,                 /* window menu handle      */
                      hInstance,             /* program instance handle */
                      NULL);                 /* create parameters       */

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage (&msg, NULL, 0, 0))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return (msg.wParam);
  }

long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned	iMessage;
WORD     wParam;
LONG     lParam;
  {
  HMENU  hMenu;
  HANDLE hClipData;
  HANDLE hMem;
  LPSTR  lpText;
  char	szBuffer[80];

  switch (iMessage)
    {
    case WM_CREATE:
      OpenClipboard (hWnd);
      hMem = GlobalAlloc (GMEM_MOVEABLE | GMEM_ZEROINIT,
                         (long) strlen (szFuncName));
      lpText = (LPSTR) GlobalLock (hMem);
      lstrcpy (lpText, (LPSTR) szFuncName);
      SetClipboardData (CF_TEXT, hMem);
      GlobalUnlock (hMem);
      CloseClipboard ();

    case WM_COMMAND:
      if (wParam == 100)
        {
        OpenClipboard (hWnd);
        MessageBox (hWnd, (LPSTR)"Getting clipboard data", (LPSTR)szFuncName,
            MB_OK);
        hClipData = GetClipboardData (CF_TEXT);
        if (hClipData != NULL)
          MessageBox (hWnd, (LPSTR)"Data Received", (LPSTR)szFuncName, MB_OK);
        else
          MessageBox (hWnd, (LPSTR)"There is no data", (LPSTR)szFuncName,
              MB_OK);
        CloseClipboard ();
        }
      break;

    case WM_DESTROY:
      GlobalFree (hMem);        /*  Free up the memory  */
      PostQuitMessage (0);
      break;

    default:
      return (DefWindowProc (hWnd, iMessage, wParam, lParam));
    }
  return (0L);
  }
