/*
Author: Gregg Spaulding
        Tencor Instruments
        2400 Charleston Road
        Mountain View, Ca. 94043
*/

#include <windows.h>
#include "select.h"
#include <stdlib.h>

int FAR PASCAL lstrlen ( LPSTR );
int FAR PASCAL lstrcpy ( LPSTR, LPSTR );

typedef struct {
    int yBitStart;     /* y location to start bitmap at */
    int yCharStart;    /* y location to start text at */
    int xCharWidth;    /* width of a character */
    int yCharHeight;   /* height of character */
    RECT frect;        /* formatting rectangle for text */
    BOOL bFocus;       /* this control has the input focus */
    HWND hWndList;
    int index, count;  /* which item is selected and how many items total */
    HANDLE handle[1];  /* expandable array of handles to strings */
} HSELECT;
typedef HSELECT NEAR * NPHSELECT;

#define GWW_MEMHANDLE        0
#define GWW_MEMSIZE          2

extern HANDLE hInstLib;
extern HANDLE hBitmap;
#define XBITMAP    16
#define YBITMAP    16

/* we hope that this is never needed */
static char * szMemErr = "Out of Memory!";

static void near ChangeSelection (HWND hWnd, BOOL bRel, int sel);


long FAR PASCAL SelectWndProc( HWND hWnd, unsigned message,
    WORD wParam, LONG lParam)
{
    HDC hDC, hMemDC;          /* display contexts for drawing */
    PAINTSTRUCT ps;           /* for BeginPaint and EndPaint */
    TEXTMETRIC tm;            /* need info about character sizes */
    RECT rect;                /* generic */
    HANDLE hSelect, hString;  /* memory handles */
    NPHSELECT pSelect;        /* pointer to structure header */
    NPSTR pString;            /* pointer to a string */
    POINT pt1,pt2;
    int i;                    /* generic */

    switch (message)
    {
        case WM_CREATE:

            /* calculate size of initial node not including any */
            /* handles to strings, allocate the memory and lock it */

            i = sizeof(HSELECT)-sizeof(HANDLE);
            if ((hSelect = LocalAlloc( LMEM_MOVEABLE, i ) ) == NULL) {
                MessageBox( hWnd, szMemErr, NULL, MB_ICONEXCLAMATION);
                break;
            }
            pSelect = (NPHSELECT)LocalLock( hSelect );

            /* place the memory handle and the current node size into */
            /* the window structure for later retrieval */

            SetWindowWord ( hWnd, GWW_MEMHANDLE, hSelect );
            SetWindowWord ( hWnd, GWW_MEMSIZE, i );

            /* calculate the height of a character */

            hDC = GetDC ( hWnd );
            GetTextMetrics ( hDC, &tm );
            ReleaseDC( hWnd, hDC );
            pSelect->yCharHeight = i = tm.tmHeight + tm.tmExternalLeading;

            /* fill in the structure */

            GetClientRect ( hWnd, &rect );
            pSelect->yCharStart = (rect.bottom - i) / 2;
            pSelect->xCharWidth = tm.tmAveCharWidth;
            pSelect->yBitStart = (rect.bottom - YBITMAP) / 2;
            rect.left = XBITMAP + 4;
            rect.top = pSelect->yCharStart - 2;
            rect.bottom = rect.top + i + 4;
            pSelect->frect = rect;
            pSelect->count = 0;
            pSelect->index = -1;
            pSelect->hWndList = pSelect->bFocus = FALSE;

            /* unlock the structure and return */

            LocalUnlock ( hSelect );
            break;

        case WM_DESTROY:
            
            /* all we have to do is free up all associated memory */

            hSelect = GetWindowWord ( hWnd, GWW_MEMHANDLE );
            pSelect = (NPHSELECT)LocalLock ( hSelect );
            for (i=0; i<pSelect->count; i++) LocalFree ( pSelect->handle[i] );
            if (pSelect->hWndList != NULL) DestroyWindow(pSelect->hWndList);
            LocalUnlock ( hSelect );
            LocalFree ( hSelect );
            LocalShrink ( 0, 200 );    /* bring heap down to min size */
            break;

        case WM_GETDLGCODE:

            /* prevent the Dialog Manager from capturing the arrow keys */
            /* we want them here for our keyboard interface */

            return DLGC_WANTARROWS;

        case WM_PAINT:

            hDC = BeginPaint ( hWnd, &ps );

            /* lock down the structure associated with this control */

            hSelect = GetWindowWord ( hWnd, GWW_MEMHANDLE );
            pSelect = (NPHSELECT)LocalLock ( hSelect );

            /* draw the bitmap'd circular arrows */

            hMemDC = CreateCompatibleDC ( hDC );
            SelectObject ( hMemDC, hBitmap );
            BitBlt ( hDC, 2, pSelect->yBitStart,
                XBITMAP, YBITMAP, hMemDC, 0, 0, SRCCOPY );
            DeleteObject ( hMemDC );

            /* draw the current string selection */

            rect = pSelect->frect;
            if (pSelect->index > -1) {
                hString = pSelect->handle[pSelect->index];
                pString = LocalLock ( hString );
                i = lstrlen(pString);
                TextOut ( hDC, rect.left+2, pSelect->yCharStart, pString, i );
                LocalUnlock( hString );

                /* if we currently have the input focus, invert it */

                if (pSelect->bFocus) {
                    rect.right = rect.left + i*pSelect->xCharWidth + 4;
                    InvertRect( hDC, &rect );
                }
            }
            LocalUnlock( hSelect );
            EndPaint ( hWnd, &ps );
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            /* focus please! */
            SetFocus(hWnd);
            break;

        case WM_LBUTTONUP:
            /* obvious */
            ChangeSelection (hWnd,TRUE,1);
            break;

        case WM_RBUTTONUP:

            /* pop up list box with all choices */

            hSelect = GetWindowWord ( hWnd, GWW_MEMHANDLE );
            pSelect = (NPHSELECT)LocalLock ( hSelect );
            GetClientRect( hWnd,&rect );
            /* calculate corners of box in screen coordinates */
            pt1.x = pSelect->frect.left;
            pt1.y = rect.bottom;
            pt2.x = rect.right;
            pt2.y = pt1.y + pSelect->yCharHeight * pSelect->count;
            ClientToScreen(hWnd,&pt1);
            ClientToScreen(hWnd,&pt2);
				pt2.x += 2*GetSystemMetrics( SM_CXDLGFRAME );
				pt2.y += 2*GetSystemMetrics( SM_CYDLGFRAME );
            pSelect->hWndList = CreateWindow (
                "listbox", "", WS_POPUP | WS_DLGFRAME | LBS_NOTIFY,
                pt1.x, pt1.y, pt2.x - pt1.x, pt2.y - pt1.y,
                hWnd, NULL, hInstLib, NULL );

            /* WINDOWS has set the parent to be the dialog manager, */
            /* we must make this call to regain our child */

            SetWindowWord ( pSelect->hWndList, GWW_HWNDPARENT, hWnd);

            /* fill the list box with all choices */

            for (i=0;i<pSelect->count;i++) {
                hString = pSelect->handle[i];
                pString = LocalLock( hString );
                SendMessage(pSelect->hWndList, LB_ADDSTRING,
                    0, (LONG)(LPSTR)pString );
                LocalUnlock ( hString);
            }
            SendMessage (pSelect->hWndList, LB_SETCURSEL,
                pSelect->index, 0L);

            /* listbox has been created and filled, now show it */

            ShowWindow ( pSelect->hWndList, SW_SHOW );
            LocalUnlock (hSelect);
            EnableWindow (GetParent(hWnd), FALSE);
            break;

          case WM_COMMAND:
              /* listbox is informing us that something has happened */
              hSelect = GetWindowWord ( hWnd, GWW_MEMHANDLE );
              pSelect = (NPHSELECT)LocalLock ( hSelect );
              PostMessage ( hWnd, SL_SETCURSEL,
                  SendMessage ( pSelect->hWndList, LB_GETCURSEL, 0, 0L), 0L );
              if (HIWORD(lParam) == LBN_SELCHANGE) {
                  DestroyWindow(pSelect->hWndList);
                  pSelect->hWndList = NULL;
                  EnableWindow (GetParent(hWnd), TRUE);
                  SetFocus(hWnd);
              }
              LocalUnlock(hSelect);
              break;
            
        case WM_KEYUP:
            /* keyboard interface, simple enough */
            switch (wParam) {
                case VK_LEFT:
                case VK_UP:
                    ChangeSelection(hWnd,TRUE,1);
                    break;
                case VK_RIGHT:
                case VK_DOWN:
                    ChangeSelection(hWnd,TRUE,-1);
                    break;
                default:
                    break;
            }
            break;

        case WM_KILLFOCUS:

            /* if we're losing the input focus, force a repaint to */
            /* uninvert the selection back to normal */

            hSelect = GetWindowWord ( hWnd, GWW_MEMHANDLE );
            pSelect = (NPHSELECT)LocalLock ( hSelect );
                if (!pSelect->hWndList)    {
                pSelect->bFocus = FALSE;
                InvalidateRect ( hWnd, &(pSelect->frect), TRUE );
                }
            LocalUnlock ( hSelect );
            break;

        case WM_SETFOCUS:

            /* if we're gaining the input focus, force a repaint to */
            /* invert the selection to highlighted */

            hSelect = GetWindowWord ( hWnd, GWW_MEMHANDLE );
            pSelect = (NPHSELECT)LocalLock ( hSelect );
            pSelect->bFocus = TRUE;
            InvalidateRect ( hWnd, &(pSelect->frect), TRUE );
            LocalUnlock ( hSelect );
            break;

        case SL_ADDSTRING:

            /* sent from dialog function to add a new string to */
            /* out current set of possible selections */
            /* increase the current header structure size to accomodate */
            /* an additional string handle */

            hSelect = GetWindowWord ( hWnd, GWW_MEMHANDLE );
            i = GetWindowWord ( hWnd, GWW_MEMSIZE );
            i += sizeof( HANDLE );
            if ((hSelect = LocalReAlloc ( hSelect, i, LMEM_MOVEABLE ))
            == NULL) {
                MessageBox( hWnd, szMemErr, NULL, MB_ICONEXCLAMATION);
                break;
            }
            SetWindowWord ( hWnd, GWW_MEMHANDLE, hSelect );
            SetWindowWord ( hWnd, GWW_MEMSIZE, i );

            /* lock down the structure and allocate a new piece of */
            /* memory large enough to hold the added string */

            pSelect = (NPHSELECT)LocalLock ( hSelect );
            i = lstrlen( (LPSTR)lParam ) + 1; 
            if ((hString = LocalAlloc ( LMEM_MOVEABLE,
            lstrlen( (LPSTR)lParam ) + 1 )) == NULL) {
                MessageBox( hWnd, szMemErr, NULL, MB_ICONEXCLAMATION);
                break;
            }

            /* lock down the memory, copy the string into it, link it */
            /* into the structure, increase the count, and unlock */

            pString = LocalLock ( hString );
            lstrcpy ( pString, (LPSTR)lParam );
            LocalUnlock ( hString );
            pSelect->handle[pSelect->count++] = hString;
            LocalUnlock ( hSelect );
            break;

        case SL_SETCURSEL:
            
            /* sent from dialog function to arbitrarily set the current */
            /* selection to one of the strings.  typically sent in */
            /* response to the WM_INITDIALOG message */

            ChangeSelection ( hWnd, FALSE, wParam );
            break;

        case SL_GETCURSEL:

            /* sent by dialog function to request which string is currently */
            /* selected.  the index is returned */

            hSelect = GetWindowWord ( hWnd, GWW_MEMHANDLE );
            pSelect = (NPHSELECT)LocalLock( hSelect );
            lParam = pSelect->index;
            LocalUnlock ( hSelect );
            return lParam;

        case SL_GETTEXT:

            /* sent by dialog function to request the current string */
            /* (text not ordinal number as with SL_GETCURSEL) */

            hSelect = GetWindowWord ( hWnd, 0 );
            pSelect = (NPHSELECT)LocalLock( hSelect );
            if (wParam < 0 || wParam >= pSelect->count) {
                *(LPSTR)lParam = NULL;
                return -1;
            }
            else {
                hString = pSelect->handle[wParam];
                pString = LocalLock ( hString );
                lstrcpy ( (LPSTR)lParam, pString );
                LocalUnlock ( hString );
            }    
            LocalUnlock ( hSelect );
            break;

        case SL_GETCOUNT:

            /* returns the number of strings currently defined for */
            /* this control */

            hSelect = GetWindowWord ( hWnd, 0 );
            pSelect = (NPHSELECT)LocalLock( hSelect );
            wParam = pSelect->count;
            LocalUnlock ( hSelect );
            return wParam;

        default:
            return((long)DefWindowProc(hWnd, message, wParam, lParam));
    }
    return(0L);
}


static void near ChangeSelection (HWND hWnd, BOOL bRel, int sel)
{
    HANDLE hSelect;
    NPHSELECT pSelect;

     /* get the handle to the structure and lock it down */
    hSelect = GetWindowWord ( hWnd, GWW_MEMHANDLE );
    pSelect = (NPHSELECT)LocalLock( hSelect );

     /* if change is relative, take care of it */
    if (bRel) {
        if (sel > 0) pSelect->index++;
        else if (sel < 0) pSelect->index--;
        if (sel > 0 && pSelect->index == pSelect->count)
            pSelect->index = 0;
        else if (sel < 0 && pSelect->index < 0)
            pSelect->index = pSelect->count - 1;
    }
     /* if change is absolute, set it */
    else if (sel >= -1 && sel < pSelect->count) pSelect->index = sel;
    
    /* force a repaint */
    InvalidateRect ( hWnd, &(pSelect->frect), TRUE );
    LocalUnlock( hSelect );

    /* inform our parent that we are changing the selection */
    PostMessage ( GetParent(hWnd), WM_COMMAND,
        GetWindowWord (hWnd, GWW_ID), MAKELONG(hWnd, SLN_SELCHANGE) );
}
