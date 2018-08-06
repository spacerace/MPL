/****************************************************************************
   
    TED.C - Tiny Editor: Sample application

    Created by Microsoft Corporation, IBM Corporation 1989

-----------------------------------------------------------------------------

    This module contains the main processing elements of the TED sample
    application.  This application is a very simple editor which utilizies
    an MLE to do all the text interaction with the user.  Full help support
    is built in using Help Manager. The application is organized into the
    following 15 files.


        Ted.c     - Contains the main processing loop and all the MLE handler
                    functions.

        Ted.h     - Contains the #define and function prototypes.

        Ted.rc    - Contains the Menu entries, the accelerator table and
                    the string resources.

        Ted.ico   - The icon that appears when Ted is minimized and also
                    in the about box.

        Ted.def   - Module definition file for Ted.


        Tedmem.c  - Contains the memory allocation functions.

        Tedmem.h  - Contains the #define and the function prototypes for
                    the memory allocation functions.


        Tedhelp.c - Contains the help manager support functions.

        Tedhelp.h - Contains all the ID's for the Help Manager help panels

        Tedhelp.rc- Contains the Help Manager tables and sub-tables
                    that associate each selectable item with a particular
                    help panel in the Tedhp.itl file.


        Tedhp.itl - Contains the Help Manager help panels.

        Tedhp.h   - Contains the #defines for the help panel id's

        Teddlg.dlg- Contains the Dialog templates for the Find and
                    about dialog boxes.

        Teddlg.h  - Contains the #define ID's for the Find and about dialog
                    boxes


        MAKEFILE  - The makefile (nmake.exe) for the TED application.




    DLL's used:


        opendlg.dll - This dll is provided in the toolkit and it contains
                      the open/save as dialog used for choosing files.
                      The directory it resides in must be specified in
                      the LIBPATH.

    Import libraries used:


        opendlg.lib - This contains the functions used to interface
                      to the open/save dialog DLL.

        opendlg.h   - This contains the functions prototypes etc
                      used to interface to the open/save dialog DLL.


    Help Libraries used:


        tedhp.hlp   - This is the Help Manager help panel storage file
                      It must be either in the directory specified by
                      the HELP environment variable, in \OS2\HELP, or
                      in the current directory.



        Modification History

            891005    James Bratsanos, Updated comment so help file (.hlp)
                                       can now be in current directory.

            890911    James Bratsanos, Created



****************************************************************************/


/****************************************************************************

    Include Files, Macros, Defined Constants

****************************************************************************/

#define INCL_WIN
#define INCL_DOS

#include <os2.h>

#include <stdio.h>
#include <opendlg.h>    /* Header file for toolkit open/save dialogs */
#include <string.h>

#include "ted.h"
#include "tedhelp.h"
#include "teddlg.h"
#include "tedmem.h"



/****************************************************************************

    Variables Used Globally in this module

****************************************************************************/

HWND  hwndFrame   = NULL;   /* The window handle of the frame */
HWND  hwndMenu    = NULL;   /* The window handle of our menu  */
HWND  hwndMLE     = NULL;   /* The window handle of the MLE (Client of Frame*/
HWND  hwndDlgFind = NULL;   /* The window handle of the FIND dialog box */
HAB   hab         = NULL;   /* The anchor block handle for TED */
PFNWP pfnwpold    = NULL;   /* A pointer to the old window procedure of */
                            /* the Client (the MLE window proc )        */

HMQ   hmq ;                 /* Message queue handle */

PFNWP pfnwpoldframe = NULL; /* A pointer to the old window procedure of */
                            /* the Frame */


struct _TEDINFO {           /* Used to keep track of the file name and  */
                            /* path currently being edited              */

    UCHAR uchFileName[ CCHMAXPATH ];         /* Just the file name */
    UCHAR uchFullPathName[ CCHMAXPATH];      /* The full path      */
    UCHAR uchPgmName[  CCHMAXPATH ];         /* Program name from title bar */

};

struct _TEDINFO tedinfo;



/****************************************************************************

    main - This is the main procedure of the ted application.  It does
           any initialization that is required goes into the standard
           get / dispatch message loop and, finally does any cleanup
           that is required.

****************************************************************************/

VOID main (VOID)
{


    static ULONG flFrameFlags = FCF_STANDARD;
    QMSG         qmsg ;     /* Information used to dispatch msg's */


    /********************************************************************
        Create the anchor block (HAB) and the message queue for our
        application.
    ********************************************************************/



    hab = WinInitialize (0) ;
    hmq = WinCreateMsgQueue (hab, 0) ;



    /********************************************************************
        Create the frame window and specify that the client is an MLE
        So this way the MLE will handle all the WM_SIZE, WM_PAINT msg
        and we dont have to.
    ********************************************************************/


    hwndFrame = WinCreateStdWindow (HWND_DESKTOP,
   		  		    WS_VISIBLE | MLS_WORDWRAP |
                                                 FS_ACCELTABLE,
			            &flFrameFlags,
				    WC_MLE ,
				    NULL,
			            WS_VISIBLE | MLS_HSCROLL | MLS_VSCROLL |
		  		                 MLS_WORDWRAP ,
				    (HMODULE) NULL,
				    ID_RESOURCE,
				    &hwndMLE ) ;


    if ( hwndFrame == NULL || hwndMLE == NULL )
    {
        TEDDisplayErrorID( SID_UNABLE_TO_CREATE_FRAME  );

        TEDCleanupExit();      /* Cleanup and exit , NOTE: never returns!! */
    }



    /*  Sub class the frame so we get the WM_CONTROL MSG's from the MLE */



    if ( ( pfnwpoldframe = WinSubclassWindow( hwndFrame, FrameWndProc ))
                                  == (PFNWP) 0 )
    {
        TEDDisplayErrorID( SID_UNABLE_TO_SUB_FRAME );

        TEDCleanupExit();      /* Cleanup and exit , NOTE: never returns!! */
    }

    /********************************************************************
        Subclass the client window in order to receive the MSG's from
        our menu.
    ********************************************************************/


    if ( (pfnwpold = WinSubclassWindow( hwndMLE , ClientWndProc ))
                                       ==   (PFNWP) 0 )
    {
        TEDDisplayErrorID( SID_UNABLE_TO_SUB_CLIENT );

        TEDCleanupExit();      /* Cleanup and exit , NOTE: never returns!! */
    }


    /********************************************************************
        Since the MLE sends messages to its owner and WinCreateStdWindow
        does not assign an owner to its client then set the owner of the
        MLE to be the frame (Since we subclassed the frame window
        earlier we now receive the WM_CONTROL (notification messages)
        from the MLE
    ********************************************************************/


    if ( !WinSetOwner( hwndMLE, hwndFrame ))
    {
        TEDDisplayErrorID( SID_UNABLE_TO_SET_FRAME_OWNER );

        TEDCleanupExit();      /* Cleanup and exit , NOTE: never returns!! */
    }



    MLEInit ( hwndMLE );



    /*  Save off the id of our menu so we can send it msg's as required. */


    hwndMenu = WinWindowFromID(hwndFrame, FID_MENU) ;



    /*  Set the initial state of the Word Wrap menu item on our menu */


    MLESetWrap( hwndMLE, hwndMenu, FALSE );

    /*****************************************************************
        Init the Help Manager. No need to check for success since TED
        will function fine without help.
    *****************************************************************/


    TEDInitHelp( hwndFrame, hab );



    /* Get the name of the program so we can use it later in the title */

    WinQueryWindowText( hwndFrame ,
                        sizeof(tedinfo.uchPgmName ),
                        tedinfo.uchPgmName);



    /********************************************************************
        Setup the path name portion of our program to be NULL and the
        title to be Untitled
    ********************************************************************/


    TEDInitFileName();


    TEDUpdateTitleBar( hwndFrame, tedinfo.uchPgmName, tedinfo.uchFileName );



    /********************************************************************
        Stay in the standard Presentation manager processing loop
        until a WM_QUIT is received. This causes WinGetMsg() to
        return FALSE and thus the loop is exited.
    ********************************************************************/


    while(WinGetMsg (hab, &qmsg, NULL, 0, 0))
    {
        WinDispatchMsg (hab, &qmsg) ;
    }



    /********************************************************************
        Do any cleanup the help manager requires. We do not need
        to cleanup the ABOUT dialog since it is modal and is destroyed
        every time the user removes it.
    ********************************************************************/


    TEDTerminateHelp( hwndFrame );



    TEDCleanupExit();     /* Cleanup and exit , NOTE: never returns!! */


}



/**********************************************************************

    TEDInitFileName - This function initalizes the full path of the
                      current file to be NULL and the current file
                      name to be "Untitled". This way the save dialog
                      need only check if the full path is null and
                      thus it will know that no file has been set.
                      As soon as we do a OPEN or SAVE AS then the full
                      path will be set to something other than NULL
                      and thus the file will no longer be considered
                      Untitled.


**********************************************************************/


VOID TEDInitFileName()
{


    /* Load the string that will signifiy an Untitled file name */


    if ( WinLoadString( hab,
                        (HMODULE) NULL,
                        SID_TED_UNTITLED,
                        sizeof( tedinfo.uchFileName),
                        tedinfo.uchFileName )  == (SHORT) 0 )
    {
        MESSAGE("Cannot load untitled string ");
    }

    tedinfo.uchFullPathName[0] = '\000';   /* Make the path empty */

}




/**********************************************************************

    FrameWndProc - This window procedure is used to catch
                   notification messages (WM_CONTROL) from the MLE
                   and also Help Manager errors.  This procedure
                   actually is subclassed on the frame and all
                   non processed messages are passed through to the
                   default frame proc.

**********************************************************************/

MRESULT EXPENTRY FrameWndProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{

    MRESULT mresult;    /* Used to return Help Panel ID to Help Manager */


    switch( msg )
    {
        case WM_CONTROL:                          /* Control Message      */

	    switch( LOUSHORT( mp1 ) )             /* ID of control        */
	    {
		case FID_CLIENT :                 /* Verify its from MLE  */


		    switch( HIUSHORT( mp1 ) )     /* Notification Message */
		    {

                    /******************************************************
                        The MLE will send a MLN_CHANGE message when
                        something in the MLE is modified. This could
                        be used to do smart closing where TED would
                        notify the user if an exit attempt was made
                        when the MLE text had unsaved changes.
                    ******************************************************/

			case MLN_CHANGE:

                        /* Just do the old frame procedure processing */

			    break;

                    /******************************************************
                        The MLE will send a MLN_TEXTOVERFLOW message when
                        a key stroke would cause the text in the MLE to
                        exceed the TEXT_LIMIT set
                    ******************************************************/

			case MLN_TEXTOVERFLOW:

			    TEDDisplayErrorID( SID_MLEMSG_TEXTOVERFLOW );

                            return FALSE; /* Tell MLE to not allow overflow */

			    break;


                    /******************************************************
                        The MLE will send a MLN_OVERFLOW message when
                        an action other that a key stroke would cause
                        the text in the MLE to exceed the allowable
                        amount, or the format rectangle would have
                        been inadequate to contain the text.

                        NOTE: mp2 is a pointer to a OVERFLOW structure
                              containing detailed information as to
                              what caused the overflow.
                    ******************************************************/


			case MLN_OVERFLOW:

                            TEDDisplayErrorID( SID_MLEMSG_MLN_OVERFLOW );

			    return FALSE; /* Tell MLE to not allow overflow */

			    break;


			default:

                        /* Just do the old frame procedure processing */

			    break;

		    } /* End switch( HIUSHORT( mp1 )) */
		    break;

		default :

                /* Just do the old frame procedure processing */

		    break;


	    }  /* End switch( LOUSHORT( mp1 ) ) */


        /* Just do the old frame procedure processing */

	    break;


        default:

        /********************************************************************
            Process any help messages that come through.
            If TEDProcessHelpMsg() can handle any of the Help messages it
            receives the TRUE is returned and we return whatever was
            passed back. Other wise the code falls through to the default
            window procedure.
        ********************************************************************/


            if ( TEDProcessHelpMsg( hwnd, msg, mp1, mp2, &mresult ) )
            {
                return( mresult );
            }

	    break;

    }  /* End switch( msg ) */


    return( (*pfnwpoldframe) (hwnd, msg, mp1, mp2)) ;


}





/**********************************************************************

    ClientWndProc - This procedure is the window proc (subclassed) for
                    the MLE. It handles any of the menu selections made
                    in our menu and passes everything else through to
                    the default window procedure (which is the MLE )

**********************************************************************/



MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
    CHAR achtmp[50];  /* Used to format the string when the user chooses the */
                      /* Query lines , menu choice to see how many lines     */
                      /* there currently are in the MLE                      */

    LONG     lFileSize;       /* The amount of text in the MLE */
    HFILE    hf;              /* File handle used for reading/writing */
    USHORT   usAction;        /* Action field of DosOpen call */


    switch (msg)
    {


        case WM_COMMAND:

            switch( LOUSHORT(mp1) )    /* Switch on Menu ID */
            {

      	        case IDM_NEWFILE :

                /********************************************************
                    If we disable the MLE then it wont try to keep the
                    display current while were deleting its contents.
                    This way the MLE pointer will turn to an hour glass,
                    all its text will be deleted (by selecting all the
                    text in the MLE and deleting it) then the hour glass
                    will be changed back to the system pointer.  This
                    way it appears better to the user since he does not
                    visually see all the text in the mle get selected
                    and then deleted.
                ********************************************************/

                    MLEDisable( hwndMLE );

	            MLEDeleteContents( hwndMLE );

                    MLEEnable( hwndMLE );

                    TEDInitFileName();

                    TEDUpdateTitleBar( hwndFrame,
                                       tedinfo.uchPgmName,
                                       tedinfo.uchFileName );

                    break ;

	        case IDM_OPENFILE :


	            MLEOpenFile( hwnd,
                                 tedinfo.uchFileName,
                                 tedinfo.uchFullPathName );

                    TEDUpdateTitleBar( hwndFrame,
                                       tedinfo.uchPgmName,
                                       tedinfo.uchFileName );

                    break ;

                case IDM_SAVE:

                /************************************************************
                    If uchFullPathName is empty then the file name is
                    Untitled so we cannot do a save (since we have no name)
                    so send a message to ourselves as though the user had
                    clicked on the Save as menu item. If the file name is
                    not Untitled then open the current file and save the
                    contents of the MLE.
                ************************************************************/

                    if ( tedinfo.uchFullPathName[0] == '\000' )
                    {

                        return ( WinSendMsg( hwnd,
                                             WM_COMMAND,
                                             MPFROMSHORT( IDM_SAVEAS ),
                                             mp2 ));


                    }
                    else
                    {


                        if ( DosOpen( tedinfo.uchFullPathName,
                                      &hf,
                                      &usAction,
                                      (ULONG) 0,
                                      (USHORT) 0,
                                      FILE_OPEN,
                                      OPEN_ACCESS_WRITEONLY |
                                        OPEN_SHARE_DENYWRITE,
                                      (ULONG) 0 ) )
                        {

                            TEDDisplayErrorID( SID_CANT_OPEN_EX_FILE );

                        }
                        else
                        {

                            MLESaveToFile( hf );
                            DosClose( hf );

                        }

                    }
                    break;

                case IDM_SAVEAS :

	            MLESaveFile( hwnd,
                                 tedinfo.uchFileName,
                                 tedinfo.uchFullPathName );

                    TEDUpdateTitleBar( hwndFrame,
                                       tedinfo.uchPgmName,
                                       tedinfo.uchFileName );

                    break ;

                case IDM_FIND :



                /**********************************************************
                    If the handle to our Find dialog is not null then the
                    Find dialog is loaded so we need only show the window
                    and then give it the focus. If the handle is NULL then
                    the Find dialog has never been loaded so we need to
                    load it.  We use WinLoadDlg() because the Find dialog
                    is modeless ie the user can switch between the find
                    dialog and the mle without first having to terminate the
                    find dialog
                **********************************************************/

                    if ( hwndDlgFind != NULL )
                    {

                        WinShowWindow( hwndDlgFind, TRUE );
                        WinSetFocus( HWND_DESKTOP, hwndDlgFind );


                    }
                    else
                    {

         	        if ( (hwndDlgFind = WinLoadDlg(HWND_DESKTOP,
                                                       hwnd,
                                                       FindDlgProc,
                                                       (HMODULE) NULL,
                                                       DLG_FIND,
                                                       NULL )) == NULL )
                        {

                            TEDDisplayErrorID( SID_DLG_CANT_LOAD_FIND );

                        }
                    }
                    break ;

            /***********************************************************
                Since the MLE supports the CUT, COPY PASTE and UNDO we
                need only send the apropriate msg to the MLE and let it
                do all the work
            ***********************************************************/

	        case IDM_CUT	  :

	            WinSendMsg( hwndMLE, MLM_CUT, NULL, NULL );
	            break ;

	        case IDM_COPY	  :

	            WinSendMsg( hwndMLE, MLM_COPY, NULL, NULL );
                    break;

                case IDM_PASTE   :

	            WinSendMsg( hwndMLE, MLM_PASTE, NULL, NULL ) ;
                    break ;

	        case IDM_UNDO	  :

	            WinSendMsg( hwndMLE, MLM_UNDO, NULL, NULL );
                    break ;

                case IDM_SELECT_ALL :

                /**********************************************************
                    In order to Select all the text in the MLE we need to
                    first query the mle as to the amount of text that is
                    currently in it. Then we send a message to the mle
                    to select the text from the beggining of the mle (IPT 0)
                    to the amount of text the mle told us it had in it.
                **********************************************************/

                    lFileSize= (LONG) WinSendMsg( hwndMLE ,
		         			  MLM_QUERYTEXTLENGTH,
			     		          NULL,
					          NULL );

	            WinSendMsg(hwndMLE,
                               MLM_SETSEL,
                               (MPARAM) 0L,
                               (MPARAM) lFileSize);

	            break;


                case IDM_QUERYLINES:

                /* achtmp can only hold a message that is 50 bytes long */

        	    sprintf( achtmp, "%ld Lines in Window",
      			             (LONG) WinSendMsg( hwndMLE,
    				        		MLM_QUERYLINECOUNT,
						        NULL,
						        NULL )) ;

 	            MESSAGE( achtmp );
	            break;


	        case IDM_SETWRAP :

                    MLESetWrap( hwndMLE, hwndMenu, TRUE );
	            break ;


	        case IDM_DISPLAY_HELP:

                    TEDSendHelpMsg( HM_DISPLAY_HELP );
	            break;


	        case IDM_EXT_HELP:

	            TEDSendHelpMsg( HM_EXT_HELP );
	            break;

	        case IDM_INDEX_HELP:

	            TEDSendHelpMsg( HM_HELP_INDEX );
	            break;


	        case IDM_KEYS_HELP:

	            TEDSendHelpMsg( HM_KEYS_HELP );
	            break;

                case IDM_ABOUT   :      /* About TED dialog */

	            if ( WinDlgBox( HWND_DESKTOP,
		                    hwndMLE,
		                    AboutDlgProc,
		                    (HMODULE) NULL,
		                    DLG_ABOUT,
		                    NULL )  == DID_ERROR )
                    {

                        TEDDisplayErrorID( SID_DLG_CANT_LOAD_ABOUT );

                    }
	            break ;

                default:

	        /* Fall through to old procedure processing */

	            break;


            }  /* End switch ( LOUSHORT( mp1 )) */

            break ;

        default:

        /* Fall through to old procedure processing */

            break;



    }  /* End switch ( msg ) */

    return( (*pfnwpold) (hwnd, msg, mp1, mp2)) ;
}




/**********************************************************************

    AboutDlgProc - This procedure is the dialog procedure for the
                   About box.

**********************************************************************/


MRESULT EXPENTRY AboutDlgProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{


    switch( msg )
    {

        case WM_INITDLG:

            TEDInitDlgSysMenu( hwnd );    /* setup the system menu */

            break;

        case WM_CLOSE:

            if ( !WinDismissDlg(hwnd, TRUE))
            {
                TEDDisplayErrorID( SID_DLG_FIND_CANT_DISMISS );

                TEDCleanupExit(); /* Cleanup and exit , NOTE: never returns!! */
            }


            return((MRESULT)TRUE);

            break;


        case WM_COMMAND:                        /* Process the Buttons  */


            switch (LOUSHORT(mp1))
            {

                case DID_OK:                    /* OK */

                    if ( !WinDismissDlg( hwnd, TRUE ))
                    {
                        TEDDisplayErrorID( SID_DLG_FIND_CANT_DISMISS );
                    }

                    return(MRESULT) TRUE;

                    break;

                default:

                    return(MRESULT) TRUE;
                    break;
            }

        default:

            return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );

            break;

    }
    return (MRESULT) FALSE;
}



/**********************************************************************

    FindDlgProc - This procedure is the dialog procedure for the
                  Find dialog box.  It is modeless so the user
                  is allowed to switch between the Find dialog box
                  and the MLE.

**********************************************************************/



MRESULT EXPENTRY FindDlgProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
    UCHAR   auchFindText[MAX_STRING_SIZE];  /* The find text from the     */
                                            /* FIND dialog box            */

    UCHAR   auchChangeText[MAX_STRING_SIZE];/* The change to text from    */
                                            /* the FIND dialog box        */

    ULONG   ulSearchFlags;                  /* Search flags passed to MLE */


    MLE_SEARCHDATA seMLESearchData;         /* The MLE search structure   */


    switch (msg)
    {

        case WM_INITDLG:


        /*******************************************************************
            Send a msg to each of the entry fields which contain the
            FIND and CHANGE TO text in order to limit the size to the
            buffer we have allocated to receive that text.
        *******************************************************************/

            WinSendMsg( WinWindowFromID( hwnd, DID_FIND_TEXT),
                        EM_SETTEXTLIMIT,
                        MPFROMSHORT( MAX_STRING_SIZE),
                        NULL );


            WinSendMsg( WinWindowFromID( hwnd, DID_CHANGE_TEXT),
                        EM_SETTEXTLIMIT,
                        MPFROMSHORT( MAX_STRING_SIZE),
                        NULL );


            TEDInitDlgSysMenu( hwnd );


            break;


        case WM_COMMAND:


            switch (SHORT1FROMMP(mp1))
            {

                case DID_FIND_CHANGE_ALL:


                /* Get the FIND and CHANGE TO text the user entered */

                    WinQueryWindowText( WinWindowFromID( hwnd, DID_FIND_TEXT),
                                        MAX_FNAME_LEN,
                                        auchFindText );


                    WinQueryWindowText( WinWindowFromID( hwnd, DID_CHANGE_TEXT),
                                        MAX_FNAME_LEN,
                                        auchChangeText );


                    ulSearchFlags = MLFSEARCH_CHANGEALL;

                    if ( WinSendMsg( WinWindowFromID(hwnd,DID_SENSITIVE),
                                     BM_QUERYCHECK,
                                     NULL,
                                     NULL))
                    {

                        ulSearchFlags |= MLFSEARCH_CASESENSITIVE;
                    }


                /* Fill the MLE search structure with the search/change data */

                    seMLESearchData.cb         = sizeof( seMLESearchData );

                    seMLESearchData.pchFind    = auchFindText;
                    seMLESearchData.cchFind    = strlen( auchFindText );

                    seMLESearchData.pchReplace = auchChangeText;
                    seMLESearchData.cchReplace = strlen( auchChangeText );

                    seMLESearchData.iptStart   =  0; /* Start at cursor IPT  */
                    seMLESearchData.iptStop    = -1; /* Search to end of MLE */



                    if ( seMLESearchData.cchFind ) /* Non-empty string to    */
                    {                              /* to search for          */



                        if ( !WinSendMsg( hwndMLE,
                                          MLM_SEARCH,
                                          (MPARAM) ulSearchFlags,
                                          (MPARAM) &seMLESearchData ) )
                        {

                            TEDDisplayErrorID( SID_CANT_FIND );

                        }
                        else
                        {

                            WinSetFocus( HWND_DESKTOP, hwndMLE );

                        }

                    }
                    break;



                case DID_FIND:


                    WinQueryWindowText( WinWindowFromID( hwnd, DID_FIND_TEXT),
                                        MAX_FNAME_LEN,
                                        auchFindText );


                    ulSearchFlags = MLFSEARCH_SELECTMATCH;

                    if ( WinSendMsg( WinWindowFromID(hwnd,DID_SENSITIVE),
                                     BM_QUERYCHECK,
                                     NULL,
                                     NULL))
                    {

                        ulSearchFlags |= MLFSEARCH_CASESENSITIVE;
                    }


                    seMLESearchData.cb       = sizeof( seMLESearchData );

                    seMLESearchData.pchFind  = auchFindText;
                    seMLESearchData.cchFind  = strlen( auchFindText );


                /*************************************************************
                    The search always starts at the cursor point and
                    continues to the end of the MLE.  It would be better
                    to put a WRAP box in the Find dialog box so the search
                    would wrap around to the beginning if the user so desired.
                    Currently the user has to <ctrl> HOME to get to the top
                    of the MLE before doing a find/Change all once the
                    cursor is at the bottom of the MLE.
                *************************************************************/


                    seMLESearchData.iptStart = -1;   /* Start at cursor IPT  */
                    seMLESearchData.iptStop  = -1;   /* Search to end of MLE */


                /* Is there some text to search for ? */

                    if ( seMLESearchData.cchFind )
                    {

                    /**********************************************************
                        Since a WinSendMsg is done here instead of a WinPost
                        then the search will be synchronous.  The
                        application will wait until the search is complete
                        before continuing.  This is ok in this application
                        since we are limiting the search.  The MLE sends
                        notification messages occasionally when it is searching
                        so the the user can have the option of aborting the
                        search.
                    **********************************************************/


                        if ( !WinSendMsg( hwndMLE,
                                          MLM_SEARCH,
                                          (MPARAM) ulSearchFlags,
                                          (MPARAM) &seMLESearchData ) )
                        {

                            TEDDisplayErrorID( SID_CANT_FIND );

                        }
                        else
                        {

                            WinSetFocus( HWND_DESKTOP, hwndMLE );

                        }

                    }

                    break;

                 case MBID_CANCEL:   /* modeless so don't no need to dismiss */

                    WinPostMsg( hwnd, WM_CLOSE, NULL, NULL );
                    break;

                 default:

                    return( WinDefDlgProc( hwnd, msg, mp1, mp2 ));
                    break;

            } /* End switch (SHORT1FROMMP(mp1)) */
            break;

            return (MRESULT) FALSE;

        default:
            return( WinDefDlgProc( hwnd, msg, mp1, mp2 ));
	    break;

    } /* End switch ( msg ) */

    return (MRESULT) FALSE ;

}




/**********************************************************************

    MLESaveFile - This procedure enables the save file dialog, lets
                  the user choose a file, then saves the contents of
                  the MLE to that file.

**********************************************************************/



VOID MLESaveFile( HWND hwnd, PSZ pszFname, PSZ pszFullName )
{

    BOOL    fSuccess = TRUE;     /* Flag if user want to save               */
    DLF	    dlf;                 /* Data structure for save dialog          */
    HFILE   hf;                  /* File handle of file to save MLE in      */


/*****************************************************************************
    Initialize the data structure used by the save as toolkit dialog.
    Call DlgFile which will put up the save as dialog and return back
    a handle to the opened file.
*****************************************************************************/


    SetupDLF( &dlf,                      /* Pointer to dialog data structure */
              DLG_SAVEDLG | DLG_HELP,    /* Type of dialog to bring up       */
              &hf,                       /* Pointer to file handle           */
              "\\*.DOC",                 /* Mask of files to bring up        */
              "Dialog Error",            /* Msgbox title if Error            */
              "Save as",                 /* Title of Save as dialog window   */
              "No Help Available");      /* Help message                     */


    dlf.szFileName[0] = dlf.szOpenFile[0] = '\000' ;



    switch( DlgFile( hwnd, &dlf ))
    {

	case TDF_ERRMEM:
	case TDF_INVALID:
        case TDF_NOOPEN:

	    fSuccess = FALSE;
	    break;

	case TDF_NOSAVE:

	    fSuccess = FALSE;
	    break;

	default:

	    break;

    }

/*****************************************************************************
    If fSuccess is TRUE then the user chose a file and we have the handle
    to it.  So we save the contents of the MLE to that file handle and update
    the file name information.
*****************************************************************************/


    if ( fSuccess )
    {

        if ( MLESaveToFile( hf ))
        {

        /* Copy the file info into the correct place */

            strcpy( pszFname, dlf.szFileName );
            strcpy( pszFullName, dlf.szOpenFile );

        }

        DosClose( hf );


    }

}

/*****************************************************************************

    MLESaveToFile - This procedure takes a passed file handle and writes
                    the contents of the MLE to it.

          Returns - True if the MLE was written to the file handle
                    succesfully otherwise FALSE is returned.

*****************************************************************************/

BOOL MLESaveToFile( HFILE hf )
{

    PCHAR   pvBuff;              /* Pointer to buffer with exported MLE data*/
    IPT	    iptFormatedTextSize; /* The number of bytes of FORMATTED data   */
    IPT     iptTextSize;         /* The number of bytes in the MLE          */
                                 /* NOTE!!! not equal to iptFormatedTextSize*/

    IPT	    iptData;             /* Temp used to hold IPT data              */
    USHORT  cbBytesWritten;      /* Number of bytes written from DosWrite   */
    BOOL    bRetVal = TRUE;      /* Default to OK                           */


    /*************************************************************************
        First we query the MLE to find out how much data is it. The
        reason we ask for it with the QUERYFORMATTEXTLENGTH is because
        the MLE stores the text in a different format than it exports it.
        Mainly because it stores hard returns as one byte (LF) although
        based on the formatting currently enabled it may export the LF as
        a CR/LF in which case asking the MLE how much data it has with the
        QUERYTEXTLENGTH message would yield a number too small.

        In essence we are asking the MLE :

             "If the data were exported at this time how many bytes would
              be required to store it??"
    *************************************************************************/


        iptFormatedTextSize = (IPT) WinSendMsg( hwndMLE,
                                                MLM_QUERYFORMATTEXTLENGTH,
                                                (MPARAM) (IPT) 0L,  /*beg MLE*/
                                                (MPARAM) (IPT) -1 );/*end MLE*/



    /* Now Query MLE to find out how much data there is (IPT format) */

        iptTextSize = (IPT) WinSendMsg( hwndMLE,
                                        MLM_QUERYTEXTLENGTH,
                                        NULL,
                                        NULL );


    /* Allocate a buffer based on the formated size */

    /*********************************************************************
       The "+ (IPT) 1" is due to a bug in the MLE that should have been
       corrected.  This plus one will not effect the sample app once
       the bug is fixed
    *********************************************************************/


        if ( MyAllocMem( &pvBuff, iptFormatedTextSize + (IPT) 1, 0 ) ) {


	    TEDDisplayErrorID( SID_CANT_ALLOCATE_MEMORY );

            bRetVal = FALSE;

        }
        else
        {

	/* Set up IMPORT/EXPORT transfer location */

	    WinSendMsg( hwndMLE,
                        MLM_SETIMPORTEXPORT,
			(MPARAM) pvBuff,                 /* Pointer to buff */
			(MPARAM) iptFormatedTextSize );  /* Size of buff    */



        /* Now do the Export into the allocated BUFFER */


	    iptData = 0L;

	    WinSendMsg( hwndMLE,
                        MLM_EXPORT,                /* EXPORT TYPE             */
                        (MPARAM) &iptData,         /* IPT start for EXPORT    */
                        (MPARAM) &iptTextSize ) ;  /* Amount to EXPORT (bytes)*/


        /**********************************************************************
            iptTextSize is decremented by the MLE based on how many bytes
            were actually exported.  If this number is not 0 then an incorrect
            amount of data was exported.
        **********************************************************************/


            if ( iptTextSize != (IPT) 0L )
            {
                TEDDisplayErrorID( SID_UNABLE_TO_EXPORT );
                bRetVal = FALSE;
            }


        /**********************************************************************
            Since we might of been shrinking the size of the file we need to
            reset the size of the file to 0. This way the size of the file
            becomes the number of bytes that we will write to it.
        **********************************************************************/

	    if ( bRetVal && DosNewSize( hf, 0L ) )
            {
                TEDDisplayErrorID( SID_UNABLE_TO_SET_NEWSIZE );
                bRetVal = FALSE;
	    }


        /**********************************************************************
           Note iptFormatedTextSize is not checked since we limit the
           size of the MLE to something that will fit in a USHORT.
        **********************************************************************/

  	    if ( bRetVal && DosWrite( hf,
                                      (PVOID) pvBuff,
                                      (USHORT) iptFormatedTextSize,
                                      &cbBytesWritten ) )
            {

                TEDDisplayErrorID( SID_DOSWRITE_FAILED );
                bRetVal = FALSE;

            }

        /* Free the buffer we used for the transfer */

            MyFreeMem( pvBuff );


   	    if ( bRetVal && ( iptFormatedTextSize != (IPT) cbBytesWritten ))
	    {
	        TEDDisplayErrorID( SID_WRONG_BYTE_COUNT );
                bRetVal = FALSE;
	    }





        } /* End if Buffer available */

    return ( bRetVal );   /* return success flag */

}


/*****************************************************************************

    MLEOpenFile - This procedure prompts the user for a file name to edit
                  using the open file dialog supplied with the toolkit.
                  If a user chooses a file to open then the file name and
                  fully qualified path are copied into the passed parameters.

*****************************************************************************/

VOID MLEOpenFile( HWND hwnd, PSZ pszFname, PSZ pszFullName )
{

    BOOL       fSuccess = TRUE;  /* Success flag for file chosen by user */
    DLF	       dlf;              /* Data structure for Open dialog       */
    HFILE      hf;               /* File handle of file chosen by user   */
    PCHAR      pvBuff;           /* Buffer user to import file into MLE  */
    USHORT     cbBytesRead;      /* Number of bytes read by DosRead      */
    FILESTATUS stsInfo;          /* Information about the file           */
    IPT	       iptData;          /* Position in MLE to import file       */


    SetupDLF( &dlf,
              DLG_OPENDLG | DLG_HELP,
              &hf,
              "\\*.DOC",
              "Dialog Error",            /* Msgbox title if Error            */
              "Open",
              "No help available");


    dlf.szFileName[0] = dlf.szOpenFile[0] = '\0' ;



    switch( DlgFile( hwnd, &dlf ))
    {

	case TDF_ERRMEM:
	case TDF_INVALID:

	   fSuccess = FALSE;
	   break;

	case TDF_NOOPEN:

	   fSuccess = FALSE;
	   break;

	default:

	   break;
    }



    if ( fSuccess ) {


    /* Find out the size of the file */

        if ( DosQFileInfo( hf,
                           FIL_STANDARD,
		           (PBYTE) &stsInfo,
		           (USHORT) sizeof( FILESTATUS )))
        {

	    TEDDisplayErrorID( SID_DOSQFILEINFO_FAILED );

        }


    /* If file is too large then truncate it */

        if ( stsInfo.cbFile > (LONG) TXT_LIMIT )
        {

            TEDDisplayErrorID( SID_FILE_TOO_LARGE );

            stsInfo.cbFile = (LONG) TXT_LIMIT;

        }


    /* Allocate a buffer to do the IMPORT */

        if (  MyAllocMem( (PVOID *) &pvBuff,
                          (LONG) stsInfo.cbFile,
                          0    ))
        {

            TEDDisplayErrorID( SID_CANT_ALLOCATE_MEMORY );

        }
        else
        {


        /*****************************************************************
           stsInfo.cbFile is always truncated to TXT_LIMIT so it will
           always fit in a short.
        *****************************************************************/

	    if ( !DosRead( hf,
	      		   (PVOID) pvBuff ,
			   (USHORT) stsInfo.cbFile,
			   &cbBytesRead )) {


	        MLEDisable( hwndMLE );



	    /* Delete whatever is in MLE right now */


	        MLEDeleteContents( hwndMLE );


	    /* Reset the undo since a new file was imported into the MLE */

	        WinSendMsg( hwndMLE, MLM_RESETUNDO, NULL, NULL );



	    /* Read the data succesfully, now import to MLE */

	        WinSendMsg( hwndMLE,
                            MLM_SETIMPORTEXPORT,
			    (MPARAM) pvBuff,
			    (MPARAM) cbBytesRead );

	        iptData = 0L;

   	        WinSendMsg( hwndMLE,
                            MLM_IMPORT,
                            (MPARAM) &iptData,
		            (MPARAM) cbBytesRead );

            /****************************************************************
                Here we should check to see if the number of bytes imported
                matches the number of bytes we told the MLE to import.
            *****************************************************************


	    /* Release memory */

	        MyFreeMem( pvBuff );


            /* Copy the file name info into the correct place */

                strcpy( pszFname, dlf.szFileName );
                strcpy( pszFullName, dlf.szOpenFile );


                MLEEnable( hwndMLE );


	    }
            else
            {
	        TEDDisplayErrorID( SID_DOS_READ_FAILED );

	    }

        }

        DosClose( hf );

    }

}



/****************************************************************************

    MLEInit - This procedure initializes certain MLE parameters to a known
              state.

****************************************************************************/


VOID MLEInit( HWND hwnd )
{

    /*************************************************************************
        Set the text limit of the MLE to TXT_LIMIT. Any attempt to exceed
        this limit will result in a WM_CONTROL message (notification)
        to the MLE.
    *************************************************************************/

        WinSendMsg( hwnd,
                    MLM_SETTEXTLIMIT,
                    (MPARAM) ( (LONG) TXT_LIMIT),
                    NULL );


    /*************************************************************************
        Set the MLE format to MLFIE_CFTEXT which is the Cliboard text format.
        This format uses CR/LF for line delineation on export and recognizes
        either LF, CR/LF, or LF/CR as a line delineation on import.
    *************************************************************************/

        WinSendMsg( hwnd,
                    MLM_FORMAT,
                    (MPARAM) (USHORT) MLFIE_CFTEXT,
                    NULL );


}


/*****************************************************************************

    MLESetWrap - This procedure determines the current set of the WRAP state
                 in the MLE and optionally (fToggle TRUE ) toggles it. In
                 either case the Menu item Word wrap is checked or unchecked
                 in order to display the correct wrap state of the MLE.

*****************************************************************************/



VOID MLESetWrap( HWND hwnd, HWND hwndMenuWrap, BOOL fToggle )
{
    BOOL fWrap;         /* The wrap state of the MLE */


    /* First query the wrap from the MLE */

    fWrap = (BOOL)SHORT1FROMMR( WinSendMsg( hwnd, MLM_QUERYWRAP, NULL, NULL ));


    if ( fToggle )
    {

        fWrap = !fWrap;

        if ( !WinSendMsg( hwnd, MLM_SETWRAP, (MPARAM) fWrap, NULL ) )
        {
	    TEDDisplayErrorID( SID_CANT_SET_WRAP );

	    return;
        }

    }


    /* Show the checked state on the menu item */

    WinSendMsg(hwndMenuWrap,
               MM_SETITEMATTR,
	       MPFROM2SHORT(IDM_SETWRAP, TRUE),
	       MPFROM2SHORT(MIA_CHECKED,
	       fWrap ? MIA_CHECKED : FALSE) ) ;


}




/*****************************************************************************

    MLEEnable - This procedure Enables the MLE so it will take input from the
                user and update itself to correctly track what is in the
                MLE buffer.

*****************************************************************************/

VOID MLEEnable( HWND hwnd )
{
    WinSendMsg( hwnd, MLM_ENABLEREFRESH, NULL, NULL );
}






/*****************************************************************************

    MLEDisable - This procedure disables the MLE so it will not take input
                 from the user, change its pointer to the system wait icon
                 and not update itself.

*****************************************************************************/

VOID MLEDisable( HWND hwnd )
{
    WinSendMsg( hwnd, MLM_DISABLEREFRESH, NULL, NULL );
}




/*****************************************************************************

    MLEDeleteContents - This procedure deletes all the text in the MLE.

      Returns - TRUE if succesfull.

*****************************************************************************/


BOOL MLEDeleteContents( HWND hwnd )
{

    IPT iptTextLength;      /* Length of the text currently in the MLE */



    iptTextLength = (IPT) WinSendMsg( hwnd, MLM_QUERYTEXTLENGTH, NULL, NULL );


/* Now delete all the text in the MLE */

    return (  (IPT) WinSendMsg( hwnd,
                                MLM_DELETE,       /* Delete MSG */
                                (MPARAM) 0,       /* Start IPT location */
                                (MPARAM) iptTextLength ) == iptTextLength );



}

/*****************************************************************************

    TEDDisplayErrorID - This procedure loads the text associated with the
                        passed in resource identifier and displays it to
                        the user in the form of a message box.

              Returns - TRUE if the user clicked on okay

*****************************************************************************/


BOOL TEDDisplayErrorID( USHORT usid )
{

    UCHAR  uchTempBuff[ MAX_STRING_SIZE ]; /* Temp used to load string into*/
    USHORT usRetVal;                       /* Return val */


    if ( WinLoadString( hab, (HMODULE) NULL, usid, sizeof( uchTempBuff ), uchTempBuff ))
    {

       usRetVal = MESSAGE( uchTempBuff );

    }
    else
    {

       usRetVal = MESSAGE("Cannot load resource string");

    }

    return( usRetVal == MBID_OK );
}


/*****************************************************************************

    TEDInitDlgSysMenu - This procedure takes the system menu from the passed
                        window handle and deletes the :

                                MINIMIZE
                                MAXIMIZE
                                SIZE
                                RESTORE
                                TASKMANAGER

                        entries from the system menu.

*****************************************************************************/


VOID TEDInitDlgSysMenu( HWND hDlg )
{
    HWND hSysMenu;      /* Window handle of the system menu attached to the */
                        /* passed dialog handle                             */




    hSysMenu=WinWindowFromID(hDlg, FID_SYSMENU);

    WinSendMsg(hSysMenu,
               MM_DELETEITEM,
               MPFROM2SHORT(SC_MINIMIZE,TRUE),
               MPFROMSHORT(NULL));

    WinSendMsg(hSysMenu,
               MM_DELETEITEM,
               MPFROM2SHORT(SC_MAXIMIZE,TRUE),
               MPFROMSHORT(NULL));

    WinSendMsg(hSysMenu,
               MM_DELETEITEM,
               MPFROM2SHORT(SC_SIZE,TRUE),
               MPFROMSHORT(NULL));

    WinSendMsg(hSysMenu,
               MM_DELETEITEM,
               MPFROM2SHORT(SC_RESTORE,TRUE),
               MPFROMSHORT(NULL));

    WinSendMsg(hSysMenu,
               MM_DELETEITEM,
               MPFROM2SHORT(SC_TASKMANAGER,TRUE),
               MPFROMSHORT(NULL));


}


/***************************************************************************

    TEDCleanupExit - This procedure cleans up by determining if valid
                     handles existed and then destroying them.

                     NOTE!! This procedure calls DosExit and thus never
                            returns.

***************************************************************************/


VOID TEDCleanupExit(VOID)
{


/* If the find dialog was ever used then destroy it */

    if ( hwndDlgFind != (HWND) NULL )
    {
        WinDestroyWindow( hwndDlgFind );
    }




/*  Standard Presentation Manager Application exit sequence. */


    if ( hwndFrame != (HWND) NULL )
    {
        WinDestroyWindow (hwndFrame ) ;
    }


    if ( hmq != (HMQ) NULL )
    {
        WinDestroyMsgQueue (hmq) ;
    }


    if ( hab != (HAB) NULL )
    {
        WinTerminate (hab) ;
    }


    DosExit( EXIT_PROCESS, 0 );


}


/*****************************************************************************

    TEDUpdateTitleBar - This function updates the title (FID_TITLEBAR) of the
                        passed in hwnd with the two passed in string.
                        The strings are format as  <STRING> - <STRING>.
                        This is used to display the program name and file
                        currently being edited.

*****************************************************************************/



VOID TEDUpdateTitleBar( HWND hwnd, PSZ pszPgmName, PSZ pszFileName )
{

    UCHAR auchTemp [ 2 * CCHMAXPATH ];
    HWND hwndTitle;


    hwndTitle = WinWindowFromID( hwnd , FID_TITLEBAR);


    sprintf( auchTemp, "%s - %s", pszPgmName, pszFileName );

    WinSetWindowText(hwndTitle, auchTemp );


}
