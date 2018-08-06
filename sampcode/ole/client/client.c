//---------------------------------------------------------------------------
// client.c
//
// Copyright (c) Microsoft Corporation, 1990-
//---------------------------------------------------------------------------

#include "windows.h"
#include "ecd.h"
#include "cldemo.h"
#include "client.h"

//--- Globals --------------------------------------------------------------
extern HWND	hwndEdit;
extern BOOL fInsUpdate;
extern HWND rgchChildWnd[];
extern int irgchChild;
// extern BOOL fObjectClose;

LPECDCLIENT     lpclient;
ECDCLIENTVTBL   clientTbl;
ECDSTREAMVTBL   streamTbl;
LPAPPSTREAM     lpstream;
HANDLE			hObjectUndo = NULL;
LPECDOBJECT     lpObjectUndo;
WORD            wUpdateUndo, wTypeUndo;
BOOL            fEcdrelease;
BOOL            fEcdReleaseError;
BOOL            fEcdReleaseStat;
HANDLE          hobjStream = NULL;
HANDLE          hobjClient = NULL;
// HANDLE 			hobjPrn = NULL;
HANDLE hWinTitle = 0;
int	nOffDocNm, nOffItemNm;


//--- Local Functions Declaration -------------------------------------------
void GetUpdateStatus(HWND hDlg);
void PrepareUndo(PITEM pitem);
void PrepareUndoClear(PITEM pitem);
void PrintWindowTitle(PITEM pitem, HWND hwnd);
VOID SetRectSize(HWND hwnd, PITEM pitem);
void PrintLinkName(PITEM pitem, HWND hwnd);
void SetData(PITEM pitem, HWND hDlg);


extern HWND GetTopChild();
extern HWND GetNextWnd(LPINT i);

//---------------------------------------------------------------------------
// InitClient
// Purpose: Set up this application as a client
// Returns: TRUE if successful
//---------------------------------------------------------------------------
BOOL InitClient (HANDLE hInstance)
    {
    
    // Set up the client structure

    if (!(hobjClient = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
                                   sizeof(ECDCLIENT))))
            return FALSE;

    if (!(lpclient = (LPECDCLIENT)(GlobalLock(hobjClient))))
        {
        GlobalFree(hobjClient);
        hobjClient = NULL;
        return FALSE;
        }

    lpclient->lpvtbl = (LPECDCLIENTVTBL)&clientTbl;

    lpclient->lpvtbl->CallBack = MakeProcInstance(CallBack, hInstance);

    // Set up the stream structure

    if (!(hobjStream = GlobalAlloc (GMEM_MOVEABLE, sizeof(APPSTREAM))))
        {
        GlobalUnlock(hobjClient);
        GlobalFree(hobjClient);
        hobjClient = NULL;
        return FALSE;
        }

    if(lpstream = (LPAPPSTREAM)(GlobalLock(hobjStream)))
        {
        lpstream->lpstbl = (LPECDSTREAMVTBL)&streamTbl;
        streamTbl.Get =
            (LONG  (pascal far *) (LPECDSTREAM, LPSTR, LONG))
            MakeProcInstance((FARPROC) ReadStream, hInstance);
        streamTbl.Put =
            (LONG  (pascal far *) (LPECDSTREAM, LPSTR, LONG))
            MakeProcInstance((FARPROC) WriteStream, hInstance);
        streamTbl.Seek =
            (LONG  (pascal far *) (LPECDSTREAM, LONG))
            MakeProcInstance((FARPROC) PosStream, hInstance);
        streamTbl.Status =  MakeProcInstance((FARPROC) StatusStream, hInstance);
        lpstream->hobjStream = hobjStream;
        }
    else
        {
        GlobalUnlock(hobjClient);
        GlobalFree(hobjClient);
        hobjClient = NULL;
        GlobalFree(hobjStream);
        hobjStream = NULL;
        return FALSE;
        }
    return TRUE;
}

//---------------------------------------------------------------------------
// TermClient
// Purpose: To clean up after termination
// Returns: void
//---------------------------------------------------------------------------
void TermClient (void)
{
    // Get rid of the client structure
    if (hobjClient)
        {
        GlobalUnlock(hobjClient);
        GlobalFree(hobjClient);
        }

    // Get rid of the stream structure
    if (hobjStream)
        {
        GlobalUnlock(hobjStream);
        GlobalFree(hobjStream);
        }
}

//---------------------------------------------------------------------------
// CallBack
// Purpose: Client Callback Function
//---------------------------------------------------------------------------
int FAR PASCAL
CallBack(LPECDCLIENT lpclient, int flags, LPECDOBJECT lpObject)
{
	RECT rc;
	HWND hDC;
	ECDSTATUS EcdStat;
    switch(flags)
        {
        case ECD_SAVED:
        case ECD_CHANGED:
        case ECD_CLOSED:
            {
            HANDLE hitem;
            PITEM  pitem;
            HWND   hwnd, hNext;
				short mm;
          
            // The file needs to be saved
            fDirty = TRUE;

            // Find out if we need to update our visuals immediately
				if (fInsUpdate){ // it is for case when object created by EcdCreate
					fInsUpdate = FALSE;
					}

            hwnd = GetTopWindow(hwndMain);
				while(hwnd){
					if (hwnd != hwndEdit){
						hitem = GetWindowWord(hwnd, 0);
						pitem = (PITEM) LocalLock(hitem);
            		if (pitem->lpecdobject == lpObject && (flags == ECD_CLOSED)){
							pitem->fOpen = FALSE;
							}
            		if (pitem->lpecdobject == lpObject){
							SetRectSize(hwnd, pitem);
            		    InvalidateRect(hwnd, NULL, TRUE);
							}
            		LocalUnlock(hitem);
						}
					hwnd = GetWindow(hwnd, GW_HWNDNEXT);
					}
            
            }
            	break;

        case ECD_RELEASE:
            fEcdrelease = TRUE;
				if ((fEcdReleaseStat = EcdQueryReleaseError(lpObject)) != ECD_OK){
					fEcdReleaseError = TRUE;
					}
				else
					fEcdReleaseError = FALSE;

            break;

        case ECD_QUERY_PAINT:
            // Yes, paint!
            return TRUE; 
            break;

        case ECD_RENAMED:
            // The file needs to be saved
            fDirty = TRUE;
            break;

        default:
            break;
    }
    return 0;
}

//---------------------------------------------------------------------------
// ClientUpdateMenu
// Purpose: Update the Edit menu
// Returns: void
//---------------------------------------------------------------------------
void ClientUpdateMenu(HWND hwnd, HMENU hMenu)
{
    HWND    hwndChild;
    HANDLE  hitem;
    PITEM   pitem;
    int     mfPaste = MF_GRAYED;
    int     mfPasteLink = MF_GRAYED;
    int     mfProperties = MF_GRAYED;
    int     mfClear = MF_GRAYED;
    int     mfCut   = MF_GRAYED;
    int     mfCopy  = MF_GRAYED;
    int     mfUndo  = MF_GRAYED;
    int     mfClose  = MF_GRAYED;
    int     mfCopyLink  = MF_GRAYED;
    int     mfReconnect  = MF_GRAYED;

    // Enable "Paste" if there is a paste-able object in the clipboard */
	 if (hObjectUndo)
		mfUndo = MF_ENABLED;
    if (EcdQueryCreateFromClip(PROTOCOL, ecdrender_draw, 0) == ECD_OK)
        mfPaste = MF_ENABLED;
    
    // Enable "Paste Link" if there is a link-able object in the clipboard 
    if (EcdQueryLinkFromClip(PROTOCOL, ecdrender_draw, 0) == ECD_OK)
        mfPasteLink = MF_ENABLED;
    
    // Enable "Clear", "Cut", "Copy" and maybe "Properties..." 
    // if we have a child window
		if (GetWindowWord(hwndMain, 0)) // if there is any window
        {
        mfClear = MF_ENABLED;
        mfCut   = MF_ENABLED;
        mfCopy  = MF_ENABLED;
        mfCopyLink  = MF_ENABLED;
    	 	hwndChild = GetTopWindow(hwnd);
			if (hwndChild == hwndEdit)
			hwndChild = GetTopWindow(hwnd);
        hitem = GetWindowWord(hwndChild, 0);
        pitem = (PITEM) LocalLock(hitem);
        if (pitem->wType != IDM_STATIC)
            mfProperties = MF_ENABLED;
		  if (pitem->fOpen)
        	mfClose  = MF_ENABLED;
		  if (pitem->fClose)
			mfReconnect = MF_ENABLED;
        LocalUnlock(hitem);
        }

    EnableMenuItem(hMenu, IDM_UNDO,       mfUndo);
    EnableMenuItem(hMenu, IDM_CUT,        mfCut);
    EnableMenuItem(hMenu, IDM_COPY,       mfCopy);
    EnableMenuItem(hMenu, IDM_CLOSE,      mfClose);
    EnableMenuItem(hMenu, IDM_COPYLINK,   mfCopyLink);
    EnableMenuItem(hMenu, IDM_LINK,       mfPasteLink);
    EnableMenuItem(hMenu, IDM_PASTE,      mfPaste);
    EnableMenuItem(hMenu, IDM_CLEAR,      mfClear);
    EnableMenuItem(hMenu, IDM_PROPERTIES, mfProperties);
    EnableMenuItem(hMenu, IDM_RECONNECT,  mfReconnect);
}

//---------------------------------------------------------------------------
// ClientCutCopy
// Purpose: Does "Cut" and "Copy"
// Returns: void
//---------------------------------------------------------------------------
void ClientCutCopy(HWND hwndMain, WORD w)
{
    HWND hwnd;
    HANDLE hitem;
    PITEM pitem;

    hwnd = GetTopWindow(hwndMain);
    hitem = GetWindowWord(hwnd, 0);
    pitem = (PITEM) LocalLock(hitem);
    if (pitem->lpecdobject && OpenClipboard(hwndMain))
        {
        EmptyClipboard();
        if (w = IDM_COPY)
            {
            EcdCopyToClipboard(pitem->lpecdobject);
            }
        else
            {
            EcdCutToClipboard(pitem->lpecdobject);
            pitem->lpecdobject = NULL;
            }
        CloseClipboard();
        }
    LocalUnlock(hitem);
}


//---------------------------------------------------------------------------
// ClientPasteLink
// Purpose: Does "Paste" and "Paste Link"
// Returns: void
//---------------------------------------------------------------------------
void ClientPasteLink(HWND hwndMain, WORD w)
{
    HWND hwnd = NULL;
    HANDLE hitem;
    PITEM pitem = NULL;
    ECDCLIPFORMAT   cfFormat;

	RECT rc;
	HDC hdc;
	short mm;
	POINT pt;
	ECDSTATUS EcdStat;

    // Allocate memory for a new item
    hitem = LocalAlloc (LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof (ITEM));
    if (hitem == NULL || ((pitem = (PITEM)LocalLock (hitem)) == NULL))
        goto  errRtn;

    if (hwnd = CreateItemWindow(hwndMain, FALSE, NULL))
        {
        SetWindowWord (hwnd, 0, hitem);
        BringWindowToTop (hwnd);
        }
    else goto errRtn;

		rgchChildWnd[irgchChild++] = hwnd;
        SetWindowWord (hwndMain, 0, (GetWindowWord(hwndMain, 0)+1) );

    // Grab the item from the clipboard
    switch (w)
        {
        case IDM_PASTE:
            if (!OpenClipboard(hwndMain))
                goto errRtn;
            else if( (EcdStat = EcdCreateFromClip(PROTOCOL, lpclient, 0, NULL, &(pitem->lpecdobject),
                ecdrender_draw,  0)) != ECD_OK)
                {
					if (EcdStat == ECD_WAIT_FOR_RELEASE)
						WaitForRelease(pitem);
					if (fTestEcdStat(EcdStat)){
						 CloseClipboard();
						 goto errRtn;
               	}
                }
            CloseClipboard();
            break;
        case IDM_LINK:
            if (!OpenClipboard(hwndMain))
                goto errRtn;
            else if ( (EcdStat = EcdCreateLinkFromClip(PROTOCOL, lpclient, 0, NULL, &(pitem->lpecdobject),
                    ecdrender_draw, 0)) != ECD_OK)
                {
  					if (EcdStat == ECD_WAIT_FOR_RELEASE)
						WaitForRelease(pitem);
					if (fTestEcdStat(EcdStat)){
						 	CloseClipboard();
						 	goto errRtn;
               		}
                }
            CloseClipboard();
            break;
        }
                                    
		pitem->fOpen = FALSE;
		pitem->fClose = FALSE;
    cfFormat = EcdEnumObjectFormats (pitem->lpecdobject, NULL);
    if (cfFormat == cfNative)
		{
        pitem->wType = IDM_PASTE;  
		}
    else if (cfFormat == cfLink)
        pitem->wType = IDM_LINK;  

    pitem->wUpdate = IDD_AUTO;
	 
    EcdSetHostNames(pitem->lpecdobject, szHostname, szHostobjectname);

	GetCaretPos((LPPOINT)&pt);
	pt.y += 15;
	hdc = GetDC(hwnd);
	mm = SetMapMode(hdc, MM_HIMETRIC);
	DPtoLP(hdc, (LPPOINT)&pt, 1); 

	EcdQueryBounds(pitem->lpecdobject, &rc);
	rc.top = pt.y ;
	rc.bottom += pt.y ;
	LPtoDP(hdc, (LPPOINT)&rc, 2);


	SetMapMode(hdc, mm);
	MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top +
		GetSystemMetrics(SM_CYCAPTION), TRUE);
	ReleaseDC(hwnd, hdc);
   SendMessage (hwnd, WM_USER, 0, 0L);

    ShowWindow(hwnd, SW_SHOW);
	 InvalidateRect(hwnd, NULL, TRUE);

    LocalUnlock(hitem);

    return;

 errRtn:
    if (pitem)
        LocalUnlock (hitem);

    if (hitem)
        LocalFree (hitem);

    if (hwnd)
        {
        SetWindowWord(hwnd, 0, NULL);
        DestroyWindow(hwnd);
        }

}

//---------------------------------------------------------------------------
// ClientCreateNewItem
// Purpose: Creates a new window and associates a new item
// Returns: void
//---------------------------------------------------------------------------
void ClientCreateNewItem (HWND hwndMain, BOOL fVisible,
    LPRECT lprect, LPECDOBJECT lpobject, WORD wType, WORD wUpdate)
{
    HWND   hwndItem;
    HANDLE hitem;
    PITEM  pitem = NULL;
	BOOL fIconic = FALSE;

    // Allocate memory for a new item
    hitem = LocalAlloc (LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof (ITEM));

    if (hitem == NULL || ((pitem = (PITEM)LocalLock (hitem)) == NULL))
        goto  errRtn;

    pitem->wType = wType;    
    pitem->wUpdate = wUpdate;
    pitem->lpecdobject = lpobject;

	pitem->fOpen = FALSE;
	pitem->fClose = FALSE;

    // Create a window in which to put the new item
	if ((lprect->left == 0) && (lprect->top == 0) && (lprect->right == 0) && (
		lprect->bottom == 0)) // i.e. iconic window
		fIconic = TRUE;
    if (hwndItem = CreateItemWindow(hwndMain, fIconic, lprect))
        {
			if (!fIconic)
				MoveWindow(hwndItem, lprect->left, lprect->top,
					lprect->right - lprect->left, lprect->bottom - lprect->top, TRUE);
        EcdSetHostNames(pitem->lpecdobject, szHostname, szHostobjectname);
        SetWindowWord (hwndItem, 0, hitem);
			rgchChildWnd[irgchChild++] = hwndItem;
   	  SendMessage (hwndItem, WM_USER, 0, 0L);
        if (fVisible)
            ShowWindow(hwndItem, SW_SHOW);
				InvalidateRect(hwndItem, NULL, TRUE);
        }
    else goto errRtn;
    LocalUnlock(hitem);
    return;

 errRtn:
    if (pitem)
        LocalUnlock (hitem);

    if (hitem)
        LocalFree (hitem);
}

//---------------------------------------------------------------------------
// ClientPaint
// Purpose: Ask the server to paint for us
// Returns: void
//---------------------------------------------------------------------------
void ClientPaint(HWND hwnd, HDC hdc, PITEM pitem)
{
    RECT rc;
	HDC saveDC;
	short mm;
	POINT pt;
	int bot;

    // Find the coordinates of the "display" area


    GetClientRect (hwnd, (LPRECT)&rc);
    
    // Convert the rectangle's coordinates to MM_HIMETRIC 
    SetMapMode(hdc, MM_HIMETRIC);
    DPtoLP(hdc,(LPPOINT)&rc,2);

    // Ask the server to draw the object 
    EcdDraw (pitem->lpecdobject, hdc, (LPRECT)&rc, hdc);
}

//---------------------------------------------------------------------------
// ClientEdit
// Purpose: Ask the server to edit the object
// Returns: void
//---------------------------------------------------------------------------
void ClientEdit(HWND hwnd, PITEM pitem)
{
    RECT rc;
	HWND hdc;
	short mm;
	ECDSTATUS EcdStat;

    GetClientRect(hwnd,&rc);
	 if (EcdQueryOpen(pitem->lpecdobject) != ECD_OK) //
    	EcdStat = EcdOpen (pitem->lpecdobject, TRUE, hwnd, &rc);
	 else{
    	EcdStat = EcdOpen (pitem->lpecdobject, TRUE, hwnd, &rc);
		/* should be following code
		if (EcdReconnect(pitem->lpecdobject) != ECD_OK){
			MessageBox(hwndMain, "Error Reconnecting ",
                          szAppName, MB_ICONASTERISK | MB_OK);
    		EcdOpen (pitem->lpecdobject, TRUE, hwnd, &rc);
			}
		*/
		}
		pitem->fOpen = TRUE;
		if (EcdStat != ECD_OK){
			if (EcdStat == ECD_WAIT_FOR_RELEASE)
				WaitForRelease(pitem);
				if (fTestEcdStat(EcdStat)){
				}
			}
			
	hdc = GetDC(hwnd);
    mm = SetMapMode(hdc, MM_HIMETRIC);
    DPtoLP(hdc,(LPPOINT)&rc,2);
	EcdSetBounds(pitem->lpecdobject, (LPRECT)&rc);
	SetMapMode(hdc, mm);
	ReleaseDC(hwnd, hdc);

}

//---------------------------------------------------------------------------
// ClientDelete
// Purpose: Ask the server to delete the object
// Returns: void
//---------------------------------------------------------------------------
void ClientDelete(PITEM pitem)
{
    MSG msg;

    if (EcdDelete (pitem->lpecdobject) == ECD_WAIT_FOR_RELEASE)
        {
    		fEcdrelease = FALSE;
        	do
            {
            GetMessage(&msg, NULL, NULL, NULL);
            TranslateMessage(&msg);
            DispatchMessage(&msg); 
            }
        while (!fEcdrelease);
        }
}

//---------------------------------------------------------------------------
// SetFile
// Purpose: Set up the stream structure
// Returns: void
//---------------------------------------------------------------------------
void SetFile (int fh)
{
    lpstream->fh = fh;
}

//---------------------------------------------------------------------------
// PropertiesDlgProc
// Purpose: Handles the Properties... dialog box events
//---------------------------------------------------------------------------
BOOL FAR PASCAL
PropertiesDlgProc(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    HWND hwnd;
    HANDLE hitem;
    PITEM pitem;
    LPECDOBJECT lpobject;
    HSTR hstr;
    LPSTR lpstrInfo, lpstr1, lpstr2;

    switch (message)
        {
        case WM_INITDIALOG:
            lpObjectUndo = NULL;
            wUpdateUndo = 0;
				hwnd = GetTopChild();
            hitem = GetWindowWord(hwnd, 0);
            pitem = (PITEM) LocalLock(hitem);
            PrepareUndo(pitem);
            CheckDlgButton(hDlg, pitem->wUpdate, TRUE);
            if (pitem->wType == IDM_PASTE)
                EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);
					if (pitem->wType != IDM_PASTE)
						PrintLinkName(pitem, hDlg);
				 //		PrintWindowTitle(pitem, GetDlgItem(hDlg, IDD_LINKINFO));
					else
				 		PrintWindowTitle(pitem, GetDlgItem(hDlg, IDD_WHICH));
            
            LocalUnlock(hitem);
            return (TRUE);

        case WM_COMMAND:
            switch (wParam)
                {
                case IDD_FREEZE:
                case IDD_UPDATE:
						/*
                    hwnd = GetTopWindow(hwndMain);
							if (hwnd == hwndEdit)
								hwnd = GetWindow(hwnd, GW_HWNDNEXT);
							*/
							hwnd = GetTopChild();
                    hitem = GetWindowWord(hwnd,0);
                    pitem = (PITEM) LocalLock(hitem);
                    if (wParam == IDD_FREEZE)
                        {
                        if (pitem->wType == IDM_PASTE)
                            EnableWindow(GetDlgItem(hDlg, IDCANCEL), TRUE);
                        else
                            EnableWindow(GetDlgItem(hDlg, IDD_UPDATE), FALSE);
                        EnableWindow(GetDlgItem(hDlg, IDD_EDIT), FALSE);
                        EnableWindow(GetDlgItem(hDlg, IDD_FREEZE), FALSE);
                        if (EcdObjectConvert(pitem->lpecdobject, SPROTOCOL,
                            lpclient, NULL, NULL, &lpobject) == ECD_OK)
                            {
                            ClientDelete (pitem);
                            pitem->lpecdobject = lpobject;
                            pitem->wType = IDM_STATIC;
                            }
                        }
                    else // (wParam == IDD_UPDATE)
                        {
                        EcdUpdate(pitem->lpecdobject);
                        }
                    fDirty = TRUE;
                    InvalidateRect(hwnd, NULL, TRUE);
                    LocalUnlock(hitem);
                    break;

                case IDD_EDIT:
                    {
                    RECT rc;

                    GetUpdateStatus(hDlg);
							hwnd = GetTopChild();

                    hitem = GetWindowWord(hwnd, 0);
                    pitem = (PITEM) LocalLock(hitem);
                    EcdDelete(lpObjectUndo);
        					lpObjectUndo = NULL;
                    GetClientRect(hwnd,&rc);
                    EcdOpen (pitem->lpecdobject, TRUE, hwnd, &rc);
                    LocalUnlock(hitem);
                    fDirty = TRUE;
                    InvalidateRect(hwnd, NULL, TRUE);
                    }
                    /* Fall through */

                case IDOK:
                    GetUpdateStatus(hDlg);
                    EcdDelete(lpObjectUndo);
        					lpObjectUndo = NULL;
							hwnd = GetTopChild();
            			hitem = GetWindowWord(hwnd, 0);
            			pitem = (PITEM) LocalLock(hitem);
							if (pitem->wType != IDM_PASTE)
								SetData(pitem, hDlg);
							LocalUnlock(hitem);
							GlobalFree(hWinTitle);
                    EndDialog(hDlg, TRUE);
                    return TRUE;

                case IDCANCEL:
							hwnd = GetTopChild();

                    hitem = GetWindowWord(hwnd, 0);
                    pitem = (PITEM) LocalLock(hitem);
                    if (lpObjectUndo)
                        {
                        EcdDelete(pitem->lpecdobject);
                        pitem->lpecdobject = lpObjectUndo;
                        pitem->wType = wTypeUndo;
                        pitem->wUpdate  = wUpdateUndo;
                        }
                    LocalUnlock(hitem);
						InvalidateRect(hwnd, NULL, TRUE);
							GlobalFree(hWinTitle);
                    EndDialog(hDlg, TRUE);
                    return TRUE;
                }
            break;
        }
    return (FALSE);
}


//---------------------------------------------------------------------------
// GetUpdateStatus
// Purpose: Read the status of the update buttons in the Properties Dialog
//          associate to a linked object.  
//          Updates the internal data structures and inform the dll.
// Return: void
//---------------------------------------------------------------------------
void GetUpdateStatus(HWND hDlg)
{
    WORD wUpdate;
    HWND hwnd;
    HANDLE hitem;
    PITEM pitem;
	 ECDSTATUS EcdRet;
	 ECDOPT_UPDATE EcdOpt;

	  
	hwnd = GetTopChild();
    hitem = GetWindowWord(hwnd, 0);
    pitem = (PITEM) LocalLock(hitem);
    if (pitem->wType == IDM_LINK)
        {
        wUpdate = (IsDlgButtonChecked(hDlg, IDD_AUTO) ?
            IDD_AUTO : IDD_MANUAL);
        if (wUpdate != pitem->wUpdate)
            {
            pitem->wUpdate = wUpdate;
				EcdRet = EcdGetLinkUpdateOptions(pitem->lpecdobject,
					(ECDOPT_UPDATE FAR *)&EcdOpt);
				if ((wUpdate == IDD_AUTO )&& (EcdOpt != ecdupdate_always))
					EcdSetLinkUpdateOptions(pitem->lpecdobject, ecdupdate_always);
				else if ((wUpdate == IDD_MANUAL) && (EcdOpt != ecdupdate_oncall))
					EcdSetLinkUpdateOptions(pitem->lpecdobject, ecdupdate_oncall);
            }
        }
    LocalUnlock(hitem);
}

//---------------------------------------------------------------------------
// PrepareUndo
// Purpose: Keep track of the current object to be able to Undo later
// Return: void
//---------------------------------------------------------------------------
void PrepareUndo(PITEM pitem)
{
    // Delete the old copy of the object if necessary
    if (lpObjectUndo)
        EcdDelete(lpObjectUndo);

    // Create a copy of the current object
    if (EcdClone(pitem->lpecdobject, lpclient,
        NULL, NULL, (LPECDOBJECT FAR *)&lpObjectUndo) != ECD_OK) 
        lpObjectUndo = NULL;
    else
        {
        wUpdateUndo = pitem->wUpdate;
        wTypeUndo = pitem->wType;
        }
}


//---------------------------------------------------------------------------
// PrepareUndoClear
// Purpose: Keep track of the current object to be able to Undo later
// Return: void
//---------------------------------------------------------------------------
void PrepareUndoClear(PITEM pitem)
{
	PITEM pObjectUndo;
    // Delete the old copy of the object if necessary
    if (hObjectUndo){
		pObjectUndo = (PITEM)LocalLock(hObjectUndo);
		EcdDelete(pObjectUndo->lpecdobject);
		}
	else {
		hObjectUndo = LocalAlloc (LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof (ITEM));
		pObjectUndo = (PITEM) LocalLock(hObjectUndo);				
      }            
    // Create a copy of the current object
    if (EcdClone(pitem->lpecdobject, lpclient,
        NULL, NULL, (LPECDOBJECT FAR *)&pObjectUndo->lpecdobject) != ECD_OK) {
		LocalUnlock(hObjectUndo);
		LocalFree(hObjectUndo);
      hObjectUndo = NULL;
		}
	else{
		pObjectUndo->wType = pitem->wType;				
		pObjectUndo->wUpdate = pitem->wUpdate;
		LocalUnlock(hObjectUndo);
		}
}

//---------------------------------------------------------------------------
// ReadStream
// Purpose: Read from an open file
//---------------------------------------------------------------------------
LONG FAR PASCAL ReadStream(LPAPPSTREAM lpStream, LPSTR lpstr, LONG cb)
{
    return _lread(lpStream->fh, lpstr, (WORD)cb);
}

//---------------------------------------------------------------------------
// WriteStream
// Purpose: Write to an open file
//---------------------------------------------------------------------------
LONG FAR PASCAL WriteStream(LPAPPSTREAM lpStream, LPSTR lpstr, LONG cb)
{
    return _lwrite(lpStream->fh, lpstr, (WORD)cb);
}

//---------------------------------------------------------------------------
// PosStream
// Purpose: Reposition file pointer
//---------------------------------------------------------------------------
LONG FAR PASCAL PosStream(LPAPPSTREAM lpStream, LONG pos)
{
    return  _llseek(lpStream->fh, (LONG)pos, 0);
}

//---------------------------------------------------------------------------
// StatusStream
// Purpose: Returns the status of a file operation
// Returns: NULL (Not implemented)
//---------------------------------------------------------------------------
WORD FAR PASCAL StatusStream(LPAPPSTREAM lpStream)
{
    return NULL;
}


VOID SetRectSize(HWND hwnd, PITEM pitem){
	RECT rc, rc1;
	HDC hDC;
	short mm;
	
	if (!IsIconic(hwnd)){
		GetWindowRect(hwnd, &rc1);
      ScreenToClient(hwndMain, (LPPOINT) &rc1);
		hDC = GetDC(hwnd);
		mm = SetMapMode(hDC, MM_HIMETRIC);								
		EcdQueryBounds(pitem->lpecdobject, &rc);								
		LPtoDP(hDC, (LPPOINT)&rc, 2);								
		SetMapMode(hDC, mm);								
		if ((rc.right - rc.left == 0) || (rc.bottom - rc.top == 0)){	
			rc.right = 200 ;														
			rc.bottom = 200;								
			rc.left = 0;								
			rc.top = 0;								
			}								
		MoveWindow(hwnd, rc1.left, rc1.top, rc.right - rc.left,
			rc.bottom - rc.top + GetSystemMetrics(SM_CYCAPTION), TRUE);			
		ReleaseDC(hwnd, hDC);
		}

}



/* Combine This function with PrintWindowTitle, lots of problems */

void PrintLinkName(PITEM pitem, HWND hwnd){

	ECDSTATUS EcdStat;
  	LPSTR pTitle, pTitle1;
   LPSTR lpstrInfo, lpstr1, lpstr2;
	HSTR hstr;

            if (((EcdStat = EcdGetData(pitem->lpecdobject,
                    (pitem->wType == IDM_PASTE) ? cfOwnerLink : cfLink,
                    &hstr))
                    == ECD_OK)
                && hstr)
                {
                // Both link formats are:  "szClass szDocument szItem 0"
                lpstrInfo = GlobalLock(hstr);
					 lpstr1 = lpstrInfo;
					 hWinTitle = GlobalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, 512);
					 pTitle = pTitle1 = (LPSTR)GlobalLock(hWinTitle);
					 while (*lpstr1){
						*(pTitle++) = *(lpstr1++);
						}
					 *(pTitle++) = '\0';
					 lpstr1 += 1;
                SetWindowText(GetDlgItem(hwnd, IDD_LINKINFO), (LPSTR)pTitle1);
					 nOffDocNm = pTitle - pTitle1;
					pTitle1 = pTitle;
					 while (*lpstr1){
						*(pTitle++) = *(lpstr1++);
						}
					 *(pTitle++) = '\0';
					 lpstr1 += 1;
                SetWindowText(GetDlgItem(hwnd, IDD_DOCNM), (LPSTR)pTitle1);

					nOffItemNm = pTitle - pTitle1 + nOffDocNm;
					pTitle1 = pTitle;
					 while (*lpstr1){
						*(pTitle++) = *(lpstr1++);
						}
					 *(pTitle++) = '\0';
					 lpstr1 += 1;
                SetWindowText(GetDlgItem(hwnd, IDD_ITEMNM), (LPSTR)pTitle1);

					 GlobalUnlock(hWinTitle);
//					LocalFree(hWinTitle);
            	 GlobalUnlock(hstr);
                }
					}

void SetData(PITEM pitem, HWND hDlg){
		char szObj[128];
		BOOL fSet = FALSE;
		LPSTR lpWinTitle;
		ECDSTATUS EcdStat;

		lpWinTitle = GlobalLock(hWinTitle);
		GetDlgItemText(hDlg, IDD_DOCNM, (LPSTR)szObj, 128);
		if (lstrcmpi((LPSTR)szObj, (LPSTR)(lpWinTitle + nOffDocNm))){
			lstrcpy((LPSTR)(lpWinTitle + nOffDocNm), (LPSTR)szObj);
			fSet = TRUE;
			}
		GetDlgItemText(hDlg, IDD_ITEMNM, (LPSTR)szObj, 128);
		if (lstrcmp((LPSTR)szObj, (LPSTR)(lpWinTitle + nOffItemNm))){
			lstrcpy((LPSTR)(lpWinTitle + nOffItemNm), (LPSTR)szObj);
			fSet = TRUE;
			}
		GlobalUnlock(hWinTitle);
		if (fSet){
			EcdStat = EcdSetData(pitem->lpecdobject,
                    (pitem->wType == IDM_PASTE) ? cfOwnerLink : cfLink,
							hWinTitle);
			PrintError(EcdStat);
			}
		}



