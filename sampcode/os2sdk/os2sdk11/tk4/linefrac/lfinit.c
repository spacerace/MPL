/****************************************************************************
*
*   lfinit.c -- Initialization portion of LineFractal Application.
*
*   Created by Microsoft Corporation, 1989
*
****************************************************************************/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>

#define INCL_GLOBALS
#include "linefrac.h"

#define INCL_LFINIT
#define INCL_LFMAIN
#include "lffuncs.h"




/************************************************************************
*
*   Global Variables
*
************************************************************************/

extern GLOBALDATA global;
extern ULONG aulRops[];




/****************************************************************************
*
*   LfInitApp
*
*   Register application window class and creates standard window.
*
****************************************************************************/

BOOL FAR
LfInitApp(VOID)
{
    char szTitle[24];
    ULONG ctldata;
    PID pid;
    TID tid;
    HSWITCH hsw;
    static SWCNTRL swctl = { 0, 0, 0, 0, 0, SWL_VISIBLE,
			     SWL_JUMPABLE, "LineFractal", 0 };


    /*  Register Application Window Class  */

    WinLoadString( global.hab, NULL, IDS_TITLE, sizeof(szTitle), (PCH)szTitle );
    if ( !WinRegisterClass( global.hab, (PCH)szTitle, (PFNWP)LineFracWndProc,
	    CS_SIZEREDRAW, 0 ))
	return FALSE;


    /* Load the window's default pointer. */
    if (!((global.hptr)[0]   = WinLoadPointer( HWND_DESKTOP, NULL, IDR_KOCH0 )))
	return FALSE;
    if (!((global.hptr)[1]   = WinLoadPointer( HWND_DESKTOP, NULL, IDR_KOCH1 )))
	return FALSE;
    if (!((global.hptr)[2]   = WinLoadPointer( HWND_DESKTOP, NULL, IDR_KOCH2 )))
	return FALSE;
    if (!((global.hptr)[3]   = WinLoadPointer( HWND_DESKTOP, NULL, IDR_KOCH3 )))
	return FALSE;
    if (!(global.hptrTrack   = WinLoadPointer( HWND_DESKTOP, NULL, IDR_TRACKINGPTR )))
	return FALSE;
    if (!(global.hptrPaste   = WinLoadPointer( HWND_DESKTOP, NULL, IDR_PASTINGPTR )))
	return FALSE;
    if (!(global.hptrWait    = WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE)))
	return FALSE;



    /* Create a window instance of class "LineFractal" */

    ctldata = FCF_STANDARD &
     ~(ULONG)(FCF_ICON | FCF_ACCELTABLE | FCF_TASKLIST);

    if (global.hwndFrame = WinCreateStdWindow(
	HWND_DESKTOP,		   /* specify desktop as parent window	    */
	WS_VISIBLE,		   /* window styles			    */
	&ctldata,		   /* frame creation flags		    */
	(PCH)szTitle,		   /* window class name 		    */
	(PCH)szTitle,		   /* name appearing in window caption	    */
	0L,			   /*					    */
	(HMODULE)NULL,		   /* use current executable module id	    */
	IDR_LINEFRACTAL,	   /* menu id				    */
	(HWND FAR *)&global.hwnd   /* window handle			    */
	))
    {
	global.fFirstThread		  = TRUE;

	global.fAutoSwitch		  = FALSE;
	global.usSwitchDelay		  = 1 * 1000;	  /* 1 sec */
	global.fThreadInheritance	  = TRUE;
	global.fUpdateMenusOnThreadSwitch = TRUE;
	global.fTimerOn 		  = FALSE;
	global.usMinTimerDelay		  = 1 * 1000;	  /* 1 sec */
	global.usMaxTimerDelay		  = 5 * 1000;	  /* 5 sec */

	global.usRopIndex		  = 0;
	global.ulPasteROP		  = aulRops[global.usRopIndex];
	global.usPtrIncr		  = 1;
	global.usPtrCounter		  = 1;
	global.usUserPtr		  = 2;
	global.usCurPtr 		  = global.usUserPtr;
	global.usPtrThreshold		  = 5;
	global.fUseClipbrd		  = FALSE;
	global.fTracking		  = FALSE;
	global.fSelecting		  = FALSE;
	global.fPasting 		  = FALSE;
	global.fMouseChangesRecursion	  = TRUE;
	global.fAnimatePtr		  = TRUE;
	global.fTempSelect		  = FALSE;
	global.fShowSelectRc		  = FALSE;
	global.fHaveSelectRc		  = FALSE;
	global.fHaveCutCopyDC		  = FALSE;

	global.fUpdateAttrs		  = FALSE;
	global.flLineAttrs		  = 0L;
	global.flMarkerAttrs		  = 0L;
	global.flAreaAttrs		  = 0L;
	global.flImageAttrs		  = 0L;
	global.flMiscAttrs		  = 0L;


	WinQueryWindowRect(global.hwnd, &global.rcl);

	global.cThr = 0;	/* start with no active threads */
	global.pThrTop = NULL;	/* make sure this guy's invalid */


#define INIT_MENU_ITEM(val, var)     \
	TOGGLE_MENU_ITEM(global.hwndFrame, (val), (var))

	/* Check boolean menu items if they are set to true. */

	INIT_MENU_ITEM(IDM_AUTOSWITCH,		 global.fAutoSwitch);
	INIT_MENU_ITEM(IDM_UPDATEMENUS,		 global.fUpdateMenusOnThreadSwitch);
	INIT_MENU_ITEM(IDM_THREADINHERITANCE,	 global.fThreadInheritance);
	INIT_MENU_ITEM(IDM_USECLIPBRD,		 global.fUseClipbrd);
	INIT_MENU_ITEM(IDM_MOUSECHANGESRECURSION, global.fMouseChangesRecursion);
	INIT_MENU_ITEM(IDM_ANIMATEPTR,		 global.fAnimatePtr);

	CHECK_MENU_ITEM(global.hwndFrame, IDM_PASTES+global.usRopIndex);


	/* Add ourselves to the switch list. */

	WinQueryWindowProcess(global.hwndFrame, &pid, &tid);
	swctl.hwnd	= global.hwndFrame;
	swctl.idProcess = pid;
	hsw = WinAddSwitchEntry(&swctl);

	return TRUE;
    }
    return FALSE;
}
