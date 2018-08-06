/*
**  VLB.H     Virtual List Box header file
**
**  Author: Robert A. Wood
**
**    Date: 03/19/90
*/

#define VLB_CALLBACK  lpVLB->CallBack
#define VLBSTRLEN     255     // maximum virtual string length
typedef LONG (FAR PASCAL * VLBPROC)( WORD, WORD, LONG FAR *, LPSTR );

//  VCB   Virtual CallBack function Messages
//*******************************************************************
#define VCB_CREATE      1   // listbox has just been created
#define VCB_DESTROY     2   // listbox is to be destroyed
#define VCB_SETFOCUS    3   // listbox has received focus
#define VCB_KILLFOCUS   4   // listbox is to lose focus
#define VCB_LENGTH      5   // request for total strings
#define VCB_WIDTH       6   // request for vitual width
#define VCB_FULLSTRING  7   // get the full string specified
#define VCB_STRING      8   // get string specify by StringId
#define VCB_VTHUMB      9   // get string specify by thumb
#define VCB_SEARCH      10  // get string for search criteria
#define VCB_FIRST       11  // get the first string
#define VCB_LAST        12  // get the last string
#define VCB_NEXT        13  // get the next string
#define VCB_PREV        14  // get the previous string
#define VCB_HTHUMB      15  // move str start specify thumb
#define VCB_LEFT        16  // move str start left one char
#define VCB_RIGHT       17  // move str start right one char
#define VCB_PAGELEFT    18  // move str start left one col
#define VCB_PAGERIGHT   19  // move str start right one col

//  VLB Messages Sent to the VLB Procedure
//*******************************************************************
#define VLB_GETCOUNT     WM_USER+1   // get total virtual strings
#define VLB_GETSELCOUNT  WM_USER+2   // get total selected strings
#define VLB_GETSTRLEN    WM_USER+3   // get total string length
#define VLB_GETSELSTR    WM_USER+4   // get the select String
#define VLB_GETSELID     WM_USER+5   // get the select StringId(s)
#define VLB_SETCURSEL    WM_USER+6   // selects the specify StringId
#define VLB_SETSEL       WM_USER+7   // sets selection of a StringId
#define VLB_RELOAD       WM_USER+8   // reload curr. display strings

// VLB_RELOAD wParam options
//*******************************************************************
#define RELOAD_STRINGS   0
#define RELOAD_STRINGPOS 1
#define RELOAD_STRINGID  2

// Notifications Sent to the VLB Parent by the VLB Procedure
//*******************************************************************
#define VLBN_DBLCLK     LBN_DBLCLK    // double clicked on a string
#define VLBN_ERRSPACE   LBN_ERRSPACE  // can't allocate memory
#define VLBN_SELCHANGE  LBN_SELCHANGE // selected string changed

// VLB Control Information
//*******************************************************************
typedef struct
{
   WORD ListBoxId;             // list box control id
   BOOL MultiSelection;        // Single or Multi selection
   RECT ClientRect;            // client area rectangle
   WORD CharWidth;             // character width
   WORD CharHeight;            // character height
   BYTE DisplayStrings;        // max number of displayed strings
   BYTE DisplayChars;          // max characters in displayed strings
   LONG FocusString;           // string position of focus frame
   LONG TotalStrings;          // number of virtual strings
   LONG TotalWidth;            // number of virtual chars per string
   LONG FirstDisplayString;    // number of first displayed string
   LONG TotalSelectedStrings;  // number of selected strings
   LONG MaxSelectedStrings;    // maximum number of selected strings
   VLBPROC CallBack;           // VLB CallBack function
   HANDLE hDisplayBuffer;      // handle to buffer of display strings
   HANDLE hStringIds;          // handle - array of display StringIds
   HANDLE hSelectedStringIds;  // handle - array of select StringIds
   LONG SelectedStringId;      // Single selection selected StringId
   int ScrollWindow;           // scroll lines for ScrollWindow()
} VLB, FAR *LPVLB;             // 62 bytes

// Function called by application to initialize VLB
//*******************************************************************
BOOL FAR PASCAL InitVLB( HANDLE, HWND, WORD, VLBPROC );

// Internal functions
//*******************************************************************
LONG FAR PASCAL VLBProc( HWND hCtl, unsigned message, WORD wParam ,
                         LONG lParam );
BOOL FAR PASCAL LoadVLB( HWND hCtl, WORD wParam, LONG lParam );
BOOL FAR PASCAL ScrollVLB( HWND hCtl, WORD wParam, int Scroll );
VOID FAR PASCAL SetSelectedString( HWND hCtl, WORD wParam, LPVLB);
VOID FAR PASCAL SetFocusString( WORD wParam, LPVLB lpVLB );
VOID InvertSelectedStrings( HDC hCtl, LPVLB lpVLB, int StringPos );
VOID FrameFocusString( HWND hCtl, LPVLB lpVLB, BOOL draw );
VOID FAR PASCAL PaintVLB( HWND hCtl, LPVLB lpVLB );
VOID FAR PASCAL GetVLBColors( HWND hCtl, HANDLE hDC );

//*** END OF VLB.H **************************************************
