/*
 * Form Library - PRIVATE HEADER FILE
 *
 * LANGUAGE      : Microsoft C 5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 *    Eikon Systems, Inc.
 *    989 East Hillsdale Blvd, Suite 260
 *    Foster City, California 94404
 *    415-349-4664
 *
 * 07/12/90 1.00 - Kevin P. Welch - initial creation.
 *
 */

/* edit form dialog box definitions */
#define  ID0_SAVE             90
#define  ID0_PRINT            91
#define  ID0_CANCEL           92
#define  ID0_PAGE             93
#define  ID0_COMMENT          94

/* edit form property list definitions */
#define  SET_DATA(x,y)        SetProp(x,MAKEINTRESOURCE(96),y)
#define  SET_COMMENT(x,y)     SetProp(x,MAKEINTRESOURCE(97),y)
#define  SET_CRNTPAGE(x,y)    SetProp(x,MAKEINTRESOURCE(98),y)
#define  SET_CTLLIST(x,y,z)   SetProp(x,MAKEINTRESOURCE(99+y),z)

#define  GET_DATA(x)          GetProp(x,MAKEINTRESOURCE(96))
#define  GET_COMMENT(x)       GetProp(x,MAKEINTRESOURCE(97))
#define  GET_CRNTPAGE(x)      GetProp(x,MAKEINTRESOURCE(98))
#define  GET_CTLLIST(x,y)     GetProp(x,MAKEINTRESOURCE(99+y))

#define  REM_DATA(x)          RemoveProp(x,MAKEINTRESOURCE(96))
#define  REM_COMMENT(x)       RemoveProp(x,MAKEINTRESOURCE(97))
#define  REM_CRNTPAGE(x)      RemoveProp(x,MAKEINTRESOURCE(98))
#define  REM_CTLLIST(x,y)     RemoveProp(x,MAKEINTRESOURCE(99+y))

/* global form data structures */
#define  MAX_NAME       64
#define  MAX_DATA       1024

#define  MAX_TITLE      128
#define  MAX_HEADER     256
#define  MAX_FOOTER     256

typedef struct {
   WORD     wPages;
   HANDLE   hInstance;
   HANDLE   hTempData;
   FARPROC  fnOldMsgHook;
   char     szTitle[MAX_TITLE];
} FORM;

/* global dialog box definitions */
#define  MAX_TEXT       256
#define  MAX_CLASS      128
#define  MAX_CAPTION    128

typedef struct {
   LONG        lStyle;
   BYTE        wControls;
   WORD        wX;
   WORD        wY;
   WORD        wCX;
   WORD        wCY;
   char        szMenu[1];
   char        szClass[1];
   char        szCaption[MAX_CAPTION];
} DLGBOX;

typedef struct {
   WORD        wX;
   WORD        wY;
   WORD        wCX;
   WORD        wCY;
   WORD        wID;
   LONG        lStyle;
   char        szClass[MAX_CLASS];
   char        szText[MAX_TEXT];
} DLGCTL;

typedef DLGBOX FAR *    LPDLGBOX;
typedef DLGCTL FAR *    LPDLGCTL;

/* global variable definitions */
extern FORM             FormInfo;

/* dialog handling function definitions */
BOOL FAR PASCAL         GetDlg( LPSTR, LPDLGBOX );
HANDLE FAR PASCAL       GetDlgData( HWND );
BOOL FAR PASCAL         GetDlgCtlData( HWND, HWND, LPSTR, WORD );

/* control handling function definitions */
BOOL FAR PASCAL         GetCtl( LPSTR, WORD, LPDLGCTL );
BOOL FAR PASCAL         GetCtlName( HANDLE, WORD, LPSTR, WORD );
BOOL FAR PASCAL         GetCtlData( HANDLE, LPSTR, LPSTR, WORD );
BOOL FAR PASCAL         GetCtlComment( HANDLE, WORD, LPSTR, WORD );
BOOL FAR PASCAL         SetCtlData( HWND, HWND, LPDLGCTL, LPSTR );

/* string handling function definitions */
BOOL FAR PASCAL         StringMatch( LPSTR, LPSTR );
VOID FAR PASCAL         StringCat( LPSTR, LPSTR, WORD );
VOID FAR PASCAL         StringCopy( LPSTR, LPSTR, WORD );
VOID FAR PASCAL         StringJoin( LPSTR, LPSTR, WORD );

/* utility function definitions */
HDC FAR PASCAL          GetPrinterDC( VOID );
VOID FAR PASCAL         CenterPopup( HWND, HWND );
WORD FAR PASCAL         FormMsgHookFn( WORD, WORD, LONG );
VOID FAR PASCAL         Warning( HWND, LPSTR, LPSTR, WORD );

/* general function definitions */
WORD FAR PASCAL         WEP( WORD );
HANDLE FAR PASCAL       FormInit( HANDLE );
BOOL FAR PASCAL         FormEditDlgFn( HWND, WORD, WORD, LONG );
BOOL FAR PASCAL         FormPrintDlgFn( HWND, WORD, WORD, LONG );

/* general macro definitions */
#define ID(x)           GetWindowWord(x,GWW_ID)
#define PARENT(x)       GetWindowWord(x,GWW_HWNDPARENT)
#define INSTANCE(x)     GetWindowWord(x,GWW_HINSTANCE)

#define WARNING(x,y)    Warning(x,y,__FILE__,__LINE__)
