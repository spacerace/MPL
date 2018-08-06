#ifdef  TEST
#define     PROBE_BUSY(a) {\
    extern  BOOL    a;     \
    a = !a;                \
    if (a)                 \
        return ECD_BUSY;   \
}
#else
#define     PROBE_BUSY(a)
#endif

/* file menu items */

#define     IDM_NEW      100
#define     IDM_OPEN     101
#define     IDM_SAVE     102
#define     IDM_SAVEAS   103
#define     IDM_PRINT    104
#define     IDM_EXIT     105
#define     IDM_ABOUT    106
#define     IDM_NEWITEM  107

/* edit menu items */

#define     IDM_UNDO     200
#define     IDM_CUT      201
#define     IDM_COPY     202
#define     IDM_PASTE    203
#define     IDM_CLEAR    204

/* Control IDs */

#define     IDC_FILENAME  400
#define     IDC_EDIT      401
#define     IDC_FILES     402
#define     IDC_PATH      403
#define     IDC_LISTBOX   404


#define     IDM_RED       500
#define     IDM_GREEN     501
#define     IDM_BLUE      502
#define     IDM_BLACK     503
#define     IDM_YELLOW    504


#define     IDM_RECT                600
#define     IDM_ROUNDRECT           601
#define     IDM_HALLOWRECT          602
#define     IDM_HALLOWROUNDRECT     603



int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
void DeleteInstance (void);
long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);
long FAR PASCAL DocWndProc(HWND, unsigned, WORD, LONG);
long FAR PASCAL ItemWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
HANDLE FAR PASCAL OpenDlg(HWND, unsigned, WORD, LONG);
HANDLE FAR PASCAL SaveDlg(HWND, unsigned, WORD, LONG);


void SeparateFile(HWND, LPSTR, LPSTR, LPSTR);
void UpdateListBox(HWND);
void AddExt(PSTR, PSTR);
void ChangeDefExt(PSTR, PSTR);




// server related stuff.
typedef struct  _SRVR  *PSRVR;
typedef struct  _SRVR  FAR *LPSRVR;


typedef  struct _SRVR {
    ECDSERVER     ecdsrvr;
    HANDLE        hsrvr;          // handle to our server
    LHSERVER      lhsrvr;         // registration handle
    HWND          hwnd;           // corresponding server window
}SRVR;


void            DeleteServer (PSRVR);
BOOL            InitServer (HWND, HANDLE);
int FAR PASCAL SrvrOpen (LPECDSERVER, LHDOCUMENT, LPSTR, LPECDDOCUMENT FAR *);
int FAR PASCAL SrvrCreate (LPECDSERVER, LHDOCUMENT, LPSTR, LPSTR, LPECDDOCUMENT FAR *);
int FAR PASCAL SrvrCreateFromTemplate (LPECDSERVER, LHDOCUMENT, LPSTR, LPSTR, LPSTR, LPECDDOCUMENT FAR *);
int FAR PASCAL SrvrEdit (LPECDSERVER, LHDOCUMENT, LPSTR, LPSTR, LPECDDOCUMENT FAR *);
int FAR PASCAL SrvrExit (LPECDSERVER);
int FAR PASCAL SrvrRelease (LPECDSERVER);


// server related stuff.
typedef struct  _DOC  *PDOC;

typedef  struct _DOC {
    ECDDOCUMENT      ecddoc;
    HANDLE           hdoc;          // handle to our server
    LHDOCUMENT       lhdoc;         // registration handle
    HWND             hwnd;          // corresponding server window
    ATOM             aName;         // docmnet name.
    POINT            ptAnchor;      // anchor point
}DOC ;

int FAR PASCAL  DocSave (LPECDDOCUMENT);
int FAR PASCAL  DocClose (LPECDDOCUMENT);
int FAR PASCAL  DocRelease (LPECDDOCUMENT);
int FAR PASCAL  DocGetObject (LPECDDOCUMENT, LPSTR, LPECDOBJECT FAR *, LPECDCLIENT);
int FAR PASCAL  DocSetDocDimensions (LPECDDOCUMENT, LPRECT);
int FAR PASCAL  DocSetHostNames (LPECDDOCUMENT, LPSTR, LPSTR);


int    ChangeDocName (PDOC, LPSTR);
PDOC   InitDoc (HWND, LHDOCUMENT);


typedef struct _ITEM     *PITEM;
typedef struct _ITEM FAR *LPITEM;


typedef struct _ITEM  {   /*ECDOBJECT*/
    ECDOBJECT   ecdobject;
    HANDLE      hitem;
    LPECDCLIENT lpecdclient;
    HWND        hwnd;
    ATOM        aName;
    int         cmdShape;
    int         cmdColor;
    int         width;  /* Added these two fields to test the loading of */
    int         height; /* rendering (based on native data) Dll */
    int         ref;
    char        name[30];
}ITEM;

void    SetItemName (PITEM, LPSTR);
void    SetNewItemName (PITEM);
HANDLE  GetNative (PITEM);
HBITMAP GetBitmap (PITEM);
HANDLE  GetPicture (PITEM);
HANDLE  GetLink (PITEM);


int FAR PASCAL  ItemOpen (LPECDOBJECT);
int FAR PASCAL  ItemDelete (LPECDOBJECT);
int FAR PASCAL  ItemGetData (LPECDOBJECT, WORD, LPHANDLE);
int FAR PASCAL  ItemSetData (LPECDOBJECT, WORD, HANDLE);
int FAR PASCAL  ItemSetTargetDevice (LPECDOBJECT, HANDLE);
ECDCLIPFORMAT   FAR PASCAL ItemEnumFormats (LPECDOBJECT, ECDCLIPFORMAT);


void    DrawDragRect (HWND);
PDOC    CreateNewDoc (PSRVR, LONG, LPSTR);
void    CutCopyItem (HWND);
void    PaintItem (HWND);
PITEM   CreateNewItem (PDOC, int, int, BOOL);
BOOL    DestroyItem (HWND);
PDOC    CreateDocFromFile (PSRVR, LPSTR, LONG);
BOOL    SaveDocIntoFile (PDOC, LPSTR);
void    DeleteDoc (PDOC);
void    FreeVTbls (void);
void    InitVTbls (void);


void    SendDocChangeMsg (PDOC, WORD);
void    SendDocRenameMsg (PSRVR, WORD);
void    SendItemChangeMsg (PITEM, WORD);
void    SendSrvrChangeMsg (PSRVR, WORD);
BOOL    ProcessCmdLine (HWND, LPSTR);
void    ReSizeAllItems (HWND, int, int);
void    ReSizeAllDocs (HWND, int, int);
