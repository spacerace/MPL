/*
 *
 *  GetMenu
 *  
 *  This program demonstrates the use of the function GetMenu.
 *  This function retrieves a handle to the menu of the specified window.
 *
 */

#include <windows.h>
#include "getmenu.h"

long FAR PASCAL GMWndProc (HWND, unsigned int, WORD, LONG);

static HANDLE hWnd;

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  static char szFileName[] = "getmenu";
  static char szFuncName[] = "GetMenu";
  MSG   msg;
  HMENU hMenu;
  
  if (!hPrevInstance)
     {
     WNDCLASS rClass;

     rClass.lpszClassName = (LPSTR)szFileName;
     rClass.hInstance     = hInstance;
     rClass.lpfnWndProc   = DefWindowProc;
     rClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
     rClass.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
     rClass.lpszMenuName  = (LPSTR)"GetMenuFunc";
     rClass.hbrBackground = GetStockObject(WHITE_BRUSH);
     rClass.style         = CS_HREDRAW | CS_VREDRAW;
     rClass.cbClsExtra    = 0;
     rClass.cbWndExtra    = 0;
   
     RegisterClass((LPWNDCLASS)&rClass);
     }

  hWnd = CreateWindow((LPSTR)szFileName, (LPSTR)szFuncName,
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      (HWND)NULL, (HMENU)NULL,
                      (HANDLE)hInstance, (LPSTR)NULL);
 
   ShowWindow(hWnd, cmdShow);
   UpdateWindow(hWnd);

   MessageBox(GetFocus(), (LPSTR)"Getting Menu Handle",
              (LPSTR)"GetMenu()", MB_OK);
   hMenu = GetMenu(hWnd);
   if (hMenu == NULL)
      MessageBox(GetFocus(), (LPSTR)"Error In Getting Menu Handle",
                 (LPSTR)"GetMenu()", MB_OK);
   else
      MessageBox(GetFocus(), (LPSTR)"Menu Handle Obtained Successfully",
                 (LPSTR)"GetMenu()", MB_OK);
   EnableMenuItem(hMenu, ENTRY_1, MF_BYCOMMAND | MF_GRAYED);
   DrawMenuBar(hWnd);
   MessageBox(GetFocus(), (LPSTR)"'Item_1' Grayed",
              (LPSTR)"GetMenu()", MB_OK);

  return 0;
}

