/*
Function(s) demonstrated in this program: GetPolyFillMode
					  SetPolyFillMode

Description:  This program demonstrates differences in the two
	      Polygon filling modes, ALTERNATE and WINDING.  In ALTERNATE
	      mode, every other region is filled.  In WINDING, all regions
	      are filled.  The two shapes drawn here demonstrate the
	      difference.

	      SetPolyFillMode is used to select the mode;

	      GetPolyFillMode is used to retrieve the current fill mode and
	      print it on the screen while the polygon is drawn.

*/

#include <windows.h>

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);
void DrawPoly (HDC, short, short, short, short);


int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
    HANDLE	hInstance, hPrevInstance;
    LPSTR	lpszCmdLine;
    int 	nCmdShow;
    {
    static char szAppName[]="Get/Set/PolyFillMode";
    static BYTE byExtra[]={32,66,121,32,75,114,97,105,103,32,
			   66,114,111,99,107,115,99,104,109,105,100,116,32};
    HWND	hWnd;
    WNDCLASS	wndclass;
    MSG 	msg;

    if (!hPrevInstance)
	 {
	 wndclass.style 	= CS_HREDRAW | CS_VREDRAW;
	 wndclass.lpfnWndProc	= WndProc;
	 wndclass.cbClsExtra	= 0;
	 wndclass.cbWndExtra	= 0;
	 wndclass.hInstance	= hInstance;
	 wndclass.hIcon 	= LoadIcon(NULL, IDI_APPLICATION);
	 wndclass.hCursor	= LoadCursor(NULL, IDC_ARROW);
	 wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
	 wndclass.lpszMenuName	= szAppName;
	 wndclass.lpszClassName = szAppName;

	 if (!RegisterClass(&wndclass))
	      return FALSE;
	 }

    hWnd=CreateWindow(szAppName, szAppName,
		   WS_OVERLAPPEDWINDOW,
		   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		   NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0))
	{
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	}
    return (msg.wParam);
    }

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
    HWND	hWnd;
    unsigned	iMessage;
    WORD	wParam;
    LONG	lParam;
    {
    HDC 	hDC;
    HMENU	hMenu;
    HPEN	hPen;
    PAINTSTRUCT ps;

    switch(iMessage)
	{
	case WM_PAINT:
	    hDC=BeginPaint(hWnd, &ps);
	    hPen=CreatePen(PS_SOLID, 2, 0L);

	    SelectObject(hDC, hPen);
	    SelectObject(hDC, GetStockObject(GRAY_BRUSH));

	    TextOut(hDC, 10, 10, "PolyFillMode=", 13);
	    TextOut(hDC, 10, 190, "ALTERNATE", 9);
	    TextOut(hDC, 140,190, "WINDING", 7);

	    DrawPoly (hDC,  40,  70, ALTERNATE, 0);
	    DrawPoly (hDC, 170,  70, WINDING,	0);
	    DrawPoly (hDC,  40, 150, ALTERNATE, 1);
	    DrawPoly (hDC, 170, 150, WINDING,	1);

	    EndPaint(hWnd, &ps);
	    DeleteObject(hPen);
	    break;

	case WM_DESTROY:
	    PostQuitMessage(0);
	    break;

	default:
	    return DefWindowProc (hWnd, iMessage, wParam, lParam);
	}
    return (0L);
    }

void DrawPoly (hDC, xOrg, yOrg, nMode, nPt)
    HDC 	hDC;
    short	xOrg, yOrg, nMode, nPt;
    {
    short	xOld, yOld;
    long	lView;
    /* Define points in figures */
    static POINT pts0[]={0,30, -30,-30, 30,0, -30,30,
			 0,-30, 30,30, -30,0, 30,-30,
			 0,30};

    static POINT pts1[]={0,-30, 25,25, -35,-10,
			 35,-10, -25,25, 0,-30};

    xOld=LOWORD(lView=GetViewportOrg(hDC));
    yOld=HIWORD(lView);

    SetPolyFillMode(hDC, nMode);
    if (GetPolyFillMode(hDC)==ALTERNATE)
	TextOut(hDC, 40,20, "ALTERNATE",9);
    else
	TextOut(hDC, 40,20, "WINDING  ",9);

    /* Set selected origin */
    SetViewportOrg(hDC, xOrg, yOrg);

    /* Draw either Polygon 1 or 2 */
    if (nPt)
	Polygon(hDC, pts1, sizeof(pts1)/sizeof(POINT));
    else
	Polygon(hDC, pts0, sizeof(pts0)/sizeof(POINT));

    /* Restore old origin */
    SetViewportOrg(hDC, xOld, yOld);
    return;
    }
