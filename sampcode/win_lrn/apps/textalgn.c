/*
 * This application demonstrates the GetTextAlign() and SetTextAlign()
 * Windows functions. This program is a subset of the SHOWFONT learning
 * guide application. See that program for a more comprehensive example.
 *
 * In this application, a vertical and/or horizontal alignment is
 * selected via the "Options..." menu (or the defaults are used),
 * and the left mouse button is clicked in the client area of this
 * demo application.
 *
 */

#include "windows.h"
#include <string.h>
#include "textalgn.h"

/* Function Prototypes */

void SetMyDC(HDC);
short StringOut(HDC, short, short, PSTR, HFONT);
void ShowString(HWND);
long FAR PASCAL TextAlgnWndProc(HWND, unsigned, WORD, LONG);

HANDLE  hInst;
HFONT   hSFont, hFont;
char    line[4][64];
LOGFONT LogFont;
POINT   ptCurrent = {0, 0};
short   nBkMode = OPAQUE;
DWORD   rgbBkColor = RGB(255, 255, 255);
DWORD   rgbTextColor = RGB(0, 0, 0);
short   nAlignLCR = TA_LEFT;
short   nAlignTBB = TA_TOP; 
WORD    wPrevVAlign = IDM_ALIGNBASE;
WORD    wPrevHAlign = IDM_ALIGNLEFT;
char    AppName[] = "Get/SetTextAlign() Demo";
char    WindowTitle[80];


int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR  lpCmdLine;
int    nCmdShow;
{
    HWND hWnd;
    PWNDCLASS pWndClass;
    MSG msg;

    if (!hPrevInstance) {
       pWndClass->hCursor       = LoadCursor(NULL, IDC_ARROW);
       pWndClass->hIcon         = NULL;
       pWndClass->lpszMenuName  = (LPSTR) "TextAlgn";
       pWndClass->lpszClassName = (LPSTR) "TextAlgn";
       pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
       pWndClass->hInstance     = hInstance;
       pWndClass->style         = NULL;
       pWndClass->lpfnWndProc   = TextAlgnWndProc;
       RegisterClass((LPWNDCLASS) pWndClass);
       }

    hInst = hInstance;

    hWnd = CreateWindow((LPSTR)"TextAlgn",
                        (LPSTR)"TextAlgn",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

    if (!hWnd)
        return (FALSE);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (msg.wParam);
}


/****************************************************************************

    FUNCTION: SetMyDC(HDC)

    PURPOSE: Initializes the DC

****************************************************************************/

void SetMyDC(hDC) 
HDC hDC;
{
    SetBkMode(hDC, nBkMode);
    SetBkColor(hDC, rgbBkColor);
    SetTextColor(hDC, rgbTextColor);
    SetTextAlign(hDC, nAlignLCR | nAlignTBB);
}

/****************************************************************************

    FUNCTION: StringOut(HDC, short, short, PSTR, HFONT)

    PURPOSE: Outputs a string to application's window

****************************************************************************/

short StringOut(hDC, X, Y, pString, hFont)
HDC   hDC;
short X;
short Y;
PSTR  pString;
HFONT hFont;
{
    HFONT hOldFont;
    DWORD dwExtent;

    hOldFont = SelectObject(hDC, hFont);
    if (hOldFont != NULL) {
        dwExtent = GetTextExtent(hDC, pString, strlen(pString));
        TextOut(hDC, X, Y, pString, strlen(pString));
        SelectObject(hDC, hOldFont);
    }
    return (LOWORD(dwExtent));
}

/****************************************************************************

    FUNCTION: ShowString(HWND)

    PURPOSE: Show string in current font

****************************************************************************/

void ShowString(hWnd)
HWND hWnd;
{
    HFONT hItalicFont;
    HFONT hBoldFont;
    HFONT hUnderlineFont;
    HFONT hStrikeOutFont;
    HDC   hDC;
    short X, tmpX;
    short Y;
    short nAlign;

    GetObject(hFont, sizeof(LOGFONT), (LPSTR) &LogFont);
    LogFont.lfItalic    = TRUE;
    hItalicFont         = CreateFontIndirect(&LogFont);
    LogFont.lfItalic    = FALSE;
    LogFont.lfUnderline = TRUE;
    hUnderlineFont      = CreateFontIndirect(&LogFont);
    LogFont.lfUnderline = FALSE;
    LogFont.lfStrikeOut = TRUE;
    hStrikeOutFont      = CreateFontIndirect(&LogFont);
    LogFont.lfStrikeOut = FALSE;
    LogFont.lfWeight    = FW_BOLD;
    hBoldFont           = CreateFontIndirect(&LogFont);

    hDC = GetDC(hWnd);
    SetMyDC(hDC);
    X = ptCurrent.x;
    Y = ptCurrent.y;
    nAlign = nAlignLCR | nAlignTBB;                 /* GetTextAlign(hDC); */
    if ((nAlign & TA_CENTER) == TA_CENTER) {
        tmpX = X;
        nAlignLCR = TA_LEFT;
        SetTextAlign(hDC, nAlignLCR | nAlignTBB);
        X += StringOut(hDC, X, Y, ", and ", hFont);
        X += StringOut(hDC, X, Y, "strikeout", hStrikeOutFont);
        X += StringOut(hDC, X, Y, " in a single line.", hFont);
        X = tmpX;
        nAlignLCR = TA_RIGHT;
        SetTextAlign(hDC, nAlignLCR | nAlignTBB);
        X -= StringOut(hDC, X, Y, "underline", hUnderlineFont);
        X -= StringOut(hDC, X, Y, ", ", hFont);
        X -= StringOut(hDC, X, Y, "italic", hItalicFont);
        X -= StringOut(hDC, X, Y, ", ", hFont);
        X -= StringOut(hDC, X, Y, "bold", hBoldFont);
        X -= StringOut(hDC, X, Y, "You can use ", hFont);
        nAlignLCR = TA_CENTER;
    }
    else if ((nAlign & TA_CENTER) == TA_RIGHT) {
        X -= StringOut(hDC, X, Y, " in a single line.", hFont);
        X -= StringOut(hDC, X, Y, "strikeout", hStrikeOutFont);
        X -= StringOut(hDC, X, Y, ", and ", hFont);
        X -= StringOut(hDC, X, Y, "underline", hUnderlineFont);
        X -= StringOut(hDC, X, Y, ", ", hFont);
        X -= StringOut(hDC, X, Y, "italic", hItalicFont);
        X -= StringOut(hDC, X, Y, ", ", hFont);
        X -= StringOut(hDC, X, Y, "bold", hBoldFont);
        X -= StringOut(hDC, X, Y, "You can use ", hFont);
    }
    else  {
        X += StringOut(hDC, X, Y, "You can use ", hFont);
        X += StringOut(hDC, X, Y, "bold", hBoldFont);
        X += StringOut(hDC, X, Y, ", ", hFont);
        X += StringOut(hDC, X, Y, "italic", hItalicFont);
        X += StringOut(hDC, X, Y, ", ", hFont);
        X += StringOut(hDC, X, Y, "underline", hUnderlineFont);
        X += StringOut(hDC, X, Y, ", and ", hFont);
        X += StringOut(hDC, X, Y, "strikeout", hStrikeOutFont);
        X += StringOut(hDC, X, Y, " in a single line.", hFont);
    }
    ReleaseDC(hWnd, hDC);

    DeleteObject(hItalicFont);
    DeleteObject(hUnderlineFont);
    DeleteObject(hStrikeOutFont);
    DeleteObject(hBoldFont);
}

/****************************************************************************

    FUNCTION: TextAlgnWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE: Processes messages

****************************************************************************/

long FAR PASCAL TextAlgnWndProc(hWnd, message, wParam, lParam)
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
{
    HDC         hDC;
    PAINTSTRUCT ps;
    int         i;

    switch(message) {
        case WM_CREATE:
            hSFont = GetStockObject(SYSTEM_FONT);
            hFont  = hSFont;
            GetObject(hFont, sizeof(LOGFONT), (LPSTR) &LogFont);

            for (i=0; i<64; i++) {
                line[0][i] = (char)i;
                line[1][i] = (char)i+64;
                line[2][i] = (char)i+128;
                line[3][i] = (char)i+192;
            }
            break;

        case WM_PAINT:
            hDC = BeginPaint(hWnd, &ps);
            SetMyDC(hDC);
            EndPaint(hWnd, &ps);
            break;

        case WM_COMMAND:
            switch (wParam) {
                case IDM_CLEAR:
                    InvalidateRect(hWnd, (LPRECT)NULL, TRUE);
                    break;

                case IDM_ALIGNLEFT:
                    nAlignLCR = TA_LEFT;
                    CheckMenuItem(GetMenu(hWnd), wPrevHAlign, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevHAlign = wParam;
                    break;

                case IDM_ALIGNCENTER:
                    nAlignLCR = TA_CENTER;
                    CheckMenuItem(GetMenu(hWnd), wPrevHAlign, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevHAlign = wParam;
                    break;

                case IDM_ALIGNRIGHT:
                    nAlignLCR = TA_RIGHT;
                    CheckMenuItem(GetMenu(hWnd), wPrevHAlign, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevHAlign = wParam;
                    break;

                case IDM_ALIGNTOP:
                    nAlignTBB = TA_TOP;
                    CheckMenuItem(GetMenu(hWnd), wPrevVAlign, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevVAlign = wParam;
                    break;

                case IDM_ALIGNBASE:
                    nAlignTBB = TA_BASELINE;
                    CheckMenuItem(GetMenu(hWnd), wPrevVAlign, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevVAlign = wParam;
                    break;

                case IDM_ALIGNBOTTOM:
                    nAlignTBB = TA_BOTTOM;
                    CheckMenuItem(GetMenu(hWnd), wPrevVAlign, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevVAlign = wParam;
                    break;
            }
            break;

        case WM_LBUTTONUP:
            ptCurrent.x = LOWORD(lParam);
            ptCurrent.y = HIWORD(lParam);
            ShowString(hWnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (0L);
}
