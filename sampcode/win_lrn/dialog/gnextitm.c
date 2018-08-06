/*
 * This application demonstrates the use of GetNextDlgGroupItem() and
 * GetNextDlgTabItem(). In the dialog box, choosing the radiobutton
 * and selecting the direction and the "NextTab" or "NextGroup" push
 * button will change the focus to the next repsective item -- depending
 * upon whether the previous flag and the "Tab" or "Group" selection.
 *
 */

#include "windows.h"
#include "gnextitm.h"

/* Global Variables */
static HANDLE hInst;       /* Instance handle for dialog box.          */
FARPROC       lpprocNext;  /* Long pointer to dialog procedure.        */
HWND          hGrpCurrent; /* Handle to the current group push button. */
HWND          hTabCurrent; /* Handle to the current tab push button.   */
BOOL          bDirCurrent; /* Current direction to get the next item.  */

/* Forward References */
long FAR PASCAL NextWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL NextItem(HWND, unsigned, WORD, LONG);

/* Dialog Procedure */
BOOL FAR PASCAL NextItem(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
   HWND hGNextItem; /* Next Group Item */
   HWND hTNextItem; /* Next Tab Item   */
   int  i;          /* Counter         */

   switch (message) {
      case WM_COMMAND:
         switch (wParam) {
            case ID_DONE: /* If all done, quit */
               EndDialog(hDlg, TRUE);
               return TRUE;

            case ID_DIRECTION: /* Change the "Previous" flag for direction */
               if (bDirCurrent == TRUE)
                  bDirCurrent = FALSE;
               else
                  bDirCurrent = TRUE;
               return TRUE;

            case ID_TABFRST: /* Manually set the tab button focus */
            case ID_TABSEC:
            case ID_TABTHRD:
               CheckRadioButton(hDlg, ID_TABFRST,
                                ID_TABTHRD, wParam);
               hTabCurrent = GetDlgItem(hDlg, wParam);
               return TRUE;

            case ID_GRPFRST: /* Manually set the group button focus */
            case ID_GRPSEC:
            case ID_GRPTHRD:
               CheckRadioButton(hDlg, ID_GRPFRST,
                                ID_GRPTHRD, wParam);
               hGrpCurrent = GetDlgItem(hDlg, wParam);
               return TRUE;

            case ID_NEXTTAB: /* Get the next tab item */
               /* The first two cases are needed because the dialog
                  manager does neat things with tabs. */
               if ((hTabCurrent == GetDlgItem(hDlg, ID_TABFRST)) &&
                   (bDirCurrent == TRUE)) {
                   CheckRadioButton(hDlg, ID_TABFRST,
                                    ID_TABTHRD, ID_TABTHRD);
                   hTabCurrent = GetDlgItem(hDlg, ID_TABTHRD);
                   }
               else if ((hTabCurrent == GetDlgItem(hDlg, ID_TABTHRD)) &&
                        (bDirCurrent == FALSE)) {
                   CheckRadioButton(hDlg, ID_TABFRST,
                                    ID_TABTHRD, ID_TABFRST);
                   hTabCurrent = GetDlgItem(hDlg, ID_TABFRST);
                   }
               else { /* Finally, you can do things properly */
                  hTNextItem = GetNextDlgGroupItem(hDlg,
                                                  hTabCurrent,
                                                  bDirCurrent);
                  hTabCurrent = hTNextItem;
                  for (i = ID_TABFRST; i <= ID_TABTHRD; i = i + 25)
                     if (GetDlgItem(hDlg, i) == hTabCurrent)
                        CheckRadioButton(hDlg, ID_TABFRST,
                                         ID_TABTHRD, i);
                  }
               return TRUE;

            case ID_NEXTGROUP: /* Groups are much neater to implement */
               hGNextItem = GetNextDlgGroupItem(hDlg,
                                               hGrpCurrent,
                                               bDirCurrent);
               hGrpCurrent = hGNextItem;
               for (i = ID_GRPFRST; i <= ID_GRPTHRD; i = i + 25)
                  if (GetDlgItem(hDlg, i) == hGrpCurrent)
                     CheckRadioButton(hDlg, ID_GRPFRST,
                                      ID_GRPTHRD, i);
               return TRUE;

            default:
               return FALSE;
            }

      case WM_INITDIALOG: /* Initalize everything */
         hTabCurrent = GetDlgItem(hDlg, ID_TABFRST);
         CheckRadioButton(hDlg, ID_TABFRST, ID_TABTHRD, ID_TABFRST);
         hGrpCurrent = GetDlgItem(hDlg, ID_GRPFRST);
         CheckRadioButton(hDlg, ID_GRPFRST, ID_GRPTHRD, ID_GRPFRST);
         bDirCurrent = FALSE;
         CheckDlgButton(hDlg, ID_DIRECTION, bDirCurrent);
         SetFocus(GetDlgItem(hDlg, ID_DONE));
         return FALSE;

      default:
         return FALSE;
      }
}

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;

    if (!hPrevInstance) {
       PWNDCLASS   pNextClass;

       pNextClass = (PWNDCLASS)LocalAlloc(LPTR, sizeof(WNDCLASS));

       pNextClass->hCursor        = LoadCursor(NULL, IDC_ARROW);
       pNextClass->hIcon          = (HANDLE)NULL;
       pNextClass->lpszMenuName   = (LPSTR)"NextMenu";
       pNextClass->lpszClassName  = (LPSTR)"gnextitm";
       pNextClass->hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
       pNextClass->hInstance      = hInstance;
       pNextClass->style          = CS_HREDRAW | CS_VREDRAW;
       pNextClass->lpfnWndProc    = NextWndProc;

       if (!RegisterClass((LPWNDCLASS)pNextClass))
           /* Initialization failed.
            * Windows will automatically deallocate all allocated memory.
            */
           return FALSE;

       LocalFree((HANDLE)pNextClass);
       }

    hWnd = CreateWindow((LPSTR)"gnextitm",
                        (LPSTR)"GetNextDlgTabItem() & GetNextDlgGroupItem() Demo",
                        WS_TILEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        (HWND)NULL,        /* no parent                 */
                        (HMENU)NULL,       /* use class menu            */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL);      /* no params to pass on      */

    /* Save instance handle for DialogBox */
    hInst = hInstance;

    /* Bind callback function with module instance */
    lpprocNext = MakeProcInstance((FARPROC)NextItem, hInstance);

    /* Make window visible according to the way the app is activated */
    ShowWindow(hWnd, cmdShow);
    UpdateWindow(hWnd);

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}


/* Procedures which make up the window class. */
long FAR PASCAL NextWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message)
    {
    case WM_COMMAND:
        switch (wParam)
        {
        case ID_NEXTBOX: /* Go for it! */
            DialogBox(hInst, MAKEINTRESOURCE(NEXTBOX), hWnd, lpprocNext);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    return(0L);
}
