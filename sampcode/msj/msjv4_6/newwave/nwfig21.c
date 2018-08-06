/********************************************************************/
/*                          ShapeWndProc                            */
/*                                                                  */
/*       Main procedure to handle all messages sent to HPSHAPE.     */
/*                                                                  */
/*          COPYRIGHT HEWLETT-PACKARD COMPANY 1987, 1988            */
/********************************************************************/

long FAR PASCAL ShapeWndProc( hWnd, message, wParam, lParam )
    HWND            hWnd;
    unsigned        message;         
    WORD            wParam;
    LONG            lParam;
{
    APIRTNTYPE      applRtn;  /* used as a return value */
    APICMDSTRUCT    extCmd;   /* the external command struc for apl */
    INTCMDSTRUCT    intCmd;   /* the internal command struc for apl */


    /* function called for windows that are created with            */
    /* NW_CreateWindow to specially handle some messages            */

    if( NW_MessageFilter(hWnd, message, wParam, lParam,
                                               (LONG FAR *) &applRtn))
        {
        return (applRtn);
        }

    /* is API to intercept messages or has an API menu item been    */
    /* seleceted such as the help or the task                       */

    if( APIInterceptOn(gAPIModeFlags) || APIHaveMenu (message, wParam))
        {
        /* is this a message for the API - may set certain flags    */
        APIUserActionInterface(ghAPI, hWnd, (LPAPIUNSIGNED)&message,
                                 wParam, lParam, API_NO_MODE);
        }

    applRtn = (APIERRTYPE)0L;        
    /* Still a command to be handled? or did APIUserAct.. take it?  */
    if( APIHaveMessage (message) )
        {
        intCmd.wCmd = API_NO_CMD;           
         
        /* are we currently playing back a message or recorded task */
        if (APIPlaybackMsg(message))                                 
            /* translate to the internal format of the application  */
            TranslateToInternalProcessor(message, wParam, lParam,
                                                            &intCmd);
        else                                                          
            /* you are in record mode or a user interactive command */
            ActionProcessor(hWnd, message, wParam, lParam, &intCmd,     
                            &applRtn);
                                                                      
        /* Is there a command created in the action processor?      */
        /* that need to be executed */
        if( APIHaveCommand(intCmd.wCmd) )
            {
            gapplErr = API_NO_ERR;      
                       
            /* are we in  a CBT */
            if( APIMonitorOn(gAPIModeFlags) )
               {   
                /* set to external language and pass the external   */
                /* form of the command to the Agent                 */

                TranslateToExternalProcessor(&intCmd,&extCmd);
                APICommandInterface(ghAPI, (LPAPICMDSTRUCT)&extCmd,
                                    API_NO_MODE);
                /* the internal command must be cancelled if the    */
                /*  APICommandInterface or APIClgCommandInterface   */
                /*  has nullified the command */
                if (extCmd.wCmd == API_NO_CMD)
                   intCmd.wCmd = API_NO_CMD;
               }

            /* has  the command been formed  and has to be executed */
            if( APIHaveCommand(intCmd.wCmd) )
                /* perform the command */
                CommandProcessor(hWnd, message, wParam, lParam, &intCmd,
                                 &applRtn);
                         
            /* is a command being played back or being recorded     */
            if( APIPlaybackOn(gAPIModeFlags) ||
                                    APIRecordOn (gAPIModeFlags))
                {
                if( APIRecordOn(gAPIModeFlags) )
                    {
                    /* translate to ext lang and tell Agent that    */
                    /* command is complete and ready for next       */
                    /* command                                      */
                    TranslateToExternalProcessor(&intCmd,&extCmd);
                    APIRecordInterface(ghAPI,(LPAPICMDSTRUCT)&extCmd,
                                       API_NO_MODE);            
                    }                                           
                /* return control back to the user */
                APIReturnInterface( ghAPI, gapplErr, API_NO_MODE );
                }

            } /* EndIf of APIHaveCommand */

        }  /* EndIf of APIHaveMessage */

    return(applRtn);    

} /* End of ShapeWndProc */
