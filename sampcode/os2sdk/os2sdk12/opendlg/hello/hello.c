/*
 * HELLO.C -- A simple program which calls the OpenDlg library
 * Created by Microsoft Corporation, 1989
 */
#define INCL_PM
#include <os2.h>
#include <opendlg.h>
#include "hello.h"
/*
 * Globals
 */
HAB     hAB;
HMQ     hMqHello;
HWND    hWndHello;
HWND    hWndHelloFrame;
CHAR    szClassName[]	= "Hello World";
CHAR	szMessage[]	= " - File Dialog Sample";
CHAR	szExtension[]	= "\\*.*";
CHAR	szHelp[]	= "Help would go here.";
DLF	vdlf;
HFILE	vhFile;
/*
 * Main routine...initializes window and message queue
 */
int cdecl main( ) {
    QMSG qmsg;
    ULONG ctldata;

    hAB = WinInitialize(0);

    hMqHello = WinCreateMsgQueue(hAB, 0);

    if (!WinRegisterClass( hAB, (PCH)szClassName, (PFNWP)HelloWndProc,
		CS_SIZEREDRAW, 0))
        return( 0 );

    /* Create the window */
    ctldata = FCF_STANDARD & ~(FCF_ACCELTABLE);
    hWndHelloFrame = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE, &ctldata,
                                         szClassName, szMessage,
                                         WS_VISIBLE, (HMODULE) NULL, ID_RESOURCE,
					 (HWND FAR *)&hWndHello );

    WinShowWindow( hWndHelloFrame, TRUE );

    /* Poll messages from event queue */
    while( WinGetMsg( hAB, (PQMSG)&qmsg, (HWND)NULL, 0, 0 ) )
        WinDispatchMsg( hAB, (PQMSG)&qmsg );

    /* Clean up */
    WinDestroyWindow( hWndHelloFrame );
    WinDestroyMsgQueue( hMqHello );
    WinTerminate( hAB );
}

MRESULT CALLBACK HelloWndProc(hWnd, msg, mp1, mp2)
/*
 * This routine processes WM_COMMAND, WM_PAINT.  It passes
 * everything else to the Default Window Procedure.
 */
HWND hWnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    HPS		hPS;
    POINTL	pt;
    CHARBUNDLE	cb;
    RECTL	rcl;

    switch (msg) {

	case WM_COMMAND:
	    switch (COMMANDMSG(&msg)->cmd) {

		case IDM_OPEN: /* Demonstrate Open... dialog call */
		    SetupDLF( &vdlf
			    , DLG_OPENDLG
			    , &vhFile
			    , szExtension
			    , NULL
			    , "Open Title"
			    , szHelp );
	 	    DlgFile(hWndHelloFrame, &vdlf);
		    break;

		case IDM_SAVE: /* Demonstrate Save As... dialog call */
		    SetupDLF( &vdlf
			    , DLG_SAVEDLG
			    , &vhFile
			    , szExtension
			    , NULL
			    , "Save Title"
			    , szHelp);
		    lstrcpy( (PSZ)vdlf.szOpenFile, (PSZ)"foo.bar");
		    DlgFile(hWndHelloFrame, &vdlf);
		    break;

		case IDM_ABOUT:
		    WinDlgBox(HWND_DESKTOP, hWnd, AboutDlgProc,
			      (HMODULE) NULL, IDD_ABOUT, NULL);
		    return 0;

		default: break;
	    }
	    break;

	case WM_PAINT:
	    /* Open the presentation space */
	    hPS = WinBeginPaint(hWnd, NULL, &rcl);

	    /* Fill the background with Dark Blue */
	    WinFillRect(hPS, &rcl, CLR_DARKBLUE);

	    /* Write "Hello World" in Red */
	    pt.x = pt.y = 0L;
	    cb.lColor = CLR_RED;
	    GpiSetAttrs(hPS, PRIM_CHAR, CBB_COLOR, 0L, &cb);
	    GpiCharStringAt(hPS, &pt, (LONG)sizeof(szClassName)-1, szClassName);

	    /* Finish painting */
	    WinEndPaint(hPS);
	    break;

	default: return WinDefWindowProc(hWnd, msg, mp1, mp2); break;
    }
    return 0L;
}

MRESULT CALLBACK AboutDlgProc(hDlg, msg, mp1, mp2)
/*
    About... dialog procedure
*/
HWND hDlg;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    switch(msg) {
	case WM_COMMAND:
	    switch(COMMANDMSG(&msg)->cmd) {
		case DID_OK: WinDismissDlg(hDlg, TRUE); break;
		default: break;
	    }
	default: return WinDefDlgProc(hDlg, msg, mp1, mp2);
    }
    return FALSE;
}
