/*
 *  This program demonstrates the use of the function CloseMetaFile.
 *  This function closes the metafile DC and creates a metafile handle
 *  that can be used to play the metafile using PlayMetaFile.
 *
 */

#include <windows.h>

long	FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

static char	szAppName[] = "cmetafil";
static char	szFuncName[] = "CloseMetaFile";

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
{
  HWND     hWnd;
  WNDCLASS rClass;
  MSG      msg;

  if (!hPrevInstance)
  {
    rClass.style         = CS_HREDRAW | CS_VREDRAW;
    rClass.lpfnWndProc   = WndProc;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;
    rClass.hInstance     = hInstance;
    rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = NULL;
    rClass.lpszClassName = szAppName;

    if (!RegisterClass (&rClass))
      return TRUE;
  }

hWnd = CreateWindow (szAppName,            /* window class name       */
szFuncName,                 /* window caption          */
WS_OVERLAPPEDWINDOW,        /* window style            */
CW_USEDEFAULT,              /* initial x position      */
0,                          /* initial y position      */
CW_USEDEFAULT,              /* initial x size          */
0,                          /* initial y size          */
NULL,                       /* parent window handle    */
NULL,                       /* window menu handle      */
hInstance,                  /* program instance handle */
  NULL);                      /* create parameters       */

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage (&msg, NULL, 0, 0))
  {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
  }
  return (msg.wParam);
}


long	FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned	iMessage;
WORD     wParam;
LONG     lParam;
{
  HANDLE hMF;
  HDC    hDCMeta;
  HDC    hDC;
  HMENU  hMenu;
  BOOL   bPlayed;

  switch (iMessage)
  {
  case WM_CREATE:
    hMenu = CreateMenu ();
    ChangeMenu (hMenu, NULL, (LPSTR)"Play", 100, MF_APPEND);
    SetMenu (hWnd, hMenu);
    break;
  case WM_INITMENU:
    InvalidateRect (hWnd, NULL, TRUE);
    break;
  case WM_COMMAND:
    if (wParam == 100)
    {
      hDC = GetDC (hWnd);
      hDCMeta = CreateMetaFile ((LPSTR)"CMETAFIL.MET");
      Rectangle (hDCMeta, 10, 10, 600, 200);
      Arc (hDCMeta, 10, 10, 600, 200, 10, 15, 600, 150);
      MessageBox (GetFocus(), (LPSTR)"Creating handle to metafile",
          (LPSTR) szFuncName, MB_OK);
      hMF = CloseMetaFile (hDCMeta);
      if (hMF != NULL)
      {
	MessageBox (GetFocus(), (LPSTR)"Metafile handle created",
	    (LPSTR) szFuncName, MB_OK);
	bPlayed = PlayMetaFile (hDC, hMF);
	MessageBox (GetFocus(), (LPSTR)"This is the metafile",
	    (LPSTR) szFuncName, MB_OK);
      }
      else
	MessageBox (GetFocus(), (LPSTR)"Metafile handle not created",
	    (LPSTR) szFuncName, MB_OK);
      DeleteMetaFile (hMF);
      ReleaseDC (hWnd, hDC);
    }
    break;
  case WM_DESTROY:
    PostQuitMessage (0);
    break;
  default:
    return DefWindowProc (hWnd, iMessage, wParam, lParam);
  }
  return (0L);
}


