/*
 *  Graphics Exchange example OS/2 PM DDE Server program - Source Code
 *
 *  The Graphics Exchange server application creates a picture using
 *  GPI graphics orders and transmits changes to this picture through
 *  a permanent DDE data link.  The server application simulates a
 *  phone messaging system, which displays phone messages in a listbox
 *  and updates its picture as the number of messages increases.  The
 *  actual receiving of messages is not demonstrated in this program.
 *  Instead, the timer is used to simulate the arrival of phone messages
 *  at a constant interval.
 */

#include "os2.h"
#include "st.h"
#include "server.h"

/*
 *  Graphics Exchange server main program & message processing loop
 *
 *  This main routine controls the Presentation Manager
 *  initialization and termination as well as implementing the main
 *  polling loop for the server application.
 */

HAB  hab;

int cdecl main()
{
    QMSG   qmsg;         /* input message structure definition       */
    HMQ    hmq;          /* message queue handle                     */
    HWND   hFrame;       /* frame window handle                      */

    hab = WinInitialize(NULL);
    hmq = WinCreateMsgQueue(hab,0);

    if((hFrame = PhoneInit()) == 0)             /* srvrini.c */
       return(FALSE);

    /* Poll messages from event queue */
    while(WinGetMsg(hab, (PQMSG)&qmsg, (HWND)NULL, 0, 0)) {
          WinDispatchMsg(hab, (PQMSG)&qmsg);
    }

    WinDestroyWindow(hFrame);
    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
}
