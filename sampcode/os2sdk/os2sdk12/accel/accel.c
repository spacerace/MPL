/*
 *  ACCEL.C -- Sample demonstrating calls included with INCL_WINACCELERATORS
 *
 *  Overview:
 *	Accelerators are used to reduce the number of keystrokes needed to
 *  execute a command (hence "accelerating" a user's processing time)
 *
 *  Strategy:
 *	This application allows the user to experiment with various settings,
 *  by popping up a dialog box in which the user can specify an accelerator.
 *  One possible modification to this program is to have the user hit the
 *  desired key sequence, and to use KbdCharIn() to figure out what the key
 *  sequence is, and then set the accelerator.  Another is to implement the
 *  "Delete" operation, by perhaps listing the accelerators in a list box.
 *  This wasn't done primarily because that would require reorganization
 *  (compression) of the accelerator table:  it could not be easily done with
 *  a WinDeleteAccel call (because such a call does not exist).
 */
#define INCL_WINACCELERATORS
#define	INCL_WINBUTTONS			// Needed for checkboxes in dialogs
#define	INCL_WINDIALOGS
#define	INCL_WINMESSAGEMGR
#define	INCL_WINFRAMEMGR		// for SC_MINIMIZE constant
#define INCL_WINWINDOWMGR
#include <os2.h>

#include <malloc.h>			// Needed for dynamic memory allocation
#include <stdio.h>			// Needed for sscanf() call
#include "accel.h"			// Needed for resource IDs
/*
 * Globals
 */
char	ach[8]; 			// Temporary:  used to store Key: field
char	szAppName[]	= "ACCEL.EXE";
char	szClassName[]	= "Accelerator";
char	szMessage[]	= " - Accelerator Table Example";
int	cbSize; 			// Size of Accel. Table in bytes
int	iTemp;				// Used to store Key: value, if number
void	*pTemp; 			// Used so free() won't give warnings
HAB	hab;
HACCEL	haccSystem;			// Handle to system accelerator table
HACCEL	haccTable;			// Handle to app-local acceltable
HMQ     hmqAccel;
HWND	hwndAccel;			// Client window
HWND	hwndAccelFrame; 		// Frame window
PACCELTABLE	pacctTable;		// Points to table with ACCEL entries
/*
    Macros
*/
#define Message(s) WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, s, \
			szAppName, 0, MB_OK | MB_ICONEXCLAMATION)
#define	Check(b)   WinSendDlgItemMsg(hwndDlg, b, \
			BM_SETCHECK, MPFROMSHORT(1), 0L)
#define Checked(b) WinSendDlgItemMsg(hwndDlg, b, BM_QUERYCHECK, 0L, 0L)
/*
    Internals
*/
BOOL InitializeAccelTable(void);
/*
 * Main routine...initializes window and message queue
 */
void cdecl main(void) {
    QMSG qmsg;
    ULONG ctldata;

    /* Initialize a PM application */
    hab = WinInitialize(0);
    hmqAccel = WinCreateMsgQueue(hab, 0);

    /* Register the main window's class */
    if (!WinRegisterClass(hab, szClassName, AccelWndProc, CS_SIZEREDRAW, 0))
        return;
    /*
	Create the window
	We create it without an accelerator table, but we'll load one later
    */
    ctldata = FCF_STANDARD & ~FCF_ACCELTABLE;
    hwndAccelFrame = WinCreateStdWindow(HWND_DESKTOP, WS_VISIBLE, &ctldata,
	szClassName, szMessage, WS_VISIBLE, (HMODULE) NULL, IDR_ACCEL, &hwndAccel);
    WinShowWindow(hwndAccelFrame, TRUE);
    /*
	Load the accelerator tables
    */
    if (!InitializeAccelTable()) {
	Message("Accelerator table not initialized!");
	return;
    }

    /* Poll messages from event queue */
    while(WinGetMsg(hab, (PQMSG)&qmsg, (HWND)NULL, 0, 0))
        WinDispatchMsg(hab, (PQMSG)&qmsg);

    /* Clean up */
    if (!WinDestroyAccelTable(haccTable))
	Message("Could not destroy ACCELTABLE");
    WinDestroyWindow(hwndAccelFrame);
    WinDestroyMsgQueue(hmqAccel);
    WinTerminate(hab);
}

MRESULT CALLBACK AccelWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2) {
/*
 * This routine processes WM_PAINT.  It passes
 * everything else to the Default Window Procedure.
 */
    HPS		hPS;
    RECTL	rcl;

    switch (msg) {

	case WM_HELP:
	    /* If WM_HELP, pop up Help dialog box */
	    WinDlgBox(HWND_DESKTOP, hwnd, AboutDlgProc, (HMODULE) NULL, IDD_HELP, NULL);
	    break;

	case WM_COMMAND:
	    switch (COMMANDMSG(&msg)->cmd) {

		/* On most WM_COMMAND messages, give the About... box */
		case IDM_ABOUT:
		    WinDlgBox(HWND_DESKTOP, hwnd, AboutDlgProc,
			      (HMODULE) NULL, IDD_ABOUT, NULL);
		    break;

		/* Create your own accelerator dialog */
		case IDM_CREATE:
		    WinDlgBox(HWND_DESKTOP, hwnd, CreateDlgProc,
			      (HMODULE) NULL, IDD_CREATE, NULL);

		default: break;
	    }
	    break;

	case WM_PAINT:
	    /* Open the presentation space */
	    hPS = WinBeginPaint(hwnd, NULL, &rcl);

	    /* Fill the background with Dark Blue */
	    WinFillRect(hPS, &rcl, CLR_DARKBLUE);

	    /* Finish painting */
	    WinEndPaint(hPS);
	    break;

	default: return WinDefWindowProc(hwnd, msg, mp1, mp2); break;
    }
    return 0L;
}

MRESULT CALLBACK AboutDlgProc(hwndDlg, msg, mp1, mp2)
/*
    About... dialog procedure
*/
HWND hwndDlg;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    switch(msg) {
	case WM_COMMAND:
	    switch(COMMANDMSG(&msg)->cmd) {
		case DID_OK: WinDismissDlg(hwndDlg, TRUE);
		default: break;
	    }
	default: return WinDefDlgProc(hwndDlg, msg, mp1, mp2);
    }
    return FALSE;
}

MRESULT CALLBACK CreateDlgProc(hwndDlg, msg, mp1, mp2)
/*
    Create Accelerator dialog procedure
*/
HWND hwndDlg;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    switch(msg) {
	case WM_INITDLG:
	    /* Set the defaults */
	    Check(IDD_CHAR); Check(IDD_CMD);
	    break;

	case WM_COMMAND:
	    switch(COMMANDMSG(&msg)->cmd) {
		case DID_OK:
		    /* Get the accelerator table (allocate an extra space) */
		    cbSize = WinCopyAccelTable(haccTable, NULL, 0);
		    pTemp = (void *) malloc(cbSize + sizeof(ACCEL));
		    pacctTable = (PACCELTABLE) pTemp;
		    cbSize = WinCopyAccelTable(haccTable, pacctTable, cbSize);
		    
#define accNew	pacctTable->aaccel[pacctTable->cAccel]

		    /*
			Command:
			    if SYSCOMMAND, make the window minimize.
			    if HELP, we'll pop up a dialog box.
			    otherwise, pop up the About... dialog box.
		    */
		    if (Checked(IDD_SYSCMD)) accNew.cmd = SC_MINIMIZE;
		    else accNew.cmd = IDM_ABOUT;

		    /* Get the states from the dialog box */
		    accNew.fs = 0;
		    if (Checked(IDD_ALT))	accNew.fs |= AF_ALT;
		    if (Checked(IDD_CHAR))	accNew.fs |= AF_CHAR;
		    if (Checked(IDD_CONTROL))	accNew.fs |= AF_CONTROL;
		    if (Checked(IDD_FHELP))	accNew.fs |= AF_HELP;
		    if (Checked(IDD_LONEKEY))	accNew.fs |= AF_LONEKEY;
		    if (Checked(IDD_SCANCODE))	accNew.fs |= AF_SCANCODE;
		    if (Checked(IDD_SHIFT))	accNew.fs |= AF_SHIFT;
		    if (Checked(IDD_SYSCMD))	accNew.fs |= AF_SYSCOMMAND;
		    if (Checked(IDD_VKEY))	accNew.fs |= AF_VIRTUALKEY;

		    /* Get the key to be defined */
		    WinQueryDlgItemText(hwndDlg, IDD_ENTRY, 8, ach);
		    if (('0' <= ach[0]) && (ach[0] <= '9')) {
			sscanf(ach, "%i", &iTemp);
			accNew.key = (USHORT) iTemp;
		    }
		    else accNew.key = (USHORT) ach[0];

		    /* Increment the count of accelerator records */
		    pacctTable->cAccel++;

		    /* Cleanup, then create a new accelerator table */
		    WinDestroyAccelTable(haccTable);
		    haccTable = WinCreateAccelTable(hab, pacctTable);

		    /* Set the new accelerator table, and clean up */
		    WinSetAccelTable(hab, haccTable, hwndAccelFrame);
		    free(pTemp);

		case DID_CANCEL:
		    WinDismissDlg(hwndDlg, TRUE);

		default: break;
	    }
	default: return WinDefDlgProc(hwndDlg, msg, mp1, mp2);
    }
    return FALSE;
}

BOOL InitializeAccelTable(void) {
    /*
	Initialize the accelerator table by loading it from the
	resource file.	Note that you can load an accelerator
	table from a DLL, if you change the NULL parameter.
	The system accelerator table is accessible after this
	call:  one possible use for this would be a List...
	dialog box, which would list all system & app. accelerators.
    */
    haccSystem = WinQueryAccelTable(hab, NULL);
    haccTable = WinLoadAccelTable(hab, 0, IDR_ACCEL);
    return WinSetAccelTable(hab, haccTable, hwndAccelFrame);
}
