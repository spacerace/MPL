/*
 * Page Selection Control - PRIVATE HEADER FILE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 * Eikon Systems, Inc.
 * 989 East Hillsdale Blvd, Suite 260
 * Foster City, California 94404
 * 415-349-4664
 *
 * 11/30/89 1.00 - Kevin P. Welch - initial creation.
 *
 */

/* Page Control Definitions */
#ifndef RC_INVOKED
extern HANDLE      hPageInst;
#endif

#define PAGE_CLASSEXTRA    0  
#define PAGE_WNDEXTRA      18
#define PAGE_VERSION       100
#define PAGE_NAME          "Page"
#define PAGE_COLOR         NULL
#define PAGE_WNDSTYLE      WS_CHILD|WS_VISIBLE|WS_TABSTOP
#define PAGE_CLASSSTYLE    CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_GLOBALCLASS

#define ID_IDENTIFIER      100

/* General Definitions */
#define ID(x)              GetWindowWord(x,GWW_ID)
#define PARENT(x)          GetWindowWord(x,GWW_HWNDPARENT)
#define INSTANCE(x)        GetWindowWord(x,GWW_HINSTANCE)
