/*
 * WINDOWS 2.00 DEBUG UTILITY - SOURCE CODE (DEBUG.C)
 *
 * LANGUAGE : microsoft C 5.0
 * MODEL    : small or medium
 * STATUS   : operational
 *
 * 12/11/87 1.00 - Kevin P. Welch - initial creation.
 *
 */
 
#include <windows.h>
#include <string.h>
#include <fcntl.h>
#include <types.h>
#include <stat.h>
#include "debug.h"

/* internal macros */
#define	TOGGLE(x,y)	CheckDlgButton(x,y,!IsDlgButtonChecked(x,y))
#define	ENABLE(x,y,z)	EnableWindow(GetDlgItem(x,y),z)

/* general definitions */
#define	SHARED		S_IREAD|S_IWRITE
#define	MODE_APPEND	O_RDWR|O_APPEND|O_BINARY	
#define	MODE_CREATE	O_RDWR|O_CREAT|O_BINARY

/* listbox viewport definitions */
#define	PORT_X		20	/* viewport x origin */
#define	PORT_Y		40	/* viewport y origin */
#define	PORT_WIDTH	460	/* viewport width */
#define	PORT_HEIGHT	140	/* viewport height */

/* debug statement data structure */
#define	MAX_FILTER	32	/* maximum filter categories */

typedef struct {

	/* general options */
	BOOL	bSetup;		/* debug setup flag */
	BOOL	bActive;	/* debug active flag */
	
	/* count options */
	BOOL	bCount;		/* count events flag */
	LONG	lMsgCount;	/* debug message count */

	/* display options */
	BOOL	bDisplay;	/* display debug events */
	HWND	hListBox;	/* listbox window handle */
	WORD	wEntries;	/* current listbox lines */
	WORD	wMaxEntries;	/* maximum listbox lines */
	
	/* filter options */
	BOOL	bFilter;	/* filter debug events */
	WORD	wFilterSize;	/* size of categories */
	WORD	Filter[MAX_FILTER];	/* list of categories */

	/* log-file options */
	BOOL	bLog;		/* log debug events */
	char	szLogFile[64];	/* debug log file name */

} OPTIONS;

/* global data definitions */
static OPTIONS	Options;	

/*
 * DebugSetup( hWnd, wMsg, wMax ) : bResult
 *
 *	hWnd	main application window handle
 *	wMsg	debug control panel system message
 *	wMax    maximum number of debug lines
 *
 * This function is responsible for creating the debug viewport
 * window class and for attaching the control panel to the system
 * menu of the window handle provided (this implies that the window
 * handle provided has a system menu).
 *
 * A boolean value of TRUE is returned if the setup process was
 * successful.
 *
 */
 
BOOL FAR PASCAL DebugSetup( hWnd, wMsg, wMax )
	HWND	hWnd;
	WORD	wMsg;
	WORD	wMax;
{
	/* local variables */
	BOOL		bOk;		/* boolean result */
	HMENU		hSysMenu;	/* temporary menu handle */
	WNDCLASS	WndClass;	/* temporary class 
						structure */
	HANDLE		hInstance;	/* handle to window 
						instance */
	char		szModule[64];	/* main module file name */
	
	/* boolean result */
	bOk = FALSE;
	hInstance = GetWindowWord( hWnd, GWW_HINSTANCE );
	
	/* check if debug already activated */
	if ( !Options.bSetup ) {

		/* retrieve system menu handle */
		hSysMenu = GetSystemMenu( hWnd, FALSE );
		if ( hSysMenu ) {
		
			/* change system menu to reflect control panel */
			ChangeMenu( hSysMenu, 0, NULL, 999, 
					MF_APPEND|MF_SEPARATOR );
			ChangeMenu( hSysMenu, 0, "&Debug...", wMsg, 
					MF_APPEND|MF_STRING);
	
			/* create listbox window -initially hidden */
			Options.hListBox = CreateWindow(
					"LISTBOX",		/* class name */
					"",				/* window caption */
					WS_BORDER|WS_VSCROLL,	/* window style */
					PORT_X,			/* x position */
					PORT_Y,			/* y position */
					PORT_WIDTH,		/* width */
					PORT_HEIGHT,	/* height */
					NULL,			/* parent window */
					NULL,			/* menu */
					hInstance,		/* instance handle */
					(LPSTR)NULL		/* other data */
				);
					
			/* continue if successful */
			if ( Options.hListBox ) {
								
				/* define options data */
				bOk = TRUE;
				Options.bSetup = TRUE;
				Options.wMaxEntries = wMax;

				/* define default log file name */
				GetModuleFileName( hInstance, szModule, 
									sizeof(szModule) );
					
				*strrchr(szModule,'.') = 0;
				sprintf(
					Options.szLogFile,
					"%s.LOG",
					(char *)(strrchr(szModule,'\\')+1)
				);

			}
		
		} 

	} 

	/* return final result */
	return( bOk );

}

/*
 * DebugControl( hWnd ) : BOOL FAR PASCAL
 *
 *		hWnd	main application window handle
 *
 * This function enables the user to monitor and adjust the debug
 * control panel settings.  When this function is called a modal
 * dialog box is displayed which allows the user to set the
 * debug options.
 *
 * The value returned indicates how the user terminated the
 * dialog box - TRUE implies the OK button was pressed, FALSE
 * the CANCEL button.
 *
 */
 
BOOL FAR PASCAL DebugControl( hWnd )
	HWND		hWnd;
{
	/* local variables */
	FARPROC		lpProc;			/* temporary function address */
	BOOL		bResult;		/* boolean result value */
	HANDLE		hInstance;		/* window instance handle */

	/* retrieve instance handle */
	hInstance = GetWindowWord( hWnd, GWW_HINSTANCE );
	
	/* display dialog box */
	lpProc = MakeProcInstance( DebugControlDlgFn, hInstance );
	bResult = DialogBox(hInstance,(LPSTR)"DebugControl",hWnd,lpProc);
	FreeProcInstance( lpProc );

	/* return final result */
	return( bResult );

}

/*
 * DebugControlDlgFn( hDlg, wMsg, wParam, lParam ) : BOOL FAR PASCAL
 *
 *		hDlg		window handle
 *		wMsg		dialog message
 *		wParam		word parameter
 *		lParam		long parameter
 *
 * This function processes all the messages related to the
 * debug control panel dialog box.  With the dialog box the
 * user can add, change, and remove debugging channels.
 *
 */

BOOL FAR PASCAL DebugControlDlgFn( hDlg, wMsg, wParam, lParam )
	HWND		hDlg;
	WORD		wMsg;
	WORD		wParam;
	LONG		lParam;
{
	/* local variables */
	BOOL		bResult;	/* boolean result variable */
	
	/* initialization */
	bResult = TRUE;

	/* process message */
	switch( wMsg )
		{
	case WM_INITDIALOG :	/* initialize dialog box */
		
		{
			/* local variables */
			WORD	i;			/* loop variable */
			int		xPopup;		/* popup x position */
			int		yPopup;		/* popup y position */
			int		cxPopup;	/* width of popup window */
			int		cyPopup;	/* height of popup window */
			RECT	rectWnd;	/* temporary window rect */
			char	szToken[8];	/* character token */
			char	szFilterList[64];	/* filter list string */
			
			/* The following code centers the dialog box in the
			 * middle of the screen.  It uses the dimensions of 
			 * the display and the dialog box and repositions the
			 * dialog box accordingly.
			 */
			
			/* retrieve popup rectangle  */
			GetWindowRect( hDlg, (LPRECT)&rectWnd );
			
			/* calculate popup extents */
			cxPopup = rectWnd.right - rectWnd.left;
			cyPopup = rectWnd.bottom - rectWnd.top;
	
			/* calculate new location & move dialog box */
			xPopup = ( GetSystemMetrics(SM_CXSCREEN) - cxPopup) / 2;
			yPopup = ( GetSystemMetrics(SM_CYSCREEN) - cyPopup) / 2;
			
			MoveWindow( hDlg, xPopup, yPopup, cxPopup, cyPopup, TRUE);

			/* define filter list */
			szFilterList[0] = 0;
			if ( Options.wFilterSize ) {
				sprintf( szFilterList, "%u", Options.Filter[0] );
				for ( i=1; i<Options.wFilterSize; i++ ) {
					sprintf( szToken, ",%u", Options.Filter[i] );
					strcat( szFilterList, szToken );
				}
			}
			
			/* check ON-OFF radio buttons */
			CheckRadioButton(
				hDlg, 
				BUG_OFF, 
				BUG_ON, 
				(Options.bActive) ? BUG_ON :BUG_OFF
			);
		
			/* define check boxes */
			CheckDlgButton( hDlg, BUG_COUNT, Options.bCount );
			CheckDlgButton( hDlg, BUG_DISPLAY, Options.bDisplay );
			CheckDlgButton( hDlg, BUG_FILTER, Options.bFilter );
			CheckDlgButton( hDlg, BUG_LOG, Options.bLog );
	
			/* display filter categories & log file name */
			SetDlgItemText( hDlg, BUG_FILTERLIST, szFilterList );
			SetDlgItemText( hDlg, BUG_LOGFILE, Options.szLogFile );
		
			/* disable check boxes & edit fields if debug inactive */
			if ( !Options.bActive ) {

				/* disable check boxes */
				ENABLE( hDlg, BUG_COUNT, FALSE );
				ENABLE( hDlg, BUG_DISPLAY, FALSE );
				ENABLE( hDlg, BUG_FILTER, FALSE );
				ENABLE( hDlg, BUG_LOG, FALSE );
	
				/* disable edit fields */
				ENABLE( hDlg, BUG_FILTERLIST, FALSE );
				ENABLE( hDlg, BUG_LOGFILE, FALSE );
	
			} else {
		
				/* enable edits field */
				ENABLE( hDlg, BUG_FILTERLIST, IsDlgButtonChecked(hDlg,
						BUG_FILTER) );
				ENABLE( hDlg, BUG_LOGFILE, IsDlgButtonChecked(hDlg,
						BUG_LOG) );
		
			}
		
		}
				
		break;
	case WM_COMMAND : /* dialog box command */

		/* process sub-message */
		switch( wParam )
			{
		case BUG_ON : /* turn debug on */
			
			/* redefine radio button */
			CheckRadioButton( hDlg, BUG_OFF, BUG_ON, BUG_ON );
			
			/* enable check boxes */
			ENABLE( hDlg, BUG_COUNT, TRUE );
			ENABLE( hDlg, BUG_DISPLAY, TRUE );
			ENABLE( hDlg, BUG_FILTER, TRUE );
			ENABLE( hDlg, BUG_LOG, TRUE );
			
			/* enable edits field */
			ENABLE( hDlg, BUG_FILTERLIST, IsDlgButtonChecked(hDlg,
					BUG_FILTER) );
			ENABLE( hDlg, BUG_LOGFILE, IsDlgButtonChecked(hDlg,
					BUG_LOG) );
				
			break;
		case BUG_OFF : /* turn debug off */
			
			/* redefine radio button */
			CheckRadioButton( hDlg, BUG_OFF, BUG_ON, BUG_OFF );
			
			/* enable check boxes */
			ENABLE( hDlg, BUG_COUNT, FALSE );
			ENABLE( hDlg, BUG_DISPLAY, FALSE );
			ENABLE( hDlg, BUG_FILTER, FALSE );
			ENABLE( hDlg, BUG_LOG, FALSE );
			
			/* enable edit fields */
			ENABLE( hDlg, BUG_FILTERLIST, FALSE );
			ENABLE( hDlg, BUG_LOGFILE, FALSE );
			
			break;
		case BUG_COUNT : /* count debug events */
			TOGGLE( hDlg, BUG_COUNT );						
			break;
		case BUG_DISPLAY : /* display debug events */
			TOGGLE( hDlg, BUG_DISPLAY );
			break;
		case BUG_FILTER : /* filter debug events */
			TOGGLE( hDlg, BUG_FILTER );
			ENABLE( hDlg, BUG_FILTERLIST, IsDlgButtonChecked(hDlg,
					BUG_FILTER) );
			break;
		case BUG_LOG : /* log debug events */
			TOGGLE( hDlg, BUG_LOG );
			ENABLE( hDlg, BUG_LOGFILE, IsDlgButtonChecked(hDlg,
					BUG_LOG) );
			break;
		case BUG_OK : /* done with debug control panel */
			
			{
				/* local variables */
				WORD		i;
				char *		spToken;
				char		szToken[8];
				char		szCaption[64];
				char		szFilterList[32];

				/* capture radio button state */
				Options.bActive = IsDlgButtonChecked( hDlg, BUG_ON );
			
				/* capture check box states */
				Options.bCount = IsDlgButtonChecked( hDlg, BUG_COUNT);
				Options.bDisplay=IsDlgButtonChecked(hDlg,BUG_DISPLAY);
				Options.bFilter=IsDlgButtonChecked( hDlg,BUG_FILTER);
				Options.bLog = IsDlgButtonChecked( hDlg, BUG_LOG );

				/* capture filter list changes - no limit checking! */
				GetDlgItemText(
					hDlg,
					BUG_FILTERLIST,
					szFilterList,
					sizeof(szFilterList)
				);
			
				Options.wFilterSize = 0;
				spToken = strtok( szFilterList, " ,;" );
				while ( spToken ) {
					Options.Filter[ Options.wFilterSize++ ] = 
													atoi( spToken );
					spToken = strtok( NULL, " ,;" );
				}
					
				/* capture log file changes */
				GetDlgItemText(
					hDlg,
					BUG_LOGFILE,
					Options.szLogFile,
					sizeof(Options.szLogFile)
				);
			
				/* display listbox if necessary */
				if ( Options.bActive ) {

			
					/* update viewport window caption */
					if ( Options.bFilter && Options.wFilterSize ) {
						
						/* define filter list */
						szFilterList[0] = 0;
						if ( Options.wFilterSize ) {
							sprintf( szFilterList, "%u", 
										Options.Filter[0] );
							for ( i=1; i<Options.wFilterSize; i++ ) {
								sprintf( szToken, ",%u", 
										Options.Filter[i] );
								strcat( szFilterList, szToken );
							}
						}	
						
						sprintf( 
							szCaption, 
							"%s - (%s)", 
							Options.szLogFile, 
							szFilterList 
						);
					
					} else
						sprintf( szCaption, "%s - (ALL)", 
									Options.szLogFile );
					
					/* define caption & make window visible */
					SetWindowText( Options.hListBox, szCaption );
					ShowWindow( Options.hListBox, SHOW_OPENWINDOW );
					
				} else
					ShowWindow( Options.hListBox, HIDE_WINDOW );
				
				/* exit */
				EndDialog( hDlg, TRUE );
			
			}
			
			break;
		case BUG_CANCEL :	/* cancel control panel */
		case IDCANCEL :
			EndDialog( hDlg, FALSE );
			break;
		default :			/* ignore all others */
			bResult = FALSE;
			break;
		}
		
		break;
	default :
		bResult = FALSE;
		break;
	}

	/* return final result */
	return( bResult );

}

/*
 * Debug( wCategory, ParmList) : BOOL FAR
 *
 *		wCategory		debug message classification
 *		ParmList		debug parameter list
 *
 * This function outputs a formatted message to the debug
 * utility routines.  The wCategory variable determines 
 * how the message is filtered.  The ParmList is a standard
 * printf parameter list.  A value of TRUE is returned if
 * the message was successfully processed and not filtered.
 *
 * Note that this function CANNOT be declared of type
 * PASCAL as it uses a variable number of arguments!  Also
 * note that alternative methods are available for handling
 * a variable number of function parameters.
 *
 */
 
BOOL FAR Debug( wCategory, ParmList )
	WORD		wCategory;
	struct {
		char	cBytes[64];
	} ParmList;
{
	/* local variables */
	WORD		i;					/* temporary loop variable */
	int			hFile;				/* log file handle */
	BOOL		bResult;			/* result of function */
	BOOL		bInclude;			/* include message flag */
	char		szStatement[132];	/* temporary statement */

	/* initialization */
	bResult = FALSE;

	/* process statement if debug active */
	if ( Options.bSetup && Options.bActive ) {
	
		bInclude = TRUE;

		/* check for inclusion in filter list */
		if ( Options.bFilter ) {
			
			/* search filter list */
			for (
				i=0; 
				(i<Options.wFilterSize)&&(Options.Filter[i]!=
														wCategory); 
				i++ 
			);
		
			/* turn off include if not found */
			if ( i >= Options.wFilterSize )
				bInclude = FALSE;
		
		}
		
		/* format debug statement if included */
		if ( bInclude ) {
		
			/* prepare for debug statement */
			i = 0;
			bResult = TRUE;
			Options.lMsgCount++;
			
			/* define debug statement */
			if ( Options.bCount ) 
				i = sprintf( szStatement, "%ld ", Options.lMsgCount );
			i += sprintf( &szStatement[i], ParmList );
				
			/* display debug statement in listbox */
			if ( Options.bDisplay ) {			
				
				/* Note: since we are passing the formatted
				 * debugging statement to the listbox using
				 * a SendMessage we cannot do anything which
				 * would cause memory to be shuffled - this
				 * invalidates the pointer!
				 */
				
				/* add new statement to listbox */
				SendMessage(
					Options.hListBox,
					LB_ADDSTRING,
					(WORD)0,
					(LONG)(LPSTR)szStatement
				);
						
				/* remove first statement from listbox if necessary */
				if ( Options.wEntries >= Options.wMaxEntries ) {
					SendMessage(
						Options.hListBox,
						LB_DELETESTRING,
						0,
						(LONG)0
					);
				} else
					Options.wEntries++; 
			
				/* make last statement visible */
				SendMessage(
					Options.hListBox,
					LB_SETCURSEL,
					(WORD)Options.wEntries-1,
					(LONG)0
				);

			}
			
			/* output debug statement to logfile */
			if ( Options.bLog ) {
			
				/* open or create log file */
				hFile = open( Options.szLogFile, MODE_APPEND, SHARED);
				if ( hFile < 0 )
					hFile=open(Options.szLogFile,MODE_CREATE,SHARED);
					
				/* write message to file if sucessful */
				if ( hFile > 0 ) {
					write( hFile, szStatement, i );
					write( hFile, "\r\n", 2 );
					close( hFile );
				} 
			
			}
		
		} 
	
	} 
	
	/* return result */
	return( bResult );

}	
