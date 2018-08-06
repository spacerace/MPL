
/* Function to terminate connection with server.
 */
int isdisconnect()
{
    int   iRetVal;

    iRetVal = SendMessage(hWndServer, wmTermISAM,
hWndClient, 0L);
    if (iRetVal != ISOK) {
        /* Error terminating connection. */
        if (iRetVal == 0L)
            iserrno = ISNOSERVER;
        else
            iserrno = iRetVal;
        return -1;
    }
    /* Successfully terminated connection with server. */
    hWndServer = NULL;

    return 0;
}
