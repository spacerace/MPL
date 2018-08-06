/****** Standard include files */

#define INCL_WINSYS
#define INCL_WINCOMMON
#define INCL_WINMESSAGEMGR
#define INCL_WINATOM
#define INCL_WINDDE
#define INCL_WINTIMER
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <ddeml.h>

/****** Resource IDs *****/

#define IDR_DEMO    1

/****** Menu command IDs *****/
/* (by convention, hi byte is menu index, lo byte is dispatch table index) */

/* File menu */

#define CMD_NEW     0x0000
#define CMD_OPEN    0x0001
#define CMD_SAVE    0x0002
#define CMD_SAVEAS  0x0003

/* Edit menu */
#define CMD_UNDO    0x0104
#define CMD_CUT     0x0105
#define CMD_COPY    0x0106
#define CMD_PASTE   0x0107
#define CMD_CLEAR   0x0108

/* Number of menu commands */
#define CCMDS       (4+5)

#define UNUSED

/*** demo.c function declarations */

BOOL DemoInit(VOID);
VOID CommandMsg(USHORT cmd);
int cdecl main(int argc, char** argv);
MRESULT FAR PASCAL DemoWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT FAR PASCAL DemoFrameWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
VOID DemoPaint(HWND hwnd, HPS hps, RECTL* prcl);
HDMGDATA EXPENTRY callback(HCONV hConv, HSZ hszTopic, HSZ hszItem,
        USHORT usFmt, USHORT usType, HDMGDATA hDmgData);


/*** testsubs.c function declarations */

BOOL InitTestSubs(VOID);
VOID DrawString(HWND hwnd, char* sz);
MRESULT FAR PASCAL StrWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
BOOL StrWndCreate(HWND hwnd, int cchLine, int cLine);
VOID PaintStrWnd(HWND hwnd, HPS hps, PRECTL prcl);
VOID StartTime(VOID);
VOID StopTime(HWND hwndStrWnd, char* szFmt);
