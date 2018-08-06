#include "windows.h"

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL GetCRectInit(HANDLE);
long FAR PASCAL GetCRectWndProc(HWND, unsigned, WORD, LONG);

HANDLE hInst;

/**************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;           /* current instance             */
HANDLE hPrevInstance;       /* previous instance            */
LPSTR lpCmdLine;            /* command line                 */
int nCmdShow;               /* show-window type (open/icon) */
{
    HWND hWnd;              /* window handle                */
    MSG msg;                /* message                      */


    if (!hPrevInstance)     /* Has application been initialized? */
        if (!GetCRectInit(hInstance))
            return (NULL);  /* Exits if unable to initialize     */

    hInst = hInstance;      /* Saves the current instance        */

    hWnd = CreateWindow("GetCRect",     /* window class            */
        "GetCRect Sample Application",  /* window name             */
        WS_OVERLAPPEDWINDOW,           /* window style            */
        CW_USEDEFAULT,                 /* x position              */
        CW_USEDEFAULT,                 /* y position              */
        CW_USEDEFAULT,                 /* width                   */
        CW_USEDEFAULT,                 /* height                  */
        NULL,                          /* parent handle           */
        NULL,                          /* menu or child ID        */
        hInstance,                     /* instance                */
        NULL);                         /* additional info         */

    if (!hWnd)                         /* Was the window created? */
        return (NULL);

    ShowWindow(hWnd, nCmdShow);        /* Shows the window        */
    UpdateWindow(hWnd);                /* Sends WM_PAINT message  */

    while (GetMessage(&msg,     /* message structure                      */
            NULL,               /* handle of window receiving the message */
            NULL,               /* lowest message to examine              */
            NULL))              /* highest message to examine             */
        {
        TranslateMessage(&msg); /* Translates virtual key codes           */
        DispatchMessage(&msg);  /* Dispatches message to window           */
    }
    return (msg.wParam);        /* Returns the value from PostQuitMessage */
}


/*************************************************************************/

BOOL GetCRectInit(hInstance)
HANDLE hInstance;                 /* current instance           */
{
    HANDLE hMemory;               /* handle to allocated memory */
    PWNDCLASS pWndClass;          /* structure pointer          */
    BOOL bSuccess;                /* RegisterClass() result     */

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);

    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = GetCRectWndProc;
    pWndClass->hInstance = hInstance;
    pWndClass->hIcon = NULL;
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->lpszMenuName = (LPSTR) "GetCRectMenu";
    pWndClass->lpszClassName = (LPSTR) "GetCRect";

    bSuccess = RegisterClass(pWndClass);

    LocalUnlock(hMemory);                           /* Unlocks the memory    */
    LocalFree(hMemory);                             /* Returns it to Windows */

    return (bSuccess);           /* Returns result of registering the window */
}

/**************************************************************************/

long FAR PASCAL GetCRectWndProc(hWnd, message, wParam, lParam)
HWND hWnd;                      /* window handle                   */
unsigned message;               /* type of message                 */
WORD wParam;                    /* additional information          */
LONG lParam;                    /* additional information          */
{
    FARPROC lpProcAbout;        /* pointer to the "About" function */
    HMENU hMenu;                /* handle to the System menu       */
    RECT  rRect;                /* Will hold client rectangle  */
    HANDLE hDC;                 /* Handle to the display context  */
    PAINTSTRUCT ps;             /* Paint Structure  */

    switch (message) {
        case WM_SIZE:
        case WM_PAINT:
            GetClientRect( hWnd,           /*  Window handle  */
                         (LPRECT)&rRect ); /*  Structure holding  */
            InvalidateRect( hWnd, (LPRECT)&rRect, TRUE );
                               /*  Erase the background  */
            hDC = BeginPaint ( hWnd, &ps );   /*  Get the display context  */
/*  We are going to draw an X through the window  */            
            MoveTo( hDC, rRect.left, rRect.top );
            LineTo( hDC, rRect.right, rRect.bottom );
            MoveTo( hDC, rRect.right, rRect.top );
            LineTo( hDC, rRect.left, rRect.bottom );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:                  /* message: window being destroyed */
            PostQuitMessage(0);
            break;

        default:                          /* Passes it on if unproccessed    */
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}


