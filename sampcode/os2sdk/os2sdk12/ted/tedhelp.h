/**********************************************************************

    Tedhelp.h

    Created by Microsoft Corporation, IBM Corporation 1989

**********************************************************************/




/**********************************************************************/
/*  Function prototypes                                               */
/**********************************************************************/

MRESULT APIENTRY TEDHelpError( HWND, USHORT, MPARAM, MPARAM );

USHORT  APIENTRY TEDInitHelp( HWND, HAB );
VOID    APIENTRY TEDTerminateHelp( HWND );
VOID    APIENTRY TEDSendHelpMsg( USHORT );
VOID    APIENTRY TEDAssociateHelp( HWND );
BOOL    APIENTRY TEDProcessHelpMsg( HWND, USHORT, MPARAM, MPARAM, MRESULT );



/**********************************************************************/
/*  Help file name                                                    */
/**********************************************************************/
#define HELP_LIBRARY	 "TEDHP.HLP"	   /* Help panel library       */

/**********************************************************************/
/*  TEDInitHelp return values                                        */
/**********************************************************************/

#define HELP_OK          0    /* Help initialized OK        */
#define HELP_FAILED      1    /* Help initialization failed */
#define NO_HELP 	 2    /* TED.HLP not found	     */
