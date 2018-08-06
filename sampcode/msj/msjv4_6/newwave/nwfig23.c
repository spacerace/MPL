/*********************************************************************/
/*                         CommandProcessor                          */
/*                                                                   */
/* Handle any of the commands passed from the Action Process or API  */
/*                                                                   */
/*          COPYRIGHT HEWLETT-PACKARD COMPANY 1987, 1988             */
/*********************************************************************/

void PASCAL   CommandProcessor (hWnd, message, wParam, lParam, intCmd,
                                pRtn)
    HWND          hWnd;
    unsigned      message;
    WORD          wParam;
    LONG          lParam;
    PINTCMDSTRUCT intCmd;
    LONG          *pRtn;          /* application return error code */
{
    HMENU         hMenu;          
    RECT          rcRect;

    switch( intCmd->wCmd )
        {
        case API_MINIMIZE_WINDOW_CDCMD:
            if (IsIconic(hWnd))
                NW_Restore(hWnd);
            else
                {
                if (!IsZoomed(hWnd))
                     GetWindowRect(hWnd, (LPRECT)&gWinPosn.rcRect);
                NW_Minimize(hWnd);
                }
            break;

        case API_MAXIMIZE_WINDOW_CDCMD:
            if (IsZoomed(hWnd))
                NW_Restore(hWnd);
            else
                {
                if (!IsIconic(hWnd))
                    GetWindowRect(hWnd, (LPRECT)&gWinPosn.rcRect);
                NW_Maximize(hWnd);
                }
            break;

        case API_RESTORE_WINDOW_CDCMD:
            NW_Restore(hWnd);
            break;                                      

        case API_CLOSE_WINDOW_CDCMD:

            SaveWindowPosition(hWnd);
            GetWindowRect(hWnd, (LPRECT) &rcRect);      
            ShowWindow(hWnd, SW_HIDE);
            UpdateWindow(hWnd);
            APINotReady(ghAPI, API_NO_MODE);
            if (!OMF_Closing(ghOMF, (LPRECT) &rcRect))
               NoteError();
            break;

        case NEW_SHAPE:
            /* get the handle to the menu of the current window */
            hMenu = GetMenu(hWnd);                                

            /* Uncheck the old menu item */
            CheckMenuItem(hMenu, gnShape, MF_UNCHECKED);

            /* check the new menu item */
            if ((gnShape = intCmd->internal.ICmd) != SHAPE_NONE)
                CheckMenuItem(hMenu, gnShape, MF_CHECKED);

            /* send a paint message */
            InvalidateRect(hWnd, (LPRECT)NULL, TRUE);
            UpdateWindow(hWnd);  /* Force repaint for every shape,
                                    not just last shape, when playing
                                    back several NEW_SHAPE commands */
                      
            /* are there views of the shape program in any other    */
            /* object?  set the new_data flag for all views of      */
            /* shapes */

            if (OMF_SetNewData(ghOMF, 0))
                 {
                 /* notify the destinations */                 
                 if (!OMF_AnnounceNewData(ghOMF))
                      NoteError();
                 }
    
            break;

        default:
            NoteError();
            break;
        }

} /* end of CommandProcessor */
