/*
 *  SetPriority
 *  setpri.c
 *
 *  This program modifies its priority using the SetPriority function.  It
 *  has two menu items.  One to set the priority high and the other to set
 *  it low.
 *
 */

#include "windows.h"      /* required for all Windows applications */
#include "setpri.h"       /* specific to this program              */

HANDLE hInst;             /* current instance                      */

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;         /* current instance             */
HANDLE hPrevInstance;     /* previous instance            */
LPSTR lpCmdLine;          /* command line                 */
int nCmdShow;             /* show-window type (open/icon) */
{
    HWND hWnd;            /* window handle       */
    MSG msg;              /* message             */
    HANDLE hMenu;         /* Handle to the menu  */

    if (!hPrevInstance)              /* Has application been initialized? */
        if (!SetPriInit(hInstance))
            return (NULL);           /* Exits if unable to initialize     */

    hInst = hInstance;               /* Saves the current instance        */

    hMenu = LoadMenu( hInst, (LPSTR)"SetPriMenu" );
                                     /*  Load the menu    */

    hWnd = CreateWindow("SetPri",   /* window class      */
        "SetPri Sample Application",  /* window name       */
        WS_OVERLAPPEDWINDOW,         /* window style      */
        CW_USEDEFAULT,               /* x position        */
        CW_USEDEFAULT,               /* y position        */
        CW_USEDEFAULT,               /* width             */
        CW_USEDEFAULT,               /* height            */
        NULL,                        /* parent handle     */
        hMenu,                       /* menu or child ID  */
        hInstance,                   /* instance          */
        NULL);                       /* additional info   */

    if (!hWnd)                       /* Was the window created? */
        return (NULL);

    ShowWindow(hWnd, nCmdShow);      /* Shows the window        */
    UpdateWindow(hWnd);              /* Sends WM_PAINT message  */

    while (GetMessage(&msg,     /* message structure                      */
            NULL,               /* handle of window receiving the message */
            NULL,               /* lowest message to examine              */
            NULL))              /* highest message to examine             */
        {
        TranslateMessage(&msg);  /* Translates virtual key codes           */
        DispatchMessage(&msg);   /* Dispatches message to window           */
    }
    return (msg.wParam);         /* Returns the value from PostSetPriMessage */
}

BOOL SetPriInit(hInstance)
HANDLE hInstance;                /* current instance           */
{
    HANDLE hMemory;              /* handle to allocated memory */
    PWNDCLASS pWndClass;         /* structure pointer          */
    BOOL bSuccess;               /* RegisterClass() result     */

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);

    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = SetPriWndProc;
    pWndClass->hInstance = hInstance;
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->lpszMenuName = (LPSTR) "SetPriMenu";
    pWndClass->lpszClassName = (LPSTR) "SetPri";

    bSuccess = RegisterClass(pWndClass);

    LocalUnlock(hMemory);     /* Unlocks the memory    */
    LocalFree(hMemory);       /* Returns it to Windows */

    return (bSuccess);        /* Returns result of registering the window */
}

long FAR PASCAL SetPriWndProc(hWnd, message, wParam, lParam)
HWND hWnd;                    /* window handle                   */
unsigned message;             /* type of message                 */
WORD wParam;                  /* additional information          */
LONG lParam;                  /* additional information          */
{
    FARPROC lpProcAbout;      /* pointer to the "About" function */
    HMENU hMenu;              /* handle to the System menu       */

    switch (message) {
        case WM_SYSCOMMAND:   /* message: command from system menu */
            if (wParam == ID_ABOUT) {
                lpProcAbout = MakeProcInstance(About, hInst);

                DialogBox(hInst,         /* current instance         */
                    "AboutBox",          /* resource to use          */
                    hWnd,                /* parent handle            */
                    lpProcAbout);        /* About() instance address */

                FreeProcInstance(lpProcAbout);
                break;
            }

            else                         /* Lets Windows process it       */
                return (DefWindowProc(hWnd, message, wParam, lParam));

        case WM_CREATE:                  /* message: window being created */

            /* Get the handle of the System menu */

            hMenu = GetSystemMenu(hWnd, FALSE);

            /* Add a separator to the menu */

            ChangeMenu(hMenu,                    /* menu handle         */
                NULL,                            /* menu item to change */
                NULL,                            /* new menu item       */
                NULL,                            /* menu identifier     */
                MF_APPEND | MF_SEPARATOR);       /* type of change      */

            /* Add new menu item to the System menu */

            ChangeMenu(hMenu,                    /* menu handle         */
                NULL,                            /* menu item to change */
                "A&bout SetPri...",             /* new menu item       */
                ID_ABOUT,                        /* menu identifier     */
                MF_APPEND | MF_STRING);          /* type of change      */
            break;

        case WM_COMMAND:
            if ( wParam == IDM_HIGH )
              SetPriority( GetCurrentTask(),  /*  Priority of current task  */
                           -15 );  /*  -15 is the highest priority  */
            if ( wParam == IDM_LOW )
              SetPriority( GetCurrentTask(),  /*  Again, the current task  */
                           15 );  /*  15 is the lowest priority  */

/*  The default value for the priority is zero  */

            break;

        case WM_DESTROY:             /* message: window being destroyed */
            PostQuitMessage(0);
            break;

        default:                     /* Passes it on if unproccessed    */
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message) {
        case WM_INITDIALOG:              /* message: initialize dialog box */
            return (TRUE);

        case WM_COMMAND:                 /* message: received a command */
            if (wParam == IDOK) {        /* "OK" box selected?          */
                EndDialog(hDlg, NULL);   /* Exits the dialog box        */
                return (TRUE);
            }
            break;
    }
    return (FALSE);                      /* Didn't process a message    */
}
