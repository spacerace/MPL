/****** Resource IDs *****/

#define IDR_SPY    1
#define IDD_DLGSPY  0x001
#define MessagesDlg 0x002
#define OutputsDlg  0x003
#define WindowsDlg  0x004
#define SaveListDlg 0x005
#define AboutDlg    0x006
#define ListNearDlg 0x007
#define MsgQueueDlg 0x008

/****** Menu command IDs *****/
/* (by convention, hi byte is menu index, lo byte is dispatch table index) */

/* Spy Menu */
#define CMD_ACTIVE      0x0000
#define CMD_EXIT        0x0001
#define CMD_ABOUT       0x0002
#define CMD_CLRWIN      0x0003
#define CMD_SAVEWIN     0x0004
#define CMD_SAVEOPT     0x0005
#define CMD_LISTNEAR    0x0006

/* Window Menu */
#define CMD_WINDOWS     0x0100
#define CMD_QUEUES      0x0101
#define CMD_WNMSSEL     0x0102
#define CMD_WNMSDSL     0x0103
#define CMD_ALLWNDWS    0x0104
#define CMD_ALLFRAMES   0x0105
#define CMD_WNDPWIN     0x0106
#define CMD_WNDPALL     0x0107

/* Message Menu */
#define CMD_MESSAGES    0x0200
#define CMD_MGDABLE     0x0201
#define CMD_MGEABLE     0x0202
#define CMD_ALPHASORT   0x0203

/* Hooks Menu */
#define CMD_INPUTHOOK   0x0300
#define CMD_SENDMSGHOOK 0x0301
#define CMD_SENDEXTEND  0x0302
#define CMD_SENDSTACK   0x0303

/* Outputs Menu */
#define CMD_OUTSCREEN   0x0400
#define CMD_OUTTERM     0x0401
#define CMD_OUTFILE     0x0402
#define CMD_OUTPUTS     0x0403


#define IDD_DLGOPTIONS  0x010
#define IDD_DLGWINDOWS  0x020
#define IDD_DLGSAVELIST 0x030
#define IDD_DLGLISTNEAR 0x040

/* From the Main Dialog */
#define DID_SPYLIST     0x0100


/* From the Options Dialog */
#define DID_OMSGLIST    0x0100
#define DID_MALL        0x0101
#define DID_MNONE       0x0102
#define DID_MCON        0x0103
#define DID_MCOFF       0x0104
#define DID_MMON        0x0105
#define DID_MMOFF       0x0106
#define DID_MFON        0x0107
#define DID_MFOFF       0x0108
#define DID_MSGEDIT     0x0109

#define DID_OINPUT      0x0120
#define DID_OSENDMSG    0x0121
#define DID_OTHERMSGS   0x0122

#define DID_WINDOW      0x0110
#define DID_DEBUG       0x0111
#define DID_FILE        0x0112
#define DID_WINDOWLINES 0x0113
#define DID_FILENAME    0x0114

/* From the Windows Dialog */
#define DID_WINDOWLIST  0x0200
#define DID_WHANDLE     0x0201
#define DID_WPARENT     0x0202
#define DID_WCHILD      0x0203
#define DID_WOWNER      0x0204
#define DID_WRECT       0x0205
#define DID_WID         0x0207
#define DID_WSELALL     0x0208
#define DID_WUNSELALL   0x0209
#define DID_WSELMOUSE   0x020a
#define DID_WSTYLE      0x020b
#define DID_WCSTYLE     0x020c
#define DID_WPFNWP      0x020d
#define DID_WHMQ        0x020e
#define DID_WCLASS      0x020f
#define DID_WOLINE1     0x0210
#define DID_WOLINE2     0x0211
#define DID_WOLINE3     0x0212
#define DID_WOLINE4     0x0213
#define DID_WTEXT       0x0214
#define DID_WPID        0x0215
#define DID_WTID        0x0216

/* From the save file dialog */
#define DID_APPEND              0x0300

/* From the symbol dialog */
#define DID_SYMLIST             0x0400
#define DID_ADDRLABEL           0x0401
#define DID_ADDR                0x0402
#define DID_PIDLABEL            0x0403
#define DID_PID                 0x0404

/*
 * Define data structures
 */
typedef struct _SPYOPT { /* Spy Options */
    /* Now define the output options */
    /* In options dialog */
    BOOL        fWindow;
    BOOL        fFile;
    SHORT       cWindowLines;
    HFILE       hfileSpy;               /* if all lines go to file */

    /* SendMsg Hook Output options */
    USHORT      bHooks;                 /* Which hooks are enabled */
    BOOL        fSendExtend;            /* Should extended data be displayed */
    BOOL        fSendStack;             /* Should call stack be displayed */

    /* In Save file dialog */
    BOOL        fAppend;

    /* In Message Dialog */
    BOOL        fDispOtherMsgs;         /* Undefined messages displayed? */
    BOOL        fAlphaSortMsgList;      /* Should the message list be sorted */

} SPYOPT;       /* spyoptions */

typedef struct _spystr {

    /* Define file names at end, will define as seperate strings in os2.ini */
    CHAR        szFileName[100];
    char        szSaveFileName[100];
} SPYSTR;       /* Strings are seperated from options */

typedef struct _IDTONAME {
    USHORT  id;             /* id of window */
    char    *szIdName;      /* String with text of ID */
} IDTONAME;

/*
 * Define the system defined limits
 */
#define CBSTACK     4096
/* This is a real hack, estimate size of WND Structure */
#define SIZEOFWND   34
#define MAXMSGBYTES 100

/*
 * Define Message output information
 */

#define MSGI_ENABLED    0x0001
#define MSGI_MOUSE      0x0002  /* Mouse type messages */
#define MSGI_KEY        0x0004  /* Key type messages */
#define MSGI_FREQ       0x0010  /* Frequent messages generaly ignored */


typedef struct _MSGI {

    USHORT  msg;
    char    *szMsg;
    USHORT  wOptions;
    UCHAR   bMPTypes;
    SHORT   iListBox;
} MSGI;

/*
 * Now define all of the global strutures that are exported from
 * the initializition file
 */
extern SPYOPT       spyopt;
extern SPYSTR       spystr;
extern MSGI         rgmsgi[];
extern SHORT        cmsgi;          /* Count of message info items */
extern IDTONAME     rgidtoname[];
extern SHORT        cToName;        /* Count of items in cToName */


/*
 * External definitions used between source files
 */

extern HAB         hab;
extern HMQ         hmqSpy;
extern HWND        hwndSpy;
extern HWND        hwndSpyFrame;
extern HWND        hwndSpyList;
extern HWND        hwndWindowLB;
extern HWND        hwndMessageLB;
extern HHEAP       hHeap;
extern SHORT       cxBorder;
extern SHORT       cyBorder;

extern HPOINTER    hptrArrow;
extern HPOINTER    hptrSelWin;

extern USHORT      iCurItemFocus;              /* Index to item that has the focus */
extern BOOL        fSpyActive;                 /* Any non-zero is true */
extern BOOL        fTrackingListBox;           /* Tracking windows active ? */
extern BOOL        fAllFrames;                 /* Are we processing all frames ? */
extern BOOL        fAllWindows;                /* Are we processing all windows ? */

extern HWND        hwndWinDlgDisp;             /* hwnds info in Window Dialog */



/*
 * simple structure for sorting Hwnds in dumping
 */
typedef struct _spwd {
    HWND    hwnd;
    SHORT   index;
} SPWD;

#define     MAXSPYDUMP  1000         /* Max of a thousand windows */
extern SHORT wDumpCount;             /* Count of which window is being dumped */
extern SPWD  *pspwd;

/*
 * External function definitions
 */
/*
 * Spy.c
 */
extern MRESULT EXPENTRY SpyWndProc();
extern USHORT  UConvertStringToNum(char *);
extern void    OutputString(char [], SHORT);
extern MSGI    *PmsgiFromMsg(USHORT);

/*
 * Message.c
 */
extern MRESULT EXPENTRY SpyMsgDlgProc();
extern void    UpdateHooksMsgTable(void);
extern void    EnableOrDisableMsg(BOOL);

/*
 * Options.c
 */
extern MRESULT EXPENTRY SpyOutputsDlgProc();
extern MRESULT EXPENTRY SpySaveListDlgProc();
extern MRESULT EXPENTRY AboutWndProc();

/*
 * Queue.c
 */
extern MRESULT EXPENTRY SpyQueuesDlgProc();
extern void    EnableOrDisableMsg(BOOL);
extern void    UpdateHooksMsgTable(void);

/*
 * Symbol.c
 */
extern MRESULT EXPENTRY ListNearDlgProc();

/*
 * Window.c
 */
extern MRESULT EXPENTRY SpyWindowsDlgProc();
extern void    DumpOneWindowInfo(void);
extern SHORT   DumpAllWindowsInfo(HWND, SHORT);
extern void    DumpWindowIndex(SHORT);
extern void    SelOrDeselWithMouse(BOOL);
extern HWND    HwndSelWinWithMouse(HWND, void (*)(HWND, HWND));

/*
 * SpyUtil.asm
 */
extern  BOOL FValidPointer (CHAR FAR *pVoid, SHORT cbStruct);
extern  BOOL FGuessValidPointer (CHAR FAR *pVoid, SHORT cbStruct);
