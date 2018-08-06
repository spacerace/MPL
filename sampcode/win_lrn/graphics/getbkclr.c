/*
 *
 *   This program demonstrates the use of the GetBkColor function. GetBkColor
 *   gets the double word value of the color that goes behind text, etc.
 *   in the given device.
 *
 */

#include <windows.h>
#include <stdio.h>

#define STR_PUR1	"The purpose of this program is too return the RGB value"
#define STR_PUR2	" that represents the background color. The value is"
#define STR_PUR3	" represented as (rrr, ggg, bbb)."

BOOL HelloInit (HANDLE);
int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
long FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);

char  szVerbage[47] = "The Background color is behind these characters";

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit(hInstance)
HANDLE hInstance;
{
    WNDCLASS   rHelloClass;

    rHelloClass.hCursor        = LoadCursor(NULL,IDC_ARROW );
    rHelloClass.hIcon		    = NULL;
    rHelloClass.lpszMenuName   = (LPSTR)NULL;
    rHelloClass.lpszClassName	 = (LPSTR)"Sample Application";
    rHelloClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    rHelloClass.hInstance      = hInstance;
    rHelloClass.style          = CS_HREDRAW | CS_VREDRAW;
    rHelloClass.lpfnWndProc    = HelloWndProc;

    if (!RegisterClass((LPWNDCLASS)&rHelloClass))
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    return TRUE;        /* Initialization succeeded */
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;
    HMENU hMenu;
	 char	 szPurpose[255];                /* Message box buffers */
    char  szBuff[80];	
    HDC   hDC;                          /*   display context   */
    DWORD dwBkColor;            /* return value from GetBkColr */

    HelloInit( hInstance );
    hWnd = CreateWindow((LPSTR)"Sample Application",
								(LPSTR)"GetBkColor",
								WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    hDC = GetDC(hWnd);

	 sprintf(szPurpose,"%s%s%s\0",STR_PUR1,STR_PUR2,STR_PUR3);

    MessageBox(hWnd,(LPSTR)szPurpose,(LPSTR)"GET BACKGROUNDCOLOR",MB_OK);

    TextOut(hDC,5,5,(LPSTR)szVerbage,(short)47);

    dwBkColor = GetBkColor(hDC);  /* Get the background color of the DC */

    sprintf(szBuff,
            "RGB value of the background color: (%hu, %hu, %hu)",
            GetRValue(dwBkColor), GetGValue(dwBkColor),
            GetBValue(dwBkColor));
    MessageBox(hWnd, (LPSTR)szBuff, (LPSTR)"GET BACKGROUND COLOR", MB_OK);

    SetBkColor(hDC,RGB(0x00,0x00,0x00)); /* change the background to black */
    SetTextColor(hDC,RGB(0xff,0xff,0xff));
    TextOut(hDC,5,25,(LPSTR)szVerbage,(short)47);

    dwBkColor = GetBkColor(hDC);  /* Get the background color of the DC */

    sprintf(szBuff,
            "RGB value of the background color: (%hu, %hu, %hu)",
            GetRValue(dwBkColor), GetGValue(dwBkColor),
            GetBValue(dwBkColor));
    MessageBox(hWnd, (LPSTR)szBuff, (LPSTR)"GET BACKGROUND COLOR", MB_OK);

    ReleaseDC(hWnd,hDC);

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}

/* Procedures which make up the window class. */
long FAR PASCAL HelloWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_SYSCOMMAND:
	return DefWindowProc(hWnd,message,wParam,lParam);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_PAINT:
        BeginPaint(hWnd,(LPPAINTSTRUCT)&ps);
        EndPaint(hWnd,(LPPAINTSTRUCT)&ps);
        break;

    default:
        return DefWindowProc(hWnd,message,wParam,lParam );
        break;
    }
    return(0L);
}
