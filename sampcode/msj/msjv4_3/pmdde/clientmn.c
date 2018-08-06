/*
 *  Graphics Exchange example OS/2 PM DDE Client program - Source Code
 *
 *  The Graphics Exchange client application serves as an application
 *  which displays graphics representing the running server
 *  applications.  Server applications may differ greatly in the
 *  function provided, but they all use DDE to transfer their graphics
 *  data to the client.  In our model, the client always establishes
 *  a permanent data link with the server to receive updates as the
 *  state of the server application warrants a change in the graphics'
 *  appearance.  Both multiple clients and servers may be executed
 *  simultaneously.
 *
 *  Note that the DDE techniques demonstrated in this program are a
 *  simplification of the general specification and is provided for
 *  tutorial purposes.  In ALL cases where you may observe
 *  differences between these applications and the DDE
 *  specification, please follow the published protocol.
 *
 *  Special thanks to Viktor Grabner, Tony Williams, Ed Fries, and
 *  Tandy Trower for their help and support with OS/2 PM DDE.
 */


#include "os2.h"
#include "st.h"
#include "client.h"

/*
 *  Graphics Exchange client main program & message processing loop
 *
 *  This main routine controls the Presentation Manager
 *  initialization and termination as well as implementing the main
 *  polling loop for the client application.
 */

HAB  hab1;         /* Presentation Manager anchor block handle */

int cdecl main()
{
    QMSG qmsg;     /* input message structure definition       */
    HMQ hmq;       /* message queue handle                     */
    HWND hFrame;   /* frame window handle                      */

    hab1 = WinInitialize(NULL);

    hmq = WinCreateMsgQueue(hab1,0);

    if ((hFrame = ClientInit()) == 0)
        return(FALSE);

    /* Poll messages from event queue */
    while(WinGetMsg(hab1,(PQMSG)&qmsg,(HWND)NULL,0,0)){
        WinDispatchMsg(hab1,(PQMSG)&qmsg);
    }

    WinDestroyWindow(hFrame);
    WinDestroyMsgQueue(hmq);
    WinTerminate(hab1);
}
