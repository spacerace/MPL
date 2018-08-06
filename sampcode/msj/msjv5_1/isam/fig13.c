

/* WinTrieve C API, error codes are returned in global
variable, iserrno. If an API function returns -1 it
indicates an error. */

int     iserrno;

/* These values need to be available to other WinTrieve C
API functions. For the example code given assume these
variables are globally defined. This does not necessarily
reflect the actual implementation. */

HWND    hWndServer;           /* server window handle */
HWND    hWndClient;           /* client window handle */
WORD    wmInitISAM;           /* WM_INITIATE_ISAM message
value */
WORD    wmSendISAM;           /* WM_SEND_ISAM message value
*/
WORD    wmTermISAM;           /* WM_TERMINATE_ISAM message
value */

/* Make connection to server. Argument hWnd is handle to a
client window. Should ensure that hWnd remains valid
throughout the life of the connection. */

int isconnect(
    HWND    hWnd )
{
    static char    *szServerName = "ISAM SERVER";
    static char    *szInit = "WM_INITIATE_ISAM";
    static char    *szSend = "WM_SEND_ISAM";
    static char    *szTerm = "WM_TERMINATE_ISAM";
    static int     iFirstTime = 1;
    long           lRetVal;

    /* Obtain WinTrieve protocol message values. */
    if (iFirstTime) {
        /* Only do this once. */
        wmInitISAM = RegisterWindowMessage((LPSTR)szInit);
        wmSendISAM = RegisterWindowMessage((LPSTR)szSend);
        wmTermISAM = RegisterWindowMessage((LPSTR)szTerm);
        iFirstTime = 0;
    }

    /* Get the handle of the server's communications window.
*/
    hWndServer = FindWindow((LPSTR)szServerName,
(LPSTR)szServerName);
    if (hWndServer == 0L) {
        /* Error, server not running. */
        iserrno = ISNOSERVER;
        return -1;            /* Error, server not running.
*/
    }

    lRetVal = SendMessage(hWndServer, WM_INITIATE_ISAM,
hWnd, 0L);
    if (lRetVal != ISOK) {
        /* Error, two possible conditions, server must
         * have died or maximum connections reached.
         */
        if (lRetVal == 0L)    /* Assign error code to
            iserrno = ISNOSERVER;  / * global variable. */
        else
            iserrno = retVal;
        return -1;            /* Indicates error. */
    }
    /* Connection successful. */
    hWndClient = hWnd;
    return 0;
}
