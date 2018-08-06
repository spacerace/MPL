#define SERVERONLY
#include "windows.h"
#include "ecd.h"
#include "shapes.h"

//////////////////////////////////////////////////////////////////////////
//
// (c) Copyright Microsoft Corp. 1990 - All Rights Reserved
//
/////////////////////////////////////////////////////////////////////////


// We alis the null item with the very first item in the file.
// Since we never copy more than one item to clip board, there
// is no problem for the objects being edited. If they are created
// from template, we give the first child window back.


extern      HANDLE  hInst;

extern      char    *pcolors[];
extern      char    *pshapes[];
extern      HBRUSH  hbrColor [5];

extern      WORD    cfLink;
extern      WORD    cfOwnerLink;
extern      WORD    cfNative;

extern      WORD    cShapes;

ECDDOCUMENTVTBL     docVTbl;
ECDOBJECTVTBL       itemVTbl;
ECDSERVERVTBL       srvrVTbl;


void FreeVTbls ()
{

    FreeProcInstance(srvrVTbl.Open);
    FreeProcInstance(srvrVTbl.Create);
    FreeProcInstance(srvrVTbl.CreateFromTemplate);
    FreeProcInstance(srvrVTbl.Edit);
    FreeProcInstance(srvrVTbl.Exit);
    FreeProcInstance(srvrVTbl.Release);

    FreeProcInstance (docVTbl.Save);
    FreeProcInstance (docVTbl.Close);
    FreeProcInstance (docVTbl.GetObject);
    FreeProcInstance (docVTbl.Release);

    FreeProcInstance (docVTbl.SetHostNames);
    FreeProcInstance (docVTbl.SetDocDimensions);


    FreeProcInstance (itemVTbl.Show);
    FreeProcInstance (itemVTbl.GetData);
    FreeProcInstance (itemVTbl.SetData);
    FreeProcInstance (itemVTbl.Delete);
    FreeProcInstance (itemVTbl.SetTargetDevice);
    FreeProcInstance (itemVTbl.EnumFormats);

}

void InitVTbls ()
{

    // srvr vtable.

    srvrVTbl.Open    = MakeProcInstance(SrvrOpen, hInst);
    srvrVTbl.Create  = MakeProcInstance(SrvrCreate, hInst);
    srvrVTbl.CreateFromTemplate = MakeProcInstance(SrvrCreateFromTemplate, hInst);
    srvrVTbl.Edit    = MakeProcInstance(SrvrEdit, hInst);
    srvrVTbl.Exit    = MakeProcInstance(SrvrExit, hInst);
    srvrVTbl.Release = MakeProcInstance(SrvrRelease, hInst);

    // doc table
    docVTbl.Save       = MakeProcInstance(DocSave, hInst);
    docVTbl.Close      = MakeProcInstance(DocClose, hInst);
    docVTbl.GetObject  = MakeProcInstance(DocGetObject, hInst);
    docVTbl.Release    = MakeProcInstance(DocRelease, hInst);

    docVTbl.SetHostNames        = MakeProcInstance(DocSetHostNames, hInst);
    docVTbl.SetDocDimensions    = MakeProcInstance(DocSetDocDimensions, hInst);

    // item table.

    itemVTbl.Show      = MakeProcInstance (ItemOpen, hInst);
    itemVTbl.GetData   = MakeProcInstance (ItemGetData, hInst);
    itemVTbl.SetData   = MakeProcInstance (ItemSetData, hInst);
    itemVTbl.Delete    = MakeProcInstance (ItemDelete, hInst);

    itemVTbl.SetTargetDevice =  MakeProcInstance (ItemSetTargetDevice, hInst);
    itemVTbl.EnumFormats =  MakeProcInstance (ItemEnumFormats, hInst);

}

BOOL ProcessCmdLine (hwnd, lpcmdline)
LPSTR   lpcmdline;
HWND    hwnd;
{

    // Look for any file name on the command line.
    char    buf[100];
    int      i;

    while (*lpcmdline){
        // skip blanks
        while ( *lpcmdline && *lpcmdline == ' ')
            lpcmdline++;

        if (*lpcmdline == '-' || *lpcmdline == '/'){
            // skip the options.
            while ( *lpcmdline && *lpcmdline != ' ')
                lpcmdline++;


        } else {
            // looks like we found the file name. terminate with NULL and
            // open the document.

            // !!! check for the buffer limits.

            i = 0;
            while ( *lpcmdline && *lpcmdline != ' ')
                buf[i++] =*lpcmdline++;

            buf[i] = 0;

           // now open the document.
           if(CreateDocFromFile ((PSRVR)GetWindowLong (hwnd, 0),
                (LPSTR)buf, NULL))
                return TRUE;
            else
                return FALSE;
        }
    }
    return TRUE;


}

BOOL InitServer (hwnd, hInst)
HWND    hwnd;
HANDLE  hInst;
{


    HANDLE      hsrvr = NULL;
    PSRVR       psrvr = NULL;
    int         retval;

    hsrvr = LocalAlloc (LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof (SRVR));

    if (hsrvr == NULL || (psrvr = (PSRVR) LocalLock (hsrvr)) == NULL)
        goto errRtn;

    psrvr->hsrvr = hsrvr;

    psrvr->ecdsrvr.lpvtbl = &srvrVTbl;

    retval = EcdRegisterServer ((LPSTR)"miShapes", (LPECDSERVER)psrvr,
                (LONG FAR *) &psrvr->lhsrvr, hInst, ECD_SRVR_MULTI);


    if (retval != ECD_OK)
        goto errRtn;

    psrvr->hwnd = hwnd;        // corresponding main window
    SetWindowLong (hwnd, 0, (LONG)(LPSRVR)psrvr);
    return TRUE;

errRtn:
    if (psrvr)
        LocalUnlock (hsrvr);

    if (hsrvr)
        LocalFree (hsrvr);

    return FALSE;

}

int     FAR     PASCAL SrvrRelease (lpecdsrvr)
LPECDSERVER   lpecdsrvr;
{

    PSRVR   psrvr;
    HANDLE  hsrvr;
    HWND    hwnd;


    psrvr = (PSRVR)((LPSRVR)lpecdsrvr);

    if (psrvr->lhsrvr)
        DeleteServer (psrvr);

    else {
        // This delete server should release the server immediately
        hwnd = psrvr->hwnd;
        LocalUnlock (hsrvr = psrvr->hsrvr);
        LocalFree (hsrvr);
        DestroyWindow(hwnd);
    }
    return TRUE;    // return something
}

void DeleteServer (psrvr)
PSRVR   psrvr;
{
     LHSERVER lhsrvr;


#ifdef TEST
     EcdRevokeServer ((LHSERVER)psrvr);
#else

     lhsrvr = psrvr->lhsrvr;
     psrvr->lhsrvr = NULL;
     if (lhsrvr){
        ShowWindow (psrvr->hwnd, FALSE);
        EcdRevokeServer (lhsrvr);
     }
#endif
}

int     FAR     PASCAL SrvrOpen (lpecdsrvr, lhdoc, lpdocname, lplpecddoc)
LHDOCUMENT     lhdoc;
LPECDSERVER    lpecdsrvr;
LPSTR          lpdocname;
LPECDDOCUMENT FAR *lplpecddoc;
{
    PDOC    pdoc;


    PROBE_BUSY(bBusy);

    // errors are not taken care properly
    if(!(pdoc = CreateDocFromFile ((PSRVR)((LPSRVR)lpecdsrvr), lpdocname, lhdoc)))
        return ECD_ERROR_MEMORY;

    *lplpecddoc = (LPECDDOCUMENT)pdoc;
    return ECD_OK;

}


int     FAR     PASCAL SrvrCreate (lpecdsrvr, lhdoc, lpclassname, lpdocname, lplpecddoc)
LHDOCUMENT         lhdoc;
LPECDSERVER   lpecdsrvr;
LPSTR         lpclassname;
LPSTR         lpdocname;
LPECDDOCUMENT  FAR *lplpecddoc;
{

   PDOC pdoc;
   // errors are not taken care properly

   PROBE_BUSY(bBusy);

   if (!(pdoc = CreateNewDoc ((PSRVR)lpecdsrvr, lhdoc, lpdocname)))
        return ECD_ERROR_MEMORY;

   CreateNewItem (pdoc, IDM_RECT, IDM_RED, TRUE);
   *lplpecddoc = (LPECDDOCUMENT)pdoc;

   return ECD_OK;

}

int     FAR     PASCAL SrvrCreateFromTemplate (lpecdsrvr, lhdoc, lpclassname, lpdocname, lptemplatename, lplpecddoc)
LHDOCUMENT    lhdoc;
LPECDSERVER   lpecdsrvr;
LPSTR         lpclassname;
LPSTR         lpdocname;
LPSTR         lptemplatename;
LPECDDOCUMENT  FAR *lplpecddoc;
{

    PDOC    pdoc;

    PROBE_BUSY(bBusy);

    if(!(pdoc = CreateDocFromFile ((PSRVR)lpecdsrvr, lptemplatename, lhdoc)))
        return ECD_ERROR_MEMORY;

    ChangeDocName (pdoc, (LPSTR)lptemplatename);
    *lplpecddoc = (LPECDDOCUMENT)pdoc;
    return ECD_OK;

}

int     FAR     PASCAL SrvrEdit (lpecdsrvr, lhdoc, lpclassname, lpdocname, lplpecddoc)
LHDOCUMENT    lhdoc;
LPECDSERVER   lpecdsrvr;
LPSTR         lpclassname;
LPSTR         lpdocname;
LPECDDOCUMENT  FAR *lplpecddoc;
{

   PDOC pdoc;

   PROBE_BUSY(bBusy);

   // errors are not taken care properly

   if (!(pdoc = CreateNewDoc ((PSRVR)lpecdsrvr, lhdoc, lpdocname)))
        return ECD_ERROR_MEMORY;

   *lplpecddoc = (LPECDDOCUMENT)pdoc;

   return ECD_OK;


}

int     FAR     PASCAL SrvrExit (lpecdsrvr)
LPECDSERVER   lpecdsrvr;
{

    // Server lib is calling us to exit.
    // Let us hide the main window.
    // But let us not delete the window.
    PSRVR   psrvr;

    PROBE_BUSY(bBusy);

    psrvr = (PSRVR)lpecdsrvr;

    ShowWindow (psrvr->hwnd, FALSE);
    EcdRevokeServer (psrvr->lhsrvr);
    return ECD_OK;



}

// doucment functions.

PDOC    InitDoc (hwnd, lhdoc)
HWND        hwnd;
LHDOCUMENT  lhdoc;
{
    HANDLE      hdoc = NULL;
    PDOC        pdoc = NULL;
    char        buf[128];
    PSRVR       psrvr;



    hdoc = LocalAlloc (LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof (DOC));

    if (hdoc == NULL || (pdoc = (PDOC) LocalLock (hdoc)) == NULL)
        goto errRtn;

    pdoc->hdoc = hdoc;
    pdoc->hwnd = hwnd;        // corresponding main window

    psrvr = (PSRVR)GetWindowLong (GetParent (hwnd), 0);
    GetWindowText (hwnd, (LPSTR)buf, 128);
    if (lhdoc == NULL){
        if (EcdRegisterDocument (psrvr->lhsrvr, (LPSTR)buf, (LPECDDOCUMENT)pdoc,
            (LHDOCUMENT FAR *)&pdoc->lhdoc) != ECD_OK)
            goto errRtn;
    } else
        pdoc->lhdoc = lhdoc;

    pdoc->aName = GlobalAddAtom ((LPSTR)buf);
    SetWindowLong (hwnd, 0, (LONG)pdoc);

    pdoc->ecddoc.lpvtbl = &docVTbl;

    // set the anchor point for children
    pdoc->ptAnchor.x = 0;
    pdoc->ptAnchor.y = 0;

    return pdoc;

errRtn:

    if (pdoc)
        LocalUnlock (hdoc);

    if (hdoc)
        LocalFree;

    return (PDOC)NULL;

}


int FAR PASCAL  DocSave (lpecddoc)
LPECDDOCUMENT    lpecddoc;
{

    PROBE_BUSY(bBusy);
    // not yet implemented
    return ECD_OK;


}

void DeleteDoc (pdoc)
PDOC    pdoc;
{
    LHDOCUMENT   lhdoc;

#ifdef TEST
    EcdRevokeDocument ((LHDOCUMENT)pdoc);

#else
    lhdoc = pdoc->lhdoc;
    pdoc->lhdoc = NULL;
    if (lhdoc) {
        ShowWindow (pdoc->hwnd, FALSE);
        EcdRevokeDocument (lhdoc);
    }
#endif

}


int FAR PASCAL  DocClose (lpecddoc)
LPECDDOCUMENT    lpecddoc;
{
    PDOC    pdoc;

    PROBE_BUSY(bBusy);
    pdoc = (PDOC)lpecddoc;

    SendDocChangeMsg (pdoc, ECD_CLOSED);
    DeleteDoc (pdoc);
    return ECD_OK;

}

int FAR PASCAL DocSetHostNames(lpecddoc, lpclientName, lpdocName)
LPECDDOCUMENT    lpecddoc;
LPSTR            lpclientName;
LPSTR            lpdocName;
{


    PDOC    pdoc;
    char    buf[200];
    int     len;
    HWND    hwnd;

    PROBE_BUSY(bBusy);

    // Do something to show that the titiel are chaning
    pdoc = (PDOC)lpecddoc;
    hwnd = pdoc->hwnd;

    SetWindowText (hwnd, lpdocName);;

    hwnd = GetParent (hwnd);
    lstrcpy ((LPSTR)buf, lpclientName);
    lstrcat ((LPSTR)buf, (LPSTR)":");
    lstrcat ((LPSTR)buf, (LPSTR)"Sample mishapes server application");
    len = lstrlen ((LPSTR)buf);
    SetWindowText (hwnd, (LPSTR)buf);

    return ECD_OK;

}
int FAR PASCAL DocSetDocDimensions(lpecddoc, lprc)
LPECDDOCUMENT   lpecddoc;
LPRECT  lprc;
{
    PROBE_BUSY(bBusy);
    return ECD_OK;


}

int FAR PASCAL  DocRelease (lpecddoc)
LPECDDOCUMENT    lpecddoc;
{

    // !!! what is this supposed to do?
    // Revoke document calls DocRelease.

    PDOC    pdoc;
    HANDLE  hdoc;
    HWND    hwnd;


    PROBE_BUSY(bBusy);

    pdoc = (PDOC)lpecddoc;
    GlobalDeleteAtom (pdoc->aName);
    hwnd = pdoc->hwnd;
    LocalUnlock (hdoc = pdoc->hdoc);
    LocalFree (hdoc);
    DestroyWindow(hwnd);
    return TRUE;        // return something

}

int ChangeDocName (pdoc, lpname)
PDOC    pdoc;
LPSTR   lpname;
{

    GlobalDeleteAtom (pdoc->aName);
    pdoc->aName = GlobalAddAtom (lpname);
    SetWindowText (pdoc->hwnd, lpname);
    return TRUE;        // return something

}


int FAR PASCAL  DocGetObject (lpecddoc, lpitemname, lplpecdobject, lpecdclient)
LPECDDOCUMENT       lpecddoc;
LPSTR               lpitemname;
LPECDOBJECT FAR *   lplpecdobject;
LPECDCLIENT         lpecdclient;
{
    PDOC    pdoc;
    HWND    hwnd;
    ATOM    aName;
    PITEM   pitem;


    PROBE_BUSY(bBusy);

    pdoc  = (PDOC)lpecddoc;
    aName = GlobalFindAtom (lpitemname);
    hwnd = GetWindow ((hwnd = pdoc->hwnd), GW_CHILD);

    // go thru all the child window list and find the window which is
    // matching the given item name.

    while (hwnd){

       // !!! We are trying to match the doc item with any item
       // OK only if there is only one item in a doc. Otherwise
       // we will be in troubles. Should work without any problems.
       // for embedded objects.

        pitem = (PITEM)GetWindowLong (hwnd, 0);

        // Alias the null with the very first item
        if (pitem->aName == aName || pitem->aName == NULL || aName == NULL)
            goto rtn;

        hwnd = GetWindow (hwnd, GW_HWNDNEXT);
    }
    // we did not find the item we wanted. So, create one

    // create a window with empty native data.

    if (CreateNewItem (pdoc, IDM_RECT, IDM_RED, FALSE)) {
        // newly created window is always the first child
        pitem = (PITEM)GetWindowLong (GetWindow (pdoc->hwnd, GW_CHILD), 0);
        BringWindowToTop (pitem->hwnd);
    } else
        return ECD_ERROR_MEMORY;
rtn:
    pitem->aName = GlobalAddAtom (lpitemname);
    // If the item is not null, then do not show the window.
    *lplpecdobject = (LPECDOBJECT)pitem;
    pitem->lpecdclient = lpecdclient;
    return ECD_OK;

}


// item related routines

void SetNewItemName (pitem)
PITEM   pitem;
{
    char    buf[3];

    lstrcpy ((LPSTR)pitem->name, (LPSTR)"Shape #");


    if (cShapes++ > 100)
        cShapes = 1;

    buf[0] = (char)((cShapes / 10)  + (int)'0');
    buf[1] = (char)((cShapes % 10)  + (int)'0');
    buf[2] = 0;

    lstrcat ((LPSTR)pitem->name, (LPSTR)buf);
    pitem->aName = GlobalAddAtom ((LPSTR)pitem->name);
}

void SetItemName (pitem, lpname)
PITEM   pitem;
LPSTR   lpname;
{
    pitem->aName = GlobalAddAtom (lpname);
    lstrcpy ((LPSTR)pitem->name, lpname);
}

void CutCopyItem (hwnd)
HWND    hwnd;
{
    PITEM       pitem;

    if (OpenClipboard (hwnd)){

        EmptyClipboard ();

        // firt set the clipboard

        pitem = (PITEM) GetWindowLong (GetWindow(
                GetWindow (hwnd, GW_CHILD), GW_CHILD), 0);

        // Check for null handles from the render routines.
        SetClipboardData (CF_METAFILEPICT, GetPicture (pitem));
        SetClipboardData (CF_BITMAP, GetBitmap (pitem));
        SetClipboardData (cfNative, GetNative (pitem));
        SetClipboardData (cfLink, GetLink (pitem));
        SetClipboardData (cfOwnerLink, GetLink (pitem));
        CloseClipboard ();
    }


}

HANDLE  GetNative (pitem)
PITEM   pitem;
{


    LPSTR       lpsrc;
    LPSTR       lplink = NULL;
    HANDLE      hlink = NULL;
    int         i;



    hlink = GlobalAlloc (GMEM_DDESHARE | GMEM_ZEROINIT, sizeof (ITEM));
    if (hlink== NULL || (lplink = (LPSTR)GlobalLock (hlink)) == NULL)
        goto errRtn;

    lpsrc = (LPSTR)pitem;
    for (i = 0; i <  sizeof(ITEM); i++)
        *lplink++ = *lpsrc++;

    GlobalUnlock (hlink);
    return hlink;

errRtn:
    if (lplink)
        GlobalUnlock (hlink);

    if (hlink)
        GlobalFree (hlink);

}

HANDLE  GetLink (pitem)
PITEM   pitem;
{

    char        buf[128];
    LPSTR       lpsrc;
    LPSTR       lplink = NULL;
    HANDLE      hlink = NULL;
    int         len;
    int         i;

    // make the link
    lstrcpy ((LPSTR)buf, (LPSTR)"miShapes");
    len = lstrlen ((LPSTR)buf) + 1;
    len += GetWindowText (GetParent (pitem->hwnd), (LPSTR)buf + len , 128 - len) + 1;
    len += GlobalGetAtomName (pitem->aName, (LPSTR)buf + len, 128 - len) + 1;
    buf[len++] = 0;       // throw in another null at the end.


    hlink = GlobalAlloc (GMEM_DDESHARE | GMEM_ZEROINIT, len);
    if (hlink== NULL || (lplink = (LPSTR)GlobalLock (hlink)) == NULL)
        goto errRtn;

    lpsrc = (LPSTR)buf;
    for (i = 0; i <  len; i++)
        *lplink++ = *lpsrc++;

    GlobalUnlock (hlink);

    return hlink;
errRtn:
    if (lplink)
        GlobalUnlock (hlink);

    GlobalFree (hlink);
    return NULL;
}

HANDLE     GetPicture (pitem)
PITEM       pitem;
{

    LPMETAFILEPICT  lppict = NULL;
    HANDLE          hpict = NULL;
    HANDLE          hMF = NULL;
    HANDLE          hdc;
    RECT            rc;
    HPEN            hpen;
    HPEN            holdpen;
    int             mm;


    // Now set the bitmap data.

    hdc = CreateMetaFile(NULL);

    GetClientRect (pitem->hwnd, (LPRECT)&rc);

    // paint directly into the bitmap
    SelectObject (hdc, hbrColor [ pitem->cmdColor - IDM_RED]);
    PatBlt (hdc, 0,0, rc.right, rc.bottom, WHITENESS);
    SetWindowOrg (hdc, 0, 0);
    SetWindowExt (hdc, rc.right, rc.bottom);


    hpen = CreatePen (PS_SOLID, 9, 0x00808080);
    holdpen = SelectObject (hdc, hpen);

    switch (pitem->cmdShape){
        case IDM_ROUNDRECT:
            RoundRect (hdc, rc.left, rc.top, rc.right, rc.bottom,
                    (rc.right - rc.left) >> 2, (rc.bottom - rc.top) >> 2);
            break;

        case IDM_RECT:
            Rectangle (hdc, rc.left, rc.top, rc.right, rc.bottom);
            break;

        case IDM_HALLOWRECT:
            SelectObject (hdc, GetStockObject (HOLLOW_BRUSH));
            Rectangle (hdc, rc.left, rc.top, rc.right, rc.bottom);
            break;

        case IDM_HALLOWROUNDRECT:
            SelectObject (hdc, GetStockObject (HOLLOW_BRUSH));
            RoundRect (hdc, rc.left, rc.top, rc.right, rc.bottom,
                    (rc.right - rc.left) >> 2, (rc.bottom - rc.top) >> 2);
            break;

    }

    hMF = CloseMetaFile (hdc);

    if(!(hpict = GlobalAlloc (GMEM_DDESHARE, sizeof (METAFILEPICT))))
        goto errRtn;

    if ((lppict = (LPMETAFILEPICT)GlobalLock (hpict)) == NULL)
        goto errRtn;

    lppict->mm = MM_ANISOTROPIC;
    lppict->hMF = hMF;


    hdc = GetDC (pitem->hwnd);
    mm = SetMapMode(hdc, MM_HIMETRIC);
    DPtoLP(hdc, (LPPOINT)&rc, 2);
    SetMapMode(hdc, mm);
    ReleaseDC (pitem->hwnd, hdc);

    lppict->xExt =  rc.right - rc.left;
    lppict->yExt =  rc.top - rc.bottom;
    GlobalUnlock (hpict);
    return hpict;

errRtn:
    if (lppict)
        GlobalUnlock (hpict);

    if (hpict)
        GlobalFree (hpict);

    if (hMF)
        DeleteMetaFile (hMF);
    return NULL;

}


HBITMAP     GetBitmap (pitem)
PITEM       pitem;
{

    HDC         hdc;
    HDC         hdcmem;
    RECT        rc;
    HBITMAP     hbitmap;
    HBITMAP     holdbitmap;
    HPEN        hpen;
    HPEN        holdpen;


    // Now set the bitmap data.

    hdc = GetDC (pitem->hwnd);
    hdcmem = CreateCompatibleDC (hdc);
    GetClientRect (pitem->hwnd, (LPRECT)&rc);
    hbitmap = CreateCompatibleBitmap (hdc, rc.right - rc.left, rc.bottom - rc.top);
    holdbitmap = SelectObject (hdcmem, hbitmap);

    // paimt directly into the bitmap
    SelectObject (hdcmem, hbrColor [ pitem->cmdColor - IDM_RED]);
    PatBlt (hdcmem, 0,0, rc.right, rc.bottom, WHITENESS);

    hpen = CreatePen (PS_SOLID, 9, 0x00808080);
    holdpen = SelectObject (hdcmem, hpen);

    switch (pitem->cmdShape){
        case IDM_ROUNDRECT:
            RoundRect (hdcmem, rc.left, rc.top, rc.right, rc.bottom,
                    (rc.right - rc.left) >> 2, (rc.bottom - rc.top) >> 2);
            break;

        case IDM_RECT:
            Rectangle (hdcmem, rc.left, rc.top, rc.right, rc.bottom);
            break;

        case IDM_HALLOWRECT:
            SelectObject (hdcmem, GetStockObject (HOLLOW_BRUSH));
            Rectangle (hdcmem, rc.left, rc.top, rc.right, rc.bottom);
            break;

        case IDM_HALLOWROUNDRECT:
            SelectObject (hdcmem, GetStockObject (HOLLOW_BRUSH));
            RoundRect (hdcmem, rc.left, rc.top, rc.right, rc.bottom,
                    (rc.right - rc.left) >> 2, (rc.bottom - rc.top) >> 2);
            break;

    }



    hbitmap = SelectObject (hdcmem, holdbitmap);
    hpen = SelectObject (hdcmem, holdpen);
    DeleteObject (hpen);
    DeleteDC (hdcmem);
    ReleaseDC (pitem->hwnd, hdc);
    return hbitmap;

}

PITEM   CreateNewItem (pdoc, cmdShape, cmdColor, bVisible)
PDOC    pdoc;
int     cmdShape;
int     cmdColor;
BOOL    bVisible;
{

    RECT        rc;
    HANDLE      hitem = NULL;
    PITEM       pitem = NULL;
    HWND        hwnd;



    GetClientRect (pdoc->hwnd, (LPRECT)&rc);

    hwnd = CreateWindow(
        "ItemClass",
        "Item",

        WS_DLGFRAME| WS_CHILD | WS_CLIPSIBLINGS |
                    (bVisible ? WS_VISIBLE : 0),
        pdoc->ptAnchor.x,
        pdoc->ptAnchor.y,
        (rc.right - rc.left) >> 1,
        (rc.bottom - rc.top)>> 1,
        pdoc->hwnd,
        NULL,
        hInst,
        NULL
    );


    if (!hwnd)
        return (FALSE);

    pdoc->ptAnchor.x += 20;
    pdoc->ptAnchor.y += 20;


    // Now create the item info;
    hitem = LocalAlloc (LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof (ITEM));

    if (hitem == NULL || ((pitem = (PITEM)LocalLock (hitem)) == NULL))
        goto  errRtn;

    pitem->cmdShape = cmdShape;;
    pitem->cmdColor = cmdColor;
    pitem->hitem    = hitem;
    pitem->aName    = NULL;
    pitem->hwnd     = hwnd;
    pitem->ecdobject.lpvtbl = &itemVTbl;
    
    pitem->width    = (rc.right - rc.left) >> 1;
    pitem->height   = (rc.bottom - rc.top) >> 1;

    SetWindowLong (hwnd, 0, (LONG)pitem);
    return pitem;

 errRtn:
    if (pitem)
        LocalUnlock (hitem);

    if (hitem)
        LocalFree (hitem);

    return (PITEM)NULL;

}

int FAR PASCAL  ItemOpen (lpecdobject)
LPECDOBJECT     lpecdobject;
{

    // !!! This routine should activate the app
    // and prepare it ready for the editing. Activation
    // should bring on to the top and restore


    PITEM   pitem;
    HWND    hwnd;

    PROBE_BUSY(bBusy);
    pitem = (PITEM)lpecdobject;
    BringWindowToTop (pitem->hwnd);
    BringWindowToTop (GetParent (pitem->hwnd));
    SetActiveWindow ((hwnd = GetParent (GetParent (pitem->hwnd))));
    SendMessage (hwnd, WM_SYSCOMMAND, SC_RESTORE, 0L);
    return ECD_OK;


}


int FAR PASCAL  ItemDelete (lpecdobject)
LPECDOBJECT     lpecdobject;
{

    PITEM   pitem;

    PROBE_BUSY(bBusy);
    pitem = (PITEM)lpecdobject;
    if (--pitem->ref)
        return ECD_OK;

    if (IsWindow(pitem->hwnd))
        DestroyWindow (pitem->hwnd);
    return ECD_OK;
}


int FAR PASCAL  ItemGetData (lpecdobject, cfFormat, lphandle)
LPECDOBJECT     lpecdobject;
WORD            cfFormat;
LPHANDLE        lphandle;
{

    PITEM   pitem;

    // PROBE_BUSY(bBusy);

    // asking data for emtyp object.
    pitem = (PITEM)lpecdobject;

    if (pitem->cmdShape  == NULL||
        pitem->cmdColor == NULL)
        return ECD_ERROR_MEMORY;

    if (cfFormat == cfNative){

        if (!(*lphandle = GetNative (pitem)))
            return ECD_ERROR_MEMORY;

        return ECD_OK;
    }


    if (cfFormat == CF_BITMAP){

        if (!(*lphandle = (HANDLE)GetBitmap (pitem)))
            return ECD_ERROR_MEMORY;

        return ECD_OK;
    }

    if (cfFormat == CF_METAFILEPICT){

        if (!(*lphandle = GetPicture (pitem)))
            return ECD_ERROR_MEMORY;

        return ECD_OK;
    }

    if (cfFormat == cfLink || cfFormat == cfOwnerLink){

        if (!(*lphandle = GetLink (pitem)))
            return ECD_ERROR_MEMORY;

        return ECD_OK;
    }
    return ECD_ERROR_MEMORY;          // this is actually unknow format.

}


int FAR PASCAL   ItemSetTargetDevice (lpecdobject, hdata)
LPECDOBJECT     lpecdobject;
HANDLE          hdata;
{
    LPSTR   lpdata;

    lpdata = (LPSTR)GlobalLock (hdata);
    // Print the lpdata here.
    GlobalUnlock (hdata);
    GlobalFree (hdata);
    return ECD_OK;

}

int FAR PASCAL  ItemSetData (lpecdobject, cfFormat, hdata)
LPECDOBJECT     lpecdobject;
WORD            cfFormat;
HANDLE          hdata;
{

    LPITEM   lpitem;
    PITEM    pitem;

    PROBE_BUSY(bBusy);
    pitem = (PITEM)lpecdobject;

    if (cfFormat != cfNative)
        return ECD_ERROR_MEMORY;


    lpitem = (LPITEM)GlobalLock (hdata);
    if (lpitem){
        pitem->cmdShape = lpitem->cmdShape;
        pitem->cmdColor = lpitem->cmdColor;
        SetItemName (pitem, (LPSTR)pitem->name);
        // This is not necessary.
        ShowWindow (pitem->hwnd, TRUE);
        InvalidateRect (pitem->hwnd, (LPRECT)NULL, TRUE);
        GlobalUnlock (hdata);
    }

    GlobalFree (hdata);
    return ECD_OK;
}

ECDCLIPFORMAT   FAR PASCAL ItemEnumFormats (lpecdobject, cfFormat)
LPECDOBJECT     lpecdobject;
ECDCLIPFORMAT   cfFormat;
{
    if (cfFormat == 0)
        return cfLink;

    if (cfFormat == cfLink)
        return cfOwnerLink;

    if (cfFormat == cfOwnerLink)
        return CF_BITMAP;

    if (cfFormat == CF_BITMAP)
        return CF_METAFILEPICT;

    if (cfFormat == CF_METAFILEPICT)
        return cfNative;

    //if (cfFormat == cfNative)
    //    return NULL;

    return NULL;
}

BOOL DestroyItem (hwnd)
HWND    hwnd;
{
    PITEM   pitem;
    HANDLE  hitem;


    pitem = (PITEM)GetWindowLong (hwnd, 0);
    if (pitem->aName)
        GlobalDeleteAtom (pitem->aName);

    LocalUnlock (hitem = pitem->hitem);
    LocalFree (hitem);
    return TRUE;        // return something

}

void    PaintItem (hwnd)
HWND    hwnd;
{

    HDC             hdc;
    PITEM           pitem;
    RECT            rc;
    PAINTSTRUCT     ptSt;
    HPEN            hpen;
    HPEN            holdpen;


    BeginPaint (hwnd, (LPPAINTSTRUCT)&ptSt);
    hdc = GetDC (hwnd);

    // Do not paint anything for the whole document item
    // For document item the name is null.

    pitem = (PITEM)GetWindowLong (hwnd, 0);

    GetClientRect (hwnd, (LPRECT)&rc);
    PatBlt (hdc, 0,0, rc.right, rc.bottom, WHITENESS);
    SelectObject (hdc, hbrColor [ pitem->cmdColor - IDM_RED]);
    hpen = CreatePen (PS_SOLID, 9, 0x00808080);
    holdpen = SelectObject (hdc, hpen);

    switch (pitem->cmdShape){
        case IDM_ROUNDRECT:
            RoundRect (hdc, rc.left, rc.top, rc.right, rc.bottom,
                    (rc.right - rc.left) >> 2, (rc.bottom - rc.top) >> 2);
            break;

        case IDM_RECT:
            Rectangle (hdc, rc.left, rc.top, rc.right, rc.bottom);
            break;

        case IDM_HALLOWRECT:
            SelectObject (hdc, GetStockObject (HOLLOW_BRUSH));
            Rectangle (hdc, rc.left, rc.top, rc.right, rc.bottom);
            break;

        case IDM_HALLOWROUNDRECT:
            SelectObject (hdc, GetStockObject (HOLLOW_BRUSH));
            RoundRect (hdc, rc.left, rc.top, rc.right, rc.bottom,
                    (rc.right - rc.left) >> 2, (rc.bottom - rc.top) >> 2);
            break;

    }

    hpen = SelectObject (hdc, holdpen);
    DeleteObject (hpen);

    ReleaseDC (hwnd, hdc);
    EndPaint (hwnd, (LPPAINTSTRUCT)&ptSt);
}



void    SendSrvrChangeMsg (psrvr, options)
PSRVR   psrvr;
WORD    options;
{

    HWND    hwnd;

    // if we are releasing the document, do not do anything
    if (psrvr->lhsrvr == NULL)
        return;

    hwnd = GetWindow (psrvr->hwnd, GW_CHILD);
    while (hwnd){
        SendDocChangeMsg ((PDOC)GetWindowLong (hwnd, 0), options);
        hwnd = GetWindow (hwnd, GW_HWNDNEXT);

    }

}

void    SendDocRenameMsg (psrvr, options)
PSRVR   psrvr;
WORD    options;
{

    HWND    hwnd;
    PITEM   pitem;
    PDOC    pdoc;
    char    buf[128];

    // if we are releasing the document, do not do anything
    if (psrvr->lhsrvr == NULL)
        return;

    hwnd = GetWindow (psrvr->hwnd, GW_CHILD);
    pdoc = (PDOC)GetWindowLong (hwnd, 0);

    // if we are releasing the document, do not do anything
    if (pdoc->lhdoc == NULL)
        return;

    hwnd = GetWindow (pdoc->hwnd, GW_CHILD);

    while (hwnd){
        pitem = (PITEM)GetWindowLong (hwnd, 0);
        // Sending rename to obe item is sufficient
        if (pitem->lpecdclient){
            (*pitem->lpecdclient->lpvtbl->CallBack) (pitem->lpecdclient, options,
                    (LPECDOBJECT)pitem);

            return;
        }
        hwnd = GetWindow (hwnd, GW_HWNDNEXT);
    }

    // reregister the document if there are no callbacks.
    GetWindowText (pdoc->hwnd, (LPSTR)buf, 128);
    DeleteDoc (pdoc);
    CreateDocFromFile (psrvr, (LPSTR)buf, NULL);
    return;
}

void    SendDocChangeMsg (pdoc, options)
PDOC    pdoc;
WORD    options;
{

    HWND    hwnd;

    // if we are releasing the document, do not do anything
    if (pdoc->lhdoc == NULL)
        return;

    hwnd = GetWindow (pdoc->hwnd, GW_CHILD);
    while (hwnd){
        SendItemChangeMsg ((PITEM)GetWindowLong (hwnd, 0), options);
        hwnd = GetWindow (hwnd, GW_HWNDNEXT);

    }

}


void    SendItemChangeMsg (pitem, options)
PITEM   pitem;
WORD    options;
{

    if (pitem->lpecdclient)
        (*pitem->lpecdclient->lpvtbl->CallBack) (pitem->lpecdclient, options,
                (LPECDOBJECT)pitem);
}
