/*
    clock.c	Presentation Manager Analog Clock Application
    Created by Microsoft Corporation, 1989
*/
#define INCL_PM
#include <os2.h>
#include <string.h>
#include "res.h"

extern MRESULT EXPENTRY ClkWndProc ( HWND , USHORT , MPARAM , MPARAM ) ;

int cdecl main ( int argc , char * argv [ ] ) ;
BOOL ClkInit ( VOID ) ;
HWND hwndFrame ;
HAB hab ;
HMQ hmq ;
HSWITCH hsw ;
extern HPS hps ;
BOOL fStartAsIcon = FALSE ;

/*
    main(argc, argv)	Main program
*/
int cdecl main ( int argc , char * argv [ ] )
{
    QMSG qmsg ;

    /* have we been asked to start ourselves as an icon? */
    if (argc > 0) {
	if ( strcmpi ( argv [ 1 ] , "iconic" ) == 0 )
	    fStartAsIcon = TRUE ;
    }

    if ( ClkInit ( ) ) {

	while ( WinGetMsg ( hab , & qmsg , NULL , 0 , 0 ) )
	    WinDispatchMsg ( hab , & qmsg ) ;

	/* Clean up code */
	GpiDestroyPS( hps );
	WinRemoveSwitchEntry ( hsw ) ;
	WinDestroyWindow ( hwndFrame ) ;
	WinDestroyMsgQueue ( hmq ) ;
	WinTerminate ( hab ) ;
    }

    return 0 ;
}

/*
    ClkInit()		Clock Initialization routine
    Returns TRUE if successful.
*/
BOOL ClkInit ( )
{
    /* application name, switch list info, and frame creation flags */
    static PSZ pszClkName = "Clock" ;
    static SWCNTRL swctl = { 0 , 0 , 0 , 0 , 0 , SWL_VISIBLE ,
			     SWL_JUMPABLE , "Clock" , 0 } ;
    static LONG fcf = FCF_SIZEBORDER | FCF_TITLEBAR | FCF_MINMAX
		      | FCF_SYSMENU ;

    HWND hwndClient ;
    PID pid ;
    TID tid ;

    if ( ( hab = WinInitialize ( 0 ) ) == NULL )
	return FALSE ;

    if ( ( hmq = WinCreateMsgQueue ( hab , 0 ) ) == NULL ) {
	WinTerminate ( hab ) ;
	return FALSE ;
    }

    if ( ! WinRegisterClass ( hab , pszClkName , ClkWndProc ,
			      CS_SIZEREDRAW , 0 ) ) {
	WinDestroyMsgQueue ( hmq ) ;
	WinTerminate ( hab ) ;
	return FALSE ;
    }

    hwndFrame = WinCreateStdWindow ( HWND_DESKTOP , ( ULONG ) NULL , & fcf ,
				     pszClkName , pszClkName , WS_VISIBLE ,
				     (HMODULE) NULL , ID_RESOURCE , & hwndClient ) ;

    if ( hwndFrame == NULL ) {
	WinDestroyMsgQueue ( hmq ) ;
	WinTerminate ( hab ) ;
	return FALSE ;
    }

    /* add ourselves to the switch list */
    WinQueryWindowProcess ( hwndFrame , & pid , & tid ) ;
    swctl . hwnd = hwndFrame ;
    swctl . idProcess = pid ;
    hsw = WinAddSwitchEntry ( & swctl ) ;

    return TRUE ;
}
