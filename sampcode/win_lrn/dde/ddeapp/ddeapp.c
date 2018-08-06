/*
 *      ddeapp.c
 *
 *      This is a dummy application used to test the DDE capabilities
 *      of Microsoft Excel.
 *
 */

#include <windows.h>
#include "ddeapp.h"

#define GMEM_DDESHARE 0x2000
#define EXCEL

#ifdef EXCEL
#define szAppDef "Excel"
#define szDocDef "Sheet1"
#define szRefDef "r1c1:c1c2"
#define szDataDef "42"
#endif

#ifdef OPUS
#define szAppDef "Opus"
#define szDocDef "opus.doc"
#define szRefDef "bkmk1"
#define szDataDef "Foo bar."
#endif

#ifdef FISH
#define szAppDef "Fish"
#define szDocDef "sub"
#define szRefDef "bkmk1"
#define szDataDef "[l]"
#endif

#ifndef FISH
#ifndef OPUS
#ifndef EXCEL
#define szAppDef ""
#define szDocDef ""
#define szRefDef ""
#define szDataDef ""
#endif
#endif
#endif


long far pascal DdeWndProc(HWND, unsigned, int, long);
BOOL far pascal InitiateDlg(HWND, unsigned, int, long);
BOOL far pascal ExecuteDlg(HWND, unsigned, int, long);
BOOL far pascal PokeDlg(HWND, unsigned, int, long);
BOOL far pascal TermDlg(HWND, unsigned, int, long);
BOOL far pascal RequestDlg(HWND, unsigned, int, long);
BOOL far pascal UnadviseDlg(HWND, unsigned, int, long);

void PostClientMsg(int, int, ATOM, int);
void ChnlBox(HWND, int);
void ClipFmtBox(HWND,int);

BOOL FInit(int);
BOOL FGetAck(HWND);
int  IclListBox(HWND,int);


HANDLE  hinstDDE;
HWND    hwndDDE;
HBRUSH  hbrBack;
HCURSOR hcrsWait;
HCURSOR hcrsArrow;

/* supported clipboard formats */
#define szClipRTF   "Rich Text Format"
#define szClipLink  "Link"
#define szClipCSV   "Csv"
#define szClipBitPrinter  "Printer_Bitmap"
#define szClipMetaPrinter "Printer_Picture"

#define icfMax 9
#define icfRTF 5
#define icfCSV 6
#define icfBitPrinter 7
#define icfMetaPrinter 8
int rgcf[icfMax] = {
        CF_TEXT,
        CF_SYLK,
        CF_DIF,
        CF_BITMAP,
        CF_METAFILEPICT
};

int  cfLink;
int  iRadioButton, iRadioButtonTemp;
int  iResult, iAck;
BOOL fInInitiate = FALSE;
HWND hwndLink;

char szApp[256]  = szAppDef;
char szDoc[256]  = szDocDef;
char szRef[256]  = szRefDef;
char szData[245] = szDataDef;

int cfCur;
int iclCur;

PLCL **hplcl;           /* channel array */
PLDL **hpldl;           /* dependent link array */


/*      WinMain
 *
 *      Main entry point for DDE dummy application.
 *
 *      Arguments:
 *              hinst
 *              hinstPrev
 *              lpszCmdLine
 *              sw
 *
 *      Returns:
 *
 */
int far pascal WinMain(hinst, hinstPrev, lpszCmdLine, sw)
HANDLE hinst;
HANDLE hinstPrev;
char far *lpszCmdLine;
int sw;
{
   MSG msg;
   PL **HplInit();

   hinstDDE = hinst;

   if (!FInit(sw))
           return(0);

   hplcl = (PLCL **)HplInit(sizeof(CL));
   hpldl = (PLDL **)HplInit(sizeof(DL));

   rgcf[icfRTF] = RegisterClipboardFormat((LPSTR)szClipRTF);
   cfLink       = RegisterClipboardFormat((LPSTR)szClipLink);
   rgcf[icfCSV] = RegisterClipboardFormat((LPSTR)szClipCSV);
   rgcf[icfBitPrinter] = RegisterClipboardFormat((LPSTR)szClipBitPrinter);
   rgcf[icfMetaPrinter] = RegisterClipboardFormat((LPSTR)szClipMetaPrinter);
   while (GetMessage((MSG far *)&msg, NULL, 0, 0)) {
           TranslateMessage((MSG far *)&msg);
           DispatchMessage((MSG far *)&msg);
   }

   return(msg.wParam);
}


/* ----------------------------------------------------- */
void Error(sz)
char *sz;
{
   MessageBeep(MB_OK|MB_ICONHAND|MB_APPLMODAL);
   MessageBox(hwndDDE, (LPSTR)sz, (LPSTR)"DDE failure", MB_OK|MB_ICONHAND|MB_APPLMODAL);
}


/* -----------------------------------------------------
 *      CchSzLen
 *
 *      Returns the length of a null-terminated string.
 *
 *      Arguments:
 *              sz
 *
 *      Returns:
 *              length of the string
 */
int CchSzLen(sz)
char far *sz;
{
   int cch;

   cch = 0;
   while (*sz++)
           cch++;
   return(cch);
}


/* ----------------------------------------------------
 *      bltbx
 *
 *      Far block transfer.
 *
 *      Arguments:
 *              lpbFrom         source buffer
 *              lpbTo           destination buffer
 *              cb              number of bytes to move
 *
 *      Returns:
 *              nothing.
 *
 *      Warning:
 *              does not handle over-lapping transfer!
 */
void bltbx(lpbFrom, lpbTo, cb)
BYTE far *lpbFrom;
BYTE far *lpbTo;
unsigned cb;
{
        while (cb--)
                *lpbTo++ = *lpbFrom++;
}


/* -----------------------------------------------------
 *      FInit
 *
 *      Initializes the window app.
 *
 *      Arguments:
 *              sw              type of ShowWindow call to make on
 *                              first window created.
 *
 *      Returns:
 *              TRUE if successful
 */
BOOL FInit(sw)
int sw;
{
   WNDCLASS cls;
   HDC hdc;
   int dxScreen, dyScreen;

/* register window class */

   hbrBack   = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
   hcrsArrow = LoadCursor(NULL, IDC_ARROW);
   hcrsWait  = LoadCursor(NULL, IDC_WAIT);

   cls.lpszClassName = (LPSTR)"DdeApp";
   cls.lpfnWndProc   = DdeWndProc;
   cls.hInstance     = hinstDDE;
   cls.hIcon         = LoadIcon(NULL, IDI_HAND);
   cls.hCursor       = NULL;
   cls.lpszMenuName  = MAKEINTRESOURCE(mbDde);
   cls.hbrBackground = hbrBack;
   cls.style         = 0;
   cls.cbClsExtra    = cls.cbWndExtra = 0;
   if (!RegisterClass((WNDCLASS far *)&cls))
           return(FALSE);

/* create initial tiled window */

   hdc = CreateDC((LPSTR)"DISPLAY", NULL, NULL, NULL);
   dxScreen = GetDeviceCaps(hdc, HORZRES);
   dyScreen = GetDeviceCaps(hdc, VERTRES);
   DeleteDC(hdc);
   hwndDDE = CreateWindow((LPSTR)"DdeApp",
                   (LPSTR)"Acme DDE Application",
                   WS_OVERLAPPEDWINDOW,
                   0, 0, dxScreen/2, dyScreen/4,
                   NULL, NULL, hinstDDE, NULL);
   ShowWindow(hwndDDE, sw);

   return(TRUE);
}


/* ----------------------------------------------------- */

BOOL far pascal InitiateDlg(hdlg, wm, wParam, lParam)
HWND hdlg;
unsigned wm;
int wParam;
long lParam;
{
    switch (wm) {
    case WM_INITDIALOG:
            SetDlgItemText(hdlg, idtxtApp, (LPSTR)szApp);
            SetDlgItemText(hdlg, idtxtDoc, (LPSTR)szDoc);
            break;
    case WM_COMMAND:
            switch (wParam) {
            case IDOK:
                    GetDlgItemText(hdlg, idtxtApp, (LPSTR)szApp, sizeof(szApp));
                    GetDlgItemText(hdlg, idtxtDoc, (LPSTR)szDoc, sizeof(szDoc));
                    EndDialog(hdlg, TRUE);
                    break;
            case IDCANCEL:
                    EndDialog(hdlg, FALSE);
                    break;
            }
    default:
            return(FALSE);
    }
    return(TRUE);
}

/* ----------------------------------------------------- */

BOOL far pascal RequestDlg(hdlg, wm, wParam, lParam)
HWND hdlg;
unsigned wm;
int wParam;
long lParam;
{
   HWND hctl;
   int icf;

   switch (wm) {
   case WM_INITDIALOG:
           ChnlBox(hdlg, idlboxChnl);
           ClipFmtBox(hdlg, idlboxFmt);
           SetDlgItemText(hdlg, idtxtRef, (LPSTR)szRef);
           SendDlgItemMessage(hdlg, idlboxChnl, LB_SETCURSEL, 0, 0L);
           return(TRUE);
   case WM_COMMAND:
           switch (wParam) {
           case IDOK:
                   if ((iclCur = IclListBox(hdlg, idlboxChnl)) == -1)
                           return(TRUE);
                   if ((icf = SendMessage(GetDlgItem(hdlg, idlboxFmt),
                                   LB_GETCURSEL, 0, 0L)) == -1)
                           return(TRUE);
                   GetDlgItemText(hdlg, idtxtRef, (LPSTR)szRef, sizeof(szRef));
                   cfCur = rgcf[icf];
                   EndDialog(hdlg, TRUE);
                   return(TRUE);
           case IDCANCEL:
                   EndDialog(hdlg, FALSE);
                   return(TRUE);
           default:
                   return(FALSE);
           }
   default:
           return(FALSE);
   }
   return(TRUE);
}


/* ----------------------------------------------------- */
void LinkBox(hdlg, idlbox, icl)
HWND hdlg;
int idlbox;
int icl;
{
    int idl;
    PLDL *ppldl;
    HWND hctl;
    char sz[256];

    hctl = GetDlgItem(hdlg, idlbox);
    SendMessage(hctl, LB_RESETCONTENT, 0, 0L);
    ppldl = (PLDL *)LocalLock((HANDLE)hpldl);
    for (idl = 0; idl < ppldl->idlMax; idl++) {
        if (!ppldl->rgdl[idl].fUsed || ppldl->rgdl[idl].icl != icl)
                continue;
        GlobalGetAtomName(ppldl->rgdl[idl].atomRef, (LPSTR)sz, sizeof(sz));
        SendMessage(hctl, LB_ADDSTRING, 0, (DWORD)(LPSTR)sz);
        SendMessage(hctl, LB_GETCOUNT, 0, 0L);
    }
    LocalUnlock((HANDLE)hpldl);
}


/* ----------------------------------------------------- */

BOOL far pascal UnadviseDlg(hdlg, wm, wParam, lParam)
HWND hdlg;
unsigned wm;
int wParam;
long lParam;
{
    HWND hctl;
    int il;

    switch (wm) {
    case WM_INITDIALOG:
            ChnlBox(hdlg, idlboxChnl);
            SendDlgItemMessage(hdlg, idlboxChnl, LB_SETCURSEL, 0, 0L);
            return(TRUE);
    case WM_COMMAND:
            switch (wParam) {
            case IDOK:
                    if ((iclCur = IclListBox(hdlg, idlboxChnl)) == -1)
                            return(TRUE);
                    if ((il = SendMessage(hctl = GetDlgItem(hdlg, idlboxRef),
                                    LB_GETCURSEL, 0, 0L)) == -1)
                            return(TRUE);
                    SendMessage(hctl, LB_GETTEXT, il, (DWORD)(LPSTR)szRef);
                    EndDialog(hdlg, TRUE);
                    return(TRUE);
            case IDCANCEL:
                    EndDialog(hdlg, FALSE);
                    return(TRUE);
            case idlboxChnl:
                    if (HIWORD(lParam) == LBN_SELCHANGE) {
                            if ((iclCur = IclListBox(hdlg, idlboxChnl)) == -1)
                                    return(TRUE);
                            LinkBox(hdlg, idlboxRef, iclCur);
                            return(TRUE);
                    }
                    return(FALSE);
            default:
                    return(FALSE);
            }
        default:
            return(FALSE);
    }
    return(TRUE);
}


/* ----------------------------------------------------- */

BOOL far pascal ExecuteDlg(hdlg, wm, wParam, lParam)
HWND hdlg;
unsigned wm;
int wParam;
long lParam;
{
   switch (wm) {
   case WM_INITDIALOG:
           ChnlBox(hdlg, idlboxChnl);
           SetDlgItemText(hdlg, idtxtRef, (LPSTR)szRef);
           SendDlgItemMessage(hdlg, idlboxChnl, LB_SETCURSEL, 0, 0L);
           break;
   case WM_COMMAND:
           switch (wParam) {
           case IDOK:
                   GetDlgItemText(hdlg, idtxtRef, (LPSTR)szRef, sizeof(szRef));
                   if ((iclCur = IclListBox(hdlg, idlboxChnl)) == -1)
                           return(TRUE);
                   EndDialog(hdlg, TRUE);
                   return(TRUE);
           case IDCANCEL:
                   EndDialog(hdlg, FALSE);
                   return(TRUE);
           }
   default:
           return(FALSE);
   }
   return(TRUE);
}

/* ----------------------------------------------------- */

BOOL far pascal PokeDlg(hdlg, wm, wParam, lParam)
HWND hdlg;
unsigned wm;
int wParam;
long lParam;
{
   switch (wm) {
   case WM_INITDIALOG:
           ChnlBox(hdlg, idlboxChnl);
           SetDlgItemText(hdlg, idtxtRef, (LPSTR)szRef);

           CheckRadioButton(hdlg, idradioText, idradioFile, idradioText);
           iRadioButtonTemp = idradioText;

           /* file is not implemented so it is disabled */
           EnableWindow( GetDlgItem(hdlg,idradioFile), FALSE );

           SendDlgItemMessage(hdlg, idlboxChnl, LB_SETCURSEL, 0, 0L);

           break;

   case WM_COMMAND:
           switch (wParam) {
           case IDOK:

                  iResult = GetDlgItemText(hdlg, idtxtData,
                                           (LPSTR)szData, sizeof(szData));
                  iResult = GetDlgItemText(hdlg, idtxtRef,
                                           (LPSTR)szRef, sizeof(szRef));
                  if ((iclCur = IclListBox(hdlg, idlboxChnl)) == -1)
                     return(TRUE);

                  iRadioButton = iRadioButtonTemp;

                  /* verify contents of Poke message */
                  /* if the button is csv
                        and the ref is < 5 characters
                        then if the data has a comma in it
                                then display a warning message
                   */

                  if ( iRadioButtonTemp == idradioCsv && iResult<5 ) {
                     /* check for comma in data */
                     MessageBox( GetFocus(), "Ref may be too small",
                                             "Warning", MB_OK);

                     }

                   EndDialog(hdlg, TRUE);
                   return(TRUE);
           case IDCANCEL:
                   EndDialog(hdlg, FALSE);
                   return(TRUE);

           case idradioText:
           case idradioCsv:
           case idradioFile:
                   CheckRadioButton(hdlg, idradioText, idradioFile,
                      (iRadioButtonTemp = wParam));
                   return(TRUE);
                   break;

           }
   default:
           return(FALSE);
   }
   return(TRUE);
}





/* ----------------------------------------------------- */

BOOL far pascal TermDlg(hdlg, wm, wParam, lParam)
HWND hdlg;
unsigned wm;
int wParam;
long lParam;
{
   switch (wm) {
   case WM_INITDIALOG:
           ChnlBox(hdlg, idlboxChnl);
           SendDlgItemMessage(hdlg, idlboxChnl, LB_SETCURSEL, 0, 0L);
           break;
   case WM_COMMAND:
           switch (wParam) {
           case IDOK:
                   if ((iclCur = IclListBox(hdlg, idlboxChnl)) == -1)
                           return(TRUE);
                   EndDialog(hdlg, TRUE);
                   break;
           case IDCANCEL:
                   EndDialog(hdlg, FALSE);
                   break;
           default:
                   break;
           }
   default:
           return(FALSE);
   }
   return(TRUE);
}


/* -----------------------------------------------------
 *      RemoveLinks
 *
 *      Removes all a channel's links from our data structures.
 *
 *      Arguments:
 *              icl             channel to remove links from
 *
 *      Returns:
 *              nothing.
 */
void RemoveLinks(icl)
int icl;
{
    int idl;
    PLDL *ppldl;

    ppldl = (PLDL *)LocalLock((HANDLE)hpldl);
    for (idl = 0; idl < ppldl->idlMax; idl++) {
            if (ppldl->rgdl[idl].fUsed && ppldl->rgdl[idl].icl == icl)
                    ppldl->rgdl[idl].fUsed = FALSE;
    }
    LocalUnlock((HANDLE)hpldl);
}



/* -----------------------------------------------------
 *      AddPl
 *
 *      Adds an item to a PL
 *
 *      Arguments:
 *              hpl
 *              pbData
 *
 *      Returns:
 *              Nothing.
 */
void AddPl(hpl, pbData)
PL **hpl;
BYTE *pbData;
{
   PL *ppl;
   BYTE *pb;
   int cbItem;
   int i, iMax;
   int dAlloc;

   ppl = (PL *)LocalLock((HANDLE)hpl);
   cbItem = ppl->cbItem;
   iMax = ppl->iMax;
   for (i = 0, pb = ppl->rg; i < iMax; i++, pb += cbItem) {
           if (!*(int *)pb)
                   goto Return;
   }
   dAlloc = ppl->dAlloc;
   LocalUnlock((HANDLE)hpl);
   LocalReAlloc((HANDLE)hpl, sizeof(PL)-1 + (iMax + dAlloc) * cbItem,
                   LMEM_MOVEABLE|LMEM_ZEROINIT);
   ppl = (PL *)LocalLock((HANDLE)hpl);
   ppl->iMax = iMax + dAlloc;
   pb = &ppl->rg[cbItem * iMax];
Return:
   bltbx((BYTE far *)pbData, (BYTE far *)pb, cbItem);
   *(int *)pb = TRUE;
   LocalUnlock((HANDLE)hpl);
}



/* -----------------------------------------------------
 *      HplInit
 *
 *      Initializes a plex
 *
 *      Arguments:
 *              cbItem          sizeof item to put in plex
 *
 *      Returns:
 *              handle to new plex
 */
PL **HplInit(cbItem)
int cbItem;
{
   PL **hpl, *ppl;

   hpl = (PL **)LocalAlloc(LMEM_MOVEABLE|LMEM_ZEROINIT, sizeof(PL)-1);
   ppl = (PL *)LocalLock((HANDLE)hpl);
   ppl->iMax = 0;
   ppl->dAlloc = 5;
   ppl->cbItem = cbItem;
   LocalUnlock((HANDLE)hpl);
   return(hpl);
}


/* -------------------------------------------------
 *      RemovePl
 *
 *      Removes an item from a plex
 *
 *      Arguments:
 *              hpl
 *              i
 *
 *      Returns:
 *              nothing.
 */
void RemovePl(hpl, i)
PL **hpl;
int i;
{
   PL *ppl;

   ppl = (PL *)LocalLock((HANDLE)hpl);
   *(int *)&ppl->rg[i * ppl->cbItem] = FALSE;
   LocalUnlock((HANDLE)hpl);
}


/* ----------------------------------------------------
 *      GetPl
 *
 *      gets an item from a plex
 *
 *      Arguments:
 *              hpl
 *              i
 *              pb
 *
 *      Returns:
 *              nothing.
 */
void GetPl(hpl, i, pb)
PL **hpl;
int i;
BYTE *pb;
{
    PL *ppl;

    ppl = (PL *)LocalLock((HANDLE)hpl);
    bltbx((BYTE far *)&ppl->rg[i * ppl->cbItem], (BYTE far *)pb, ppl->cbItem);
    LocalUnlock((HANDLE)hpl);
}


/* -----------------------------------------------------
 *      SendInitiate
 *
 *      Broadcasts WM_DDE_INITIATE message for the given reference string
 *
 *      Arguments:
 *              szApp
 *              szDoc
 *
 *      Returns:
 *              Nothing
 */
void SendInitiate(szApp, szDoc)
char *szApp;
char *szDoc;
{
    ATOM atomApp, atomDoc;

    atomApp = *szApp == 0 ? NULL : GlobalAddAtom((LPSTR)szApp);
    atomDoc = *szDoc == 0 ? NULL : GlobalAddAtom((LPSTR)szDoc);
    iAck = FALSE;        /* no conversation is active */
    fInInitiate = TRUE;
    SendMessage((HWND)-1, WM_DDE_INITIATE, hwndDDE, MAKELONG(atomApp, atomDoc));
    fInInitiate = FALSE;
    if (atomApp != NULL)
            GlobalDeleteAtom(atomApp);
    if (atomDoc != NULL)
            GlobalDeleteAtom(atomDoc);
    if (!iAck)
      MessageBox( GetFocus(), "Nobody wants to talk.", "Warning", MB_OK);

}


/* -----------------------------------------------------
 *      SendTerminate
 *
 *      Arguments:
 *              icl
 *
 *      Returns:
 *              Nothing.
 */
void SendTerminate(icl)
int icl;
{
#ifdef BOGUS
    PostClientMsg(icl, WM_DDE_UNADVISE, 0, 0);
    if (!FGetAck(icl)) {
            Error("Terminate aborted:  Unadvise failed");
            return;
    }
#endif
    PostClientMsg(icl, WM_DDE_TERMINATE, 0, 0);
    RemoveLinks(icl);
    RemovePl(hplcl, icl);
}


/* -----------------------------------------------------
 *      SendRequest
 *
 *      Sends a WM_DDE_REQUEST message
 *
 *      Arguments:
 *              icl
 *              cf
 *              szRef
 *
 *      Returns:
 *              Nothing.
 */
void SendRequest(icl, cf, szRef)
int icl;
int cf;
char *szRef;
{
   ATOM atom;

   atom = GlobalAddAtom((LPSTR)szRef);
   PostClientMsg(icl, WM_DDE_REQUEST, atom, cf);
}


/* -----------------------------------------------------
 *      SendAdvise
 *
 *      Sends WM_DDE_ADVISE message to establish hot link
 *
 *      Arguments:
 *              icl
 *              cf
 *              szRef
 *
 *      Returns:
 *              Nothing.
 */
void SendAdvise(icl, cf, szRef)
int icl;
int cf;
char *szRef;
{
   ATOM atom;
   HANDLE hDdeln;
   DDELN far *lpddeln;
   BOOL fSuccess;
   DL dl;

   atom = GlobalAddAtom((LPSTR)szRef);
   hDdeln = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (DWORD)sizeof(DDELN));
   lpddeln = (DDELN far *)GlobalLock(hDdeln);
   lpddeln->cf = cf;
   lpddeln->fAckReq = FALSE;
   lpddeln->fDeferUpd = FALSE;
   GlobalUnlock(hDdeln);
   PostClientMsg(icl, WM_DDE_ADVISE, atom, hDdeln);
   if (!FGetAck(icl)) {
      Error("Advise Failed");
      GlobalFree(hDdeln);
      /* return; */
   }
   else {
     dl.icl = icl;
     dl.atomRef = atom;
     AddPl(hpldl, &dl);
   }
}


/* -----------------------------------------------------
 *      SendUnadvise
 *
 *      Breaks a link by sending an WM_DDE_UNADVISE.
 *
 *      Arguments:
 *              icl
 *              szRef
 *
 *      Returns:
 *              Nothing.
 */
void SendUnadvise(icl, szRef)
int icl;
char *szRef;
{
   ATOM atom;
   PLDL *ppldl;
   int idl;

   if (*szRef == 0) {
      PostClientMsg(icl, WM_DDE_UNADVISE, NULL, NULL);
      if (!FGetAck(icl)) {
         Error("Unadvise Failed");
         return;
      }
      RemoveLinks(icl);
      return;
   }
   atom = GlobalAddAtom((LPSTR)szRef);
   ppldl = (PLDL *)LocalLock((HANDLE)hpldl);
   for (idl = 0; idl < ppldl->idlMax; idl++) {
      if (ppldl->rgdl[idl].fUsed &&
          ppldl->rgdl[idl].atomRef == atom &&
          ppldl->rgdl[idl].icl == icl) {
         PostClientMsg(icl, WM_DDE_UNADVISE, atom, NULL);
         if (!FGetAck(icl)) {
            Error("Unadvise Failed");
            break;
         }
         GlobalDeleteAtom(atom);
         RemovePl(hpldl, idl);
         break;
      }
   }
   LocalUnlock((HANDLE)hpldl);
}


/* -----------------------------------------------------
 *      SendExecute
 *
 *      Sends a WM_DDE_EXECUTE.
 *
 *      Arguments:
 *              icl
 *              szExec
 *
 *      Returns:
 *              Nothing.
 */
void SendExecute(icl, szExec)
int icl;
char *szExec;
{
   int cch;
   char far *lpsz;
   HANDLE hDde;

   cch  = CchSzLen((char far *)szExec);
   hDde = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (DWORD)cch+1);
   lpsz = (char far *)GlobalLock(hDde);
   bltbx((char far *)szExec, lpsz, cch+1);
   GlobalUnlock(hDde);
   PostClientMsg(icl, WM_DDE_EXECUTE, hDde, NULL);
   if (!FGetAck(icl))
           Error("Execute Failed");
   GlobalFree(hDde);
}

/* -----------------------------------------------------
 *      SendPoke
 *
 *      Sends a WM_DDE_POKE
 *
 *      Arguments:
 *              icl
 *              szRef
 *              szPoke
 *
 *      Returns:
 *              Nothing.
 */
void SendPoke(icl, szRef, szPoke)
int icl;
char *szRef;
char *szPoke;
{
   int cch;
   DDEUP far *lpddeup;
   ATOM atom;
   HANDLE hDdeup;

   switch(iRadioButton) {

     case idradioText:
          MessageBox( GetFocus(), "Text", "Sending", MB_OK);
          break;

     case idradioCsv:
          MessageBox( GetFocus(), "Csv" , "Sending", MB_OK);
          break;

     case idradioFile:
          MessageBox( GetFocus(), "File" , "Sending", MB_OK);
          break;

     default:
          MessageBox( GetFocus(), "Unknown" , NULL, MB_OK);
          break;
   }

   atom   = GlobalAddAtom((LPSTR)szRef);
   cch    = CchSzLen((char far *)szPoke);
   hDdeup = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (DWORD)sizeof(DDEUP)+cch);
   lpddeup            = (DDEUP far *)GlobalLock(hDdeup);
   lpddeup->cf        = (iRadioButton == idradioText) ? CF_TEXT : rgcf[icfCSV];
   lpddeup->fAckReq   = FALSE;
   lpddeup->fRelease  = TRUE;
   bltbx((char far *)szPoke, lpddeup->rgb, cch+1);
   GlobalUnlock(hDdeup);
   PostClientMsg(icl, WM_DDE_POKE, atom, hDdeup);
   if (!FGetAck(icl))
           Error("Poke Failed");
}

/* -----------------------------------------------------
 *      BenchPoke
 *
 *      Sends a whole bunch of WM_DDE_POKEs
 *
 *      Arguments:
 *              icl
 *              szRef
 *              szPoke
 *
 *      Returns:
 *              Nothing.
 */
void BenchPoke(icl, szRef, szPoke)
int icl;
char *szRef;
char *szPoke;
{
  int i;
  int cch;
  DDEUP far *lpddeup;
  ATOM atom;
  HANDLE hDdeup;

  Error("Start");
  for (i=0; i<10000; i++)
          {
          PostClientMsg(icl, WM_DDE_TEST, NULL, NULL);
          if (!FGetAck(icl))
                  Error("No Ack Received");
          }
  Error("Stop");
}


/* ----------------------------------------------------- */
int Waste(hwnd, wm, wParam, lParam)
HWND hwnd;
int wm;
int wParam;
long lParam;
{
        return(TRUE);
}


/* ----------------------------------------------------- */
void BenchCall()
{
  long l;

  Error("Start");
  for (l=0; l<1000000; l++)
          if (!Waste(1234, WM_DDE_TEST, 5678, 0L))
                  Error("No Ack Received");
  Error("Stop");
}


/* -----------------------------------------------------
 *      FGetAck
 *
 *      Waits for an ACK message to be returned
 *
 *      Arguments:
 *              icl             channel to receive ACK from
 *
 *      Returns:
 *              TRUE if affirmative ACK received within a fairly long
 *              time period
 *
 *      Note:
 *              This method is not recommended for large applications.
 */
BOOL FGetAck(icl)
HWND icl;
{
    MSG msg;
    int tid;
    HWND hwndInt, hwndExt;
    BOOL fAck;
#define msAckWait 60000 /* one minute */

    hwndExt = (*hplcl)->rgcl[icl].hwndExt;
    hwndInt = (*hplcl)->rgcl[icl].hwndInt;
#ifdef TIMER
    tid = SetTimer(NULL, 0, msAckWait, NULL);
#endif
    fAck = FALSE;
    for (;;) {
       GetMessage((MSG far *)&msg, NULL, 0, 0);
       if (msg.message == WM_DDE_ACK && msg.wParam == hwndExt)
          {
          if (LOWORD(msg.lParam) & 0x8000)
                  fAck = TRUE;
          if (HIWORD(msg.lParam)>= 0xc000)
                  GlobalDeleteAtom(HIWORD(msg.lParam));
          break;
          }
       TranslateMessage((MSG far *)&msg);
       DispatchMessage((MSG far *)&msg);
#ifdef TIMER
       if (PeekMessage((MSG far *)&msg, NULL,
                           WM_TIMER, WM_TIMER, PM_REMOVE) &&
                       msg.wParam == tid)
               break;
#endif
       WaitMessage();
    }
#ifdef TIMER
    KillTimer(NULL, tid);
#endif
    return(fAck);
}




/* -----------------------------------------------------
 * P A S T E  L I N K 
 *
 *  Get the paste link string from the clipboard and display it.
 *
*/

void PasteLink ()
{
   int  isp;
   char far *lpch;
   char far *lpchT;
   RECT rect;
   HDC  hdc;
   HANDLE hClipboard;

   if (!OpenClipboard(hwndDDE))
       {
       Error("Cannot open clipboard");
       return;
       }

   if ((hClipboard = GetClipboardData(cfLink)) == NULL)
       {
       Error("Cannot render clipboard in Link format");
       }
   else
       {
       hdc = GetDC(hwndDDE);
       lpch = GlobalLock(hClipboard);
       GetClientRect(hwndDDE, (LPRECT)&rect);
       hbrBack = SelectObject(hdc, hbrBack);
       PatBlt(hdc, rect.left, rect.top, rect.right - rect.left,
                rect.bottom - rect.top, PATCOPY);
       SelectObject(hdc, hbrBack);
       for (isp=0, lpchT = lpch; isp < 2; lpchT++)
               if (!*lpchT)
                       {
                       *lpchT = ' ';
                       isp++;
                       }
       DrawText(hdc, lpch, CchSzLen(lpch), (LPRECT)&rect,
               DT_LEFT|DT_EXPANDTABS);
       ReleaseDC(hwndDDE, hdc);
       GlobalUnlock(hClipboard);
       }

   CloseClipboard();
}


/* -----------------------------------------------------
 *      DoDialog
 *
 *      Brings up a dialog.
 *
 *      Arguments:
 *              idd             dialog index to bring up
 *              lpfn            dialog proc to use
 *
 *      Returns:
 *              Dialog return code
 */
int FDoDialog(idd, lpfn)
int idd;
FARPROC lpfn;
{
   int w;

   lpfn = MakeProcInstance(lpfn, hinstDDE);
   w = DialogBox(hinstDDE, MAKEINTRESOURCE(idd), hwndDDE, lpfn);
   FreeProcInstance(lpfn);
   return(w);
}


/* ----------------------------------------------------
 *      ClipFmtBox
 *
 *      Initializes clipboard format dialog list box.
 *
 *      Arguments:
 *              hdlg
 *              idlbox
 *
 *      Returns:
 *              nothing.
 */
void ClipFmtBox(hdlg, idlbox)
HWND hdlg;
int idlbox;
{
    HWND hctl;
    int icf;
    int cf;
    char sz[256];

    hctl = GetDlgItem(hdlg, idlbox);
    for (icf = 0; icf < icfMax; icf++) {
        switch (cf = rgcf[icf]) {
        case CF_TEXT:
                bltbx((LPSTR)"CF_TEXT", (LPSTR)sz, 8);
                break;
        case CF_BITMAP:
                bltbx((LPSTR)"CF_BITMAP", (LPSTR)sz, 10);
                break;
        case CF_METAFILEPICT:
                bltbx((LPSTR)"CF_METAFILEPICT", (LPSTR)sz, 12);
                break;
        case CF_SYLK:
                bltbx((LPSTR)"CF_SYLK", (LPSTR)sz, 8);
                break;
        case CF_DIF:
                bltbx((LPSTR)"CF_DIF", (LPSTR)sz, 7);
                break;
        default:
                sz[GetClipboardFormatName(cf, (LPSTR)sz, sizeof(sz))] = 0;
                break;
        }
        SendMessage(hctl, LB_ADDSTRING, 0, (DWORD)(LPSTR)sz);
    }
}


/* ----------------------------------------------------
 *      ChnlBox
 *
 *      Initializes channel dialog list box.
 *
 *      Arguments:
 *              hdlg
 *              idlbox
 *
 *      Returns:
 *              nothing.
 */
void ChnlBox(hdlg, idlbox)
HWND hdlg;
int idlbox;
{
    HWND hctl;
    PLCL *pplcl;
    int icl;
    HWND hwndExt;
    char sz[256];

    hctl = GetDlgItem(hdlg, idlbox);
    pplcl = (PLCL *)LocalLock((HANDLE)hplcl);
    for (icl = 0; icl < pplcl->iclMax; icl++) {
        if (!pplcl->rgcl[icl].fUsed)
                continue;
        hwndExt = pplcl->rgcl[icl].hwndExt;
        GetWindowText(hwndExt, (LPSTR)sz, sizeof(sz));
        SendMessage(hctl, LB_ADDSTRING, 0, (DWORD)(LPSTR)sz);
    }
    LocalUnlock((HANDLE)hplcl);
}


/* ----------------------------------------------------
 *      IclListBox
 *
 *      Given a listbox, returns the icl for the channel selected in that
 *      listobx.
 *
 *      Arguments:
 *              hdlg            dialog handle
 *              idlbox          listbox item index.
 *
 *      Returns:
 *              channel index
 */
int IclListBox(hdlg, idlbox)
HWND hdlg;
int idlbox;
{
    int il, icl;
    PLCL *pplcl;

    if ((il = SendMessage(GetDlgItem(hdlg, idlbox), LB_GETCURSEL, 0, 0L)) == -1)
            return(-1);
    pplcl = (PLCL *)LocalLock((HANDLE)hplcl);
    for (icl = 0; icl < pplcl->iclMax; icl++) {
            if (pplcl->rgcl[icl].fUsed && il-- == 0)
                    goto Return;
    }
    icl = -1;
Return:
    LocalUnlock((HANDLE)hplcl);
    return(icl);
}

/* ---------------------------------------------------- */

void PostClientMsg(icl, wm, atom, cf)
int icl;
int wm;
ATOM atom;
int cf;
{
    HWND hwndExt;
    PLCL *pplcl;

    pplcl = (PLCL *)LocalLock((HANDLE)hplcl);
    hwndExt = pplcl->rgcl[icl].hwndExt;
    PostMessage(hwndExt, wm,
            (WORD)pplcl->rgcl[icl].hwndInt,
            MAKELONG(cf, atom));
    LocalUnlock((HANDLE)hplcl);
}


int cxScrollLast;
int cyScrollLast;

/* ---------------------------------------------------- */
FShowMetaFilePict( hdc, prc, hmfp, cxScroll, cyScroll )
HDC      hdc;
register PRECT  prc;
HANDLE   hmfp;
int      cxScroll, cyScroll;
{       /* Display a metafile picture in the passed rectangle.
           { cxScroll, cyScroll } is the origin within the picture at which
                        to start displaying.
           Set cxScrollLast, cyScrollLast if they have not been set already */
   HDC hMemDC;
   HBITMAP hbm;
   LPMETAFILEPICT lpmfp;
   METAFILEPICT mfp;
   int mmT;
   POINT pt, ptT;
   int level;
   int cxRect = prc->right - prc->left;
   int cyRect = prc->bottom - prc->top;
   int cxBitmap;
   int cyBitmap;
   int f=FALSE;

   if ((lpmfp=(LPMETAFILEPICT)GlobalLock( hmfp )) != NULL)
      {
      int mm;
      METAFILEPICT mfp;

      mfp = *lpmfp;
      GlobalUnlock( hmfp );

      if ((level = SaveDC( hdc )) != 0)
         {
         /* Compute size of picture to be displayed */

         switch (mfp.mm)
                 {
         case MM_ISOTROPIC:
                 goto NoScroll;
         case MM_ANISOTROPIC:
                 if (mfp.xExt > 0 && mfp.yExt > 0)
                         {
                         /* suggested a size */
                         mm = MM_HIMETRIC;
                         goto GetSize;
                         }
NoScroll:
                 /* no suggested sizes, use the window size */
                 cyScrollLast = 0;   /* Not scrollable; resizes into avail. rect */
                 cxScrollLast = 0;
                 cxBitmap = prc->right - prc->left;
                 cyBitmap = prc->bottom - prc->top;
                 break;

         default:
                 mm = mfp.mm;
GetSize:
                 mmT = SetMapMode(hdc, mm);
                 *((long *)&ptT) = SetWindowOrg(hdc, 0, 0);
                 pt.x = mfp.xExt;
                 pt.y = mfp.yExt;
                 LPtoDP(hdc, (LPPOINT)&pt, 1);
                 cxBitmap = pt.x;
                 cyBitmap = (mm == MM_TEXT) ? pt.y : -pt.y;
                 SetMapMode(hdc, mmT);
                 SetWindowOrg(hdc, ptT.x, ptT.y);

CheckScroll:
                 if (cxScrollLast == -1)
                         {
                         cxScrollLast = cxBitmap - (prc->right - prc->left);
                         if ( cxScrollLast < 0)
                                 cxScrollLast = 0;
                         }
                 if (cyScrollLast == -1)
                         {
                         cyScrollLast = cyBitmap - (prc->bottom - prc->top);
                         if ( cyScrollLast < 0)
                                 cyScrollLast = 0;
                         }
                 break;
                 }

         /* We make the "viewport" to be an area the same size as the */
         /* clipboard object, and set the origin and clip region so as */
         /* to show the area we want. Note that the viewport may well be */
         /* bigger than the window. */
         IntersectClipRect( hdc, prc->left, prc->top,
                                 prc->right, prc->bottom );
         SetMapMode( hdc, mfp.mm );

         SetViewportOrg( hdc, prc->left - cxScroll, prc->top - cyScroll );
         switch (mfp.mm)
                 {
         case MM_ISOTROPIC:
                 if (mfp.xExt && mfp.yExt)
                         /* So we get the correct shape rectangle when
                         SetViewportExt gets called */
                         SetWindowExt( hdc, mfp.xExt, mfp.yExt );
                 break;

         case MM_ANISOTROPIC:
                 SetWindowExt(hdc, cxBitmap, cyBitmap);
                 SetViewportExt( hdc, cxBitmap, cyBitmap );
                 break;
                 }

         /* Since we may have scrolled, force brushes to align */
         SetBrushOrg( hdc, cxScroll - prc->left, cyScroll - prc->top );
         f = PlayMetaFile( hdc, mfp.hMF );

NoDisplay:
         RestoreDC( hdc, level );
         }
      }

return f;

}

/* ------------------------------------------------------
 *      DdeWndProc
 *
 *      Main window proc for the DDE application.
 *
 *      Arguments:
 *              hwnd            window receiving the message
 *              wm              the message
 *              wParam, lParam  additional info for the message
 *
 *      Returns:
 *              Depends on the message.
 */
long far pascal DdeWndProc(hwnd, wm, wParam, lParam)
HWND hwnd;
unsigned wm;
int wParam;
long lParam;
{
   int icl;
   PLCL *pplcl;
   char sz[256];
   char *pch;
   RECT rect;
   HDC hdc;
   DDEUP far *lpddeup;
   CL cl;
   HBITMAP hbmp;
   HDC hdcMem;
   BITMAP bmp;
   METAFILEPICT far *lpmfp;
   HANDLE hMF;
   BOOL fRelease;

   switch (wm) {

           case WM_DESTROY:
                   PostQuitMessage(0);
                   break;

           case WM_COMMAND:

                   switch (wParam) {

                   case idmInitiate:
                           if (FDoDialog(iddInitiate, InitiateDlg))
                                   SendInitiate(szApp, szDoc);
                           return(0L);

                   case idmTerminate:
                           if (FDoDialog(iddTerminate, TermDlg))
                                   SendTerminate(iclCur);
                           return(0L);

                   case idmRequest:
                           if (FDoDialog(iddRequest, RequestDlg))
                                   SendRequest(iclCur, cfCur, szRef);
                           return(0L);

                   case idmAdvise:
                           if (FDoDialog(iddAdvise, RequestDlg))
                                   SendAdvise(iclCur, cfCur, szRef);
                           return(0L);

                   case idmUnadvise:
                           if (FDoDialog(iddUnadvise, UnadviseDlg))
                                   SendUnadvise(iclCur, szRef);
                           return(0L);

                   case idmPoke:
                           if (FDoDialog(iddPoke, PokeDlg))
                                   SendPoke(iclCur, szRef, szData);
                           return(0L);

                   case idmExecute:
                           if (FDoDialog(iddExecute, ExecuteDlg))
                                   SendExecute(iclCur, szRef);
                           return(0L);

                   case idmBench:
                           if (FDoDialog(iddPoke, PokeDlg))
                                   BenchPoke(iclCur, szRef, szData);
                           return(0L);

                   case idmCBench:
                           BenchCall();
                           return(0L);

                   case idmPasteLink:
                           PasteLink();
                           return(0L);

                   default:
                           break;

                   }
                   break;

           case WM_DDE_TERMINATE:
                   pplcl = (PLCL *)LocalLock((HANDLE)hplcl);
                   for (icl = 0; icl < pplcl->iclMax; icl++) {
                           if (!pplcl->rgcl[icl].fUsed ||
                               pplcl->rgcl[icl].hwndExt != wParam)
                                   continue;
                           LocalUnlock((HANDLE)hplcl);
                           SendTerminate(icl);
                           return(0L);
                           }
                   LocalUnlock((HANDLE)hplcl);
                   return(0L);

           case WM_DDE_ACK:
                   if (fInInitiate) {
                           cl.hwndInt = hwnd;
                           cl.hwndExt = (HWND)wParam;
                           cl.fClient = TRUE;
                           AddPl(hplcl, &cl);
                           GlobalDeleteAtom(LOWORD(lParam));
                           iAck = TRUE;
                   } else {
                           if (LOWORD(lParam) & 0x8000)
                                   Error("Got ACK");
                           else
                                   Error("Got NACK");
                   }
                   GlobalDeleteAtom(HIWORD(lParam));
                   return(0L);

           case WM_DDE_DATA:
                   GetClientRect(hwndDDE, (LPRECT)&rect);
                   hdc = GetDC(hwndDDE);
                   hbrBack = SelectObject(hdc, hbrBack);
                   PatBlt(hdc, rect.left, rect.top,
                                   rect.right - rect.left,
                                   rect.bottom - rect.top,
                                   PATCOPY);
                   SelectObject(hdc, hbrBack);
                   lpddeup = (DDEUP far *)GlobalLock(LOWORD(lParam));
                   if (lpddeup->fAckReq)
                           PostMessage(wParam, WM_DDE_ACK,
                                   hwnd, MAKELONG(0x8000, HIWORD(lParam)));
                   else
                           GlobalDeleteAtom(HIWORD(lParam));
                   switch (lpddeup->cf)
                           {
                   default:
                           if (lpddeup->cf == rgcf[icfBitPrinter])
                                   goto GotBitmap;
                           if (lpddeup->cf == rgcf[icfMetaPrinter])
                                   goto GotMetafile;
                           if (lpddeup->cf != rgcf[icfRTF] &&
                                   lpddeup->cf != rgcf[icfCSV])
                                   break;
                           /* else fall through */
                   case CF_SYLK:
                   case CF_DIF:
                   case CF_TEXT:
                           DrawText(hdc, lpddeup->rgb,
                                           CchSzLen(lpddeup->rgb),
                                           (LPRECT)&rect,
                                           DT_LEFT|DT_EXPANDTABS);
                           break;
                   case CF_BITMAP:
GotBitmap:
                           hdcMem = CreateCompatibleDC(hdc);
                           hbmp = *(HBITMAP far *)&lpddeup->rgb[0];
                           GetObject(hbmp, sizeof(BITMAP), (BYTE far *)&bmp);
                           hbmp = SelectObject(hdcMem, hbmp);
                           BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight,
                                           hdcMem, 0, 0, SRCCOPY);
                           hbmp = SelectObject(hdcMem, hbmp);
                           DeleteDC(hdcMem);
                           if (lpddeup->fRelease)
                                   DeleteObject(hbmp);
                           break;
                   case CF_METAFILEPICT:
GotMetafile:
                           GetClientRect(hwndDDE, (LPRECT)&rect);
                           FShowMetaFilePict(hdc, &rect, 
                                     *((HANDLE far *)&lpddeup->rgb[0]),
                                             0, 0);
                           if (lpddeup->fRelease)
                                   DeleteMetaFile(*((HANDLE far *)&lpddeup->rgb[0]));
                           break;
                   }
                   ReleaseDC(hwndDDE, hdc);
                   fRelease = lpddeup->fRelease;
                   GlobalUnlock(LOWORD(lParam));
                   if (fRelease)
                           GlobalFree(LOWORD(lParam));
                   return(0L);

           default:
                   break;

   }
   return(DefWindowProc(hwnd, wm, wParam, lParam));
}


