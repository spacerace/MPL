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

char szSplitbarClass[] = "PM MDI Splitbar";
USHORT usDocNumber = 0, cDocs = 0;
USHORT cxVertSplitbar, cyHorzSplitbar, cxSplitbarCenter, cySplitbarCenter;
SHORT cxNewDoc, cyNewDoc, xCascadeInc, yCascadeInc;
SHORT xNextNewDoc, yNextNewDoc;
SHORT xCascadeSlot;
SHORT cyTitlebar, cxMinmaxButton;

/* Frame subclassing globals */
PFNWP pfnFrameWndProc;
PFNWP pfnMainFrameWndProc;

/* Main MDI globals */
HDC hdcMem;
HPS hpsMem;
HPOINTER hptrHorzSplit, hptrVertSplit, hptrHVSplit, hptrArrow;

/* Menu globals */
HBITMAP hbmChildSysMenu, hbmAabChildSysMenu;
MENUITEM miAabSysMenu;
HWND hwndFirstMenu;
