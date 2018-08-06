/***	file.C - standard file dialogs
 *
 */

#define INCL_DOSERRORS
#define INCL_WINCOMMON
#define INCL_WINBUTTONS
#define INCL_WINDIALOGS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINLISTBOXES
#define INCL_WINMENUS
#define INCL_WINMESSAGEMGR

#include <os2.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fileid.h"
#include "file.h"
#include "mem.h"

#define dbg(x)	x

typedef struct {    /* fctl */
    char    *pszTitle;			// Dialog box title
    char    *pszPattern;		// wild-card file pattern
    char    achPath[CCHMAXPATH];	// full path of file
} FILECTL;
typedef FILECTL *PFILECTL;

MRESULT EXPENTRY FileDlgProc(HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2);

USHORT		beginSearch(char *psz, USHORT *pattr);
BOOL		changeDir(HWND hwndDlg, PFILECTL pfctl);
BOOL		changeDrive(HWND hwndDlg, PFILECTL pfctl);
BOOL		changeFile(HWND hwndDlg, PFILECTL pfctl);
void		endSearch(void);
USHORT		nextSearch(char *psz, USHORT *pattr);
USHORT		niceWinDlgBox(HWND hwndParent, HWND hwndOwner, PFNWP pfnDlgProc,
			    HMODULE hmod, USHORT idDlg, PVOID pCreateParams);
BOOL		setCurDir(HWND hwndDlg);
BOOL		setDir(HWND hwndDlg);
BOOL		setDrive(HWND hwndDlg);
BOOL		setFile(HWND hwndDlg);
char *		trimBlanks(char * psz);


static BOOL	fDidOK;			// true if OK was just pressed

static HDIR	hdir;			// directory search handle

static USHORT	itDrive;		// last item selected in Drive LB
static USHORT	itDir;			// last item selected in Dir LB
static USHORT	itFile;			// last item selected in File LB


char *FileOpen(HWND hwndOwner,char *pszTitle,char *pszPattern)
{
    BOOL    fOK;
    FILECTL fctl;

    fctl.pszTitle = pszTitle;
    fctl.pszPattern = pszPattern;

    fOK = niceWinDlgBox(HWND_DESKTOP, hwndOwner, FileDlgProc,
			    NULL, IDD_FILE, &fctl);

    if (fOK)
	return MemStrDup(fctl.achPath);
    else
	return NULL;
}


USHORT	niceWinDlgBox(HWND hwndParent, HWND hwndOwner, PFNWP pfnDlgProc,
			    HMODULE hmod, USHORT idDlg, PVOID pCreateParams)
{
    HAB	    hab;
    HWND    hwndDlg;
    USHORT  us;
    SWP	    swp;
    SWP	    swpDesk;

    hwndDlg = WinLoadDlg(hwndParent, hwndOwner, pfnDlgProc,
			    hmod, idDlg, pCreateParams);

    WinQueryWindowPos(hwndDlg,&swp);  // get window position
    WinQueryWindowPos(HWND_DESKTOP,&swpDesk); // get desktop

    // center dialog box on screen

    swp.x = (swpDesk.cx - swp.cx) >> 1;
    swp.y = (swpDesk.cy - swp.cy) >> 1;

    hab = WinQueryAnchorBlock(hwndDlg);
    WinSetMultWindowPos(hab,&swp,1);

    us = WinProcessDlg(hwndDlg);
    WinDestroyWindow(hwndDlg);

    return us;
}

/***	FileDlgProc - "File" Dialog Procedure
*
*/
MRESULT EXPENTRY FileDlgProc(HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2)
{
    static char	    ach[CCHMAXPATH];
	   USHORT   attr;
    static HWND	    hwndFocus;
    static HWND	    hwndLBDir;
    static HWND	    hwndLBDrive;
    static HWND	    hwndLBFile;
    static HWND	    hwndText;
    static USHORT   idLB;
    static USHORT   idLBPrevious;
	   USHORT   it;
    static PFILECTL pfctl;
	   USHORT   rc;

    switch (msg) {

    case WM_INITDLG:
	pfctl = (PFILECTL)(VOID *)SHORT1FROMMP(mp2);	// set file control block

	hwndLBDir   = WinWindowFromID(hwnd, IDL_DIR);
	hwndLBDrive = WinWindowFromID(hwnd, IDL_DRIVE);
	hwndLBFile  = WinWindowFromID(hwnd, IDL_FILE);
	hwndText = WinWindowFromID(hwnd, IDC_TEXT);
	idLBPrevious = 0;		// no previous focus

	WinSetWindowText(hwnd, pfctl->pszTitle);  // set dialog title
	WinSetWindowText(hwndText, pfctl->pszPattern);	// init file pattern

	setDrive(hwnd);			// init drive list box
	setDir(hwnd);			// update dir list
	setFile(hwnd);			// update file list
	setCurDir(hwnd);		// init current drive/dir string

	WinDefDlgProc(hwnd, msg, mp1, mp2); // do default stuff

	hwndFocus = hwndText;
	WinSetFocus(HWND_DESKTOP,hwndFocus);
	return (MRESULT) TRUE;

    case WM_COMMAND:
	switch (LOUSHORT(mp1)) {

	case DID_OK:			// store updated list
	    dbg( printf("WM_COMMAND: DID_OK\n") );
	    WinQueryWindowText(hwndText, CCHMAXPATH, ach);
	    trimBlanks(ach);	// Trim leading/trailing blanks
	    WinSetWindowText(hwndText, ach);
	    dbg( printf("   opening %s\n",ach) );
	    rc = DosQFileMode(ach,&attr,0L); // Does file exist?
	    dbg( printf("   DosQFileMode rc=%d\n",rc) );

	    // If directory is specified, pretend file not found

	    if (attr & FILE_DIRECTORY)
		rc = ERROR_FILE_NOT_FOUND;

	    fDidOK = TRUE;
	    switch (rc) {

	    case NO_ERROR: {
		USHORT	cch=CCHMAXPATH;

		rc = DosQPathInfo(
		    ach,		// Path specifiec
		    FIL_QUERYFULLNAME,	// Get fully-qualified name
		    pfctl->achPath,	// Return buffer
		    CCHMAXPATH,		// Return buffer size
		    0L			// Reserved
		    );
		if (rc == 0)
		    WinDismissDlg(hwnd,TRUE); // return SUCCESS
		}
		return FALSE;

	    case ERROR_FILE_NOT_FOUND:
	    case ERROR_PATH_NOT_FOUND:
		// check for and process user wild-card pattern
		if (strcmp(ach,pfctl->pszPattern) != 0)
		    setFile(hwnd);
		break;			// continue dialog

	    case ERROR_ACCESS_DENIED:
	    case ERROR_DRIVE_LOCKED:
	    case ERROR_NOT_DOS_DISK:
		WinMessageBox(HWND_DESKTOP, hwnd, "Access Denied",
			      "Error", NULL, MB_ICONEXCLAMATION);
		break;

	    default:
		break;
	    }
	    WinSetFocus(HWND_DESKTOP,hwndFocus);
	    break;

	case DID_CANCEL:
	    WinDismissDlg(hwnd, NULL);
	    break;
	}
	break;

    case WM_CONTROL:
	idLBPrevious = idLB;		// remember previous focus
	idLB = SHORT1FROMMP(mp1);
	dbg (printf("WM_CONTROL: id = %04x hwndLB=%08x\n",idLB,mp2) );
	switch (SHORT2FROMMP(mp1)) {
	    case LN_ENTER:
		switch (idLB) {

		case IDL_DRIVE:
		    dbg( printf("LN_ENTER: IDL_DRIVE\n") );
		    changeDrive(hwnd,pfctl);
		    return FALSE;

		case IDL_DIR:
		    dbg( printf("LN_ENTER: IDL_DIR\n") );
		    changeDir(hwnd,pfctl);
		    return FALSE;

		case IDL_FILE:
		    dbg( printf("LN_ENTER: IDL_FILE\n") );
		    changeFile(hwnd,pfctl);
		    WinSendMsg(hwnd,WM_COMMAND,MPFROMSHORT(DID_OK),NULL);
		    return FALSE;

		default:
		    dbg( printf("LN_ENTER: unknown list box = %04x\n",idLB) );
		    return WinDefDlgProc(hwnd, msg, mp1, mp2);
		    break;
		}
		break;

	    case LN_SELECT:
		switch (idLB) {

		case IDL_DRIVE:
		    dbg( printf("LN_SELECT: IDL_DRIVE\n") );
		    it = SHORT1FROMMR(WinSendMsg(hwndFocus, LM_QUERYSELECTION, 0L, 0L));
		    if (it != LIT_NONE) {
			if (fDidOK)
			    fDidOK = FALSE;
			else {
			    itDrive = it;
			    WinSendMsg(hwndFocus, LM_QUERYITEMTEXT,
				MPFROM2SHORT(it,CCHMAXPATH), MPFROMP(ach));
			    strcat(ach,pfctl->pszPattern);
			    WinSetWindowText(hwndText, ach);
			}
		    }
		    idLB = 0;		// allow LN_SETFOCUS to work
		    return FALSE;

		case IDL_DIR:
		    dbg( printf("LN_SELECT: IDL_DIR\n") );
		    it = SHORT1FROMMR(WinSendMsg(hwndFocus, LM_QUERYSELECTION, 0L, 0L));
		    if (it != LIT_NONE) {
			if (fDidOK)
			    fDidOK = FALSE;
			else {
			    itDir = it;
			    WinSendMsg(hwndFocus, LM_QUERYITEMTEXT,
				MPFROM2SHORT(it,CCHMAXPATH), MPFROMP(ach));
			    strcat(ach,"\\");
			    strcat(ach,pfctl->pszPattern);
			    WinSetWindowText(hwndText, ach);
			}
		    }
		    idLB = 0;		// allow LN_SETFOCUS to work
		    return FALSE;

		case IDL_FILE:
		    dbg( printf("LN_SELECT: IDL_FILE\n") );
		    it = SHORT1FROMMR(WinSendMsg(hwndFocus, LM_QUERYSELECTION, 0L, 0L));
		    if (it != LIT_NONE) {
			if (fDidOK)
			    fDidOK = FALSE;
			else {
			    itFile = it;
			    changeFile(hwnd,pfctl);
			}
		    }
		    idLB = 0;		// allow LN_SETFOCUS to work
		    return FALSE;

		default:
		    dbg( printf("LN_SELECT: unknown list box = %04x\n",idLB) );
		    return WinDefDlgProc(hwnd, msg, mp1, mp2);
		    break;
		}
		break;

	    case LN_SETFOCUS:
		//* The following test prevents reselecting the last
		//  selected list box item when the user is scrolling
		//  the list box.

		if (idLB == idLBPrevious)
		    break;		// no actual focus change

		switch (idLB) {

		case IDL_DRIVE:
		    dbg( printf("LN_SETFOCUS: IDL_DRIVE\n") );
		    hwndFocus = hwndLBDrive;
		    WinSendMsg(hwndFocus, LM_SELECTITEM,
			    MPFROMSHORT(itDrive), MPFROMSHORT(TRUE));
		    return FALSE;

		case IDL_DIR:
		    dbg( printf("LN_SETFOCUS: IDL_DIR\n") );
		    hwndFocus = hwndLBDir;
		    WinSendMsg(hwndFocus, LM_SELECTITEM,
			    MPFROMSHORT(itDir), MPFROMSHORT(TRUE));
		    return FALSE;

		case IDL_FILE:
		    dbg( printf("LN_SETFOCUS: IDL_FILE\n") );
		    hwndFocus = hwndLBFile;
		    WinSendMsg(hwndFocus, LM_SELECTITEM,
			    MPFROMSHORT(itFile), MPFROMSHORT(TRUE));
		    return FALSE;

		default:
		    dbg( printf("LN_SETFOCUS: unknown list box = %04x\n",idLB) );
		    return WinDefDlgProc(hwnd, msg, mp1, mp2);
		    break;
		}
		break;

	    case LN_KILLFOCUS:
		switch (idLB) {

		case IDL_DRIVE:
		    dbg( printf("LN_KILLFOCUS: IDL_DRIVE\n") );
		    hwndFocus = hwndLBDrive;
		    WinSendMsg(hwndFocus, LM_SELECTITEM,
			    MPFROMSHORT(LIT_NONE), MPFROMSHORT(FALSE));
		    return FALSE;

		case IDL_DIR:
		    dbg( printf("LN_KILLFOCUS: IDL_DIR\n") );
		    hwndFocus = hwndLBDir;
		    WinSendMsg(hwndFocus, LM_SELECTITEM,
			    MPFROMSHORT(LIT_NONE), MPFROMSHORT(FALSE));
		    return FALSE;

		case IDL_FILE:
		    dbg( printf("LN_KILLFOCUS: IDL_FILE\n") );
		    hwndFocus = hwndLBFile;
		    WinSendMsg(hwndFocus, LM_SELECTITEM,
			    MPFROMSHORT(LIT_NONE), MPFROMSHORT(FALSE));
		    return FALSE;

		default:
		    dbg( printf("LN_KILLFOCUS: unknown list box = %04x\n",idLB) );
		    return WinDefDlgProc(hwnd, msg, mp1, mp2);
		    break;
		}
		break;

	    default:
		dbg( printf("notify code= %04x\n",SHORT2FROMMP(mp1)) );
		return WinDefDlgProc(hwnd, msg, mp1, mp2);
	}
	return FALSE;

    default:
	return WinDefDlgProc(hwnd, msg, mp1, mp2);
    }
    return FALSE;
}


BOOL	changeDrive(HWND hwndDlg, PFILECTL pfctl)
{
    USHORT  it;
    USHORT  drv;
    HWND    hwndLBDrive;
    HWND    hwndText;

    hwndLBDrive = WinWindowFromID(hwndDlg, IDL_DRIVE);
    hwndText = WinWindowFromID(hwndDlg, IDC_TEXT);

    it = SHORT1FROMMR(WinSendMsg(hwndLBDrive, LM_QUERYSELECTION, 0L, 0L));
    if (it != LIT_NONE) {
	itDrive = it;
	drv = SHORT1FROMMR(WinSendMsg(hwndLBDrive, // get drive number
			LM_QUERYITEMHANDLE, MPFROMSHORT(it), 0L));
	DosSelectDisk(drv);		// change drive
	WinSetWindowText(hwndText, pfctl->pszPattern);	// reset pattern
	setDir(hwndDlg);		// update dir list
	setFile(hwndDlg);		// update file list
	setCurDir(hwndDlg);		// update current drive/dir text
	return TRUE;
    }
    return FALSE;
}


BOOL	changeDir(HWND hwndDlg, PFILECTL pfctl)
{
    char    ach[CCHMAXPATH];
    HWND    hwndLBDir;
    HWND    hwndText;
    USHORT  it;

    hwndLBDir = WinWindowFromID(hwndDlg, IDL_DIR);
    hwndText = WinWindowFromID(hwndDlg, IDC_TEXT);

    it = SHORT1FROMMR(WinSendMsg(hwndLBDir, LM_QUERYSELECTION, 0L, 0L));
    if (it != LIT_NONE) {
	itDir = it;
	WinSendMsg(hwndLBDir, LM_QUERYITEMTEXT, // get dir
			MPFROM2SHORT(it,CCHMAXPATH), MPFROMP(ach));
	DosChDir(ach, 0L);
	WinSetWindowText(hwndText, pfctl->pszPattern);	// reset pattern
	setDir(hwndDlg);		// update dir list
	setFile(hwndDlg);		// update file list
	setCurDir(hwndDlg);		// update current drive/dir text
	return TRUE;
    }
    return FALSE;
}

BOOL	changeFile(HWND hwndDlg, PFILECTL pfctl)
{
    char    ach[CCHMAXPATH];
    HWND    hwndLBFile;
    HWND    hwndText;
    USHORT  it;

    hwndText = WinWindowFromID(hwndDlg, IDC_TEXT);
    hwndLBFile = WinWindowFromID(hwndDlg, IDL_FILE);

    it = SHORT1FROMMR(WinSendMsg(hwndLBFile, LM_QUERYSELECTION, 0L, 0L));
    if (it != LIT_NONE) {
	itFile = it;
	WinSendMsg(hwndLBFile, LM_QUERYITEMTEXT, // get dir
			MPFROM2SHORT(it,CCHMAXPATH), MPFROMP(ach));
	WinSetWindowText(hwndText, ach);  // set file name
	return TRUE;
    }
    return FALSE;
}

BOOL	setDrive(HWND hwndDlg)
{
    char    ach[3];
    ULONG   bmlDrives;
    USHORT  drvCurrent;
    HWND    hwndLBDrive;
    USHORT  i;
    USHORT  us;


    hwndLBDrive = WinWindowFromID(hwndDlg, IDL_DRIVE);

    if (DosQCurDisk(&drvCurrent,&bmlDrives) != 0) {
	dbg( printf("DosQCurDisk failed!\n") );
	return FALSE;
    }
    drvCurrent--;			// 0-based drive number

    ach[1] = ':';			// init drive string
    ach[2] = '\0';

    WinEnableWindowUpdate(hwndLBDrive,FALSE); // turn off list box updates
    WinSendMsg(hwndLBDrive, LM_DELETEALL, NULL, NULL); // delete old entries

    for (i=0; bmlDrives != NULL; i++) { // get all 1 bits!
	if (bmlDrives & 1) {		// drive exists
	    ach[0] = (char)(i + 'a');
	    us = SHORT1FROMMR(WinSendMsg(hwndLBDrive, LM_INSERTITEM,
		    MPFROMSHORT(LIT_SORTASCENDING), MPFROMP(ach)));
	    WinSendMsg(hwndLBDrive, LM_SETITEMHANDLE,	 // set drive number
		MPFROMSHORT(us), MPFROMSHORT(i+1));
	    if (i == drvCurrent) {
		itDrive = us;		// save index for selection
	    }
	}
	bmlDrives >>= 1;		// get next drive bit in bit 0
    }

    WinEnableWindowUpdate(hwndLBDrive,TRUE); // repaint list box
    return TRUE;
}

BOOL	setCurDir(HWND hwndDlg)
{
    char    ach[CCHMAXPATH];
    ULONG   bmlDrives;
    USHORT  cch;
    USHORT  drvCurrent;
    HWND    hwndCurDir;
    USHORT  rc;

    rc = DosQCurDisk(&drvCurrent,&bmlDrives);	// get current drive

    ach[0] = (char)((char)drvCurrent + 'a' - (char)1) ; // make drive letter
    ach[1] = ':';			// rest of drive string
    ach[2] = '\\';

    cch = CCHMAXPATH-3;			    // room for drive string

    rc = DosQCurDir(drvCurrent, &ach[3], &cch);

    hwndCurDir = WinWindowFromID(hwndDlg, IDC_CURDIR);
    WinSetWindowText(hwndCurDir,ach);	// set current drive/dir text
    return TRUE;
}


BOOL	setDir(HWND hwndDlg)
{
    char	    ach[20];
    static USHORT   attr;
    USHORT	    cch;
    HWND	    hwndLBDir;

    hwndLBDir  = WinWindowFromID(hwndDlg, IDL_DIR);

    WinEnableWindowUpdate(hwndLBDir,FALSE); // turn off list box updates
    WinSendMsg(hwndLBDir, LM_DELETEALL, NULL, NULL); // delete old entries

    // get all directories

    strcpy(ach,"*.*");
    attr = 0x37;
    for (cch = beginSearch(ach,&attr);
	 cch != 0;
	 cch = nextSearch(ach,&attr)) {
	if (attr & 0x0010) {		// only get directories
	//  if ((cch > 1) || (ach[0] != '.')) { // do all but "."
		WinSendMsg(hwndLBDir, LM_INSERTITEM,
		    MPFROMSHORT(LIT_SORTASCENDING), MPFROMP(ach));
	//  }
	}
    }
    endSearch();

    WinEnableWindowUpdate(hwndLBDir,TRUE); // repaint list box
    itDir = 1;
    return TRUE;
}


BOOL	setFile(HWND hwndDlg)
{
    char	    ach[20];
    static USHORT   attr;
    USHORT	    cch;
    HWND	    hwndLBFile;
    HWND	    hwndText;

    hwndLBFile = WinWindowFromID(hwndDlg, IDL_FILE);
    hwndText   = WinWindowFromID(hwndDlg, IDC_TEXT);

    WinEnableWindowUpdate(hwndLBFile,FALSE); // turn off list box updates
    WinSendMsg(hwndLBFile, LM_DELETEALL, NULL, NULL); // delete old entries

    // get only files that match user's pattern

    WinQueryWindowText(hwndText,CCHMAXPATH,ach);
    dbg( printf("file pattern = %s\n",ach) );
    attr = 0x27;			// do all but directories
    for (cch = beginSearch(ach,&attr);
	 cch != 0;
	 cch = nextSearch(ach,&attr) ) {
	WinSendMsg(hwndLBFile, LM_INSERTITEM,
		MPFROMSHORT(LIT_SORTASCENDING), MPFROMP(ach));

    }
    endSearch();
    WinEnableWindowUpdate(hwndLBFile,TRUE); // repaint list box
    itFile = 1;			    // default selection

    return TRUE;
}


USHORT	beginSearch(char *psz, USHORT *pattr)
{
    FILEFINDBUF	    findbuf;
    USHORT	    cf;
    USHORT	    rc;

    cf = 1;			    // only return 1 file
    hdir = 0xFFFF;		    // create a search handle

    rc = DosFindFirst(psz,	    // file specification
		      &hdir,	    // pointer to variable for handle
		      *pattr,	    // search attribute (everything)
		      &findbuf,	    // pointer to result buffer
		      sizeof(FILEFINDBUF), // length of result buffer
		      &cf,	    // files to find/files found
		      0L);	    // Must be zero

    if (cf != 0) {
	psz[findbuf.cchName] = '\0';	// terminate name
	strncpy(psz,findbuf.achName,findbuf.cchName); // copy name to caller
	*pattr = findbuf.attrFile;
	return findbuf.cchName;
    }
    return 0;
}


USHORT	nextSearch(char *psz, USHORT *pattr)
{
    FILEFINDBUF	    findbuf;
    USHORT	    cf;
    USHORT	    rc;

    cf = 1;	    // only return 1 file

    rc = DosFindNext(hdir,	    // pointer to variable for handle
		     &findbuf,	    // pointer to result buffer
		     sizeof(FILEFINDBUF), // length of result buffer
		     &cf);	    // files to find/files found

    if (cf != 0) {
	psz[findbuf.cchName] = '\0'; // terminate name
	strncpy(psz,findbuf.achName,findbuf.cchName); // copy name to caller
	*pattr = findbuf.attrFile;
	return findbuf.cchName;
    }
    return 0;
}


void	endSearch(void)
{
    DosFindClose(hdir);
}


/***	trimBlanks - trim off leading and trailing blanks
*
*	ENTRY	psz - string to be trimmed
*/

char * trimBlanks(char *psz)
{
    char *pch;
    char *pchDst;
    char *pchRight;

    //	Find right-most non-blank character

    for (pch = psz+strlen(psz)-1;	// start at last character
	 (pch >= psz) && (*pch == ' '); // scan backward to non-blank
	pch--)
	    ;

    *(pch+1) = '\0';			// trim trailing blanks

    pchRight = pch;

    // Find left-most non-blank character

    for (pch = psz;			// start at first character
	 (pch <= pchRight) && (*pch == ' '); // scan forward to non-blank
	 pch++)
	    ;

    // Shift string left to trim leading blanks

    if (pch > psz) {			// leading blanks to trim
	pchRight++;			// grab traling null
	pchDst = psz;			// do not destroy psz
	while (pchDst <= pchRight)
	    *pchDst++ = *pch++;		// shift string left
    }
    return psz;
}
