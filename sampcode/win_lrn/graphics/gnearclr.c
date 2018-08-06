/*
 * This example demonstrates the use of GetNearestColor(). This example
 * begins with the main menu selection where the user enters RGB values
 * into a dialog box. These values are used to compute the nearest
 * color and displayed in a message box. This loop continues until the
 * user chooses the "Done" selection.
 *
 */

#include "windows.h"
#include <stdio.h>
#include "gnearclr.h"

/* Global Variables */
static HANDLE hInst;
FARPROC lpprocColor;

/* Forward References */
long FAR PASCAL NearestWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL ColorDlg(HWND, unsigned, WORD, LONG);

/* Color Dialog Function */
BOOL FAR PASCAL ColorDlg(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
   char szOutBuf[80];      /* Output Buffer for Message Box    */
   unsigned uiRValue;      /* Edit Input for Red Value         */
   unsigned uiGValue;      /* Edit Input for Green Value       */
   unsigned uiBValue;      /* Edit Input for Blue Value        */
   BOOL bSuccess;          /* Flag if All Edit Values are Good */
   DWORD dwrgbValue;       /* RGB Value of Nearest Color       */
   HDC hDC;                /* Display Context of Dialog Box    */
   BOOL bReceived;         /* Flag if Edit Received Set Value  */

   switch (message) {
      case WM_COMMAND:
         switch (wParam) {
            case ID_DONE:  /* If all done, quit the dialog box */
               EndDialog(hDlg, TRUE);
               return TRUE;

            case ID_MYOK:  /* If ready to accept dialog values */
               /* Innocent until proven guilty */
               bSuccess = TRUE;
               /* Get all of the edit values and check them for range */
               uiRValue = GetDlgItemInt(hDlg, ID_REDIT,
                                        (BOOL FAR *)&bReceived, NULL);
               if ((bReceived == NULL) || (uiRValue > 255) ||
                   (uiRValue < 0))
                  {
                  MessageBox(hDlg, (LPSTR)"Invalid Red Value",
                             (LPSTR)"Range Error (0-255)!", MB_OK);
                  bSuccess = FALSE;
                  }
               uiGValue = GetDlgItemInt(hDlg, ID_GEDIT,
                                        (BOOL FAR *)&bReceived, NULL);
               if ((bReceived == NULL) || (uiGValue > 255) ||
                   (uiGValue < 0))
                  {
                  MessageBox(hDlg, (LPSTR)"Invalid Green Value",
                             (LPSTR)"Range Error (0-255)!", MB_OK);
                  bSuccess = FALSE;
                  }
               uiBValue = GetDlgItemInt(hDlg, ID_BEDIT,
                                        (BOOL FAR *)&bReceived, NULL);
               if ((bReceived == NULL) || (uiBValue > 255) ||
                   (uiBValue < 0))
                  {
                  MessageBox(hDlg, (LPSTR)"Invalid Blue Value",
                             (LPSTR)"Range Error (0-255)!", MB_OK);
                  bSuccess = FALSE;
                  }
               if (bSuccess) { /* If all are OK, continue */
                  /* Get the DC and get the nearest color of the user's
                     input. */
                  hDC = GetDC(hDlg);
                  dwrgbValue = GetNearestColor(hDC, RGB((BYTE)uiRValue,
                                                        (BYTE)uiGValue,
                                                        (BYTE)uiBValue)
                                              );
                  ReleaseDC(hDlg, hDC);
                  sprintf(szOutBuf, "%s%d%s%d%s%d%s",
                          "The Nearest Color is: (",
                          GetRValue(dwrgbValue),
                          " , ", GetGValue(dwrgbValue),
                          " , ", GetBValue(dwrgbValue), ").");
                  /* Print a message box and set the focus back so that
                     the user can enter values at will in a loop. */
                  MessageBox(hDlg, (LPSTR)szOutBuf,
                             (LPSTR)"GetNearestColor()", MB_OK);
                  }
               SetFocus(GetDlgItem(hDlg, ID_REDIT));
               SendMessage(GetDlgItem(hDlg, ID_REDIT), EM_SETSEL, NULL,
                           MAKELONG((WORD)0, (WORD)32767));
               return TRUE;
            
            default:
               return FALSE;
            };
         break;

      case WM_INITDIALOG:
         /* When the box first comes up, limit the text for each
            numeric value to three digits so that the user is
            less likely to enter an erroneus value. */
         SendMessage(GetDlgItem(hDlg, ID_REDIT), EM_LIMITTEXT,
                     (WORD)3, (LONG)NULL);
         SendMessage(GetDlgItem(hDlg, ID_GEDIT), EM_LIMITTEXT,
                     (WORD)3, (LONG)NULL);
         SendMessage(GetDlgItem(hDlg, ID_BEDIT), EM_LIMITTEXT,
                     (WORD)3, (LONG)NULL);
         /* Intialize all of the edit controls to zero. */
         SetDlgItemInt(hDlg, ID_REDIT, (unsigned)0, NULL);
         SetDlgItemInt(hDlg, ID_GEDIT, (unsigned)0, NULL);
         SetDlgItemInt(hDlg, ID_BEDIT, (unsigned)0, NULL);
         SetFocus(GetDlgItem(hDlg, ID_REDIT));
         SendMessage(GetDlgItem(hDlg, ID_REDIT), EM_SETSEL, NULL,
                     MAKELONG((WORD)0, (WORD)32767));
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
    MSG       msg;
    HWND      hWnd;
    PWNDCLASS pNearClass;

    if (!hPrevInstance) {
       pNearClass = (PWNDCLASS)LocalAlloc(LPTR, sizeof(WNDCLASS));

       pNearClass->hCursor        = LoadCursor(NULL, IDC_ARROW);
       pNearClass->hIcon          = NULL;
       pNearClass->lpszMenuName   = (LPSTR)"NearMenu";
       pNearClass->lpszClassName  = (LPSTR)"gnearclr";
       pNearClass->hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
       pNearClass->hInstance      = hInstance;
       pNearClass->style          = CS_HREDRAW | CS_VREDRAW;
       pNearClass->lpfnWndProc    = NearestWndProc;

       if (!RegisterClass((LPWNDCLASS)pNearClass))
           /* Initialization failed.
            * Windows will automatically deallocate all allocated memory.
            */
           return FALSE;

       LocalFree((HANDLE)pNearClass);
        }

    hWnd = CreateWindow((LPSTR)"gnearclr",
                        (LPSTR)"GetNearestColor() Demo",
                        WS_TILEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL);      /* no params to pass on */

    /* Save instance handle for DialogBox */
    hInst = hInstance;

    /* Bind callback function with module instance */
    lpprocColor = MakeProcInstance((FARPROC)ColorDlg, hInstance);

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
long FAR PASCAL NearestWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_COMMAND:
        switch (wParam)
        {
        case ID_NEAR:
           /* Let 'er rip. */
            DialogBox(hInst, MAKEINTRESOURCE(NEARBOX), hWnd, lpprocColor);
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
