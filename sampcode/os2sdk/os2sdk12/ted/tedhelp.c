/**************************************************************************

    Tedhelp.c

    Created by Microsoft Corporation, IBM Corporation 1989

---------------------------------------------------------------------------


        This file contains all the helper procedures required to
        include Help Manager in an application. The normal
        procedure is to:


            Define the .itl file which has all the help panel
            text in it.

            Define the Help tables and sub tables.  This maps
            help panel id numbers to menu id numbers so help
            manager knows which contectual help to put up
            when a user requestes it.

            Create a help instance.

            Associate a help instance with the current window
            chain.  As long as each new window created is
            part of the window chain (parent or owner) then
            there is no need to associate a help instance with
            each new window.




    Modification history

            091189  -  James Bratsanos, created


***************************************************************************/


/* Define Constants for additional includes  */


#define INCL_BASE
#define INCL_WIN



#include <os2.h>
#include <string.h>
#include "ted.h"
#include "tedhelp.h"
#include "tedhp.h"
#include "teddlg.h"




HWND hwndHelpInstance = (HWND) NULL;        /* The handle to the the Help */
                                            /* Manager help instance      */

/****************************************************************************

    TEDTerminateHelp - This procedure disassociates the help instance
                       attached to the passed in window handle, and then
                       destroys it.

****************************************************************************/


VOID APIENTRY TEDTerminateHelp( HWND hwnd )
{


    if ( hwndHelpInstance != (HWND) NULL )
    {

    /* Dissasociate the Help instance */

        WinAssociateHelpInstance( NULL, hwnd );

    /* Destroy the HELP instance */

        WinDestroyHelpInstance( hwndHelpInstance );
    }

}


/****************************************************************************

    TEDInitHelp - This procedure opens the defined help library for this
                  application then creates a help instance and associates
                  it with the passed in window handle (which is the frame)

****************************************************************************/


USHORT APIENTRY TEDInitHelp( HWND hFrame, HAB hab )
{
    HELPINIT stHMInit;                  /* Help Manager init structure       */
    BOOL     bHelpLibFound=TRUE;        /* Help library found flag           */

/***************************************************************************
    If the help tables were arrays instead of resources they would be
    set up as static arrays at this point in the procedure.
***************************************************************************/


/***************************************************************************
    The help manager will search for the help library names first in
    the HELP environment variable and then in the current directory.

    NOTE: To specify multiple library names seperate the names with
          a blank.  Note the library names must include the extension
***************************************************************************/



/************************************************************************

    Initialize Help Manager init  structure

************************************************************************/

    stHMInit.cb                        = sizeof(HELPINIT);
    stHMInit.ulReturnCode              = 0L;
    stHMInit.pszTutorialName           = NULL;
    stHMInit.phtHelpTable              = (PHELPTABLE) (0xffff0000 |
                                                      (LONG) TED_HELP_TABLE );
    stHMInit.hmodHelpTableModule       = (HMODULE) NULL;
    stHMInit.hmodAccelActionBarModule  = (HMODULE) NULL;
    stHMInit.idAccelTable              = 0;
    stHMInit.idActionBar               = 0;


    stHMInit.pszHelpWindowTitle        = "Ted Editor Help";
    stHMInit.usShowPanelId             = CMIC_HIDE_PANEL_ID;
    stHMInit.pszHelpLibraryName        = HELP_LIBRARY;



/* Create Help Manager Object Window */


/************************************************************************
    NOTE to Windows Programmers: This hwndHelpInstance is simply
                                 a window handle and not an INSTANCE
                                 handle as defined in Windows.
************************************************************************/


    if ( !( hwndHelpInstance = WinCreateHelpInstance( hab, &stHMInit )) )
    {


        if ( stHMInit.ulReturnCode == HMERR_OPEN_LIB_FILE )
        {

           TEDDisplayErrorID( SID_HERR_NO_LIBRARY );

        }
        else
        {

           TEDDisplayErrorID( SID_HELP_CANT_CREATE_INST );

        }

        return( FALSE );                /* cleanup handled at exit */

    }


/*  Associate an instance of the Help Manager with the window chain. */



    if (!WinAssociateHelpInstance( hwndHelpInstance, hFrame ))
    {

        TEDDisplayErrorID( SID_HELP_CANT_ASSOC_INST );
        return( FALSE );                  /* cleanup handled at exit */

    }


/* Check for any other initialization error from Help Manager */


    if(stHMInit.ulReturnCode)
    {

        TEDDisplayErrorID( SID_HELP_HLP_ERROR );

        return(FALSE );	      /* exit if help initialization failed  */

    }
    else
    {
        return(HELP_OK);
    }


}                                                       /* return to main */



/***************************************************************************

    TEDSendHelpMsg - This procedure sends a message to the help manager

***************************************************************************/


VOID APIENTRY TEDSendHelpMsg( USHORT usMsg )
{

    WinSendMsg( hwndHelpInstance, usMsg, NULL, NULL );
}




/***************************************************************************

    TEDAssociateHelp - This procedure associates a help instance with the
                       passed in window handle.

                       NOTE: As long as a new window is part of the original
                             window chain , where each succesive window had
                             as its owner a handle that was associated with
                             the help instance the there is no need to associate
                             a help instance with this newly created window
                             since this new window will inherit the instance
                             because it is part of the chain.

***************************************************************************/


VOID APIENTRY TEDAssociateHelp( HWND hwnd)
{
    WinAssociateHelpInstance( hwndHelpInstance, hwnd );
}



/***************************************************************************

    TEDProcessHelpMsg - This procedure determines if the Help Manager
                        error is something that can be acted on and
                        acts accordingly.

              Return  - TRUE if it handled the message. False otherwise

***************************************************************************/

BOOL APIENTRY TEDProcessHelpMsg(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2,
                        MRESULT *mresult)
{

    switch( msg )
    {

    /**************************************************************************

        In this case user pressed keys help and HELP manager sent a message
        notifying us. Help Manager expects the help panel id # in the .ITL
        file be returned so it can display that help panel in the help window.

    **************************************************************************/

        case HM_QUERY_KEYS_HELP:

            *mresult=MRFROMSHORT(KEYS_HELP_HP);
            break;

        case HM_HELPSUBITEM_NOT_FOUND:
        case HM_EXT_HELP_UNDEFINED:
        case HM_ERROR:

            *mresult = TEDHelpError(hwnd,msg,mp1,mp2);
            break;

        default:

            return (FALSE);
            break;

    }

    return (TRUE);
}




MRESULT APIENTRY TEDHelpError( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
  USHORT  SubTopicID;
  USHORT  ErrorCode;


/*  Process the Help Manager message  */

    switch (msg)
    {
        case HM_HELPSUBITEM_NOT_FOUND:

            SubTopicID = HIUSHORT(mp2);


            switch (SubTopicID)
            {
                case FID_CLIENT:         /* Edit area has input focus */
             /*************************************************/
             /* Returning false to HM in this case causes the   */
             /* extended help panel GENERAL_HP to display.      */
             /* Doing it this way instead of putting FID_CLIENT */
             /* in SubTbl_MAIN_MENU causes the Extended help    */
             /* item on the help panel's pull down to be        */
             /* grayed out, which is the desired result.        */
             /*************************************************/
                    return((MRESULT)FALSE);
                    break;

                default:
                    break;
            }

            break;

        case HM_EXT_HELP_UNDEFINED:

            break;

        case HM_ERROR:

            ErrorCode = SHORT1FROMMP(mp1);



    /* Load the correct error message                 */



            switch (ErrorCode)
            {
                case HMERR_NO_MEMORY:
                case HMERR_ALLOCATE_SEGMENT:
                case HMERR_FREE_MEMORY:

                    TEDDisplayErrorID( SID_HERR_MEMORY );
                    break;

                case HMERR_DATABASE_NOT_OPEN:
                case HMERR_OPEN_LIB_FILE:
                case HMERR_READ_LIB_FILE:
                case HMERR_CLOSE_LIB_FILE:
                case HMERR_INVALID_LIB_FILE:

                    TEDDisplayErrorID( SID_HERR_LIB_ERROR );
                    break;

                case HMERR_HELPITEM_NOT_FOUND:
                case HMERR_HELPSUBITEM_NOT_FOUND:

                    return((MRESULT)TRUE);
                    break;

                default:

                    TEDDisplayErrorID( SID_HERR_UNKNOWN );
                    break;


            }   /* End ErrorCode switch */


            break;  /* end - case HM_ERROR: */

        default:

            break;

    }  /* end the big switch */


    return((MRESULT)TRUE);

}
