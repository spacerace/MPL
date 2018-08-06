/*

Function(s) demonstrated in this program: SetProp, GetProp, RemoveProp

Description:

This program demonstrates the use of the function SetProp, GetProp,
and RemoveProp.  SetProp copies the character string and a data handle
to the property list of the window.  This has the effect of
associating a data object with the window.  The string serves as a
label for the data handle.  The data handle is retrieved from the
property list using the GetProp function and specifying the character
string.  Before the window is destroyed the RemoveProp function must
be used  to remove the single entry in the property list.  Note that there
may be more than one data items associated with a window in the property
list.

*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "property.h"

VOID FAR PASCAL lstrcpy(LPSTR, LPSTR);
long	FAR PASCAL WndProc(HWND, unsigned, WORD, long);

/* This is the structure that is used to put into the property list. */
typedef struct tagData {
  short	nRow, nSeat;
  char	szName[20];
} SEATINGS;
static char	szResName [] = "ResMenu";

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance, hPrevInstance ;
LPSTR       lpszCmdLine ;
int	nCmdShow ;
{
  static char	szAppName [] = "Property" ;
  HWND        hWnd ;
  WNDCLASS    wndclass ;
  MSG msg;

  if (!hPrevInstance)
  {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
    wndclass.lpfnWndProc   = WndProc ;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
    wndclass.lpszMenuName  = NULL ;
    wndclass.lpszClassName = szAppName ;

    if (!RegisterClass (&wndclass))
      return FALSE ;
  }

  hWnd = CreateWindow(szAppName, (LPSTR)"Setting the Property List",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0,                  
      CW_USEDEFAULT, 0,                  
      NULL, NULL, hInstance, NULL) ;

  ShowWindow (hWnd, nCmdShow) ;
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
  HANDLE hData;
  HMENU hMenu;
  char	szbuffer[200], szName[20];
  short	nRow, nSeat;
  SEATINGS FAR * pstrSeatings;
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
/* Putting the structure into the property list of the window 
 * If it is successful, retrieve it to check if the correct structure
 * is put into the property list
 */
	  hData = GlobalAlloc(GMEM_MOVEABLE, (LONG)sizeof(SEATINGS));


/* Initialize the data structure to be associated with the window */

	  pstrSeatings = (SEATINGS FAR * )GlobalLock(hData);
	  pstrSeatings->nRow = 4;
	  pstrSeatings->nSeat = 2;
	  lstrcpy (pstrSeatings->szName, "John Smith");

	  GlobalUnlock(hData);
/* Add a new entry into the property list of the window specified *
 * by hWnd. The character string "Smith" identifies the entry to  *
 * be added.
 */
	  if (!SetProp(hWnd, "Smith", hData))
	    MessageBox(hWnd, (LPSTR)"The Windows function SetProp failed.",
	        (LPSTR)"FAIL", MB_OK | MB_ICONHAND);

/* Prepare to display message */
	  sprintf (szbuffer, "The following entry has been added to the \
property list of the parent window using the Windows function \
SetProp.\n\n  Smith at row 4, seat# 2\n");


	  MessageBox(hWnd, szbuffer, "SetProp", MB_OK);

/* Retrieve an entry from the property list of the window specified *
 * by hWnd. The character string "Smith" identifies the entry to    *
 * be retrieved.
 */
	  hData = GetProp(hWnd, "Smith");

	  if (!hData)
	  {
	    MessageBox(hWnd, "GetProp has failed", "GetProp", MB_OK | MB_ICONHAND);
	  }

	  pstrSeatings = (SEATINGS FAR * )GlobalLock(hData);

/* Copy the global data objects into local data structures so that *
 * we can use sprintf.  Using small memory model, sprintf will     *
 * only accept pointer offsets into the current data segment
 */
	  nRow = pstrSeatings->nRow;
	  nSeat =  pstrSeatings->nSeat;
	  lstrcpy(szName, pstrSeatings->szName);


/* Prepare to display message */

	  sprintf(szbuffer, "The following entry has been retrieved from the \
property list of the parent window using the Windows function GetProp.\n\n \
%s at row %d, seat# %d\n",
	                   szName, nRow, nSeat);

	  MessageBox(hWnd, szbuffer, "GetProp", MB_OK);

	  GlobalUnlock(hData);
	  GlobalFree(hData);

/* Remove an entry from the property list of the window specified *
 * by hWnd. The character string "Smith" identifies the entry to  *
 * be removed.
 */
	  if (!RemoveProp(hWnd, "Smith"))
	  {
	    MessageBox(hWnd, "RemoveProp has failed",
	        "RemoveProp", MB_OK | MB_ICONHAND);
	  }

/* Prepare to display message */
	  sprintf(szbuffer, "The following entry has been removed from the \
property list of the parent window using the Windows function \
RemoveProp.\n\n %s at row %d, seat# %d\n",
	                   szName, nRow, nSeat);

	  MessageBox (hWnd, szbuffer, "RemoveProp", MB_OK);

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


