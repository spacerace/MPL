#include <windows.h>
#include "crbitmap.h"

HANDLE hInst;    
HBITMAP hBitmap;                    /* bitmap structure handle */

short nBlobBitmap[] = {
        0x0000, 0x0000,
        0x0000, 0x0000,
        0x000f, 0xf000,
        0x000f, 0xf000,
        0x00ff, 0xff00,
        0x00ff, 0xff00,
        0x0fff, 0xfff0,
        0x0fff, 0xfff0,
        0x0ff0, 0x0ff0,
        0x0ff0, 0x0ff0,
        0xfff0, 0x0fff,
        0xfff0, 0x0fff,
        0xff00, 0x00ff,
        0xff00, 0x00ff,
        0xffff, 0xffff,
        0xffff, 0xffff};

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;       /* current instance */
HANDLE hPrevInstance;   /* previous instance */
LPSTR lpCmdLine;        /* command line */
int nCmdShow;           /* show window type (open/icon) */
{
    HWND hWnd;          /* window handle */
    MSG msg;            /* message */
    HMENU hMenu;        /* handle to the menu */
    
    if  (!hPrevInstance)        /* app already initialized? */
        if (!CreateBitMapInit(hInstance))       /* nope, init it */
            return(NULL);               /* couldn't init */

    hInst = hInstance;          /* store the current instance */
    
    hWnd = CreateWindow("CreateBitMap", /* window class */
        "CreateBitMap Sample Application",      /* window name */
        WS_OVERLAPPEDWINDOW,            /* window style */
        CW_USEDEFAULT,                  /* x position */
        CW_USEDEFAULT,                  /* y position */
        CW_USEDEFAULT,                  /* width */
        CW_USEDEFAULT,                  /* height */
        NULL,                           /* parent handle */
        NULL,                           /* menu or child */
        hInstance,                      /* instance */
        NULL);                          /* additional info */
        
    if (!hWnd)                  /* did we get a valid handle? */
        return(NULL);           /* nope, couldn't open window */
        
    ShowWindow(hWnd, nCmdShow);         /* show the window */
    UpdateWindow(hWnd);                 /* send the WM_PAINT msg */
    
    while (GetMessage(&msg, NULL, NULL, NULL))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return(msg.wParam);         /* return value from PostQuitMessage */
}

/* register the window */
BOOL CreateBitMapInit(hInstance)
HANDLE hInstance; /* current instance */
{
    HANDLE hMemory;             /* handle to allocated memory */
    PWNDCLASS pWndClass;        /* structure pointer */
    BOOL bSuccess;              /* saves RegisterClass status */
    
    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    
    pWndClass->lpszClassName = (LPSTR)  "CreateBitMap";
    pWndClass->hInstance = hInstance;
    pWndClass->lpfnWndProc = CreateBitMapWndProc;
    pWndClass->style = NULL;
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) NULL;
    
    bSuccess = RegisterClass(pWndClass);
    
    LocalUnlock(hMemory);               /* unlock the memory */
    LocalFree(hMemory);                 /* return it to windows */
    
    return (bSuccess);
}

/* process messages to the window */

long FAR PASCAL CreateBitMapWndProc(hWnd, message, wParam, lParam)
HWND hWnd;                              /* window handle */
unsigned message;                       /* type of message */
WORD wParam;                            /* additional information */
LONG lParam;                            /* additional information */
{
    FARPROC lpProcAbout;                /* pointer to "About" procedure */
    HMENU hMenu;                        /* menu handle */
    HDC hMemoryDC;                      /* memory device context */
    HDC hDC;                            /* display device context */
    HANDLE hOldObject;                  /* return code from SelectObject */

    switch (message)
    {
        case WM_CREATE :                /* add command to system menu */
            hMenu = GetSystemMenu(hWnd, FALSE);
            ChangeMenu(hMenu, NULL, NULL, NULL,
                MF_APPEND | MF_SEPARATOR);
            ChangeMenu(hMenu, NULL, "A&bout CreateBitMap...",
            ID_ABOUT, MF_APPEND | MF_STRING);
            /* create bitmap */
            hBitmap = CreateBitmap(32, 16, 1, 1, (LPSTR)nBlobBitmap);
            if (hBitmap == NULL)
                MessageBox(NULL, (LPSTR)"Error creating Bitmap",
                           NULL, IDOK);
            break;
            
        case WM_SYSCOMMAND :
            switch (wParam)
            {
                case ID_ABOUT:
                    lpProcAbout = MakeProcInstance(About, hInst);
                    DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
                    FreeProcInstance(lpProcAbout);
                    break;
                    
                default:
                    return(DefWindowProc(hWnd, message, wParam, lParam));
            }
            break;

        case WM_PAINT:
            hDC = GetDC(hWnd);
            hMemoryDC = CreateCompatibleDC(hDC);
            hOldObject = SelectObject(hMemoryDC, hBitmap);
            if (hOldObject == NULL)
                MessageBox(NULL, (LPSTR)"Error selecting object",
                           NULL, IDOK);
            BitBlt(hDC, 50, 30, 32, 16, hMemoryDC, 0, 0, SRCCOPY);
            /* clean up */
            SelectObject(hMemoryDC, hOldObject);
            DeleteDC(hMemoryDC);
            ReleaseDC(hWnd, hDC);
            break;
                        
        case WM_DESTROY:                /* quit application */
	    DeleteObject(hBitmap);	/* delete bitmap */
            PostQuitMessage(NULL);      /* notify windows */
            break;
            
        default:                        /* pass it on if unprocessed */
            return(DefWindowProc(hWnd, message, wParam, lParam));
    }
    return(NULL);
}
                        
/* this function handles the "About" box */

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;                       /* type of message */
WORD wParam;                            /* additional information */
LONG lParam;                            /* additional information */
{
    switch (message)
    {
        case WM_INITDIALOG:             /* init dialog box */
            return(TRUE);               /* don't need to do anything */
        case WM_COMMAND:                /* received a command */
            if (wParam == IDOK)         /* OK box selected? */
            {
                EndDialog (hDlg, NULL);         /* then exit */
                return(TRUE);
            }
            break;
    }
    return(FALSE);                      /* didn't process a message */
}
