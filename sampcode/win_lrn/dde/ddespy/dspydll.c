#include "windows.h" 
#include "winexp.h" 
#include "ddespy.h" 
#include "dde.h" 

#define MENU

/*----------------------------------------------------------------------------*\
|                                                                              |
|   g l o b a l   v a r i a b l e s                                            |
|                                                                              |
\*----------------------------------------------------------------------------*/
/* NOTICE: The message structure for WH_CALLWNDPROC is "upside down"
           compared to the normal MSG structure. */
typedef struct mymsg {
    LONG lParam;
    WORD wParam;
    WORD message;
    HWND hwnd;
} MYMSG;
typedef MYMSG FAR *LPMYMSG;

/* Static definitions */
static  FARPROC fpxMsgHook1;
static  FARPROC fpxMsgHook2;
static  FARPROC fpxOldHook1;
static  FARPROC fpxOldHook2;

FARPROC lplpfnNextHook1;
FARPROC lplpfnNextHook2;
HWND    hSpyChild;
char    szTemp[30];
char    szTemphWnd[30];
static  int Tdx = 0;        /* Text font size */
static  int Tdy = 0;

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

#define ihqelMax 256
#define RADIX    16

HWND    hGParent;
HWND    hGMenu;
HANDLE  rghqel[ihqelMax];
int     ihqelHead;
int     ihqelTail;

int  iLen;
char pchFoo[MAXFOOLEN];    /* Buffer */

char szMessages[10][12];   /* messages */

HWND hWndMenu[10];
HWND hSpyWndMenu;  /* this is so we can attach the
                      windows text to the menu bar */

int  dllgfhFile;   /* handle to the file, if open */

/******************************************/

/* Handle to the DLL instance from Libinit.asm */
extern HANDLE LIBINST;

/*----------------------------------------------------------------------------*\
|                                                                              |
|   f u n c t i o n   d e f i n i t i o n s                                    |
|                                                                              |
\*----------------------------------------------------------------------------*/
void  FAR PASCAL InitHook( HWND );
BOOL  FAR PASCAL KillHook();
DWORD FAR PASCAL MsgHook1( WORD, WORD, LPMSG );
DWORD FAR PASCAL MsgHook2( WORD, WORD, LPMSG );
void  FAR PASCAL DebugPaint(HWND, HDC);
HWND  FAR PASCAL CreateDebugWin(HANDLE, HWND);

void  FAR PASCAL PassFileToDLL(int);
void  WriteitToFile( LPSTR , int);
void  FormatString(void);

void DebugHScroll (HWND, PTXT, int);
void DebugVScroll (HWND, PTXT, int);
void Error (char *);

char   *WM_MessageText(WORD);
HANDLE HqelGet();
void   HqelPut(HANDLE);


void FAR PASCAL PassFileToDLL(int gfhFile)
{
   dllgfhFile = gfhFile;
}


/* ------------------- Set the HOOKs ---------------- */
void FAR PASCAL InitHook( hWnd )
HWND hWnd;
{

   hSpyChild = hWnd;  /* handle to child window */

   fpxMsgHook1 = MakeProcInstance((FARPROC)MsgHook1,LIBINST);
   fpxMsgHook2 = MakeProcInstance((FARPROC)MsgHook2,LIBINST);

   fpxOldHook1 = SetWindowsHook(WH_GETMESSAGE, fpxMsgHook1);
   fpxOldHook2 = SetWindowsHook(WH_CALLWNDPROC,fpxMsgHook2);

   strcpy(szMessages[0], "INITIATE  ");
   strcpy(szMessages[1], "TERMINATE ");
   strcpy(szMessages[2], "ADVISE    ");
   strcpy(szMessages[3], "UNADVISE  ");
   strcpy(szMessages[4], "ACK       ");
   strcpy(szMessages[5], "DATA      ");
   strcpy(szMessages[6], "REQUEST   ");
   strcpy(szMessages[7], "POKE      ");
   strcpy(szMessages[8], "EXECUTE   ");

return;
}

/* ------------------- Remove the HOOKs ---------------- */
BOOL FAR PASCAL KillHook()
{
   BOOL fSuccess;

   fSuccess = UnhookWindowsHook( WH_GETMESSAGE,  fpxMsgHook1 );
   fSuccess = UnhookWindowsHook( WH_CALLWNDPROC, fpxMsgHook2 );

   FreeProcInstance( fpxMsgHook1 );
   FreeProcInstance( fpxMsgHook2 );

   return fSuccess;
}



/*----------------------------------------------------------------------------*\
|  CreateDebugWin (hParent, lpchName, dwStyle, x, y, dx, dy)                   |
|                                                                              |
|   Description:                                                               |
|     Creates a window for the depositing of debuging messages.                |
|                                                                              |
|   Arguments:                                                                 |
|     hWnd   - Window handle of the parent window.                             |
|     pcName - String to appear in the caption bar of the debuging window      |
|     dwStyle- Window style                                                    |
|     x,y    - Location of window                                              |
|     dx,dy  - Size of the window                                              |
|                                                                              |
|   Returns:                                                                   |
|     A window handle of the debuging window, or NULL if a error occured.      |
|                                                                              |
\*----------------------------------------------------------------------------*/

HWND FAR PASCAL CreateDebugWin (hInstance, hParent)
HANDLE  hInstance;
HWND    hParent;
{
    static BOOL bFirst = TRUE;   /* Is this the first call */

    HWND   hWnd;
    PTXT   pTxt;      /* pointer to a debuging window struct */

    TEXTMETRIC tm;    /* Used to find out the size of a char */
    RECT       rRect;
    HDC        hDC;
    int        i;

    if (bFirst) {
        /* Find out the size of a Char in the system font */
        hDC = GetDC(hParent);
        GetTextMetrics (hDC,(LPTEXTMETRIC)&tm);
        Tdy = tm.tmHeight;
        Tdx = tm.tmAveCharWidth;
        ReleaseDC (hParent,hDC);
        SetDebugClass(hInstance);
        hGParent = hParent;
        hGMenu = GetMenu(hParent);
        hSpyWndMenu = GetSubMenu( hGMenu ,4 );

        for (i=0 ; i<10 ; i++)         /* clear menu handles */
           hWndMenu[i] = NULL;

        bFirst = FALSE;
    }

    hInstance = GetWindowWord(hParent, GWW_HINSTANCE);
    GetClientRect (hParent,(LPRECT)&rRect);

    hWnd = CreateWindow((LPSTR)"DEBUG"
                        ,(LPSTR)"DDE SPY"
                        ,WS_CHILD | WS_VSCROLL | WS_HSCROLL
                        ,0,0
                        ,rRect.right
                        ,rRect.bottom
                        ,(HWND)hParent      /* parent window */
                        ,(HMENU)NULL        /* use class menu */
                        ,(HANDLE)hInstance  /* handle to window instance */
                        ,(LPSTR)NULL        /* no params to pass on */
                       );

    pTxt = (PTXT)LocalAlloc(LPTR,sizeof(struct TEXT_STRUCT));
    if (!pTxt || !hWnd) {
        Error ("CreateDebugWindow Failed.....");
        return NULL;
    }
    SetWindowWord (hWnd,0,(WORD)pTxt); /* Store the structure pointer with the
                                          window handle */

    pTxt->iFirst = 0;               /* Set the que up to have 1 NULL line */
    pTxt->iCount = 1;
    pTxt->iTop   = 0;
    pTxt->iLeft  = 0;
    pTxt->Text[0]= NULL;
    pTxt->Len[0] = 0;
    pTxt->MaxLen = 0;

    SetScrollRange (hWnd,SB_VERT,0,0,FALSE);
    SetScrollRange (hWnd,SB_HORZ,0,0,FALSE);
    /* Make window visible */
    ShowWindow(hWnd,SHOW_OPENWINDOW);

    return hWnd;
}


/*----------------------------------------------------------------------------*\
|   SetDebugClass(hInstance)                                                   |
|                                                                              |
|   Description:                                                               |
|     Registers a class for a DEBUG window.                                    |
|                                                                              |
|   Arguments:                                                                 |
|       hInstance       instance handle of current instance                    |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if successful, FALSE if not                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/

static BOOL SetDebugClass(hInstance)
  HANDLE hInstance;
{
    WNDCLASS rClass;

    rClass.hCursor        = LoadCursor(NULL,IDC_ARROW);
    rClass.hIcon          = (HICON)NULL;
    rClass.lpszMenuName   = (LPSTR)NULL;
    rClass.lpszClassName  = (LPSTR)"DEBUG";
    rClass.hbrBackground  = (HBRUSH)COLOR_WINDOW + 1;
    rClass.hInstance      = hInstance;
    rClass.style          = CS_HREDRAW | CS_VREDRAW;
    rClass.lpfnWndProc    = DebugWndProc;
    rClass.cbWndExtra     = sizeof (HANDLE);
    rClass.cbClsExtra     = 0;

    return RegisterClass((LPWNDCLASS)&rClass);
}



/*----------------------------------------------------------------------------*\
|   MsgHook1 (wToast,pMsg,nCode)                                                |
|                                                                              |
|   Description:                                                               |
|       The Message Hook1 to process all messages...                            |
|                                                                              |
|   Arguments:                                                                 |
|       wToast          integer (0-100) for darkness of toast in morning       |
|       pMsg            Pointer to message                                     |
|       nCode           Message context                                        |
|                                                                              |
|   Returns:                                                                   |
|       FALSE to let windows process, TRUE to ignore                           |
|                                                                              |
\*----------------------------------------------------------------------------*/
DWORD FAR PASCAL MsgHook1(code, wToast, lpmsg)
WORD     code;
WORD     wToast;
LPMSG    lpmsg;
{
   HANDLE hData;
   HANDLE hqel;
   QEL *pqel;
   char rgch[65];

   if (code == HC_ACTION &&
       lpmsg->message >= WM_DDE_FIRST &&
       lpmsg->message <= WM_DDE_LAST)
   {
      hqel=LocalAlloc(LMEM_MOVEABLE|LMEM_ZEROINIT, sizeof(QEL));
      pqel=(QEL *)LocalLock(hqel);
      pqel->msg=*lpmsg;
      switch(lpmsg->message)
      {
      case WM_DDE_TERMINATE:
              break;

      case WM_DDE_ACK:
              if ((unsigned)HIWORD(lpmsg->lParam)>=(unsigned)0xc000)
                      {
                      pqel->un.type=1;
                      goto OneAtom;
                      }
              pqel->un.type=2;
              /* fall thru... */
      case WM_DDE_EXECUTE:
              break;
      case WM_DDE_ADVISE:
              hData=(HANDLE)LOWORD(lpmsg->lParam);
              pqel->un.cf=((DDELN far *)GlobalLock(hData))->cf;
              GlobalUnlock(hData);
              goto OneAtom;
      case WM_DDE_REQUEST:
              pqel->un.cf=LOWORD(lpmsg->lParam);
              goto OneAtom;
      case WM_DDE_DATA:
      case WM_DDE_POKE:
              hData=(HANDLE)LOWORD(lpmsg->lParam);
              pqel->un.cf=((DDEUP far *)GlobalLock(hData))->cf;
              GlobalUnlock(hData);
              /* fall thru... */
      case WM_DDE_UNADVISE:
OneAtom:
              rgch[GlobalGetAtomName(HIWORD(lpmsg->lParam), rgch, 64)]='\0';
              pqel->atom1=AddAtom(rgch);
              break;
      }
      LocalUnlock(hqel);
      HqelPut(hqel);
      /* alert window that new queue data is available */
      FormatString();
      /* PostMessage(hSpyApp, WM_USER, 0, 0L); */
   }
   /* do default message handling */
   return(DefHookProc(code, wToast, (DWORD)lpmsg, (FARPROC FAR *)&fpxOldHook1));
}

/* ----------------------------------------------------- */
DWORD FAR PASCAL MsgHook2(code, wToast, lpmsgr)
WORD   code;
WORD   wToast;
LPMSGR lpmsgr;
{
   HANDLE hqel;
   QEL *pqel;
   char rgch[65];

   if (code == HC_ACTION &&
       lpmsgr->message >= WM_DDE_FIRST && 
       lpmsgr->message <= WM_DDE_LAST)
   {
       hqel=LocalAlloc(LMEM_MOVEABLE|LMEM_ZEROINIT, sizeof(QEL));
       pqel=(QEL *)LocalLock(hqel);
       pqel->msg.message =lpmsgr->message;
       pqel->msg.wParam  =lpmsgr->wParam;
       pqel->msg.lParam  =lpmsgr->lParam;
       pqel->msg.hwnd    =lpmsgr->hwnd;
       pqel->un.type     =0;
       rgch[GlobalGetAtomName(LOWORD(lpmsgr->lParam), rgch, 64)]='\0';
       pqel->atom1=AddAtom(rgch);
       rgch[GlobalGetAtomName(HIWORD(lpmsgr->lParam), rgch, 64)]='\0';
       pqel->atom2=AddAtom(rgch);
       LocalUnlock(hqel);
       HqelPut(hqel);
       /* alert window that new queue data is available */
       FormatString();
       /* PostMessage(hSpyApp, WM_USER, 0, 0L); */
   }
   /* do default message handling */
   return(DefHookProc(code, wToast, (DWORD)lpmsgr, (FARPROC FAR *)&fpxOldHook2));
}

/* -------------------- FormatString ------------- */
void  FormatString()
{

  HANDLE hqel;
  QEL *pqel;
  HMENU hMenu;
  RECT  rRect;
  char rgch[32];
  char rgch2[32];
  int i;

  /*
   *  if the message is greater than WM_USER then it is a message to
   *  be printed in the window
   */

   while ((hqel=HqelGet())!=NULL)
   {
        pqel=(QEL *)LocalLock(hqel);

#ifdef OLD
        DebugPrintf(hSpyChild,"%-10s %4.4X %4.4X %4.4X,%4.4X ",
                WM_messageText(pqel->msg.message),
                pqel->msg.wParam,
                pqel->msg.hwnd,
                LOWORD(pqel->msg.lParam),
                HIWORD(pqel->msg.lParam));
#else
        lstrcpy( pchFoo, szMessages[(pqel->msg.message)-WM_DDE_FIRST] );
        lstrcat( pchFoo, "  " );
        ultoa ((DWORD)pqel->msg.wParam, szTemp, 16);
        lstrcat( pchFoo, szTemp );
        lstrcat( pchFoo, "  " );
        ultoa ((DWORD)pqel->msg.hwnd, szTemphWnd, 16);
        lstrcat( pchFoo, szTemphWnd );
        if ( strlen(szTemphWnd) < 4)
          lstrcat( pchFoo, " " );
        lstrcat( pchFoo, "  " );
        ultoa ((DWORD)LOWORD(pqel->msg.lParam), szTemp, 16);
        lstrcat( pchFoo, szTemp );
        if ( strlen(szTemp) < 3)
          lstrcat( pchFoo, "  " );
        if ( strlen(szTemp) < 4)
          lstrcat( pchFoo, " " );
        lstrcat( pchFoo, "  " );
        ultoa ((DWORD)HIWORD(pqel->msg.lParam), szTemp, 16);
        lstrcat( pchFoo, szTemp );

        iLen = 0;
        DebugPrintf(hSpyChild, pchFoo, iLen);


#endif

#ifdef MENU
        if ( pqel->msg.message == WM_DDE_INITIATE &&
             pqel->msg.hwnd    != NULL )
        {
          for (i=0 ; i<10 ; i++) {
             if (hWndMenu[i] == NULL ) {
                 hWndMenu[i] = pqel->msg.hwnd;
                 GetWindowText ( pqel->msg.hwnd, szTemp,25 );
                 lstrcat ( szTemphWnd, ":" );
                 lstrcat ( szTemphWnd, szTemp );
                 ChangeMenu(hSpyWndMenu, 153, (LPSTR)szTemphWnd,153, 
                            i ? MF_APPEND : MF_CHANGE );
                 if (i == 0) {
                   EnableMenuItem(hGMenu, 152, MF_ENABLED );
                   DrawMenuBar(hGParent);
                 }
                 break;
             }
             else {
               if ( hWndMenu[i] == pqel->msg.hwnd )
                 break;
             }
          }
        }
#endif
        switch(pqel->msg.message)
        {
          case WM_DDE_ACK:
                switch (pqel->un.type)
                        {
                case 1:
                        goto OneAtom;
                case 2:
                        goto Execute;
                        }
                /* fall thru... */
          case WM_DDE_INITIATE:
                rgch[GetAtomName(pqel->atom1, (LPSTR)rgch, 32)]='\0';
                rgch2[GetAtomName(pqel->atom2, (LPSTR)rgch2, 32)]='\0';
#ifdef OLD
                DebugPrintf(hSpyChild,"(App:%s, Topic:%s)",rgch, rgch2);
#else
                lstrcpy( pchFoo, " (App:"                     );
                lstrcat( pchFoo,       rgch                  );
                lstrcat( pchFoo,           ", Topic:"        );
                lstrcat( pchFoo,                    rgch2    );
                lstrcat( pchFoo,                         ")" );
                DebugPrintf(hSpyChild, pchFoo, iLen);
#endif
                DeleteAtom(pqel->atom1);
                DeleteAtom(pqel->atom2);
                break;
          case WM_DDE_TERMINATE:
                break;
          case WM_DDE_UNADVISE:
OneAtom:
                rgch[GetAtomName(pqel->atom1, (LPSTR)rgch, 32)]='\0';
#ifdef OLD
                DebugPrintf(hSpyChild,"(Item:%s)",rgch);
#else
                lstrcpy( pchFoo, " (Items:"      );
                lstrcat( pchFoo,         rgch   );
                lstrcat( pchFoo,            ")" );
                DebugPrintf(hSpyChild, pchFoo, iLen);
#endif
                DeleteAtom(pqel->atom1);

                break;
          case WM_DDE_ADVISE:
          case WM_DDE_DATA:
          case WM_DDE_POKE:
          case WM_DDE_REQUEST:
                rgch[MyGetClipboardFormatName(pqel->un.cf, (LPSTR)rgch, 32)]='\0';
                rgch2[GetAtomName(pqel->atom1, (LPSTR)rgch2, 32)]='\0';
#ifdef OLD
                DebugPrintf(hSpyChild,"(Format:%s, Item:%s)",rgch, rgch2);
#else
                lstrcpy( pchFoo, " (Format:"                    );
                lstrcat( pchFoo,         rgch                  );
                lstrcat( pchFoo,             ", Items:"        );
                lstrcat( pchFoo,                      rgch2    );
                lstrcat( pchFoo,                           ")" );
                DebugPrintf(hSpyChild, pchFoo, iLen);
#endif
                DeleteAtom(pqel->atom1);
                break;
          case WM_DDE_EXECUTE:
Execute:
                break;
        }
#ifdef OLD
        DebugPrintf(hSpyChild,"\n");
#else
        lstrcpy( pchFoo, "\n" );
        DebugPrintf(hSpyChild, pchFoo, iLen);
#endif

        LocalUnlock(hqel);
        LocalFree(hqel);
   }
   /* return(0L); */
}

/*----------------------------------------------------------------------------*\
|   DebugPrintf (hWnd,str,...)                                                 |
|                                                                              |
|   Description:                                                               |
|       Writes data into the window hWnd (hWnd must be created with            |
|       CreateDebugWindow ())                                                  |
|       follows the normal C printf definition.                                |
|                                                                              |
|   Arguments:                                                                 |
|       hWnd            window handle for the Degubing window                  |
|       str             printf control string                                  |
|       ...             extra parameters as required by the contol string      |
|                                                                              |
|   NOTE: if hWnd == NULL text will be printed in the window used in the last  |
|         call to DebugPrintf.                                                 |
\*----------------------------------------------------------------------------*/

int  DebugPrintf (hWnd,format,i)
HWND hWnd;
char *format;
int  i;
{
#ifdef OLD
  return vDebugPrintf (hWnd,format,&i);
#else
  return vDebugPrintf (hWnd,format,i);
#endif
}

#ifdef OLD
int  vDebugPrintf (hWnd,format,pi)
HWND hWnd;
char *format;
int  *pi;
#else
int  vDebugPrintf (hWnd,format,pi)
HWND hWnd;
char *format;
int  pi;
#endif

{
  static HWND hWndLast = NULL;
  RECT  rect;
  int   iFree;
  int   iRet;
  int   iLine;
  int   cLine;
  int   cScroll;
  PTXT  pTxt;
  MSG   rMsg;
  POINT rPoint;
/*
  if (!SSEqualToDS()) {
    Error ("WARNING: SS != DS,  Can't printf to window");
    return 0;
  }
 */
  if (hWnd == NULL) hWnd = hWndLast;

  if (hWnd == NULL || !IsWindow (hWnd)) {
     Error ("WARNING: bad window handle,  Can't printf to window");
     return 0;  /* exit if bad window handle */
  }
  pTxt = (PTXT)GetWindowWord (hWnd,0);
  hWndLast = hWnd;
#ifdef OLD
  iRet = vsprintf(pchFoo,format,pi);
#endif

  iLine   = pTxt->iCount - pTxt->iTop;
  cLine   = LinesInDebugWindow(hWnd);
  iFree   = pTxt->iTop;
  cScroll = InsertString (pTxt,pchFoo);  /* Insert text in the que */

  /* Scroll the window if Que overflowed */
  if (pTxt->iCount == MAXLINES && pTxt->iTop == 0) {
    iFree = cScroll - iFree;
    pTxt->iTop = -iFree;
    DebugVScroll (hWnd,pTxt,iFree);
  }

  if (iLine == cLine) {                  /* Scroll the window if last line */
     DebugVScroll (hWnd,pTxt,cScroll);   /* is allso last line in window */
     iLine -= cScroll;
  }

  /* Update the scroll bars */
  SetScrollRange (hWnd,SB_VERT,0,pTxt->iCount-2,FALSE);
  SetScrollRange (hWnd,SB_HORZ,0,pTxt->MaxLen,FALSE);
  SetScrollPos   (hWnd,SB_VERT,pTxt->iTop,FALSE);
  SetScrollPos   (hWnd,SB_HORZ,pTxt->iLeft,FALSE);

  /* Now make sure the new text is painted only if visable */
  GetClientRect(hWnd,(LPRECT)&rect);
  rect.top += (iLine-1) * Tdy;
  InvalidateRect (hWnd,(LPRECT)&rect,FALSE);
  UpdateWindow (hWnd);

  return(iRet);       /* return the count of arguments printed */
}

/*----------------------------------------------------------------------------*\
|                                                                              |
\*----------------------------------------------------------------------------*/

static void NewLine (pTxt)
PTXT pTxt;
{
  int iLast = LAST(pTxt);
  int iLine,cLine;

  if (pTxt->iCount == MAXLINES) {
    LocalFree ((HANDLE)pTxt->Text[pTxt->iFirst]);
    INC (pTxt->iFirst);
    if (pTxt->iTop > 0) pTxt->iTop--;
  }
  else {
    pTxt->iCount++;
  }
  iLast = LAST(pTxt);
  pTxt->Text[iLast] = NULL;
  pTxt->Len[iLast]  = 0;
}

/*----------------------------------------------------------------------------*\
|                                                                              |
\*----------------------------------------------------------------------------*/

static int InsertString (pTxt,str)
  PTXT pTxt;
  char *str;
{
  static char   buffer[MAXFOOLEN];               /* intermediate buffer */
  int    iBuf;
  int    iLast = LAST(pTxt);
  int    cLine = 0;

  for (iBuf=0; iBuf < pTxt->Len[iLast]; iBuf++)
    buffer[iBuf] = pTxt->Text[iLast][iBuf];

  if (pTxt->Text[iLast] != NULL)
    LocalFree ((HANDLE)pTxt->Text[iLast]);

  while (*str != '\0') {
    while ((*str != '\n') && (*str != '\0'))
      buffer[iBuf++] = *str++;

    /* Test for the case of a zero length line, Only brian would do this */

    if (iBuf == 0)
       pTxt->Text[iLast] == NULL;
    else {
      if ((pTxt->Text[iLast] = (char *)LocalAlloc (LPTR,iBuf)) == NULL)
         Error ("Local alloc failed in Insert string.....");
    }

    pTxt->Len[iLast] = iBuf;
    if (iBuf > pTxt->MaxLen) pTxt->MaxLen = iBuf;
    while (--iBuf >= 0 )
      pTxt->Text[iLast][iBuf] = buffer[iBuf];

    if (*str == '\n') {   /* Now do the next string after the \n */
       str++;
       iBuf = 0;
       cLine++;
       NewLine (pTxt);
       INC(iLast);
    }
  }
  return cLine;
}


/*----------------------------------------------------------------------------*\
|   DebugWndProc( hWnd, uiMessage, wParam, lParam )                               |
|                                                                              |
|   Description:                                                               |
|       The window proc for the debuging window.  This processes all           |
|       of the window's messages.                                              |
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

long FAR PASCAL DebugWndProc( hWnd, uiMessage, wParam, lParam )
HWND     hWnd;
unsigned uiMessage;
WORD     wParam;
long     lParam;
{
    PAINTSTRUCT rPS;
    PTXT        pTxt  = (PTXT)GetWindowWord(hWnd,0);
    RECT        CRect;
    static WORD wScroll;

    switch (uiMessage) {
       /*
        * This tries to go around a BUG in 1.03 about scroll bars being confused
        */
        case WM_SYSCOMMAND:
            switch (wParam & 0xFFF0) {
                case SC_VSCROLL:
                case SC_HSCROLL:
                     wScroll = wParam & 0xFFF0;
                default:
                   return DefWindowProc(hWnd,uiMessage,wParam,lParam);
            }
            break;

        case WM_DESTROY: {
            int i,iQue;

            iQue = TOP(pTxt);
            for (i=0; i < pTxt->iCount; i++,INC(iQue))
              if (pTxt->Text[iQue] != NULL)
                 LocalFree ((HANDLE)pTxt->Text[iQue]);

            LocalFree ((HANDLE)pTxt);
            break;
        }

        case WM_VSCROLL:
            if (wScroll == SC_VSCROLL) {
               switch (wParam) {
                  case SB_LINEDOWN:
                     DebugVScroll (hWnd,pTxt,1);
                     break;
                  case SB_LINEUP:
                     DebugVScroll (hWnd,pTxt,-1);
                     break;
                  case SB_PAGEUP:
                     DebugVScroll (hWnd,pTxt,-PAGE);
                     break;
                  case SB_PAGEDOWN:
                     DebugVScroll (hWnd,pTxt,PAGE);
                     break;
                  case SB_THUMBPOSITION:
                     DebugVScroll (hWnd,pTxt,LOWORD(lParam)-pTxt->iTop);
                     break;
               }
             }
             break;

        case WM_HSCROLL:
            if (wScroll == SC_HSCROLL) {
               switch (wParam) {
                  case SB_LINEDOWN:
                     DebugHScroll (hWnd,pTxt,1);
                     break;
                  case SB_LINEUP:
                     DebugHScroll (hWnd,pTxt,-1);
                     break;
                  case SB_PAGEUP:
                     DebugHScroll (hWnd,pTxt,-PAGE);
                     break;
                  case SB_PAGEDOWN:
                     DebugHScroll (hWnd,pTxt,PAGE);
                     break;
                  case SB_THUMBPOSITION:
                     DebugHScroll (hWnd,pTxt,LOWORD(lParam)-pTxt->iLeft);
                     break;
               }
            }
            break;

        case WM_PAINT:
            BeginPaint(hWnd,(LPPAINTSTRUCT)&rPS);
            DebugPaint (hWnd,rPS.hdc);
            EndPaint(hWnd,(LPPAINTSTRUCT)&rPS);
            break;

        default:
           return DefWindowProc(hWnd,uiMessage,wParam,lParam);
    }
    return 0L;
}


/*----------------------------------------------------------------------------*\
|   DebugPaint(hWnd, hDC )                                                     |
|                                                                              |
|   Description:                                                               |
|       The paint function.                                                    |
|                                                                              |
|   Arguments:                                                                 |
|       hWnd            Window to paint to.                                    |
|       hDC             handle to update region's display context              |
|                                                                              |
|   Returns:                                                                   |
|       nothing                                                                |
|                                                                              |
\*----------------------------------------------------------------------------*/

void FAR PASCAL DebugPaint(hWnd, hDC)
HWND hWnd;
HDC  hDC;
{
  PTXT pTxt;
  int  i;
  int  iQue;
  int  xco;
  int  yco;
  char iFoo = 0;
  int  iLast;
  int  Wdy;
  RECT CRect;

  pTxt = (PTXT)GetWindowWord(hWnd,0);

  GetClientRect(hWnd,(LPRECT)&CRect);
  Wdy = CRect.bottom-CRect.top;

  iLast = LAST(pTxt);
  iQue  = TOP(pTxt);
  xco   = OFFSETX;
  yco   = OFFSETY;
  for (;;) {
    if (pTxt->Len[iQue] > pTxt->iLeft)
       TextOut (hDC,xco,yco,(LPSTR)pTxt->Text[iQue] + pTxt->iLeft,
                                   pTxt->Len[iQue]  - pTxt->iLeft
               );
    if (iQue == iLast) break;
    INC(iQue);
    yco += Tdy;
    if (yco > Wdy) break;
  }
}


/*----------------------------------------------------------------------------*\
|                                                                              |
\*----------------------------------------------------------------------------*/

void DebugVScroll (hWnd,pTxt,n)
HWND hWnd;
PTXT pTxt;
int  n;
{
  int  delta;
  RECT rect;

  GetClientRect (hWnd,(LPRECT)&rect);
  rect.left += OFFSETX;
  rect.top  += OFFSETY;

  if (n > 0) {
     delta = pTxt->iCount - pTxt->iTop - 2;
     if (n > delta) n = delta;
     pTxt->iTop += n;
     ScrollWindow (hWnd,0,-n*Tdy,(LPRECT)&rect,(LPRECT)&rect);
  }
  else {
     n *= -1;
     delta = pTxt->iTop;
     if (n > delta) n = delta;
     pTxt->iTop -= n;
     ScrollWindow (hWnd,0,n*Tdy,(LPRECT)&rect,(LPRECT)&rect);
  }
  SetScrollPos (hWnd,SB_VERT,pTxt->iTop,TRUE);
}

/*----------------------------------------------------------------------------*\
|                                                                              |
\*----------------------------------------------------------------------------*/

void DebugHScroll (hWnd,pTxt,n)
HWND hWnd;
PTXT pTxt;
int  n;
{
  int delta;
  RECT rect;

  GetClientRect (hWnd,(LPRECT)&rect);
  rect.left += OFFSETX;
  rect.top  += OFFSETY;

  if (n > 0) {
     delta = pTxt->MaxLen - pTxt->iLeft;
     if (n > delta) n = delta;
     pTxt->iLeft += n;
     ScrollWindow (hWnd,-n*Tdx,0,(LPRECT)&rect,(LPRECT)&rect);
  }
  else {
     n *= -1;
     delta = pTxt->iLeft;
     if (n > delta) n = delta;
     pTxt->iLeft -= n;
     ScrollWindow (hWnd,n*Tdx,0,(LPRECT)&rect,(LPRECT)&rect);
  }
  SetScrollPos (hWnd,SB_HORZ,pTxt->iLeft,TRUE);
}

/*----------------------------------------------------------------------------*\
|                                                                              |
\*----------------------------------------------------------------------------*/

static
int LinesInDebugWindow (hWnd)
    HWND hWnd;
{
    RECT CRect;

    GetClientRect(hWnd,(LPRECT)&CRect);
    return (CRect.bottom-CRect.top) / Tdy + 1;
}


/*----------------------------------------------------------------------------*\
| Error routine, brings up a message box with a error message                  |
\*----------------------------------------------------------------------------*/

static void Error (str)
char *str;
{
  int a = MessageBox (GetFocus(),(LPSTR)str,(LPSTR)NULL,MB_OKCANCEL);
  if (a == IDCANCEL) PostQuitMessage(0);
}

/*----------------------------------------------------------------------------*\
| routine to verify that SS == DS                                              |
\*----------------------------------------------------------------------------*/

static BOOL SSEqualToDS () {
  int a = (int)&a;
  return (int far *)&a == (int far *)(int near *)a;
}


int MyGetClipboardFormatName(cf, lpch, cchMac)
int cf;
LPSTR lpch;
int cchMac;
{
        switch(cf)
                {
        case CF_TEXT:
                lstrcpy(lpch,"TEXT");
                return(4);
        case CF_BITMAP:
                lstrcpy(lpch,"BITMAP");
                return(6);
        case CF_METAFILEPICT:
                lstrcpy(lpch,"METAFILEPICT");
                return(12);
        case CF_SYLK:
                lstrcpy(lpch,"SYLK");
                return(4);
        case CF_DIF:
                lstrcpy(lpch,"DIF");
                return(3);
        default:
                return(GetClipboardFormatName(cf, lpch, cchMac));
                }
}


/* add an element to the queue */
void HqelPut(hqel)
HANDLE hqel;
{

  rghqel[ihqelHead++]=hqel;
  if (ihqelHead==ihqelMax)
    ihqelHead=0;
  if (ihqelHead==ihqelTail)
    MessageBox(GetFocus(), "Queue overflow", NULL, MB_OK|MB_SYSTEMMODAL);
}

/* delete an element from the queue */
HANDLE HqelGet()
{
  HANDLE hqel;

  if (ihqelTail==ihqelHead)
    return(NULL);
  hqel=rghqel[ihqelTail++];

  if (ihqelTail==ihqelMax)
    ihqelTail=0;

  return(hqel);
}

#ifdef OLD
 char *WM_MessageText (uiMsg)
 WORD uiMsg;
 {
   switch (uiMsg)
   {
     case WM_DDE_INITIATE:  return("INITIATE");
     case WM_DDE_TERMINATE: return("TERMINATE");
     case WM_DDE_ADVISE:    return("ADVISE");
     case WM_DDE_UNADVISE:  return("UNADVISE");
     case WM_DDE_ACK:       return("ACK");
     case WM_DDE_DATA:      return("DATA");
     case WM_DDE_REQUEST:   return("REQUEST");
     case WM_DDE_POKE:      return("POKE");
     case WM_DDE_EXECUTE:   return("EXECUTE");
   }
   return(NULL);
 }
#endif

#ifdef OLD
 void WriteitToFile( LPSTR lpString, int iLen )
 {
   if (dllgfhFile > 0) {
     _lwrite (dllgfhFile," ",1);
     _lwrite (dllgfhFile,lpString,iLen);
   }
   return;

 }
#endif

