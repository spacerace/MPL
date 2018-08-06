/***************************************************************************\
* spytbls.c
*
*    Includes the definitions of the different tables used in the spy
*    program
*
* Created by Microsoft Corporation, 1989
\***************************************************************************/

#define	INCL_WINHEAP		/* for spy.h */
#define	INCL_WINPOINTERS	/* for spy.h */
#include <os2.h>
#include "spyhook.h"
#include <stdio.h>
#include <string.h>
#include "spy.h"


/*
 * First define the message information table.  This table will be
 * used to display messages with
 */
MSGI rgmsgi[] =
{
    {0x0000, "WM_NULL", MSGI_ENABLED, 0, 0},
        {0x0001, "WM_CREATE", MSGI_ENABLED, 0, 0},
        {0x0002, "WM_DESTROY", MSGI_ENABLED, 0, 0},
        {0x0003, "WM_OTHERWINDOWDESTROYED", MSGI_ENABLED, 0, 0},
        {0x0004, "WM_ENABLE", MSGI_ENABLED, 0, 0},
        {0x0005, "WM_SHOW", MSGI_ENABLED, 0, 0},
        {0x0006, "WM_MOVE", MSGI_ENABLED, 0, 0},
        {0x0007, "WM_SIZE", MSGI_ENABLED, 0, 0},
        {0x0008, "WM_ADJUSTWINDOWPOS", MSGI_ENABLED, MPTS(MPT_SWP, 0), 0},
        {0x0009, "WM_CALCVALIDRECTS", MSGI_ENABLED, MPTS(MPT_RECTL, MPT_SWP), 0},
        {0x000a, "WM_SETWINDOWPARAMS", MSGI_ENABLED, MPTS(MPT_WNDPRMS, 0), 0},
        {0x000b, "WM_QUERYWINDOWPARAMS", MSGI_ENABLED, 0, 0},
        {0x000c, "WM_HITTEST", MSGI_FREQ, 0, 0},
        {0x000d, "WM_ACTIVATE", MSGI_ENABLED, 0, 0},
        {0x000f, "WM_SETFOCUS", MSGI_ENABLED, 0, 0},
        {0x0010, "WM_SETSELECTION", MSGI_ENABLED, 0, 0},
        {0x0020, "WM_COMMAND", MSGI_ENABLED, 0, 0},
        {0x0021, "WM_SYSCOMMAND", MSGI_ENABLED, 0, 0},
        {0x0022, "WM_HELP", MSGI_ENABLED, 0, 0},
        {0x0023, "WM_PAINT", MSGI_ENABLED, 0, 0},
        {0x0024, "WM_TIMER", MSGI_FREQ, 0, 0},
        {0x0025, "WM_SEM1", MSGI_ENABLED, 0, 0},
        {0x0026, "WM_SEM2", MSGI_ENABLED, 0, 0},
        {0x0027, "WM_SEM3", MSGI_ENABLED, 0, 0},
        {0x0028, "WM_SEM4", MSGI_ENABLED, 0, 0},
        {0x0029, "WM_CLOSE", MSGI_ENABLED, 0, 0},
        {0x002a, "WM_QUIT", MSGI_ENABLED, 0, 0},
        {0x002b, "WM_SYSCOLORCHANGE", MSGI_ENABLED, 0, 0},
        {0x002d, "WM_SYSVALUECHANGED", MSGI_ENABLED, 0, 0},
        {0x0030, "WM_CONTROL", MSGI_ENABLED, 0, 0},
        {0x0031, "WM_VSCROLL", MSGI_ENABLED, 0, 0},
        {0x0032, "WM_HSCROLL", MSGI_ENABLED, 0, 0},
        {0x0033, "WM_INITMENU", MSGI_ENABLED, 0, 0},
        {0x0034, "WM_MENUSELECT", MSGI_ENABLED, 0, 0},
        {0x0035, "WM_MENUEND", MSGI_ENABLED, 0, 0},
        {0x0036, "WM_DRAWITEM", MSGI_ENABLED, 0, 0},
        {0x0037, "WM_MEASUREITEM", MSGI_ENABLED, 0, 0},
        {0x0038, "WM_CONTROLPOINTER", MSGI_FREQ, 0, 0},
        {0x0039, "WM_CONTROLHEAP", MSGI_ENABLED, 0, 0},
        {0x003a, "WM_QUERYDLGCODE", MSGI_ENABLED, 0, 0},
        {0x003b, "WM_INITDLG", MSGI_ENABLED, 0, 0},
        {0x003c, "WM_SUBSTITUTESTRING", MSGI_ENABLED, 0, 0},
        {0x003d, "WM_MATCHMNEMONIC", MSGI_ENABLED, 0, 0},
        {0x0040, "WM_FLASHWINDOW", MSGI_ENABLED, 0, 0},
        {0x0041, "WM_FORMATFRAME", MSGI_ENABLED, 0, 0},
        {0x0042, "WM_UPDATEFRAME", MSGI_ENABLED, 0, 0},
        {0x0043, "WM_FOCUSCHANGE", MSGI_ENABLED, 0, 0},
        {0x0044, "WM_SETBORDERSIZE", MSGI_ENABLED, 0, 0},
        {0x0045, "WM_TRACKFRAME", MSGI_ENABLED, 0, 0},
        {0x0046, "WM_MINMAXFRAME", MSGI_ENABLED, MPTS(MPT_SWP, 0), 0},
        {0x0047, "WM_SETICON", MSGI_ENABLED, 0, 0},
        {0x0048, "WM_QUERYICON", MSGI_ENABLED, 0, 0},
        {0x0049, "WM_SETACCELTABLE", MSGI_ENABLED, 0, 0},
        {0x004a, "WM_QUERYACCELTABLE", MSGI_ENABLED, 0, 0},
        {0x004b, "WM_TRANSLATEACCEL", MSGI_ENABLED, MPTS(MPT_QMSG, 0), 0},
        {0x004c, "WM_QUERYTRACKINFO", MSGI_ENABLED, 0, 0},
        {0x004d, "WM_QUERYBORDERSIZE", MSGI_ENABLED, 0, 0},
        {0x004e, "WM_NEXTMENU", MSGI_ENABLED, 0, 0},
        {0x004f, "WM_ERASEBACKGROUND", MSGI_ENABLED, MPTS(MPT_RECTL, 0), 0},
        {0x0050, "WM_QUERYFRAMEINFO", MSGI_ENABLED, 0, 0},
        {0x0051, "WM_QUERYFOCUSCHAIN", MSGI_ENABLED, MPTS(MPT_SWP, 0), 0},
        {0x0053, "WM_CALCFRAMERECT", MSGI_ENABLED, MPTS(MPT_RECTL, 0), 0},
        {0x0059, "WM_QUERYFRAMECTLCOUNT", MSGI_ENABLED, 0, 0},
        {0x0060, "WM_RENDERFMT", MSGI_ENABLED, 0, 0},
        {0x0061, "WM_RENDERALLFMTS", MSGI_ENABLED, 0, 0},
        {0x0062, "WM_DESTROYCLIPBOARD", MSGI_ENABLED, 0, 0},
        {0x0063, "WM_PAINTCLIPBOARD", MSGI_ENABLED, 0, 0},
        {0x0064, "WM_SIZECLIPBOARD", MSGI_ENABLED, 0, 0},
        {0x0065, "WM_HSCROLLCLIPBOARD", MSGI_ENABLED, 0, 0},
        {0x0066, "WM_VSCROLLCLIPBOARD", MSGI_ENABLED, 0, 0},
        {0x0067, "WM_DRAWCLIPBOARD", MSGI_ENABLED, 0, 0},
        {0x0070, "WM_MOUSEMOVE", MSGI_FREQ| MSGI_MOUSE, 0, 0},
        {0x0071, "WM_BUTTON1DOWN", MSGI_ENABLED| MSGI_MOUSE, 0, 0},
        {0x0072, "WM_BUTTON1UP", MSGI_ENABLED| MSGI_MOUSE, 0, 0},
        {0x0073, "WM_BUTTON1DBLCLK", MSGI_ENABLED| MSGI_MOUSE, 0, 0},
        {0x0074, "WM_BUTTON2DOWN", MSGI_ENABLED| MSGI_MOUSE, 0, 0},
        {0x0075, "WM_BUTTON2UP", MSGI_ENABLED| MSGI_MOUSE, 0, 0},
        {0x0076, "WM_BUTTON2DBLCLK", MSGI_ENABLED| MSGI_MOUSE, 0, 0},
        {0x0077, "WM_BUTTON3DOWN", MSGI_ENABLED| MSGI_MOUSE, 0, 0},
        {0x0078, "WM_BUTTON3UP", MSGI_ENABLED| MSGI_MOUSE, 0, 0},
        {0x0079, "WM_BUTTON3DBLCLK", MSGI_ENABLED| MSGI_MOUSE, 0, 0},
        {0x007a, "WM_CHAR", MSGI_ENABLED | MSGI_KEY, 0, 0},
        {0x00A0, "WM_DDE_INITIATE", MSGI_ENABLED, 0, 0},
        {0x00A1, "WM_DDE_REQUEST", MSGI_ENABLED, 0, 0},
        {0x00A2, "WM_DDE_ACK", MSGI_ENABLED, 0, 0},
        {0x00A3, "WM_DDE_DATA", MSGI_ENABLED, 0, 0},
        {0x00A4, "WM_DDE_ADVISE", MSGI_ENABLED, 0, 0},
        {0x00A5, "WM_DDE_UNADVISE", MSGI_ENABLED, 0, 0},
        {0x00A6, "WM_DDE_POKE", MSGI_ENABLED, 0, 0},
        {0x00A7, "WM_DDE_EXECUTE", MSGI_ENABLED, 0, 0},
        {0x00A8, "WM_DDE_TERMINATE", MSGI_ENABLED, 0, 0},
        {0x00A9, "WM_DDE_INITIATEACK", MSGI_ENABLED, 0, 0},
        {0x00af, "WM_DDE_LAST", MSGI_ENABLED, 0, 0},
        {0x00b0, "WM_QUERYCONVERTPOS", MSGI_ENABLED, 0, 0},
        {0x0120, "BM_CLICK", MSGI_ENABLED, 0, 0},
        {0x0121, "BM_QUERYCHECKINDEX", MSGI_ENABLED, 0, 0},
        {0x0122, "BM_QUERYHILITE", MSGI_ENABLED, 0, 0},
        {0x0123, "BM_SETHILITE", MSGI_ENABLED, 0, 0},
        {0x0124, "BM_QUERYCHECK", MSGI_ENABLED, 0, 0},
        {0x0125, "BM_SETCHECK", MSGI_ENABLED, 0, 0},
        {0x0126, "BM_SETDEFAULT", MSGI_ENABLED, 0, 0},
        {0x0140, "EM_QUERYCHANGED", MSGI_ENABLED, 0, 0},
        {0x0141, "EM_QUERYSEL", MSGI_ENABLED, 0, 0},
        {0x0142, "EM_SETSEL", MSGI_ENABLED, 0, 0},
        {0x0143, "EM_SETTEXTLIMIT", MSGI_ENABLED, 0, 0},
        {0x0144, "EM_CUT", MSGI_ENABLED, 0, 0},
        {0x0145, "EM_COPY", MSGI_ENABLED, 0, 0},
        {0x0146, "EM_CLEAR", MSGI_ENABLED, 0, 0},
        {0x0147, "EM_PASTE", MSGI_ENABLED, 0, 0},
        {0x0148, "EM_QUERYFIRSTCHAR", MSGI_ENABLED, 0, 0},
        {0x0149, "EM_SETFIRSTCHAR", MSGI_ENABLED, 0, 0},
        {0x0160, "LM_QUERYITEMCOUNT", MSGI_ENABLED, 0, 0},
        {0x0161, "LM_INSERTITEM", MSGI_ENABLED, 0, 0},
        {0x0162, "LM_SETTOPINDEX", MSGI_ENABLED, 0, 0},
        {0x0163, "LM_DELETEITEM", MSGI_ENABLED, 0, 0},
        {0x0164, "LM_SELECTITEM", MSGI_ENABLED, 0, 0},
        {0x0165, "LM_QUERYSELECTION", MSGI_ENABLED, 0, 0},
        {0x0166, "LM_SETITEMTEXT", MSGI_ENABLED, 0, 0},
        {0x0167, "LM_QUERYITEMTEXTLENGTH", MSGI_ENABLED, 0, 0},
        {0x0168, "LM_QUERYITEMTEXT", MSGI_ENABLED, 0, 0},
        {0x0169, "LM_SETITEMHANDLE", MSGI_ENABLED, 0, 0},
        {0x016a, "LM_QUERYITEMHANDLE", MSGI_ENABLED, 0, 0},
        {0x016b, "LM_SEARCHSTRING", MSGI_ENABLED, 0, 0},
        {0x016c, "LM_SETITEMHEIGHT", MSGI_ENABLED, 0, 0},
        {0x016d, "LM_QUERYTOPINDEX", MSGI_ENABLED, 0, 0},
        {0x016e, "LM_DELETEALL", MSGI_ENABLED, 0, 0},
        {0x0180, "MM_INSERTITEM", MSGI_ENABLED, 0, 0},
        {0x0181, "MM_DELETEITEM", MSGI_ENABLED, 0, 0},
        {0x0182, "MM_QUERYITEM", MSGI_ENABLED, 0, 0},
        {0x0183, "MM_SETITEM", MSGI_ENABLED, 0, 0},
        {0x0184, "MM_QUERYITEMCOUNT", MSGI_ENABLED, 0, 0},
        {0x0185, "MM_STARTMENUMODE", MSGI_ENABLED, 0, 0},
        {0x0186, "MM_ENDMENUMODE", MSGI_ENABLED, 0, 0},
        {0x0187, "MM_DISMISSMENU", MSGI_ENABLED, 0, 0},
        {0x0188, "MM_REMOVEITEM", MSGI_ENABLED, 0, 0},
        {0x0189, "MM_SELECTITEM", MSGI_ENABLED, 0, 0},
        {0x018a, "MM_QUERYSELITEMID", MSGI_ENABLED, 0, 0},
        {0x018b, "MM_QUERYITEMTEXT", MSGI_ENABLED, 0, 0},
        {0x018c, "MM_QUERYITEMTEXTLENGTH", MSGI_ENABLED, 0, 0},
        {0x018d, "MM_SETITEMHANDLE", MSGI_ENABLED, 0, 0},
        {0x018e, "MM_SETITEMTEXT", MSGI_ENABLED, 0, 0},
        {0x018f, "MM_ITEMPOSITIONFROMID", MSGI_ENABLED, 0, 0},
        {0x0190, "MM_ITEMIDFROMPOSITION", MSGI_ENABLED, 0, 0},
        {0x0191, "MM_QUERYITEMATTR", MSGI_ENABLED, 0, 0},
        {0x0192, "MM_SETITEMATTR", MSGI_ENABLED, 0, 0},
        {0x0193, "MM_ISITEMVALID", MSGI_ENABLED, 0, 0},
        {0x01a0, "SBM_SETSCROLLBAR", MSGI_ENABLED, 0, 0},
        {0x01a1, "SBM_SETPOS", MSGI_ENABLED, 0, 0},
        {0x01a2, "SBM_QUERYPOS", MSGI_ENABLED, 0, 0},
        {0x01a3, "SBM_QUERYRANGE", MSGI_ENABLED, 0, 0},
        {0x01a4, "SBM_SETHILITE", MSGI_ENABLED, 0, 0},
        {0x01a5, "SBM_QUERYHILITE", MSGI_ENABLED, 0, 0},
        {0x01e3, "TBM_SETHILITE", MSGI_ENABLED, 0, 0},
        {0x01e4, "TBM_QUERYHILITE", MSGI_ENABLED, 0, 0},
};

SHORT cmsgi = sizeof(rgmsgi) / sizeof(MSGI);


/*
 * Now define the table of system known window ids, that is
 * used to display the list of windows with
 */
/* Define table of window ids */
IDTONAME    rgidtoname[] = {
        {0x8001, "FID_SIZEBORDER"},
        {0x8002, "FID_SYSMENU"},
        {0x8003, "FID_TITLEBAR"},
        {0x8004, "FID_MINMAX"},
        {0x8005, "FID_MENU"},
        {0x8006, "FID_VERTSCROLL"},
        {0x8007, "FID_HORZSCROLL"},
        {0x8008, "FID_CLIENT"}
};

SHORT cToName =  sizeof(rgidtoname) / sizeof(IDTONAME);


/*
 * And define the system options
 */

SPYOPT spyopt = {
    /* Output options */
    TRUE, FALSE, 100, NULL, SPYH_INPUT | SPYH_SENDMSG,
        TRUE, TRUE,
        FALSE,
        TRUE, FALSE

};

SPYSTR spystr = {
    "spy.out",      /* Default output file */
    "spy.lis"       /* Default save to list file */
};
