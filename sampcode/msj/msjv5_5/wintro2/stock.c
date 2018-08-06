/*
  Microsoft Systems Journal Stock Application

  Written by Marc Adler
             Magma Systems
             15 Bodwell Terrace
             Millburn, New Jersey  07041

  This code accompanies PartII of MSJ's series on learning Windows.
  It is the basic skeleton of our  application. The main thing we
  do here is perform the initialization, and print out help messages
  for each menu item.
*/

#include "windows.h"
#include "stock.h"

/*
  Global Variables
*/
HANDLE hThisInstance;         /* Program instance handle */
HWND   hWndMain;              /* Handle to main window   */
HWND   hWndStatus;            /* Window for status mesages */
HANDLE hAccelTable;           /* Handle to the accelerator table */


int NEAR PASCAL WinMain(hInstance, hPrevInstance,
                        lpszCmdLine, nCmdShow)
  HANDLE hInstance;
  HANDLE hPrevInstance;
  LPSTR  lpszCmdLine;
  int    nCmdShow;
{
  MSG msg;

  /*
    Save the handle to this inctance.
  */
  hThisInstance = hInstance;

  /*
    If this is the first instance of the app,
    register window classes...
  */
  if (!hPrevInstance)
  {
    if (!InitializeApplication())
    {
      MessageBeep(0);
      return FALSE;
    }
  }

  /*
    Perform the initialization for this particular instance...
  */
  if (!InitializeInstance(lpszCmdLine, nCmdShow))
    return FALSE;


  /*
    The main message loop...
  */
  while (GetMessage(&msg, (HWND) NULL, 0, 0))
  {
    if (!TranslateAccelerator(hWndMain, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return 0;
}


BOOL FAR PASCAL InitializeApplication(VOID)
{
  WNDCLASS wc;

  /*
    Register the main window class
  */
  wc.style         = CS_VREDRAW | CS_HREDRAW;
  wc.lpfnWndProc   = MainWndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = hThisInstance;
  wc.hIcon         = LoadIcon(hThisInstance,
                     MAKEINTRESOURCE(ID_STOCK));
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName  = "StockMenu";
  wc.lpszClassName = "StockMainWindow";
  if (!RegisterClass(&wc))
    return FALSE;

  wc.style         = CS_VREDRAW | CS_HREDRAW;
  wc.lpfnWndProc   = StatusWndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = hThisInstance;
  wc.hbrBackground = GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = "StockStatus";
  if (!RegisterClass(&wc))
    return FALSE;

  return TRUE;
}


BOOL FAR PASCAL InitializeInstance(LPSTR lpCmdLine, WORD nCmdShow)
{
  /*
    Create the main window
  */
  hWndMain = CreateWindow("StockMainWindow",
                          "MSJ Stock Application",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          (HWND) NULL,
                          (HMENU) NULL,
                          hThisInstance,
                          (LPSTR) NULL);
  if (!hWndMain)
    return FALSE;

  /*
    Load the accelerators in
  */
  hAccelTable = LoadAccelerators(hThisInstance,
                                 (LPSTR) "StockAccelerators");

  /*
    Display the main window
  */
  ShowWindow(hWndMain, nCmdShow);
  UpdateWindow(hWndMain);

  return TRUE;
}


LONG FAR PASCAL MainWndProc(hWnd, msg, wParam, lParam)
  HWND hWnd;
  WORD msg;
  WORD wParam;
  LONG lParam;
{
  char szMsg[80];
  RECT rc;
  int  i;

  static HWND ahwndSubMenus[16];


  switch (msg)
  {
    case WM_CREATE  :
      /*
        Create a status window at the bottom of the main window.
      */
      GetClientRect(hWnd, (LPRECT) &rc);
      hWndStatus = CreateWindow("StockStatus",
                                 NULL,
                                 WS_CHILD | WS_BORDER,
                                 rc.left, rc.bottom - 20,
                                 rc.right - rc.left, 20,
                                 hWnd,
                                 NULL,
                                 hThisInstance,
                                 (LPSTR) 0L);
      if (hWndStatus)
        ShowWindow(hWndStatus, SW_SHOW);

      /*
        We gather up the window handles of all of the sub menus.
      */
      for (i=0; (ahwndSubMenus[i]=GetSubMenu(GetMenu(hWnd),i)); i++)
        ;

      break;


    case WM_COMMAND :
      switch (wParam)
      {
        case ID_NEW             :
          break;
        case ID_OPEN            :
          break;
        case ID_SAVE            :
          break;
        case ID_CLOSE           :
          break;
        case ID_PRINT           :
          break;
        case ID_TICK_ADD        :
          break;
        case ID_TICK_CHANGE     :
          break;
        case ID_GRAPH_PRICE     :
          break;
        case ID_GRAPH_VOLUME    :
          break;
        case ID_GRAPH_ZOOM      :
          break;
        case ID_GRAPH_COLORS    :
          break;
        case ID_GRAPH_GRID_HORZ :
          break;
        case ID_GRAPH_GRID_VERT :
          break;
        case ID_ABOUT           :
          break;
        case ID_EXIT            :
          PostQuitMessage(0);
          break;
      }
      break;

    case WM_MENUSELECT          :
    {
      WORD idMenu;
      WORD iLen;

      /*
        We trigger off of the WM_MENUSELECT message so that we
        can print informative messages about each menu option.
      */

      if (lParam & MF_POPUP)
      {
        /*
          The handle of the popup menu is returned in wParam. Search
          the array of sub-menu window handles in order to see which
          submenu we are in.
        */
        for (idMenu = 0;
             ahwndSubMenus[idMenu] && wParam !=
                                              ahwndSubMenus[idMenu];
             idMenu++)
          ;
        if (!ahwndSubMenus[idMenu])
          break;
        idMenu++;  /* make it a 1-offset value */
      }
      else
        /*
          An actual menu item identifier is returned in wparam...
        */
        idMenu = wParam;

      /*
        If lParam was <-1,0>, then the user dismissed the menu, so
        we just want to erase the previous help text from the
        status window. If not, then idMenu contains a valid menu or
        submenu identifier. Load the corresponding string from
        the string table, and print it.
      */
      if (LOWORD(lParam) == -1 && HIWORD(lParam) == 0  ||
         ((iLen = LoadString(hThisInstance, idMenu, (LPSTR) szMsg,
                             sizeof(szMsg)))))
      {
        HDC hDC = GetDC(hWndStatus);
        HBRUSH hBrush, hOldBrush;

        /*
          Use a white brush for erasing.
        */
        hBrush = CreateSolidBrush(0x00FFFFFF);
        hOldBrush = SelectObject(hDC, hBrush);

        /*
          Erase the previous menu help text, if any. Then display the
          new help text.
        */
        GetClientRect(hWndStatus, (LPRECT) &rc);
        FillRect(hDC, &rc, hBrush);
        if (LOWORD(lParam) == -1 && HIWORD(lParam) == 0)
          ;
        else
          TextOut(hDC, 0, 0, szMsg, iLen);

        /*
          Restore the old brush.
        */
        SelectObject(hDC, hOldBrush);
        DeleteObject(hBrush);
        ReleaseDC(hWndStatus, hDC);
      }
      break;
    }


    case WM_DESTROY :
      PostQuitMessage(0);
      break;

    default :
      return DefWindowProc(hWnd, msg, wParam, lParam);
  }

  return (LONG) TRUE;
}



LONG FAR PASCAL StatusWndProc(hWnd, msg, wParam, lParam)
  HWND hWnd;
  WORD msg;
  WORD wParam;
  LONG lParam;
{
  return DefWindowProc(hWnd, msg, wParam, lParam);
}
