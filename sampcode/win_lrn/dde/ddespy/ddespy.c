/*----------------------------------------------------------------------------*\
|   ddespy.c      - Windows message spy application                              |
|                                                                              |
|   Notes:                                                                     |
|       "->" means "a pointer to", "->>" means "a handle to"                   |
|                                                                              |
|   History:                                                                   |
|       01/01/87 Created                                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
|                                                                              |
|   g e n e r a l   c o n s t a n t s                                          |
|                                                                              |
\*----------------------------------------------------------------------------*/

#define DEBUGDX   100    /* Window sizes */
#define DEBUGDY   100
#define DEBUGX    50
#define DEBUGY    50
#define DEBUGXINC 10
#define DEBUGYINC 30

/*----------------------------------------------------------------------------*\
|                                                                              |
|   i n c l u d e   f i l e s                                                  |
|                                                                              |
\*----------------------------------------------------------------------------*/

#include <windows.h>
#include <winexp.h>
#include "ddespy.h"
#include "dde.h"

/*----------------------------------------------------------------------------*\
|                                                                              |
|   g l o b a l   v a r i a b l e s                                            |
|                                                                              |
\*----------------------------------------------------------------------------*/

static  char    acAppName[]="DDE SPY";

static  HANDLE  hInst;
HWND    hSpyApp;
HWND    hSpyChild;
static  FARPROC fpxProcAbout;
static  FARPROC fpxProcSpySet;
static  FARPROC fpxSpyProc;
/* static  FARPROC fpxMsgHook1;
   static  FARPROC fpxMsgHook2;
   static  FARPROC fpxOldHook1;
   static  FARPROC fpxOldHook2;
 */

/* Global variables describing the state of the window being spyied on */
static  BOOL    gbSpyOn  = FALSE;
static  HWND    ghSpyWnd = NULL;
static  BOOL    gbHook   = FALSE;
static  FARPROC OldSpyProc;

BOOL gbMouse = TRUE;          /* TRUE if mouse messages are to be ignored */
BOOL gbNC    = TRUE;

static BOOL bDisableMessages; /*  TRUE if all messages are to be ignored */


struct TEXT_STRUCT {
    int  iFirst;                  /* First line in que */
    int  iCount;                  /* Number of lines in que */
    int  iTop;                    /* Line at top of window */
    int  iLeft;                   /* X offset of the window */
    char *Text[MAXLINES];         /* Que of Text in window */
    int  Len[MAXLINES];           /* String Length of text */
    int  MaxLen;                  /* Max String Length */
};

typedef struct TEXT_STRUCT *PTXT; /* pointer to a text struct */
typedef PTXT               *HTXT; /* Handle to a text struct */

static int Tdx = 0;        /* Text font size */
static int Tdy = 0;

static HWND hERROR;

/*----------------------------------------------------------------------------*\
|                                                                              |
|   f u n c t i o n   d e f i n i t i o n s                                    |
|                                                                              |
\*----------------------------------------------------------------------------*/

long FAR PASCAL MyWndProc(HWND, unsigned, WORD, LONG);
long FAR PASCAL SpyProc(HWND, unsigned, WORD, LONG);

/* HWND CreateDebugWindow (HWND,char *,BOOL); */
void Error (char *);
BOOL SSEqualToDS (void);

/*
  long FAR PASCAL MsgHook1(WORD, WORD, LPMSG);
  long FAR PASCAL MsgHook2(WORD, WORD, LPMSG);
 */

extern void FAR PASCAL KillHook();
extern BOOL FAR PASCAL InitHook( HWND );
extern long FAR PASCAL DebugWndProc(HWND, unsigned, WORD, LONG);
extern HWND FAR PASCAL CreateDebugWin (HANDLE, HWND);

extern void FAR PASCAL PassFileToDLL(int);

/*----------------------------------------------------------------------------*\
|   DlgAbout( hDlg, uiMessage, wParam, lParam )                                |
|                                                                              |
|   Description:                                                               |
|       This function handles messages belonging to the "About" dialog box.    |
|       The only message that it looks for is WM_COMMAND, indicating the use   |
|       has pressed the "OK" button.  When this happens, it takes down         |
|       the dialog box.                                                        |
|                                                                              |
|   Arguments:                                                                 |
|       hDlg            window handle of about dialog window                   |
|       uiMessage       message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if message has been processed, else FALSE                         |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL FAR PASCAL DlgAbout( hDlg, uiMessage, wParam, lParam )
HWND     hDlg;
unsigned uiMessage;
WORD     wParam;
long     lParam;
{
    switch (uiMessage) {
        case WM_COMMAND:
            EndDialog(hDlg,TRUE);
            return(TRUE);
        case WM_INITDIALOG:
            return(TRUE);
    }
    return(FALSE);
}


VOID SetDlgText (hDlg,hWnd)
  HWND hDlg;
  HWND hWnd;
{
  static char acCaption[100];
  HWND hParent;

  if (IsWindow(hWnd)) {
     acCaption[0] = '\0';
     GetWindowText (hWnd,(LPSTR)acCaption,100);
     SetDlgItemText(hDlg,ID_CAPTION,(LPSTR)acCaption);
     hParent = GetParent(hWnd);
     if (IsWindow(hParent)) {
        GetWindowText (hParent,(LPSTR)acCaption,100);
        SetDlgItemText(hDlg,ID_PARENT,(LPSTR)acCaption);
     }
     else {
        SetDlgItemText(hDlg,ID_PARENT,(LPSTR)"<NO PARENT>");
     }

     GetModuleFileName(GetWindowWord(hWnd,GWW_HINSTANCE),
                       (LPSTR)acCaption,100 );
  }
  else {
     SetDlgItemText(hDlg,ID_CAPTION,(LPSTR)"<Undefined>");
     SetDlgItemText(hDlg,ID_MODULE, (LPSTR)"<Undefined>");
     SetDlgItemText(hDlg,ID_PARENT, (LPSTR)"<Undefined>");
  }
}

/*----------------------------------------------------------------------------*\
|   DlgSpySet( hDlg, uiMessage, wParam, lParam )                               |
|                                                                              |
|   Description:                                                               |
|       This function handles messages belonging to the SpySet dialog box.     |
|                                                                              |
|   Arguments:                                                                 |
|       hDlg            window handle of about dialog window                   |
|       uiMessage       message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if message has been processed, else FALSE                         |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL FAR PASCAL DlgSpySet( hDlg, uiMessage, wParam, lParam )
HWND     hDlg;
unsigned uiMessage;
WORD     wParam;
long     lParam;
{
    static BOOL bNC;
    static BOOL bMouse;
    static HWND hSpyWnd;
    static BOOL bHook;
    static int  iCaptureState;  /* 0 = not captured, 1 = captured */

    POINT  pCursor;

    switch (uiMessage) {
        case WM_COMMAND:
            switch (wParam) {

              case ID_OK:
                SendMessage (hSpyApp,WM_COMMAND,SPYOFF,0L);
                gbNC     = bNC;
                gbMouse  = bMouse;
                ghSpyWnd = hSpyWnd;
                gbHook   = bHook;
                if (gbHook) hSpyWnd = NULL;
                SendMessage (hSpyApp,WM_COMMAND,SPYON,0L);

                /* Fall through to cancel */
              case ID_CANCEL:
                iCaptureState++;
                EndDialog(hDlg,TRUE);
                return TRUE;

              case ID_MOUSE:
                SendDlgItemMessage(hDlg,ID_MOUSE,BM_SETCHECK,bMouse ^= 1,0L);
                return TRUE;

              case ID_NC:
                SendDlgItemMessage(hDlg,ID_NC,BM_SETCHECK,bNC ^= 1,0L);
                return TRUE;


/*              case ID_ALL:
 *               SendDlgItemMessage(hDlg,ID_ALL,BM_SETCHECK,bHook ^= 1,0L);
 *               if (bHook) hSpyWnd = NULL;
 *               SetDlgText (hDlg,hSpyWnd);
 *               return TRUE;
 */
            }
            break;

        case WM_INITDIALOG:
            SendDlgItemMessage(hDlg,ID_NC   ,BM_SETCHECK,bNC   =gbNC,0L);
            SendDlgItemMessage(hDlg,ID_MOUSE,BM_SETCHECK,bMouse=gbMouse,0L);
            SendDlgItemMessage(hDlg,ID_ALL  ,BM_SETCHECK,bHook =gbHook,0L);
            hSpyWnd = ghSpyWnd;
            if (bHook) hSpyWnd = NULL;
            SetDlgText (hDlg,hSpyWnd);
            iCaptureState=0;
            return TRUE;

        /*
         * When the dialog box is being deactivated by the user clicking
         * in a window of another application find out what window and
         * steal the focus back.  If the user tries to spy on the same
         * window two times the focus is not stolen back.
         */

        case WM_ACTIVATEAPP:
            if (wParam == 0 && !iCaptureState) {
               HWND hWnd;
               GetCursorPos((LPPOINT)&pCursor);
               hWnd = WindowFromPoint(pCursor);
               if (hWnd != hSpyWnd) {
                  SetDlgText (hDlg,hWnd);
                  PostMessage(hDlg,WM_ACTIVATE,1,0L);/* Trick used to get the */
                  hSpyWnd = hWnd;                    /* focus back */
                  bHook   = FALSE;
                  SendDlgItemMessage(hDlg,ID_ALL,BM_SETCHECK,bHook,0L);
               }
               return TRUE;
            }
            return FALSE;

        default:
            return FALSE;
     }
 }


/*----------------------------------------------------------------------------*\
|   MyInit( hInstance )                                                        |
|                                                                              |
|   Description:                                                               |
|       This is called when the application is first loaded into               |
|       memory.  It performs all initialization that doesn't need to be done   |
|       once per instance.                                                     |
|                                                                              |
|   Arguments:                                                                 |
|       hInstance       instance handle of current instance                    |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if successful, FALSE if not                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL MyInit(hInstance,hPrevInstance)
   HANDLE hInstance;
   HANDLE hPrevInstance;
{
    WNDCLASS rClass;

/* Register a class for the main application window */

    if (!hPrevInstance) {
       rClass.hCursor        = LoadCursor(NULL,IDC_ARROW);
       rClass.hIcon          = LoadIcon(hInstance,(LPSTR)"AppIcon");
       rClass.lpszMenuName   = (LPSTR)"MyMenu";
       rClass.lpszClassName  = (LPSTR)acAppName;
       rClass.hbrBackground  = (HBRUSH)COLOR_WINDOW + 1;
       rClass.hInstance      = hInstance;
       rClass.style          = CS_HREDRAW | CS_VREDRAW;
       rClass.lpfnWndProc    = MyWndProc;
       rClass.cbWndExtra     = 0;
       rClass.cbClsExtra     = 0;

       return RegisterClass((LPWNDCLASS)&rClass);
    }
    return TRUE;
}

/*----------------------------------------------------------------------------*\
|                                                                              |
|   w i n m a i n                                                              |
|                                                                              |
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
|   WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )                  |
|                                                                              |
|   Description:                                                               |
|       The main procedure for the App.  After initializing, it just goes      |
|       into a message-processing loop until it gets a WM_QUIT message         |
|       (meaning the app was closed).                                          |
|                                                                              |
|   Arguments:                                                                 |
|       hInstance       instance handle of this instance of the app            |
|       hPrevInstance   instance handle of previous instance, NULL if first    |
|       lpszCmdLine     ->null-terminated command line                         |
|       cmdShow         specifies how the window is initially displayed        |
|                                                                              |
|   Returns:                                                                   |
|       The exit code as specified in the WM_QUIT message.                     |
|                                                                              |
\*----------------------------------------------------------------------------*/

int PASCAL WinMain( hInstance, hPrevInstance, fpcCmdLine, iCmdShow )
HANDLE  hInstance, hPrevInstance;
LPSTR   fpcCmdLine;
int     iCmdShow;
{
    MSG   rMsg;
    HWND  hWnd;
    HMENU hMenu;
    HDC   hdc;
    int   dxpScreen;
    int   dypScreen;

    /* Call initialization procedure */
    if (!MyInit(hInstance,hPrevInstance))
        return FALSE;

/* create initial tiled window */

        hdc = CreateDC((LPSTR)"DISPLAY", NULL, NULL, NULL);
        dxpScreen = GetDeviceCaps(hdc, HORZRES);
        dypScreen = GetDeviceCaps(hdc, VERTRES);
        DeleteDC(hdc);

    /* Save instance handle for DialogBoxs */
    hInst = hInstance;

    hSpyApp = CreateWindow ((LPSTR)acAppName,
                            (LPSTR)acAppName,
                            WS_TILEDWINDOW,
                            0,           /*  x */
                            dypScreen/2, /*  y */
                            dxpScreen,   /* cx */
                            dypScreen/2, /* cy */
                            (HWND)NULL,        /* no parent */
                            (HMENU)NULL,       /* use class menu */
                            (HANDLE)hInstance, /* handle to window instance */
                            (LPSTR)NULL        /* no params to pass on */
                           );

    ShowWindow (hSpyApp,iCmdShow);

    /* Bind callback function with module instance */
    fpxProcAbout  = MakeProcInstance((FARPROC)DlgAbout,hInstance);
    fpxProcSpySet = MakeProcInstance((FARPROC)DlgSpySet,hInstance);
    fpxSpyProc    = MakeProcInstance((FARPROC)SpyProc,hInstance);
 /*
    fpxMsgHook1   = MakeProcInstance((FARPROC)MsgHook1,hInstance);
    fpxMsgHook2   = MakeProcInstance((FARPROC)MsgHook2,hInstance);
 */
    /* Insert "About..." into system menu */
    hMenu = GetSystemMenu(hSpyApp,FALSE);
    ChangeMenu(hMenu,0,NULL,999,MF_APPEND | MF_SEPARATOR);
    ChangeMenu(hMenu,0,(LPSTR)"About...",CMDABOUT,MF_APPEND | MF_STRING);

    /*
     * Polling messages from event queue
     */

    while (GetMessage((LPMSG)&rMsg,NULL,0,0))  {
        TranslateMessage((LPMSG)&rMsg);
        DispatchMessage((LPMSG)&rMsg);
    }

    FreeProcInstance (fpxProcAbout);
    FreeProcInstance (fpxProcSpySet);
    FreeProcInstance (fpxSpyProc);
 /*
    FreeProcInstance (fpxMsgHook1);
    FreeProcInstance (fpxMsgHook2);
  */
    return((int)rMsg.wParam);
}

/*----------------------------------------------------------------------------*\
|                                                                              |
|   w i n d o w   p r o c s                                                    |
|                                                                              |
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
|   MyWndProc( hWnd, uiMessage, wParam, lParam )                               |
|                                                                              |
|   Description:                                                               |
|       The window proc for the app's main (tiled) window.  This processes all |
|       of the parent window's messages.                                       |
|                                                                              |
|   Arguments:                                                                 |
|       hWnd            window handle for the parent window                    |
|       uiMessage       message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       0 if processed, nonzero if ignored                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/

long FAR PASCAL MyWndProc( hWnd, uiMessage, wParam, lParam )
HWND     hWnd;
unsigned uiMessage;
WORD     wParam;
long     lParam;
{
        HANDLE hqel;
        QEL *pqel;
        HMENU hMenu;
        RECT  rRect;
        char rgch[32];
        char rgch2[32];

    /*
     *  if the message is less than WM_USER then it is a message to
     *  be proccessed
     */

    switch (uiMessage) {

        case WM_CREATE:
            hSpyChild = CreateDebugWin(hInst, hWnd); 
            return 0L;

        case WM_INITMENU:
            return 0L;

        case WM_COMMAND:
            if (lParam == 0) {
                switch (wParam) {
                    case SPYOFF:
                        if (gbSpyOn) {

                           KillHook();
                         /*
                           SetWindowsHook(WH_GETMESSAGE, fpxOldHook1);
                           SetWindowsHook(WH_CALLWNDPROC, fpxOldHook2);
                          */
                           gbSpyOn = FALSE;
                           hMenu = GetMenu (hWnd);
                           ChangeMenu (hMenu,SPYOFF,(LPSTR)"Spy On",SPYON,
                                       MF_CHANGE | MF_ENABLED);
                        }
                        return(0L);

                    case SPYON:
                        if (!gbSpyOn) {
                           InitHook(hSpyChild);
                         /*
                           fpxOldHook1 = SetWindowsHook(WH_GETMESSAGE, fpxMsgHook1);
                           fpxOldHook2 = SetWindowsHook(WH_CALLWNDPROC,fpxMsgHook2);
                          */
                           gbSpyOn = TRUE;
                           hMenu = GetMenu (hWnd);
                           ChangeMenu (hMenu,SPYON,(LPSTR)"Spy Off",SPYOFF,
                                       MF_CHANGE | MF_ENABLED);
                        }
                        return(0L);

                    case SPYSET:
                        DialogBox(hInst,MAKEINTRESOURCE(DLGSPY),hWnd,fpxProcSpySet);
                        return (0L);
                }
            }
            break;

        case WM_SYSCOMMAND:
            if (wParam == CMDABOUT) {
                DialogBox(hInst,MAKEINTRESOURCE(ABOUTBOX),hWnd,fpxProcAbout);
                return(0L);
            }
            break;

        case WM_SIZE:
            MoveWindow (hSpyChild,0,0,LOWORD (lParam),HIWORD(lParam),TRUE);
            return 0L;

        case WM_DESTROY:
            SendMessage (hSpyApp,WM_COMMAND,SPYOFF,0L);

            PostQuitMessage(0L);   /* Kill the main window */
            return 0L;
    }
    return DefWindowProc (hWnd,uiMessage,wParam,lParam);
}


/*----------------------------------------------------------------------------*\
|   SpyProc( hWnd, uiMessage, wParam, lParam )                                 |
|                                                                              |
|   Description:                                                               |
|       The window proc for the app being spied on                             |
|                                                                              |
|   Arguments:                                                                 |
|       hWnd            window handle for the parent window                    |
|       uiMessage       message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       0 if processed, nonzero if ignored                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/

long FAR PASCAL SpyProc( hWnd, uiMessage, wParam, lParam )
HWND     hWnd;
unsigned uiMessage;
WORD     wParam;
long     lParam;
{
    /*
     *  This procedure is called from another task through a Data thunk and
     *  SS != DS (the SS is for the other task).  Watch out!!
     */

    PostMessage (hSpyApp,uiMessage + WM_USER,wParam,lParam);
    if (uiMessage == WM_DESTROY) {
       /*
        * The user is quitting the application being spyed on.
        * unhook the spy procedure or funny things may happen.
        */
       SendMessage (hSpyApp,WM_COMMAND,SPYOFF,0L);
       ghSpyWnd = NULL;
    }
    return CallWindowProc(OldSpyProc,hWnd,uiMessage,wParam,lParam);
}



/*----------------------------------------------------------------------------*\
|                                                                              |
|   w i n d o w   p r o c s                                                    |
|                                                                              |
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
|  THIS IS NEVER CALLED.                                                                               |
|  CreateDebugWindow (hWnd, pcText,bTiled)                                     |
|                                                                              |
|   Description:                                                               |
|     Creates a tiled window for the depositing of debuging messages.          |
|                                                                              |
|   Arguments:                                                                 |
|     hWnd   - Window handle of the parent window.                             |
|     pcText - String to appear in the caption bar of the debuging window      |
|     bTiled - FALSE => window is a popup,  Tiled otherwise.                   |
|                                                                              |
|   Returns:                                                                   |
|     A window handle of the debuging window, or NULL if a error occured.      |
|                                                                              |
\*----------------------------------------------------------------------------*/

HWND CreateDebugWindow (hParent,pchName,bTiled)
    HWND   hParent;
    char   *pchName;
    BOOL   bTiled;
{
    static int  iNextX = DEBUGX;
    static int  iNextY = DEBUGY;
    HWND        hWnd;

    MessageBeep(1);
    MessageBox( hParent, "We're here", NULL, MB_OK);
 /*
    hWnd = CreateDebugWin( hParent,
                           pchName,
                           (bTiled ? WS_TILED : WS_POPUP) |
                           WS_VSCROLL | WS_HSCROLL |
                           WS_CAPTION | WS_SYSMENU | WS_SIZEBOX,
                           iNextX,
                           iNextY,
                           DEBUGDX,
                           DEBUGDY 
                         );
 */
    iNextX += DEBUGXINC;
    iNextY += DEBUGYINC;

    return hWnd;
}

static void Error (str)
char *str;
{
  int a = MessageBox (hERROR,(LPSTR)str,(LPSTR)NULL,MB_OKCANCEL);
  if (a == IDCANCEL) PostQuitMessage(0);
}
