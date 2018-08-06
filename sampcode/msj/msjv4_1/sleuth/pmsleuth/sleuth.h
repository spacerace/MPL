/*-----------------------------------------------------------------*/
/* Sleuth.h                                                        */
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
/* Macros for things that really should be in PMWIN.H              */
/*-----------------------------------------------------------------*/

#define WinMapWindowRect( hwndFrom, hwndTo, prcl )  \
    WinMapWindowPoints( (hwndFrom), (hwndTo), (PPOINTL)(prcl), 2 )

#define WinQueryWindowPtr( hwnd, index )  \
    ( (PVOID)WinQueryWindowULong( (hwnd), (index) ) )

/*-----------------------------------------------------------------*/
/* Typedefs and Mac vs. PM stuff.                                  */
/*-----------------------------------------------------------------*/

typedef unsigned int FLAG;

#define LOCAL

#ifdef PM_MACINTOSH
#define CDECL
#else
#define CDECL cdecl
#endif

/*-----------------------------------------------------------------*/
/* Menu command definitions.  Note that to be compatible with the  */
/* Mac, these are not just arbitrary values, but are determined by */
/* the menu position.  The high-order byte is the resource ID for  */
/* the Mac's menu resource, and the low-order byte is the position */
/* in the pulldown menu.                                           */
/*-----------------------------------------------------------------*/

#if 0

#define CMD_SLEUTH  32768   /* 0x8000 */

#define CMD_ABOUT   32769   /* 0x8001 */

#define CMD_LOOK    33025   /* 0x8101 */
#define CMD_EXPAND  33026   /* 0x8102 */
#define CMD_EXIT    33028   /* 0x8104 */

#define CMD_UNDO    33281   /* 0x8201 */
#define CMD_CUT     33283   /* 0x8203 */
#define CMD_COPY    33284   /* 0x8204 */
#define CMD_PASTE   33285   /* 0x8205 */
#define CMD_CLEAR   33286   /* 0x8206 */

#else

#define CMD_SLEUTH  0x8000

#define CMD_ABOUT   0x8001

#define CMD_LOOK    0x8101
#define CMD_EXPAND  0x8102
#define CMD_EXIT    0x8104

#define CMD_UNDO    0x8201
#define CMD_CUT     0x8203
#define CMD_COPY    0x8204
#define CMD_PASTE   0x8205
#define CMD_CLEAR   0x8206

#endif

/*-----------------------------------------------------------------*/
/* String table ID numbers.                                        */
/*-----------------------------------------------------------------*/

#define IDS_CLASS   1
#define IDS_TITLE   2

/*-----------------------------------------------------------------*/
/* Dialog ID numbers.                                              */
/*-----------------------------------------------------------------*/

#define ABOUTBOX    1

/*-----------------------------------------------------------------*/
/* Window ID's                                                     */
/*-----------------------------------------------------------------*/

#define ID_SLEUTH     128   /* 0x80 */

/*-----------------------------------------------------------------*/
/* Additional scroll bar message options that we use internally.   */
/*-----------------------------------------------------------------*/

#define SBX_TOP       0xF00D
#define SBX_BOTTOM    0xF00E

/*-----------------------------------------------------------------*/
