/*
==============================================================================
==============================================================================
Figure 5: WinMain  Function for WSZ
==============================================================================
*/

/*  whatsize -- Windows application in C */

#include <windows.h>
#include "wsz.h"

long FAR PASCAL MainWndProc (HWND, unsigned, WORD, LONG) ;
BOOL FAR PASCAL AboutWndProc (HWND, unsigned, WORD, LONG) ;
BOOL FAR PASCAL UnitsWndProc (HWND, unsigned, WORD, LONG) ;

FARPROC lpAbout ;
FARPROC lpUnits ;
HANDLE  hInst;
int     CurrentUnits = ID_PIXELS ;
int     CurrentColor = ID_BLACK ;

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
    HANDLE hInstance, hPrevInstance;
    LPSTR  lpszCmdLine;
    int    nCmdShow;
    {
    MSG    msg;

    if (!Initialize (hInstance, hPrevInstance, lpszCmdLine, nCmdShow))
        return FALSE ;

    while (GetMessage ((LPMSG)&msg, NULL, 0, 0))
        {
        TranslateMessage ((LPMSG)&msg) ;
        DispatchMessage ((LPMSG)&msg) ;
        }
    return (msg.wParam) ;
    }

/*
==============================================================================
==============================================================================

Figure 6: Initialization Function for WSZ
==============================================================================
*/

BOOL Initialize (hInstance, hPrevInst, lpszCmdLine, nCmdShow)
    HANDLE   hInstance, hPrevInst ;
    LPSTR    lpszCmdLine ;
    int      nCmdShow ;
    {
    char     *szAppName [10] ;
    char     *szAbout [10] ;
    char     *szTitle [30] ;
    WNDCLASS wndclass ;
    HWND     hWnd;
    HMENU    hMenu;

    LoadString (hInstance, IDS_NAME, (LPSTR) szAppName, 10) ;
    LoadString (hInstance, IDS_ABOUT, (LPSTR) szAbout, 10) ;
    LoadString (hInstance, IDS_TITLE, (LPSTR) szTitle, 30 ) ;

    if (!hPrevInst)
        {
        wndclass.style        = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc  = MainWndProc;
        wndclass.cbClsExtra   = 0;
        wndclass.cbWndExtra   = 0;
        wndclass.hInstance    = hInstance ;
        wndclass.hIcon        = LoadIcon (hInstance, (LPSTR) szAppName);
        wndclass.hCursor      = LoadCursor (NULL, IDC_ARROW);
        wndclass.hbrBackground= (HBRUSH) GetStockObject (WHITE_BRUSH);
        wndclass.lpszMenuName = (LPSTR) szAppName;
        wndclass.lpszClassName= (LPSTR) szAppName;

        if (!RegisterClass ((LPWNDCLASS) &wndclass))
            return FALSE;
        }
    else
        {
        GetInstanceData(hPrevInst,(NPSTR)&CurrentUnits,sizeof(int));
        GetInstanceData(hPrevInst,(NPSTR)&CurrentColor,sizeof(int));
        }

    hWnd = CreateWindow (
            (LPSTR) szAppName,  /* class name       */
            (LPSTR) szTitle,    /* caption title    */
            WS_TILEDWINDOW,     /* windows style    */
            0,                  /* x (ignored)      */
            0,                  /* y (ignored)      */
            0,                  /* width (ignored)  */
            0,                  /* height (ignored) */
            (HWND) NULL,        /* parent (none)    */
            (HMENU) NULL,       /* menu (use class) */
            (HANDLE) hInstance, /* instance handle  */
            (LPSTR) NULL) ;     /* parameters       */

    hInst = hInstance ;

    lpAbout = MakeProcInstance ((FARPROC) AboutWndProc, hInstance) ;
    lpUnits = MakeProcInstance ((FARPROC) UnitsWndProc, hInstance) ;

    hMenu = GetSystemMenu (hWnd, FALSE) ;
    ChangeMenu (hMenu, 0, NULL, 999, MF_APPEND | MF_SEPARATOR) ;
    ChangeMenu (hMenu, 0, (LPSTR) szAbout, IDM_ABOUT, 
      MF_APPEND | MF_STRING) ;

    hMenu = GetMenu (hWnd) ;
    CheckMenuItem (hMenu, CurrentColor, MF_CHECKED) ;

    ShowWindow (hWnd, nCmdShow) ;
    UpdateWindow (hWnd) ;

    return TRUE ;
    }

/*
==============================================================================
==============================================================================

Figure 7: MainWndProc Procedure for WSZ
==============================================================================
*/

long FAR PASCAL MainWndProc (hWnd, message, wParam, lParam)
    HWND       hWnd;
    unsigned   message;
    WORD       wParam;
    LONG       lParam;
    {
    HMENU      hMenu ;
    static int CharHorzPix, CharVertPix ;
    static int ScrnHorzPix, ScrnVertPix ;
    static int ScrnHorzMil, ScrnVertMil ;
    static int WindHorzPix, WindVertPix ;

    switch (message)
        {
        case WM_CREATE:
            {
            TEXTMETRIC tm ;
            HDC        hDC = GetDC (hWnd) ;

            GetTextMetrics (hDC, (LPTEXTMETRIC) &tm) ;
            CharHorzPix = tm.tmAveCharWidth ;
            CharVertPix = tm.tmHeight ;

            ScrnHorzPix = GetDeviceCaps (hDC, HORZRES) ;
            ScrnVertPix = GetDeviceCaps (hDC, VERTRES) ;
            ScrnHorzMil = GetDeviceCaps (hDC, HORZSIZE) ;
            ScrnVertMil = GetDeviceCaps (hDC, VERTSIZE) ;

            ReleaseDC (hWnd, hDC) ;
            }
            break ;

        case WM_SIZE:
            WindHorzPix = LOWORD (lParam) ;
            WindVertPix = HIWORD (lParam) ;
            break ;

        case WM_ERASEBKGND:
            return TRUE ;

        case WM_PAINT:
            {
            PAINTSTRUCT ps ;
            char        szFormat [20] ;
            char        szUnits [20] ;
            char        szBuffer [60] ;
            float       nHorz = (float) WindHorzPix ;
            float       nVert = (float) WindVertPix ;
            RECT        rect ;

            if (CurrentUnits != ID_PIXELS)
                {
                nHorz *= (float) ScrnHorzMil / ScrnHorzPix ;
                nVert *= (float) ScrnVertMil / ScrnVertPix ;
                }
            if (CurrentUnits == ID_INCHES)
                {
                nHorz /= 25.4 ;
                nVert /= 25.4 ;
                }

            BeginPaint (hWnd, (LPPAINTSTRUCT) &ps) ;

            PatBlt (ps.hdc, 0, 0, WindHorzPix, WindVertPix,
               (CurrentColor == ID_WHITE) ? BLACKNESS : WHITENESS) ;

            if (CurrentColor == ID_WHITE)
                {
                SetTextColor (ps.hdc, RGB (255, 255, 255)) ;
                SetBkColor (ps.hdc, RGB (0, 0, 0)) ;
                }
            LoadString (hInst, IDS_FORMAT, (LPSTR) szFormat, 20) ;
            LoadString (hInst, CurrentUnits, (LPSTR) szUnits, 20) ;

            rect.bottom = WindVertPix - (rect.top  = CharVertPix) ;
            rect.right  = WindHorzPix - (rect.left = CharHorzPix) ;

            DrawText (ps.hdc, (LPSTR) szBuffer,
                sprintf(szBuffer,szFormat,nHorz,szUnits,nVert,szUnits),
                (LPRECT) &rect, DT_CENTER | DT_WORDBREAK) ;

            EndPaint (hWnd, (LPPAINTSTRUCT) &ps) ;
            }
            break;

        case WM_SYSCOMMAND:
            switch (wParam)
                {
                case IDM_ABOUT:
                    DialogBox(hInst,(LPSTR)"AboutBox",hWnd,lpAbout);
                    break;

                default:
                    return DefWindowProc(hWnd,message,wParam,lParam);
                }
            break;

        case WM_COMMAND :
            switch (wParam)
                {
                case ID_BLACK:
                case ID_WHITE:
                    hMenu = GetMenu (hWnd) ;
                    CheckMenuItem(hMenu,CurrentColor,MF_UNCHECKED);
                    CheckMenuItem (hMenu, CurrentColor = wParam, 
                         MF_CHECKED) ;
                    InvalidateRect (hWnd, (LPRECT) NULL, TRUE) ;
                    break ;

                case IDM_UNITS:
                    if (DialogBox (hInst, (LPSTR) "UnitsBox", hWnd, 
                              lpUnits))
                         InvalidateRect (hWnd, (LPRECT) NULL, TRUE) ;
                    break ;

                default :
                    return DefWindowProc (hWnd, message, wParam, lParam) ;
                }
            break ;

        case WM_DESTROY:
            PostQuitMessage (0) ;
            break;

        default:
            return DefWindowProc (hWnd, message, wParam, lParam) ;
            break;
        }
    return (0L) ;
    }

/*
==============================================================================
==============================================================================

Figure 8: AboutWndProc Procedure for WSZ
==============================================================================
*/

BOOL FAR PASCAL AboutWndProc (hDlg, message, wParam, lParam)
    HWND     hDlg ;
    unsigned message ;
    WORD     wParam ;
    LONG     lParam ;
    {
    switch (message)
        {
        case WM_INITDIALOG :
            return TRUE ;

        case WM_COMMAND :
            EndDialog (hDlg, TRUE) ;
            return TRUE ;

        default:
            return FALSE ;
        }
    }

/*
==============================================================================
==============================================================================

Figure 9: UnitsWndProc Procedure for WSZ
==============================================================================
*/

BOOL FAR PASCAL UnitsWndProc (hDlg, message, wParam, lParam)
    HWND       hDlg ;
    unsigned   message ;
    WORD       wParam ;
    LONG       lParam ;
    {
    static int NewUnits ;

    switch (message)
        {
        case WM_INITDIALOG :
            CheckRadioButton (hDlg,ID_PIXELS,ID_INCHES,CurrentUnits);
            SetFocus (GetDlgItem (hDlg, NewUnits = CurrentUnits)) ;
            return FALSE ;

        case WM_COMMAND :
            switch (wParam)
                {
                case ID_MILLIM:
                case ID_INCHES:
                case ID_PIXELS:
                    NewUnits = wParam ;
                    CheckRadioButton (hDlg,ID_PIXELS,ID_INCHES,NewUnits);
                    break ;

                case IDOK:
                    CurrentUnits = NewUnits ;
                    EndDialog (hDlg, TRUE) ;
                    break ;

                case IDCANCEL:
                    EndDialog (hDlg, FALSE) ;
                    break ;

                default:
                    return FALSE ;
                }
            break ;

        default:
            return FALSE ;
        }
    return TRUE ;
    }

