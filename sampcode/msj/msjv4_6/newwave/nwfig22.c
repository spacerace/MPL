/********************************************************************/
/*                         ActionProcessor                          */
/*                                                                  */
/* Handle all of the user actions - set up intCmd for Cmd Processor */
/*                                                                  */
/*          COPYRIGHT HEWLETT-PACKARD COMPANY 1987, 1988            */
/********************************************************************/

void PASCAL  ActionProcessor (hWnd, message, wParam, lParam, intCmd,
                              pRtn)
    HWND          hWnd;
    unsigned      message;
    WORD          wParam;
    LONG          lParam;
    PINTCMDSTRUCT intCmd;
    LONG          *pRtn;    /* the return value of the procedure */
{
    PAINTSTRUCT   ps ;         /* the paint structure */
                                
    /* this routine actually builds a command */
    switch( message )
        {
        /* was a paint message sent to update the screen */
        case WM_PAINT:
            BeginPaint   (hWnd, (LPPAINTSTRUCT)&ps);
            ShapePaint(hWnd, (LPPAINTSTRUCT)&ps);
            EndPaint     (hWnd, (LPPAINTSTRUCT)&ps);
            break;
                                                           
        /* was there a menu selection */
        case WM_COMMAND:     
            /* was the close option chosen */
            if (wParam == IDDCLOSE)
                intCmd->wCmd = API_CLOSE_WINDOW_CDCMD;
            else {                      
                 /* something was selected from the shape menu */
                 intCmd->wCmd = NEW_SHAPE;
                 intCmd->internal.ICmd = wParam;
                 } 
            break;                           

        /* the window being closed */
        case WM_CLOSE:
            intCmd->wCmd = API_CLOSE_WINDOW_CDCMD;
            break;              
                                     
        /* selection from system menu or minimize/maximize */
        case WM_SYSCOMMAND:
            switch (wParam)
                {
                case SC_MINIMIZE:
                    intCmd->wCmd = API_MINIMIZE_WINDOW_CDCMD;
                    break;

                case SC_MAXIMIZE:
                    intCmd->wCmd = API_MAXIMIZE_WINDOW_CDCMD;
                    break;
                                                             
                /* request for saving previous coordinates */
                case SC_RESTORE:
                    intCmd->wCmd = API_RESTORE_WINDOW_CDCMD;
                    break;

                default:                                     
                    /* let windows handle it */
                    *pRtn = DefWindowProc(hWnd, message, wParam,
                                                            lParam);
                    break;

                }  /* End of case WM_SYSCOMMAND */
             break;

        case API_INTERROGATE_MSG:

            /* The message has come via the API */
            *pRtn = InterrogateFromAPI(wParam, lParam);
            break;

        case  API_SET_MODE_FLAGS_MSG:
             if (wParam == API_SET_MODE_ON_FLAG)
                 gAPIModeFlags = gAPIModeFlags | lParam;
             else
                 gAPIModeFlags = gAPIModeFlags & lParam;
             break;

        case WM_OMF:

            /* The message has come via the OMF */
            *pRtn = MessageFromOMF(hWnd, wParam, lParam);
            break;

        default:
            *pRtn = DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }
}  /* end of action processor */

