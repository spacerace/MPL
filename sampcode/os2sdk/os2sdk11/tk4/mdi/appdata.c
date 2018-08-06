#define INCL_WINSYS
#define INCL_WINCOMMON
#define INCL_WINMESSAGEMGR
#define INCL_WINACCELERATORS
#define INCL_WINMENUS
#define INCL_WINHEAP
#define INCL_WINPOINTERS

#include <os2.h>
#include "app.h"
#include "mdi.h"

char szMDIClass[] = "PM MDI Sample App";
char szDocClass[] = "PM MDI Document";
USHORT cxBorder, cyBorder, cyHScroll, cxVScroll, cyVScrollArrow;
USHORT cxScreen, cyScreen, cyIcon, cxByteAlign, cyByteAlign;
USHORT cxSizeBorder, cySizeBorder;
ULONG clrNext = CLR_BACKGROUND;
LONG rglDevCaps[(CAPS_VERTICAL_FONT_RES - CAPS_FAMILY)];

/* Main globals */
HAB  hab;
HHEAP hHeap;
HMQ  hmqMDI;
HWND hwndMDI, hwndMDIFrame;
HWND hwndActiveDoc;
FONTMETRICS fmSystemFont;
NPDOC npdocFirst = NULL;

/* Menu globals */
HWND hwndMainMenu, hwndWindowMenu;
HWND hwndSysMenu, hwndAppMenu;

