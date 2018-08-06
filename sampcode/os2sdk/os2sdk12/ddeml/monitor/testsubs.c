/****************************** Module Header ******************************\
* Module Name: TestSubs.C
*
* Collection of useful routines for test applications
*
* Created: 16-Jan-87
*
* Copyright (c) 1985, 1986, 1987  Microsoft Corporation
*
\***************************************************************************/

#define INCL_WINHEAP
#define INCL_WINWINDOWMGR
#define INCL_WINRECTANGLES
#define INCL_WINTIMER
#define INCL_WINSYS
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "monitor.h"


/***************************************************************************/

extern HAB hab;

/* StringWindow structure */
typedef struct {
    int cchLine;
    int cLine;
    char *pchBuffer;
    char *pchBufferMax;
    char *pchBottomLine;
    char *pchOutput;
} STRWND;

int cyChar;                     /* Height of a line */
int cyDescent;

USHORT ticks;                     /* Tick count used with StartTime/StopTime */

HHEAP hhp = NULL;

/***************************************************************************/

/***************************** Public  Function ****************************\
* BOOL InitTestSubs( )
*
* This routine MUST be called before using anything in this file.  Registers
* window classes, loads brushes, etc.  Returns TRUE if successful, FALSE
* otherwise.
*
* Warnings: Must be called AFTER the global variable NULL is initialized.
*
* History:
*  19-Jan-87 by neilk  Created
\***************************************************************************/

BOOL InitTestSubs()
{
    cyChar = 14;
    cyDescent = 2;

    if (!WinRegisterClass(hab, (PCH)"StringWindow", (PFNWP)StrWndProc,
       CS_SYNCPAINT, sizeof(STRWND *)))
        return(FALSE);

    hhp = WinCreateHeap(0, 0, 0, 0, 0, 0);

    return(TRUE);
}


void
NextLine(
STRWND *psw)
{    
    psw->pchBottomLine += psw->cchLine;
    if (psw->pchBottomLine == psw->pchBufferMax)
        psw->pchBottomLine = psw->pchBuffer;
    psw->pchOutput = psw->pchBottomLine;
    *psw->pchOutput = '\0';
}


/***************************** Public  Function ****************************\
* VOID DrawString(hwnd, sz)
*
* This routine prints a string in the specified StringWindow class window.
* sz is a near pointer to a zero-terminated string, which can be produced
* with sprintf().
*
* History:
*  19-Jan-87 by neilk  Created
\***************************************************************************/

VOID DrawString(hwnd, sz)
HWND hwnd;
char *sz;
{
    register STRWND *psw;
    USHORT cLines = 0;

    psw = (STRWND *)WinQueryWindowUShort(hwnd, 0);
    NextLine(psw);
    cLines++;

    while (*sz) {
        switch (*sz) {
        case 0x0a:
            break;
            
        case 0x09:
            *psw->pchOutput++ = ' ';
            *psw->pchOutput++ = ' ';
            *psw->pchOutput++ = ' ';
            *psw->pchOutput++ = ' ';
            break;
            
        case 0x0d:
            *psw->pchOutput++ = '\0';
            NextLine(psw);
            cLines++;
            break;
            
        default:
            *psw->pchOutput++ = *sz;
        }
        sz++;
    }
    WinScrollWindow(hwnd, 0, cyChar * cLines, (PWRECT)NULL, (PWRECT)NULL, (HRGN)NULL,
      (PWRECT)NULL, SW_INVALIDATERGN);
}

/***************************** Public  Function ****************************\
* "StringWindow" window class
*
* Windows of the "StringWindow" window class are simple scrolling text output
* windows that are refreshed properly as windows are rearranged.  A text buffer
* is maintained to store the characters as they are drawn.
*
* When creating a StringWindow window, lpCreateParams is actually a ULONG
* containing the dimensions of the text buffer to be created, if 0L, then
* a 80 by 25 buffer is created.
*
* History:
*  19-Jan-87 by neilk  Created
\***************************************************************************/

MRESULT FAR PASCAL StrWndProc(hwnd, msg, mp1, mp2)
register HWND hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    register STRWND *psw;
    HPS hps;
    RECTL rclPaint;

    switch (msg) {
    case WM_CREATE:
        if (mp1     == 0L) {
            mp1     = MAKEULONG(80, 50);
        }
        if (!StrWndCreate(hwnd, SHORT1FROMMP(mp1), SHORT2FROMMP(mp1)))
            return(TRUE);
        break;

    case WM_DESTROY:
        if ((psw = (STRWND *)WinQueryWindowUShort(hwnd, 0)) != NULL) {
            WinFreeMem(hhp, (BYTE *)psw->pchBuffer, strlen(psw->pchBuffer));
            WinFreeMem(hhp, (BYTE *)psw, sizeof(STRWND));
        }
        break;

    case WM_PAINT:
        hps = WinBeginPaint(hwnd, (HPS)NULL, &rclPaint);
        PaintStrWnd(hwnd, hps, &rclPaint);
        WinEndPaint(hps);
        break;

    case WM_CALCVALIDRECTS:
#ifdef LATER
        /*
         * We want to bottom right justify the bits.
         */
        (mp2)->yTop = (mp2)->yBottom -
                (mp1)->yBottom;
#endif
        break;

    default:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0L);
}



BOOL StrWndCreate(hwnd, cchLine, cLine)
HWND hwnd;
int cchLine, cLine;
{
    register char *pch;
    char *pchEnd;
    STRWND *psw;

    if ((psw = (STRWND *)WinAllocMem(hhp, sizeof(STRWND))) == NULL)
        return(FALSE);
    /*
     * Allocate buffer for strings
     */
    if ((pch = (char *)WinAllocMem(hhp, cchLine * cLine)) == NULL) {
        WinFreeMem(hhp, (BYTE *)psw, sizeof(STRWND));
        return(FALSE);
    }

    psw->cchLine       = cchLine;
    psw->cLine         = cLine;
    psw->pchBuffer     = pch;
    psw->pchBufferMax  = pch + cchLine * cLine;
    psw->pchBottomLine = pch;
    psw->pchOutput     = pch;
    WinSetWindowUShort(hwnd, 0, (USHORT)psw);

    /*
     * Make all the lines empty
     */
    pchEnd = psw->pchBufferMax;
    while (pch != pchEnd) {
        *pch = '\0';
        pch += cchLine;
    }
    return(TRUE);
}



VOID PaintStrWnd(hwnd, hps, prcl)
HWND hwnd;
HPS hps;
PRECTL prcl;
{
    register STRWND *psw;
    register char *pch;
    int x;
    int y;
    WRECT rc;
    RECTL rcl;

    psw = (STRWND *)WinQueryWindowUShort(hwnd, 0);

    WinQueryWindowRect(hwnd, (PRECTL)&rc);

    WinFillRect(hps, (PRECTL)&rc, SYSCLR_WINDOW);

    x = rc.xLeft;
    y = rc.yBottom;
    pch = psw->pchBottomLine;

    if (prcl != NULL)
        WinIntersectRect(hab, (PRECTL)&rc, (PRECTL)&rc, prcl);

    do {
        if (y >= rc.yTop)
            break;
        if (y + cyChar >= rc.yBottom) {
            rcl.xLeft = x;
            rcl.yBottom = y + cyDescent;
            rcl.xRight = 1000;
            rcl.yTop = rcl.yBottom + cyChar;
            WinDrawText(hps, -1, (PSZ)pch, (PRECTL)&rcl, SYSCLR_WINDOWTEXT,
                    SYSCLR_WINDOW, 0);
        }
        y += cyChar;
        /*
         * Back up to previous line
         */
        if (pch == psw->pchBuffer)
            pch = psw->pchBufferMax;
        pch -= psw->cchLine;
    } while (pch != psw->pchBottomLine);
}

/***************************** Public  Function ****************************\
* VOID StartTime()
* VOID StopTime(hwndStrWnd, szMsg)
*
* These functions are used to time sections of code.  StartTime() begins the
* timing, and StopTime() stops it, printing out the elapsed time since
* StartTime() was called in the "StringWindow" class window hwndStrWnd, using
* the sprintf() formatting string szMsg.  The elapsed time in milliseconds is
* replaced for a "%d" in szMsg.
*
* History:
*  19-Jan-87 by neilk  Created
\***************************************************************************/

VOID StartTime()
{
    ticks = (USHORT)WinGetCurrentTime(hab);
}

VOID StopTime(hwndStrWnd, szFmt)
HWND hwndStrWnd;
char *szFmt;
{
    USHORT dt;
    char rgch[80];

    dt = (USHORT)(WinGetCurrentTime(hab) - ticks);
    sprintf((char *)rgch, szFmt, dt);
    DrawString(hwndStrWnd, rgch);
}
