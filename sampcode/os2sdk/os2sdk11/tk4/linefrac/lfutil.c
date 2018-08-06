/************************************************************************
*
*   lfutil.c -- Utility subroutines.
*
*   Created by Microsoft Corporation, 1989
*
************************************************************************/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>

#include <mt\stdio.h>
#include <mt\stdlib.h>
#include "linefrac.h"

#define INCL_LFUTIL
#include "lffuncs.h"




/************************************************************************
*
*   MySetWindowUShort
*
*   Sets the given control id to the value specified.
*
************************************************************************/

VOID
MySetWindowUShort(hWnd, id, num)
HWND hWnd;
USHORT id;
USHORT num;
{
    char szStr[CCHSTR];

    sprintf((NPCH)szStr, "%u", num);
    WinSetWindowText(WinWindowFromID(hWnd, id), (PCH)szStr);
}




/************************************************************************
*
*   MySetWindowLong
*
*   Sets the given control id to the value specified.
*
************************************************************************/

VOID
MySetWindowLong(hWnd, id, num)
HWND hWnd;
USHORT id;
LONG num;
{
    char szStr[CCHSTR];

    sprintf((NPCH)szStr, "%ld", num);
    WinSetWindowText(WinWindowFromID(hWnd, id), (PCH)szStr);
}




/************************************************************************
*
*   MySetWindowDouble
*
*   Sets the given control id to the value specified.
*
************************************************************************/

VOID
MySetWindowDouble(hWnd, id, num)
HWND hWnd;
USHORT id;
double num;
{
    char szStr[CCHSTR];

    sprintf((NPCH)szStr, "%lf", num);
    WinSetWindowText(WinWindowFromID(hWnd, id), (PCH)szStr);
}




/************************************************************************
*
*   MyGetWindowUShort
*
*   Returns the value from the given control id.
*
************************************************************************/

VOID
MyGetWindowUShort(hWnd, id, pus)
HWND hWnd;
USHORT id;
PUSHORT pus;
{
    char szStr[CCHSTR];

    WinQueryWindowText(WinWindowFromID(hWnd, id), CCHSTR, (PCH)szStr);
    *pus = atoi(szStr);
}




/************************************************************************
*
*   MyGetWindowLong
*
*   Returns the value from the given control id.
*
************************************************************************/

VOID
MyGetWindowLong(hWnd, id, pl)
HWND hWnd;
USHORT id;
PLONG  pl;
{
    char szStr[CCHSTR];

    WinQueryWindowText(WinWindowFromID(hWnd, id), CCHSTR, (PCH)szStr);
    *pl = atol(szStr);
}




/************************************************************************
*
*   MyGetWindowDouble
*
*   Returns the value from the given control id.
*
************************************************************************/

VOID
MyGetWindowDouble(hWnd, id, pdbl)
HWND   hWnd;
USHORT id;
PDBL   pdbl;
{
    char szStr[CCHSTR];

    WinQueryWindowText(WinWindowFromID(hWnd, id), CCHSTR, (PCH)szStr);
    *pdbl = atof(szStr);
}




/************************************************************************
*
*   MyMessageBox
*
*   Displays a message box with the given string.  To simplify matters,
*   the box will always have the same title ("LineFractal"), will always
*   have a single button ("Ok"), will always have an exclamation point
*   icon, and will always be application modal.
*
************************************************************************/

VOID
MyMessageBox(hWnd, sz)
HWND hWnd;
PSZ sz;
{
    static char *szTitle = "LineFractal";

    WinMessageBox(HWND_DESKTOP, hWnd, sz, szTitle, NULL,
		  MB_OK|MB_ICONEXCLAMATION|MB_APPLMODAL);
}
