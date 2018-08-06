/* This header file does all the necessary includes and defines all the
   structures and constants needed for the sort program                 */

#define INCL_WIN
#define INCL_WINHEAP
#define INCL_WINDIALOGS
#define INCL_GPIPRIMITIVES
#define INCL_GPIBITMAPS
#define INCL_DOSPROCESS
#define _MT

#include <os2.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <stdlib.h>

/* This structure is passed into various routines and contains various
   information, often depending on who is passing the information.
   hwnd holds the main window handle for routines that need to draw.
   fContinueCalc is used by routines that have the ability to abort
         when the calling routine sets this flag to FALSE.
   usID is set to the offset number of a thread and is used to pass
         a message back to the window proc when the thread terminates.
   (*pFunc)() passes the function to be called to the generic thread
         starting routine CalcThread.
   Array points to the beginning of the data set to be used by the
         various sorts.  This pointer is used in the Display thread
         to point to the variable that is dynamically modified to
         contain the current height of the client window.
   cArray is the number of elements in Array.  It serves no purpose in
         the Display thread.
   pcSetSize is used only by the Display thread to point to a dynamically
         modified variable which contains the user-modifiable data
         set size.                                                    */

typedef struct
{
   HWND   hwnd;
   BOOL   fContinueCalc;
   BYTE   *pcThreadCnt;
   USHORT usID;
   VOID   (*pFunc)(VOID *);
   USHORT *Array;
   USHORT cArray;
   USHORT *pcSetSize;
} CALCPARAM;
typedef CALCPARAM FAR *PCALCPARAM;

/* Function Definitions */

MRESULT EXPENTRY ClientWndProc  (HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY EntryFldDlgProc(HWND, USHORT, MPARAM, MPARAM);
INT  main(VOID);
VOID _cdecl FAR CalcThread(PCALCPARAM);
VOID _cdecl FAR DispThread(PCALCPARAM);
VOID EnableMenuItem(HWND hwnd, SHORT sMenuItem, BOOL fEnable);
VOID RandomizeData(USHORT);
VOID BubbleSort(PCALCPARAM);
VOID BatcherSort(PCALCPARAM);
VOID BatcherSortR(PCALCPARAM, USHORT, USHORT, USHORT, BOOL);
VOID QuickSort(PCALCPARAM);
VOID QuickSortR(PCALCPARAM, SHORT, SHORT);
VOID InsertionSort(PCALCPARAM);

#define STACKSIZE        4096    /* Arbitrary size for a stack     */
#define LISTCNT             4    /* Number of data sets            */
#define NELEMS            512    /* Number of items in a data set  */

#define XOFFSET            10    /* Pixel offset for axis in X dir */
#define YOFFSET            40    /* Pixel offset for axis in Y dir */

#define COLUMNOFFSET      100    /* Column offset for sort names   */
#define ROWOFFSET          12    /* Row offset for sort names      */
#define COLUMNCNT           4    /* Max # of text columns          */
#define ROWCNT              2    /* Max # of text rows             */

/****** Resource IDs *****/


#define ID_MAINMENU    1
#define BMP_SORT       1
#define IDM_START      2
#define IDM_STOP       3
#define IDM_SET        4
#define IDM_RANDOM     5
#define ID_SETCOUNT    6
#define ID_ENTRYFLD    7
#define UM_CALC_DONE   (WM_USER+0) /* Message posted when thread terminates */


/* Macro for drawing a rectangle.  Assumes hps and ptl structs are defined
   in the calling module. */

#define DrawRect(x1,y1,x2,y2,color)  ptl.x = (LONG) (x1);             \
                                     ptl.y = (LONG) (y1);             \
                                     GpiSetCurrentPosition(hps,&ptl); \
                                     ptl.x = (LONG) (x2);             \
                                     ptl.y = (LONG) (y2);             \
                                     GpiSetColor(hps,color);          \
                                     GpiBox(hps,DRO_FILL,&ptl,0L,0L);

/* Macro for drawing a line.  Assumes hps and ptl structs are defined
   in the calling module. */

#define DrawLine(x1,y1,x2,y2,color)  ptl.x = (LONG) (x1);             \
                                     ptl.y = (LONG) (y1);             \
                                     GpiSetCurrentPosition(hps,&ptl); \
                                     ptl.x = (LONG) (x2);             \
                                     ptl.y = (LONG) (y2);             \
                                     GpiSetColor(hps,color);          \
                                     GpiLine(hps,&ptl);

/* Macro for drawing a point.  Assumes hps and ptl structs are defined
   in the calling module. */

#define Draw2Pel(x1,y1,color)        ptl.x = (LONG) (x1);             \
                                     ptl.y = (LONG) (y1);             \
                                     GpiSetColor(hps,color);          \
                                     GpiSetPel(hps,&ptl);             \
                                     ptl.y++;                         \
                                     GpiSetPel(hps,&ptl);


