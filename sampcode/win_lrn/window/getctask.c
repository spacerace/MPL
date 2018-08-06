/*
 *  GetCurrentTask
 *
 *  This program will get the handle to the current task and send it a 
 *  message to kill itself.
 *
 */

#include "windows.h"      /* required for all Windows applications */
#include "getctask.h"

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
        if (!GetCTaskInit(hInstance))
            return (NULL);           /* Exits if unable to initialize     */

    hInst = hInstance;               /* Saves the current instance        */

    hMenu = LoadMenu( hInst, (LPSTR)"GetCTaskMenu" );
                                     /*  Load the menu    */

    hWnd = CreateWindow("GetCTask",   /* window class      */
        "GetCurrentTask Sample Application",  /* window name       */
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
    return (msg.wParam);         /* Returns the value from PostQuitMessage */
}

BOOL GetCTaskInit(hInstance)
HANDLE hInstance;                /* current instance           */
{
    HANDLE hMemory;              /* handle to allocated memory */
    PWNDCLASS pWndClass;         /* structure pointer          */
    BOOL bSuccess;               /* RegisterClass() result     */

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);

    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = GetCTaskWndProc;
    pWndClass->hInstance = hInstance;
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->lpszMenuName = (LPSTR) "GetCTaskMenu";
    pWndClass->lpszClassName = (LPSTR) "GetCTask";

    bSuccess = RegisterClass(pWndClass);

    LocalUnlock(hMemory);     /* Unlocks the memory    */
    LocalFree(hMemory);       /* Returns it to Windows */

    return (bSuccess);        /* Returns result of registering the window */
}

long FAR PASCAL GetCTaskWndProc(hWnd, message, wParam, lParam)
HWND hWnd;                    /* window handle                   */
unsigned message;             /* type of message                 */
WORD wParam;                  /* additional information          */
LONG lParam;                  /* additional information          */
{
    FARPROC lpProcAbout;      /* pointer to the "About" function */
    HMENU hMenu;              /* handle to the System menu       */

    switch (message) {

        case WM_COMMAND:
            if ( wParam == IDM_TRASH )
              {
              PostAppMessage( GetCurrentTask( ),  /*  Get current task  */
                              WM_QUIT,      /*  Message to quit  */
                              NULL,         /*  wParam  */
                              (LONG)NULL);  /*  lParam  */
              }
            break;

/*  PostMessage places the message into the applications message queue and
 *  does not wait for a response.  This is different from the SendMessage
 *  function in that SendMessage does wait.  The Window Procedure is called
 *  immediately when SendMessage is used.  For more information see
 *  PostMessage and SendMessage in the Programmer's Reference.
 */
        case WM_DESTROY:             /* message: window being destroyed */
            PostQuitMessage(0);
            break;

        default:                     /* Passes it on if unproccessed    */
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}
