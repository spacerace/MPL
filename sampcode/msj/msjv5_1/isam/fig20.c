

/* Table of currently connected clients. The clientTbl is
typically accessed through the three routines getClient,
putClient, delClient and reapClient. */

static HWND clientTbl[MAX_CLIENTS];

static int putClient(HWND);
static int getClient(HWND);
static int delClient(HWND);
static int reapClient(HWND);

/* Server communications window WndProc. All client requests
come through here. */

long FAR PASCAL WndProc (
    HWND       hWnd,
    unsigned   iMessage,
    WORD       wParam,
    LONG       lParam )
{
    static char    *szInit = "WM_INITIATE_ISAM";
    static char    *szSend = "WM_SEND_ISAM";
    static char    *szTerm = "WM_TERMINATE_ISAM";
    static WORD    wmInitISAM;
    static WORD    wmSendISAM;
    static WORD    wmTermISAM;

    GLOBALHANDLE   hRequestBlock;
    HWND           hWndClient;
    int            iRetVal;

    switch (iMessage) {
    case WM_CREATE:
        /* Register WinTrieve protocol messages. */
        wmInitISAM = RegisterWindowMessage(szInit);
        wmSendISAM = RegisterWindowMessage(szSend);
        wmTermISAM = RegisterWindowMessage(szTerm);

        break;
    case WM_CLOSE:
        /* Detect if any clients are still connected. */
        if (reapClients()) {
            /* There are still connected clients. What
             * is done here is server dependent. WinTrieve
             * displays a message box and returns (does
             * not terminate).
             */
        }
        else
            /* No connected clients, ok to terminate. */
            return DefWindowProc(hWnd, iMessage, wParam,
lParam);

        break;
    case WM_DESTROY:
        /* Can do some final cleanup here before the server
terminates. */

        break;
    case WM_QUERYOPEN:
        /* The server is always an icon. */

        break;
    default:
        if (iMessage == wmSendISAM) {
            /* Get handle to query block. */
            hRequestBlock = HIWORD(lParam);

            /* Check that client is connected. */
            hWndClient = wParam;
            if (getClient(hWndClient) < 0)
                return ISNOTCONN;

            /* Process the request, this is server
dependent.
             * If protocol error, for example, cannot lock
             * query block returns an error code, otherwise
             * returns ISOK.
             */
            iRetVal = ProcessRequest(hWndClient,
hRequestBlock);
            return iRetVal;
        }
        else if (iMessage == wmInitISAM) {
            /* A server must be robust. Client handles
             * that have become invalid are checked for
here.
             */
            reapClients();

            /* Initiate a connection. */
            hWndClient = wParam;

            /* Check that client isn't already connected. */
            if (getClient(hWndClient) >= 0)
                return ISCONN;

            /* Check for maximum connections. */
            if (putClient(hWndClient) < 0)
                return ISMAXCONN;

            /* Client connected. */
            return ISOK;
        }
        else if (iMessage == wmTermISAM) {
            /* Terminate a connection. */
            hWndClient = wParam;

            /* Check that client is connected. */
            if (delClient(hWndClient) < 0)
                return ISNOTCONN;

            return ISOK;
        }
        else
            /* Default message processing. */
            return DefWindowProc(hWnd, iMessage, wParam,
lParam);

        break;
    }

    return 0L;
}

/* Put new client into client table. If successful returns
index of table where the handle is stored. If the table is
full returns -1. */

static int putClient(
   HWND  hWnd )
{
        register int   i;

   for (i = 0; i < MAXHANDLES; i++) {
      if (clientTbl[i] == NULL) {
         clientTbl[i] = hWnd;
         return i;
      }
   }
   return -1;                 /* client table full */
}

/* Delete client from client table. If successful returns
index position of table where the handle was deleted from.
If handle not found returns -1. */
static int delClient(hWnd)
   HWND  hWnd;
{
   register int   i;

   for (i = 0; i < MAXHANDLES; i++) {
      if (clientTbl[i] == hWnd) {
         clientTbl[i] = NULL;
         return i;
      }
   }
   return -1;                 /* handle not found */
}

/* Returns index position in client window handles table for
the specified client handle. If client handle not found
returns -1. */
static int getClient(hWnd)
   HWND  hWnd;
{
   register int   i;

   for (i = 0; i < MAXHANDLES; i++) {
      if (clientTbl[i] == hWnd)
         return i;
   }
   return -1;
}

/* Check the client table for any invalid client handles.
For example clients that died suddenly or clients that just
plan forgot to terminate the connection properly. Returns 0
if no more clients connected. */

static int reapClients()
{
    int    i;
    int    n;
    HWND   hWndClient;

    /* Run through client table looking for connected
clients. */
    for (i = n = 0; i < MAX_CLIENTS; i++) {
        if (clientTbl[i] != NULL) {
            hWndClient = clientTbl[i];
            /* Check if still valid handle by calling
             * Windows function IsWindow.
             */
            if (IsWindow(hWndClient) == NULL) {
                /* Client died. The function cleanUp
                 * is a server specific routine that
                 * removes the state of a client
                 * if it is no longer valid.
                 */
                cleanUp(hWndClient);
                clientTbl[i] = NULL;
                continue;
            }
            n++;              /* increment connected clients
count */
        }
    }

    return n;
}
