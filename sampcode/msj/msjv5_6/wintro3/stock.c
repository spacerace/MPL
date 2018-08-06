/*
  Microsoft Systems Journal Stock Application
  Written by Marc Adler
             Magma Systems
             15 Bodwell Terrace
             Millburn, New Jersey  07041

  This is for the third article. It is the basic skeleton of our
  application. We add MDI capabilities to the code.
*/

#include <stdio.h>
#include <string.h>
#include "windows.h"
#include "stock.h"


HANDLE hThisInstance;         /* Program instance handle */
HWND hWndMain      = NULL;    /* Handle to main window   */
HWND hwndMDIClient = NULL;    /* Handle to MDI client    */
HWND hWndActive    = NULL;    /* Handle of the active MDI child */
HWND hWndStatus    = NULL;    /* Window for status mesages */

HANDLE hAccelTable;           /* The accelerator table for this app */


int NEAR PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
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
    If this is the first instance of the app, register window classes...
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
  {
    return FALSE;
  }


  /*
    The main message loop...
  */
  while (GetMessage(&msg, (HWND) NULL, 0, 0))
  {
  /* If a keyboard message is for the MDI , let the MDI client
   * take care of it.  Otherwise, check to see if it's a normal
   * accelerator key (like F3 = find next).  Otherwise, just handle
   * the message as usual.
   */
    if (!TranslateMDISysAccel(hwndMDIClient, &msg) && 
        !TranslateAccelerator(hWndMain, hAccelTable, &msg))
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
  wc.hIcon         = LoadIcon(hThisInstance, MAKEINTRESOURCE(ID_STOCK));
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName  = "StockMenu";
  wc.lpszClassName = "StockMainWindow";
  if (!RegisterClass(&wc))
  {
    return FALSE;
  }

  /* Register the MDI child class */
  wc.lpfnWndProc   = GraphWndProc;
  wc.hIcon         = LoadIcon(hThisInstance, MAKEINTRESOURCE(ID_GRAPH));
  wc.lpszMenuName  = NULL;
  wc.cbWndExtra    = CBWNDEXTRA;
  wc.lpszClassName = "GraphWindow";
  if (!RegisterClass(&wc))
  {
    return FALSE;
  }

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
    Create the frame
  */
  hWndMain = CreateWindow("StockMainWindow",
                          "MSJ Stock Application",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          CW_USEDEFAULT, 0,
                          CW_USEDEFAULT, 0,
                          (HWND) NULL,
                          (HMENU) NULL,
                          hThisInstance,
                          (LPSTR) NULL);

  if (!hWndMain)
  {
    return FALSE;
  }

  /* MDI client should have been created in response to WM_CREATE funtion for
   * frame window.
   */
  if (!hwndMDIClient)
    return FALSE;

  /*
    Load the accelerators in
  */
  hAccelTable = LoadAccelerators(hThisInstance, (LPSTR) "StockAccelerators");

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
  BOOL        rc;
  HDC         hDC;
  PAINTSTRUCT ps;
  FARPROC     lpfn;
  LPSTOCKINFO lpStockInfo;

  char        szMsg[80];
  RECT        rClient;
  int         i;

  static HWND ahwndSubMenus[16];


  switch (msg)
  {
    case WM_CREATE:
    {
      CLIENTCREATESTRUCT ccs;

      /* Find window menu where children will be listed */
      ccs.hWindowMenu  = GetSubMenu(GetMenu(hWnd), 3);
      ccs.idFirstChild = ID_WINDOW_CHILDREN;

      /* Create the MDI client filling the client area */
      hwndMDIClient = CreateWindow("mdiclient",
                                    NULL,
                                    WS_CHILD   | WS_CLIPCHILDREN | 
                                    WS_VSCROLL | WS_HSCROLL,
                                    0,0,0,0,
                                    hWnd,
                                    0xCAC,
                                    hThisInstance,
                                    (LPSTR) &ccs);
      ShowWindow(hwndMDIClient, SW_SHOW);

      /*
        Create a status window at the bottom of the main window.
      */
      GetClientRect(hWnd, (LPRECT) &rClient);
      hWndStatus = CreateWindow("StockStatus",
                                 NULL,
                                 WS_CHILD | WS_BORDER,
                                 rClient.left, rClient.bottom - Y_STATUSWINDOW,
                                 rClient.right - rClient.left, Y_STATUSWINDOW,
                                 hWnd,
                                 NULL, 
                                 hThisInstance,
                                 (LPSTR) 0L);
      if (hWndStatus)
        ShowWindow(hWndStatus, SW_SHOW);

      /*
        We gather up the window handles of all of the sub menus.
      */
      for (i = 0;  (ahwndSubMenus[i] = GetSubMenu(GetMenu(hWnd),i));  i++)
        ;


      break;
    }

    case WM_COMMAND :
      switch (wParam)
      {
        case ID_NEW   :
          GraphCreateWindow(NULL);
          break;
        case ID_OPEN  :
          break;
        case ID_SAVE  :
          break;
        case ID_CLOSE :
          break;
        case ID_PRINT :
          break;
        case ID_EXIT  :
          PostQuitMessage(0);
          break;

        case ID_TICK_ADD :
          break;
        case ID_TICK_CHANGE:
          break;

        case ID_GRAPH_OPTIONS   :
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

        case ID_WINDOW_TILE     :
          SendMessage(hwndMDIClient, WM_MDITILE, 0, 0L);
          break;
        case ID_WINDOW_CASCADE  :
          SendMessage(hwndMDIClient, WM_MDICASCADE, 0, 0L);
          break;
        case ID_WINDOW_ICONS    :
          SendMessage(hwndMDIClient, WM_MDIICONARRANGE, 0, 0L);
          break;

        case ID_WINDOW_CLOSEALL :
        {
          HWND hwndT;
          ShowWindow(hwndMDIClient, SW_HIDE);
          while ((hwndT = GetWindow(hwndMDIClient, GW_CHILD)) != NULL)
          {
            /* Skip the icon title windows */
            while (hwndT && GetWindow(hwndT, GW_OWNER))
	      hwndT = GetWindow(hwndT, GW_HWNDNEXT);
            if (!hwndT)
	      break;
            SendMessage(hwndMDIClient, WM_MDIDESTROY, (WORD) hwndT, 0L);
          }
          ShowWindow(hwndMDIClient, SW_SHOW);
          ShowWindow(hWndStatus, SW_SHOW);
          break;
        }

        case ID_ABOUT :
          break;

        default :
          /*
            We might have chosen to change the focus to one of the
            MDI children.
          */
          return DefFrameProc(hWnd, hwndMDIClient, msg, wParam, lParam);
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
             ahwndSubMenus[idMenu] && wParam != ahwndSubMenus[idMenu];
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
         ((iLen = LoadString(hThisInstance, idMenu, (LPSTR) szMsg, sizeof(szMsg)))))
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
        GetClientRect(hWndStatus, (LPRECT) &rClient);
        FillRect(hDC, &rClient, hBrush);
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


    case WM_SIZE :
      /*
        When the main window is resized, we want to move and resize the
        status window so that it is always lying on the bottom.
      */
      if (hWndStatus)
      {
        GetClientRect(hWnd, (LPRECT) &rClient);
        MoveWindow(hWndStatus, 
                   rClient.left, rClient.bottom - Y_STATUSWINDOW,
                   rClient.right - rClient.left, Y_STATUSWINDOW,
                   TRUE);
      }
      /*
        We need to call DefFrameProc, because it processes the WM_SIZE
        messages too!
      */
      goto call_DFP;


    case WM_DESTROY :
      PostQuitMessage(0);
      break;

    default :
    /*	use DefFrameProc() instead of DefWindowProc() since there
     *	are things that have to be handled differently because of MDI
     */
call_DFP:
      return DefFrameProc(hWnd, hwndMDIClient, msg, wParam, lParam);
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


/****************************************************************************/
/*                                                                          */
/* Function : GraphCreateWindow()                                           */
/*                                                                          */
/* Purpose  : Creates a new graph window (an MDI child) in response to      */
/*            the File/New and File/Open commands.                          */
/*                                                                          */
/* Returns  : The handle of the newly created MDI child window.             */
/*                                                                          */
/****************************************************************************/

HWND PASCAL GraphCreateWindow(LPSTR lpName)
{
  HWND hWnd;
  char sz[160];
  MDICREATESTRUCT mcs;

  static int nChildren = 1;


  if (!lpName)
  {
    /* If the lpName parameter is NULL, load the "Untitled" string
    *  from STRINGTABLE and set the title field of the MDI CreateStruct.
    */
    sprintf(sz, "(Untitled - %d)", nChildren++);
    mcs.szTitle = (LPSTR) sz;
  }
  else
  {
    /* Title the window with the supplied filename */
    AnsiUpper(lpName);
    mcs.szTitle = lpName;
  }

  mcs.szClass = "GraphWindow";
  mcs.hOwner  = hThisInstance;

  /* Use the default size for the window */
  mcs.x = mcs.cx = CW_USEDEFAULT;
  mcs.y = mcs.cy = CW_USEDEFAULT;

  /* Set the style DWORD of the window to default */
  mcs.style = 0L;

  /* tell the MDI Client to create the child */
  hWnd = (WORD) SendMessage(hwndMDIClient,
			    WM_MDICREATE,
			    0,
			    (LONG) (LPMDICREATESTRUCT) &mcs);
  return hWnd;
}

/****************************************************************************/
/*                                                                          */
/* Function : GraphWndProc                                                  */
/*                                                                          */
/* Purpose  : Window procedure for the graph windows (MDI child)            */
/*                                                                          */
/* Returns  :                                                               */
/*                                                                          */
/****************************************************************************/

LONG FAR PASCAL GraphWndProc(hWnd, msg, wParam, lParam)
  HWND hWnd;
  WORD msg;
  WORD wParam;
  LONG lParam;
{
  switch (msg)
  {
    case WM_MDIACTIVATE:
      /* If we're activating this child, remember it */
      hWndActive = (wParam) ? hWnd : (HWND) NULL;
      break;

    default:
      /* Again, since the MDI default behaviour is a little different,
       * call DefMDIChildProc instead of DefWindowProc()
       */
      return DefMDIChildProc(hWnd, msg, wParam, lParam);
  }

  return (LONG) FALSE;
}

