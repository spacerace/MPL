/*
 * Function (s) demonstrated in this program: AddAtom
 * Compiler version: MSC v5.10
 * Description: Program adds a character string to the atom table and creates
 *              a new atom that uniquely identifies the string.
 */

#include <windows.h>
#include "addatom.h"

int     sprintf (LPSTR, LPSTR, int);
long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance;
LPSTR       lpszCmdLine;
int         nCmdShow;
  {
  static char   szAppName [] = "AddAtom";
  HWND        hWnd;
  WNDCLASS    wndclass;
  MSG msg;
  HMENU hMenu;

  if (!hPrevInstance)
    {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass (&wndclass))
      return FALSE;
    }

  hMenu = LoadMenu (hInstance, "OurMenu");
  hWnd = CreateWindow (szAppName, "AddAtom Demonstration",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, 0,
                      CW_USEDEFAULT,  0,
                      NULL, hMenu, hInstance, NULL);

  ShowWindow (hWnd, nCmdShow);
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
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  int	AtomMaxSize = 20;
  ATOM	lpIDForAtom;
  WORD ActualAtomSize;
  int	hToAtom;
  BOOL	bTableAdded;
  char  szBuff[20];

  switch (iMessage)
    {
    case WM_COMMAND:
      if (wParam == IDM_EXECUTE)
        {
        SetFocus (hWnd);
        lpIDForAtom = AddAtom ( (LPSTR)"Hello World");
        ActualAtomSize = GetAtomName (lpIDForAtom, (LPSTR)szBuff, AtomMaxSize);
        MessageBox (GetFocus (), (LPSTR)szBuff, (LPSTR)"Atom added was...", MB_OK);
        hToAtom = GetAtomHandle (lpIDForAtom);
        lpIDForAtom = DeleteAtom (lpIDForAtom);
        if (lpIDForAtom == NULL)
          MessageBox (GetFocus (), (LPSTR)"Success", (LPSTR)"Atom Deleted", MB_OK);
        else
          MessageBox (GetFocus (), (LPSTR)"Failure", (LPSTR)"Atom not deleted", MB_OK);
        }
      else
        return (DefWindowProc (hWnd, iMessage, wParam, lParam));
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam);
    }
  return (0L);
  }
