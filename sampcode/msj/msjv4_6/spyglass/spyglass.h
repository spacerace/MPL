/*
 * WINDOWS SPYGLASS - HEADER FILE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 * 08/31/88 1.00 - Kevin P. Welch - initial creation.
 *
 */

/* system menu definitions */
#define  SC_1X          0x0001
#define  SC_2X          0x0002
#define  SC_4X          0x0004
#define  SC_8X          0x0008
#define  SC_16X         0x0010

/* general programming extensions */
#define  ID(x)          GetWindowWord(x,GWW_ID)
#define  PARENT(x)      GetWindowWord(x,GWW_HWNDPARENT)
#define  INSTANCE(x)    GetWindowWord(x,GWW_HINSTANCE)

#define  WIDTH(x)       (##x.right-##x.left)
#define  HEIGHT(x)      (##x.bottom-##x.top)

#define  WARNING(x,y)   MessageBox(x,y,"Windows SpyGlass",MB_OK|MB_ICONHAND)

/* spyglass window style & location definitions */
#define  SPYGLASS_STYLE       WS_TILEDWINDOW|WS_CLIPCHILDREN
#define  SPYGLASS_XPOS        CW_USEDEFAULT
#define  SPYGLASS_YPOS        CW_USEDEFAULT
#define  SPYGLASS_WIDTH       GetSystemMetrics(SM_CXSCREEN)/3
#define  SPYGLASS_HEIGHT      GetSystemMetrics(SM_CYSCREEN)/2

/* global function definitions */
LONG FAR PASCAL   SpyGlassWndFn( HWND, WORD, WORD, LONG );
