
/* Create the server's communications window. If successful
returns window handle. If error returns NULL. */

HWND CreateCommWindow(
    HANDLE hInstance,
    HANDLE hPrevInstance )
{
/* Server's class and window name as defined by WinTrieve
protocol specification. */

    static char    szCommName = "ISAM SERVER";

    WNDCLASS       wndclass;
    HWND           hWnd;

    /* Only one instance of the server is allowed
     * to run at any one time. Return error.
     */
    if (hPrevInstance)
      return NULL;

    /* Register the server window class. Nothing special
here. */
    wndclass.style          = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc    = WndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = 0;
    wndclass.hInstance      = hInstance;
    wndclass.hIcon          = NULL;
    wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground  = GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName   = NULL;
    wndclass.lpszClassName  = szCommName;

    if (!RegisterClass(&wndclass))
      return NULL;


    /* Create server communications window. */
    hWnd = CreateWindow(szCommName, szCommName,
                        WS_OVERLAPPED|WS_SYSMENU,
                        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
                        NULL, NULL, hInstance, NULL);
   return hWnd;
};
