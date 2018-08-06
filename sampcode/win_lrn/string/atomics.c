/*
 * Function (s) demonstrated in this program: AddAtom ()
 *                                           GlobalAddAtom ()
 *                                           FindAtom ()
 *                                           GlobalFindAtom ()
 *                                           GetAtomName ()
 *                                           GlobalGetAtomName ()
 *                                           GetAtomHandle ()
 *                                           DeleteAtom ()
 *                                           GlobalDeleteAtom ()
 * Compiler version: MSC v5.10
 * Description: This program demonstrates the various atom functions available
 *              in the Windows 2.03 Software Development Kit.
 */

#include <windows.h>
#include "process.h"
#include "atomics.h"

long	FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance;
LPSTR       lpszCmdLine;
int	nCmdShow;
{
  static char	szAppName [] = "SampleWindow";
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

  hMenu = LoadMenu (hInstance, "MainMenu");
  hWnd = CreateWindow (szAppName, "The wonders of atoms", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
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


long	FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned	iMessage;
WORD     wParam;
LONG     lParam;
{
  int	IDForAtom, IDForAtom2, ActualAtomSize, AtomMaxSize = 20, MB_Return;
  HANDLE   hToAtom;
  BOOL     bTableAdded;
  char	szbuff[20];

  switch (iMessage)
  {
  case WM_COMMAND:
    switch (wParam)
    {
    case IDM_EXECUTE:
      bTableAdded = InitAtomTable (101);
/* Only neccessary if you want more
                 than 37 entries in your atom table
                 note: 101 is the number of possible
                 entries and should be PRIME */

      IDForAtom = AddAtom ( (LPSTR)"String");
/* Add the string "String" to the atom table */

      IDForAtom2 = GlobalAddAtom ( (LPSTR)"String2");
/* Add the string "String2" to the Atom table,
                 making it accessible to other applications. */

      IDForAtom = FindAtom ( (LPSTR)"String");
/* Search through the atom table and return the atom
                  corresponding to the character string pointed
                  to by (LPSTR)"String". */
      ActualAtomSize = GetAtomName (IDForAtom, (LPSTR)szbuff, AtomMaxSize);
/* Retrieve a copy of the char string that corresponds to the
                  atom specified by IDForAtom  */
      hToAtom = GetAtomHandle (IDForAtom);
      IDForAtom = DeleteAtom (IDForAtom);
      MB_Return = MessageBox (NULL, (LPSTR)szbuff,
          (LPSTR)"Atom Example", MB_OK);
      IDForAtom = GlobalFindAtom ( (LPSTR)"String2");
      ActualAtomSize = GlobalGetAtomName (IDForAtom, (LPSTR)szbuff,
          AtomMaxSize);
      MB_Return = MessageBox (NULL, (LPSTR)szbuff,
          (LPSTR)"Global Atom Example", MB_OK);
      IDForAtom2 = GlobalDeleteAtom (IDForAtom2);
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


