#define SERVERONLY
#include "windows.h"
#include "ecd.h"
#include "shapes.h"
#include <string.h>


char FileName[128];
char PathName[128];
char OpenName[128];
char DefPath[128];
char DefSpec[13] = "*.shax104";
char DefExt[] = ".txt";
char str[255];

RECT    dragRect;
POINT   dragPt;
HWND    hwndMain;
BOOL    fCapture = FALSE;

HBRUSH  hbrColor [5];

char    *pcolors [5]  ={"RED",
                        "GREEN",
                        "BLUE",
                        "BLACK",
                        "YELLOW"
                       };
char    *pshapes [4] = {
                        ":RECTANGLE",
                        ":ROUNDRECT"
                        ":HOLLOWRECT",
                        ":HOLLOWROUNDRECT"
                       };


WORD        cfLink;
WORD        cfOwnerLink;
WORD        cfNative;
WORD        cShapes = 0;
BOOL        bBusy   = FALSE;


HANDLE hInst;
HANDLE hAccTable;


//////////////////////////////////////////////////////////////////////////
//
// Right thing to do is to wait till the termination came from the client side.
// This is lot more cleaner.
//
// (c) Copyright Microsoft Corp. 1990 - All Rights Reserved
//
/////////////////////////////////////////////////////////////////////////


int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE  hInstance;
HANDLE  hPrevInstance;
LPSTR   lpCmdLine;
int nCmdShow;
{
    MSG msg;



    if (!hPrevInstance)
        if (!InitApplication(hInstance))
            return (FALSE);




    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    if (!InitServer (hwndMain, hInstance))
        return FALSE;

    if (!ProcessCmdLine (hwndMain, lpCmdLine))
        return FALSE;

    while (GetMessage(&msg, NULL, NULL, NULL)) {

    /* Only translate message if it is not an accelerator message */

        if (!TranslateAccelerator(hwndMain, hAccTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg); 
        }
    }

    DeleteInstance ();
    return (msg.wParam);
}



BOOL InitApplication(hInstance)
HANDLE hInstance;
{
    WNDCLASS  wc;

    wc.style = NULL;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 4;
    wc.cbWndExtra = 4;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH); 
    wc.lpszMenuName =  "InitMenu";

    wc.lpszClassName = "SampleClass";
    wc.lpfnWndProc = MainWndProc;

    if (!RegisterClass(&wc))
        return NULL;


    wc.hIcon   = NULL;
    wc.hCursor = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "DocClass";
    wc.lpfnWndProc   = DocWndProc;

    if (!RegisterClass(&wc))
        return NULL;



    wc.hIcon   = NULL;
    wc.hCursor = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "ItemClass";
    wc.lpfnWndProc   = ItemWndProc;

    if (!RegisterClass(&wc))
        return NULL;

}



BOOL InitInstance(hInstance, nCmdShow)
HANDLE          hInstance;
int             nCmdShow;
{

    // initialse the proc tables.
    InitVTbls ();

    hbrColor [0] = CreateSolidBrush (0x000000ff);
    hbrColor [1] = CreateSolidBrush (0x0000ff00);
    hbrColor [2] = CreateSolidBrush (0x00ff0000);
    hbrColor [3] = CreateSolidBrush (0x00000000);
    hbrColor [4] = CreateSolidBrush (0x00ff00ff);

    // register clipboard formats.
    cfLink      = RegisterClipboardFormat("ObjectLink");
    cfOwnerLink = RegisterClipboardFormat("OwnerLink");
    cfNative    = RegisterClipboardFormat("Native");


    hInst = hInstance;
    hAccTable = LoadAccelerators(hInst, "SampleAcc");

    hwndMain = CreateWindow(
        "SampleClass",
        "Sample miServer Application",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );


    if (!hwndMain)
        return (FALSE);

    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);
    return (TRUE);

}

void    DeleteInstance ()
{

    // Free the proc table instances.
    FreeVTbls ();

    DeleteObject (hbrColor [0]);
    DeleteObject (hbrColor [1]);
    DeleteObject (hbrColor [2]);
    DeleteObject (hbrColor [3]);
    DeleteObject (hbrColor [4]);

}

// main window or server related routiens

long FAR PASCAL MainWndProc(hwnd, message, wParam, lParam)
HWND hwnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout, lpOpenDlg, lpSaveDlg;

    int     Return;
    PITEM   pitem;
    PDOC    pdoc;



    switch (message) {
        case WM_COMMAND:
            switch (wParam) {


                case IDM_NEWITEM:
                    pdoc = (PDOC)GetWindowLong (GetWindow (hwnd, GW_CHILD), 0);
                    pitem = CreateNewItem  (pdoc, IDM_RECT, IDM_RED, TRUE);
                    SetNewItemName (pitem);
                    BringWindowToTop (pitem->hwnd);
                    break;

                case IDM_RED:
                case IDM_GREEN:
                case IDM_BLUE:
                case IDM_BLACK:
                case IDM_YELLOW:

                    pitem = (PITEM)GetWindowLong (
                            GetWindow (GetWindow(hwnd, GW_CHILD), GW_CHILD), 0);
                    pitem->cmdColor = wParam;
                    InvalidateRect (pitem->hwnd, (LPRECT)NULL,  TRUE);
                    // make sure that we have the right bits on the screen
                    UpdateWindow (pitem->hwnd);
                    SendItemChangeMsg (pitem, ECD_CHANGED);
                    break;


                case IDM_RECT:
                case IDM_ROUNDRECT:
                case IDM_HALLOWRECT:
                case IDM_HALLOWROUNDRECT:

                    pitem = (PITEM)GetWindowLong (
                            GetWindow (GetWindow(hwnd, GW_CHILD), GW_CHILD), 0);
                    pitem->cmdShape = wParam;
                    InvalidateRect (pitem->hwnd, (LPRECT)NULL,  TRUE);
                    UpdateWindow (pitem->hwnd);
                    SendItemChangeMsg (pitem, ECD_CHANGED);
                    break;

                case IDM_NEW:

                    // create another document widnwo.
                    pdoc = CreateNewDoc ((PSRVR)GetWindowLong (hwnd, 0), NULL, (LPSTR)"New");
                    if (pdoc == NULL)
                        break;
                    pitem = CreateNewItem (pdoc, IDM_RECT, IDM_RED, TRUE);
                    SetNewItemName (pitem);
                    BringWindowToTop (pdoc->hwnd);
                    break;


                case IDM_ABOUT:
                    lpProcAbout = MakeProcInstance(About, hInst);
                    DialogBox(hInst, "AboutBox", hwnd, lpProcAbout);
                    FreeProcInstance(lpProcAbout);
                    break;

                case IDM_OPEN:
                    /* Call OpenDlg() to get the filename */

                    lpOpenDlg = MakeProcInstance((FARPROC) OpenDlg, hInst);
                    Return = DialogBox(hInst, "Open", hwnd, lpOpenDlg);
                    FreeProcInstance(lpOpenDlg);

                    if (!CreateDocFromFile ((PSRVR)GetWindowLong (hwnd, 0), (LPSTR)OpenName, NULL))
                        MessageBox (
                              hwndMain,
                              "reading from file failed",
                              "Server Sample Sample Application",
                               MB_OK);


                    break;

                case IDM_SAVE:

                    lpSaveDlg = MakeProcInstance((FARPROC) SaveDlg, hInst);
                    Return = DialogBox(hInst, "Save", hwnd, lpSaveDlg);
                    FreeProcInstance(lpSaveDlg);

                    SaveDocIntoFile (pdoc = (PDOC) (GetWindowLong (GetWindow (hwnd, GW_CHILD), 0)),
                        (LPSTR)OpenName);

                    // send message for save
                    SendSrvrChangeMsg ((PSRVR)GetWindowLong (hwnd, 0), ECD_SAVED);
                    SendDocRenameMsg ((PSRVR)GetWindowLong (hwnd, 0), ECD_RENAMED);
                    break;


                case IDM_SAVEAS:
                case IDM_PRINT:
                    MessageBox (
                          GetFocus(),
                          "Command not implemented",
                          "Server Sample Sample Application",
                          MB_ICONASTERISK | MB_OK);
                    break;  



                case IDM_EXIT:
                    SendSrvrChangeMsg ((PSRVR)GetWindowLong (hwnd, 0), ECD_CLOSED);
                    DeleteServer ((PSRVR)GetWindowLong (hwnd, 0));
                    break;
    
                /* edit menu commands */

                case IDM_UNDO:
                case IDM_CUT:
                case IDM_COPY:

                    CutCopyItem (hwnd);
                    break;

                case IDM_PASTE:
                case IDM_CLEAR:
                    MessageBox (
                          GetFocus(),
                          "Command not implemented",
                          "Server Sample  Sample Application",
                          MB_ICONASTERISK | MB_OK);
                    break;  

                case IDC_EDIT:
                    if (HIWORD (lParam) == EN_ERRSPACE) {
                        MessageBox (
                              GetFocus ()
                            , "Out of memory."
                            , "server Sample  Sample Application"
                            , MB_ICONHAND | MB_OK
                        );
                    }
                    break;

            } 
            break;

        case WM_SIZE:
            if (wParam == SIZENORMAL)
                ReSizeAllDocs (hwnd, LOWORD (lParam), HIWORD (lParam));

            break;

        case WM_CLOSE:
            SendSrvrChangeMsg ((PSRVR)GetWindowLong (hwnd, 0), ECD_CLOSED);
            DeleteServer ((PSRVR)GetWindowLong (hwnd, 0));
            break;

        case WM_DESTROY:
            PostQuitMessage (0);
            break;

        default:
            return (DefWindowProc(hwnd, message, wParam, lParam));
    }
    return (NULL);
}





HANDLE FAR PASCAL SaveDlg (hDlg, message, wParam, lParam)
HWND     hDlg;
unsigned message;
WORD     wParam;
LONG     lParam;
{

    switch (message) {
        case WM_COMMAND:
            switch (wParam) {

                case IDOK:
                    GetDlgItemText(hDlg, IDC_EDIT, OpenName, 128);

                    AddExt(OpenName, DefExt);
                    EndDialog(hDlg, TRUE);
                    return (TRUE);

                case IDCANCEL:
                    EndDialog(hDlg, NULL);
                    return (FALSE);
            }
            break;

        case WM_INITDIALOG:                        /* message: initialize    */
            SetDlgItemText(hDlg, IDC_EDIT, (LPSTR)"shape1.sha");
            SendDlgItemMessage(hDlg,               /* dialog handle      */
                IDC_EDIT,                          /* where to send message  */
                EM_SETSEL,                         /* select characters      */
                NULL,                              /* additional information */
                MAKELONG(0, 0x7fff));              /* entire contents      */
            SetFocus(GetDlgItem(hDlg, IDC_EDIT));
            return (FALSE); /* Indicates the focus is set to a control */
    }
    return FALSE;
}




HANDLE FAR PASCAL OpenDlg(hDlg, message, wParam, lParam)
HWND     hDlg;
unsigned message;
WORD     wParam;
LONG     lParam;
{
    HANDLE hFile=1;     /* Temp value for return */

    switch (message) {
        case WM_COMMAND:
            switch (wParam) {

                case IDC_LISTBOX:
                    switch (HIWORD(lParam)) {

                        case LBN_SELCHANGE:
                            /* If item is a directory name, append "*.*" */
                            if (DlgDirSelect(hDlg, str, IDC_LISTBOX)) 
                                strcat(str, DefSpec);

                            SetDlgItemText(hDlg, IDC_EDIT, str);
                            SendDlgItemMessage(hDlg,
                                IDC_EDIT,
                                EM_SETSEL,
                                NULL,
                                MAKELONG(0, 0x7fff));
                            break;

                        case LBN_DBLCLK:
                            goto openfile;
                    }
                    return (TRUE);

                case IDOK:
openfile:
                    GetDlgItemText(hDlg, IDC_EDIT, OpenName, 128);
                    if (strchr(OpenName, '*') || strchr(OpenName, '?')) {
                        SeparateFile(hDlg, (LPSTR) str, (LPSTR) DefSpec,
                            (LPSTR) OpenName);
                        if (str[0])
                            strcpy(DefPath, str);
                        ChangeDefExt(DefExt, DefSpec);
                        UpdateListBox(hDlg);
                        return (TRUE);
                    }

                    if (!OpenName[0]) {
                        MessageBox(hDlg, "No filename specified.",
                            NULL, MB_OK | MB_ICONHAND);
                        return (TRUE);
                    }

                    AddExt(OpenName, DefExt);

                    /* The routine to open the file would go here, and the */
                    /* file handle would be returned instead of NULL.           */
                    EndDialog(hDlg, hFile);
                    return (TRUE);

                case IDCANCEL:
                    EndDialog(hDlg, NULL);
                    return (FALSE);
            }
            break;

        case WM_INITDIALOG:                        /* message: initialize    */
            UpdateListBox(hDlg);
            SetDlgItemText(hDlg, IDC_EDIT, DefSpec);
            SendDlgItemMessage(hDlg,               /* dialog handle      */
                IDC_EDIT,                          /* where to send message  */
                EM_SETSEL,                         /* select characters      */
                NULL,                              /* additional information */
                MAKELONG(0, 0x7fff));              /* entire contents      */
            SetFocus(GetDlgItem(hDlg, IDC_EDIT));
            return (FALSE); /* Indicates the focus is set to a control */
    }
    return FALSE;
}



void UpdateListBox(hDlg)
HWND hDlg;
{
    strcpy(str, DefPath);
    strcat(str, DefSpec);
    DlgDirList(hDlg, str, IDC_LISTBOX, IDC_PATH, 0x4010);

    /* To ensure that the listing is made for a subdir. of
     * current drive dir...
     */
    if (!strchr (DefPath, ':'))
	DlgDirList(hDlg, DefSpec, IDC_LISTBOX, IDC_PATH, 0x4010);

    /* Remove the '..' character from path if it exists, since this
     * will make DlgDirList move us up an additional level in the tree
     * when UpdateListBox() is called again.
     */
    if (strstr (DefPath, ".."))
	DefPath[0] = '\0';

    SetDlgItemText(hDlg, IDC_EDIT, DefSpec);
}


void ChangeDefExt(Ext, Name)
PSTR Ext, Name;
{
    PSTR pTptr;

    pTptr = Name;
    while (*pTptr && *pTptr != '.')
        pTptr++;
    if (*pTptr)
        if (!strchr(pTptr, '*') && !strchr(pTptr, '?'))
            strcpy(Ext, pTptr);
}


void SeparateFile(hDlg, lpDestPath, lpDestFileName, lpSrcFileName)
HWND hDlg;
LPSTR lpDestPath, lpDestFileName, lpSrcFileName;
{
    LPSTR lpTmp;
    char  cTmp;

    lpTmp = lpSrcFileName + (long) lstrlen(lpSrcFileName);
    while (*lpTmp != ':' && *lpTmp != '\\' && lpTmp > lpSrcFileName)
        lpTmp = AnsiPrev(lpSrcFileName, lpTmp);
    if (*lpTmp != ':' && *lpTmp != '\\') {
        lstrcpy(lpDestFileName, lpSrcFileName);
        lpDestPath[0] = 0;
        return;
    }
    lstrcpy(lpDestFileName, lpTmp + 1);
    cTmp = *(lpTmp + 1);
    lstrcpy(lpDestPath, lpSrcFileName);
     *(lpTmp + 1) = cTmp;
    lpDestPath[(lpTmp - lpSrcFileName) + 1] = 0;
}



void AddExt(Name, Ext)
PSTR Name, Ext;
{
    PSTR pTptr;

    pTptr = Name;
    while (*pTptr && *pTptr != '.')
        pTptr++;
    if (*pTptr != '.')
        strcat(Name, Ext);
}


BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message) {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
	    if (wParam == IDOK
                || wParam == IDCANCEL) {
                EndDialog(hDlg, TRUE);
                return (TRUE);
            }
            break;
    }
    return (FALSE);
}


void   DrawDragRect (hwnd)
HWND    hwnd;
{

        HDC hdc;

        hdc = GetDC(hwnd);
        InvertRect (hdc, (LPRECT)&dragRect);
        ReleaseDC (hwnd, hdc);

}

void    ReSizeAllDocs (hwndSrvr, width, height)
HWND    hwndSrvr;
int     width;
int     height;
{
    HWND    hwnd;
    RECT    rc;


    hwnd = GetWindow (hwndSrvr, GW_CHILD);
    while (hwnd){
        GetWindowRect (hwnd, (LPRECT)&rc);
        ScreenToClient (hwndSrvr, (LPPOINT)&rc);
        MoveWindow (hwnd, rc.left, rc.top,
            width , height, TRUE);

        hwnd = GetWindow (hwnd, GW_HWNDNEXT);
    }

}
void    ReSizeAllItems (hwndDoc, width, height)
HWND    hwndDoc;
int     width;
int     height;
{
    HWND    hwnd;
    RECT    rc;


    hwnd = GetWindow (hwndDoc, GW_CHILD);
    while (hwnd){
        GetWindowRect (hwnd, (LPRECT)&rc);
        ScreenToClient (hwndDoc, (LPPOINT)&rc);
        MoveWindow (hwnd, rc.left, rc.top,
            width >> 1, height >> 1, TRUE);

        hwnd = GetWindow (hwnd, GW_HWNDNEXT);
    }

}

long FAR PASCAL ItemWndProc (hwnd, message, wParam, lParam)
HWND hwnd;
unsigned message;
WORD wParam;
LONG lParam;
{
        POINT   pt;
        HWND    hwndParent;


    switch (message) {
        case WM_PAINT:
            PaintItem (hwnd);
            break;


        case WM_SIZE:
            break;

        case WM_DESTROY:
            DestroyItem (hwnd);
            break;

        case WM_RBUTTONDOWN:


            hwndParent = GetParent (hwnd);
            GetWindowRect (hwnd, (LPRECT) &dragRect);
            ScreenToClient (hwndParent, (LPPOINT)&dragRect);
            ScreenToClient (hwndParent, (LPPOINT)&dragRect.right);

            DrawDragRect (hwndParent);

            dragPt.x = LOWORD(lParam);
            dragPt.y = HIWORD(lParam);

            ClientToScreen (hwnd, (LPPOINT)&dragPt);
            ScreenToClient (hwndParent, (LPPOINT)&dragPt);

            SetCapture (hwnd);
            fCapture = TRUE;
            break;

        case WM_MOUSEMOVE:
            if (!fCapture)
                break;

            hwndParent = GetParent (hwnd);
            DrawDragRect (hwndParent);
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);

            ClientToScreen (hwnd, (LPPOINT)&pt);
            ScreenToClient (hwndParent, (LPPOINT)&pt);

            OffsetRect ((LPRECT)&dragRect, pt.x - dragPt.x,
                                        pt.y - dragPt.y);

            dragPt.x = pt.x;
            dragPt.y = pt.y;


            DrawDragRect (hwndParent);
            break;


        case WM_RBUTTONUP:
            if (!fCapture)
                return TRUE;

            hwndParent = GetParent (hwnd);
            fCapture = FALSE;
            ReleaseCapture ();
            DrawDragRect (hwndParent);

            MoveWindow (hwnd, dragRect.left, dragRect.top,
                        dragRect.right - dragRect.left,
                        dragRect.bottom - dragRect.top, TRUE);

             // fall thru

        case WM_LBUTTONDOWN:
             BringWindowToTop (hwnd);
             break;

        default:
            return (DefWindowProc(hwnd, message, wParam, lParam));
    }
    return (NULL);
}


BOOL    SaveDocIntoFile (pdoc, lpstr)
PDOC    pdoc;
LPSTR   lpstr;
{


    HWND    hwnd;
    int     fh;
    PITEM   pitem;


    if ((fh =_lcreat (lpstr, 0)) <= 0)
        return FALSE;

    hwnd = GetWindow (pdoc->hwnd, GW_CHILD);

    while (hwnd) {
        pitem = (PITEM) GetWindowLong (hwnd, 0);
        if (!pitem->aName)
            SetNewItemName (pitem);

        if (_lwrite (fh, (LPSTR)pitem, sizeof (ITEM)) <= 0)
            break;

        hwnd = GetWindow (hwnd, GW_HWNDNEXT);
    }
    _lclose (fh);

    SetWindowText (pdoc->hwnd, (LPSTR)lpstr);
    return TRUE;
}


PDOC    CreateDocFromFile (psrvr, lpstr, lhdoc)
PSRVR       psrvr;
LPSTR       lpstr;
LHDOCUMENT  lhdoc;
{

    int     fh;
    ITEM    item;
    PITEM   pitem;
    PDOC    pdoc;


    if ((fh =_lopen (lpstr, OF_READ)) <= 0)
        return FALSE;

    if (!(pdoc = CreateNewDoc (psrvr, lhdoc, lpstr)))
        return FALSE;

    cShapes = 0;
    while (TRUE) {
        if (_lread (fh, (LPSTR)&item, sizeof (ITEM)) <= 0)
            break;

        pitem = CreateNewItem (pdoc, item.cmdShape, item.cmdColor, TRUE);
        SetItemName (pitem, (LPSTR)item.name);
        cShapes++;
    }
    _lclose (fh);


    return pdoc;
}

// doc related routines

PDOC    CreateNewDoc (psrvr, lhdoc, lptitle)
PSRVR   psrvr;
LONG    lhdoc;
LPSTR   lptitle;
{

    RECT        rc;
    HMENU       hMenu;
    HWND        hwnd;

    hwnd = psrvr->hwnd;

    // first set the menu.
    hMenu = GetMenu (hwnd);
    DestroyMenu (hMenu);

    hMenu = LoadMenu (hInst, (LPSTR)"SampleMenu");
    SetMenu (hwnd, hMenu);
    DrawMenuBar (hwnd);

    GetClientRect (hwnd, (LPRECT)&rc);
    if (IsIconic (psrvr->hwnd)){
        // create some reasonale height/wdith for satisfying the
        // data request in iconic mode
        rc.right = GetSystemMetrics (SM_CXSCREEN) >> 1;
        rc.bottom   = GetSystemMetrics (SM_CYSCREEN) >> 1;
    }
    InflateRect ((LPRECT)&rc, -5, -5);

    hwnd = CreateWindow(
        "DocClass",
        "New",
        WS_SYSMENU | WS_CAPTION | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,

        rc.left,
        rc.top,
        (rc.right - rc.left),
        (rc.bottom - rc.top),
        psrvr->hwnd,
        NULL,
        hInst,
        NULL
    );

    SetWindowText (hwnd, lptitle);
    return InitDoc (hwnd, lhdoc);
}

long FAR PASCAL DocWndProc(hwnd, message, wParam, lParam)
HWND hwnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    HWND    hwndChild;
    HWND    hwndParent;
    HMENU   hMenu;

    switch (message) {

        case WM_SIZE:
            if (wParam == SIZENORMAL)
                ReSizeAllItems (hwnd, LOWORD (lParam), HIWORD (lParam));
            break;

        case WM_CLOSE:
            SendDocChangeMsg ((PDOC)GetWindowLong (hwnd, 0), ECD_CLOSED);
            DeleteDoc ((PDOC)GetWindowLong (hwnd, 0));
            break;

        case WM_DESTROY:

            hwndParent = GetParent (hwnd);
            hwndChild = GetWindow(hwndParent, GW_CHILD);

            if (GetWindow (hwndChild, GW_HWNDNEXT))
                break;          // some more doc windows.

            // first set the menu.
            hMenu  = GetMenu (hwndMain);
            DestroyMenu (hMenu);

            // we do not ahve any more doc windows.
            hMenu = LoadMenu (hInst, (LPSTR)"InitMenu");
            SetMenu (hwndMain, hMenu);
            DrawMenuBar (hwndMain);
            break;



        case WM_LBUTTONDOWN:
            BringWindowToTop (hwnd);
            break;

        default:
            return (DefWindowProc(hwnd, message, wParam, lParam));
    }
    return (NULL);
}
