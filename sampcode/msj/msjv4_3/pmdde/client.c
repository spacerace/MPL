#include "os2.h"
#include "st.h"
#include "client.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

/*
 *  This module contains the winprocs for the Graphics Exchange client
 *  application window as well as the DDE anchor window and the DDE
 *  conversation windows.  The support routine, enumhwnd, which is
 *  used to locate DDE conversational windows is implemented here.
 */

HWND DDEanchorHWND;    /* handle of DDE anchor window     */
static USHORT winDDEid = ID_DDEANCHOR + 1;

/*
 * function enumhwnd(anchor, id)
 *
 * Given a handle of a DDE anchor window and a DD conversation ID,
 * locate the DDE window for that conversation.
 */

HWND enumhwnd(anchor, id)
HWND    anchor;          /* handle of the DDE anchor window to be searched */
USHORT  id;              /* DDE conversation to be located                 */
{
HENUM   henum;           /* enumeration handle                             */
HWND    hwndenum;        /* current enumeration window handle              */
HWND    rethwnd;         /* located window handle to be returned           */

       rethwnd = (HWND)NULL;
       henum = WinBeginEnumWindows(anchor);
       while (hwndenum = WinGetNextWindow(henum)) {
               if(id == LOUSHORT(hwndenum)){
                   rethwnd = hwndenum;
                   WinLockWindow(hwndenum,FALSE);
                   break;
               }
           WinLockWindow(hwndenum,FALSE);
       }
       WinEndEnumWindows(henum);
       return(rethwnd);
}

/*
 *  Function ClientWndProc
 *
 *  This function processes the system and application menu messages
 *  as well as messages from the graphics control child window.  A
 *  conversation_shutting_down (APPM_CONV_CLOSE) message is processed
 *  from the conversational DDE winproc by deleting that
 *  conversation's picture from the graphics control.  The WM_CLOSE message
 *  is processed by terminating ALL conversations related to this
 *  instance of this application.
 */

MRESULT APIENTRY ClientWndProc(hwnd,message,lParam1,lParam2)
HWND    hwnd;
USHORT  message;
MPARAM  lParam1;
MPARAM  lParam2;
{
RECTL     rect;           /* window rectangle work space for positioning      */
HPS       hPSW;           /* presentation space for this routine              */
HENUM     henum;          /* enumeration handle                               */
HWND      hwndenum;       /* current enumeration window handle                */
HWND      tohwnd;         /* handle of message recipient                      */
PDDESTRUCT DDEptr;        /* pointers for DDESTRUCT allocation                */

    switch (message){

        case WM_CREATE:  /* main window initialization - clear all window word memory */
             WinSetWindowULong(hwnd, WW_CLOSE, 0L);
             WinSetWindowULong(hwnd, WW_CONVCOUNT, 0L);
             WinSetWindowULong(hwnd, WW_MASTER_FLAGS, 0L);
             DDEanchorHWND = WinCreateWindow(hwnd, (PSZ)"DDE_Win_P", (PSZ)NULL,
                                    NULL, 0, 0, 0, 0, hwnd, HWND_TOP,
                                    ID_DDEANCHOR, (PVOID)NULL, (PVOID)NULL);
             break;

        case WM_PAINT:  /* repaint the window  */
             hPSW = WinBeginPaint(hwnd,NULL,(PRECTL)&rect);
             WinQueryWindowRect( hwnd, &rect );
             WinFillRect( hPSW, &rect, CLR_WHITE);
             WinEndPaint( hPSW );
             break;

          case WM_SIZE: /* adjust the window size  */
             WinQueryWindowRect(hwnd, &rect);
                 if (WinWindowFromID(hwnd,ID_GRAPHICS1)) {
                    WinSetWindowPos(WinWindowFromID(hwnd,ID_GRAPHICS1),HWND_TOP,0,0,LOUSHORT(rect.xRight-rect.xLeft),
                       LOUSHORT((rect.yTop-rect.yBottom)),SWP_SIZE|SWP_MOVE);
                 }
             break;

        case WM_CLOSE:  /* send WM_DDE_UNADVISE, shutdown all conversations for this applications, then quit */
             if (WinQueryWindowULong(hwnd, WW_CONVCOUNT)) {
                 WinSetWindowULong(hwnd, WW_CLOSE, WinQueryWindowULong(hwnd, WW_CLOSE) | WIN_CLOSING_FLAG);
                 henum = WinBeginEnumWindows(DDEanchorHWND);
                 while (hwndenum = WinGetNextWindow(henum)) {
                         WinSetWindowULong(hwndenum, WW_CONV_FLAGS,
                                            WinQueryWindowULong(hwndenum, WW_CONV_FLAGS) | WIN_TERM_FLAG);
                         tohwnd = (HWND)WinQueryWindowULong(hwndenum, WW_CONV_HWND);
                         DDEptr = st_DDE_Alloc(sizeof(DDESTRUCT) + strlen("Graphics") + 1, "DDEFMT_graphics_data");
                         DDEptr->offszItemName = (USHORT)sizeof(DDESTRUCT);
                         strcpy(DDES_PSZITEMNAME(DDEptr), "Graphics");
                         WinDdePostMsg(tohwnd, hwndenum, WM_DDE_UNADVISE, DDEptr, TRUE);
                         WinDdePostMsg(tohwnd, hwndenum, WM_DDE_TERMINATE, NULL, TRUE);
                         WinLockWindow(hwndenum, FALSE);
                 }
                 WinEndEnumWindows(henum);
             }
             else {
                 WinPostMsg(hwnd,WM_QUIT,0L,0L);  /*  quit if no conversations open  */
             }
             break;

         case APPM_CONV_CLOSE: /* decrement conversation count and delete picture  */
             WinSetWindowULong(hwnd, WW_CONVCOUNT, WinQueryWindowULong(hwnd, WW_CONVCOUNT) - 1);
             WinSendMsg(WinWindowFromID(hwnd,ID_GRAPHICS1),IC_DELETEITEM, (MPARAM)LOUSHORT(lParam1), (MPARAM)NULL);
             if (WinQueryWindowULong(hwnd, WW_CLOSE) && !WinQueryWindowULong(hwnd, WW_CONVCOUNT)){
                 WinPostMsg(hwnd,WM_QUIT,0L,0L);
             }
             break;

        case WM_DDE_INITIATE:  /* pass to DDE anchor window  */
              WinPostMsg(DDEanchorHWND, message, lParam1, lParam2);
             break;
        default:
            return(WinDefWindowProc(hwnd,message,lParam1,lParam2));

    }
    return((MRESULT)0L);
}

/*
 *  Function ClientDDEWndProc
 *
 *  This function is the wndproc for the conversational DDE window
 *  and processes the WM_DDE_DATA and WM_DDE_TERMINATE message.  The
 *  WM_DDE_DATA message is processed by adding or replacing the picture
 *  received from the server to the graphics control.  WM_DDE_TERMINATE
 *  sends a WM_DDE_TERMINATE to the server, as APPM_CONV_CLOSE message
 *  to the the client application, and then the conversational
 *  window destroys itself.
 */

MRESULT APIENTRY ClientDDEWndProc(hwnd,message,lParam1,lParam2)
HWND    hwnd;
USHORT  message;
MPARAM  lParam1;
MPARAM  lParam2;
{
PDDESTRUCT DDEstrptr, DDEstrPtrAck;  /* pointers for DDESTRUCT allocation */
PGDEDATA   gde_ptr;                  /* pointer for picture data          */


    DDEstrptr = (PDDESTRUCT)lParam2;

    switch (message){

        case WM_DDE_DATA:   /* process incoming graphics  */
            gde_ptr = (PGDEDATA)DDES_PABDATA(DDEstrptr);
            gde_ptr->hwnd_idItem = LOUSHORT(hwnd);

            if (DDEstrptr->fsStatus && DDE_FRESPONSE) {  /*  add if request  */
               WinSendMsg(WinWindowFromID(WinQueryWindow(hwnd,QW_OWNER,FALSE),ID_GRAPHICS1),IC_INSERTITEM,
                          MPFROMSHORT(ICM_END), (MPARAM)gde_ptr);
            } else {                                     /* replace if advise */
               WinSendMsg(WinWindowFromID(WinQueryWindow(hwnd,QW_OWNER,FALSE),ID_GRAPHICS1), IC_SETITEMSTRUCT,
                                          MPFROMSHORT(gde_ptr->hwnd_idItem), (MPARAM)gde_ptr);
            }

            if (DDEstrptr->fsStatus && DDE_FACKREQ) {
               DDEstrPtrAck = st_DDE_Alloc(sizeof(DDESTRUCT) + strlen("Graphics") + 1, "DDEFMT_graphics_data");
               DDEstrPtrAck->offszItemName = (USHORT)sizeof(DDESTRUCT);
               DDEstrPtrAck->fsStatus |= DDE_FACK;
               strcpy(DDES_PSZITEMNAME(DDEstrPtrAck), "Graphics");
               WinDdePostMsg((HWND)lParam1, hwnd, (ULONG)WM_DDE_ACK, DDEstrPtrAck, TRUE);
            }

            DosFreeSeg(PDDESTOSEL(DDEstrptr));

            break;

        case WM_DDE_TERMINATE:  /* post terminate to server, tell client, and die  */
              if (!WinQueryWindowULong(WinQueryWindow(hwnd, QW_OWNER, FALSE), WW_CLOSE)) {
                  WinDdePostMsg((HWND)lParam1, hwnd, WM_DDE_TERMINATE, NULL, TRUE);
              }
               WinPostMsg(WinQueryWindow(hwnd, QW_OWNER, FALSE),APPM_CONV_CLOSE, MPFROMLONG(hwnd), (MPARAM)NULL);
               WinDestroyWindow(hwnd);

             break;

        case WM_DDE_ACK:  /* unadvise and terminate conversation if server can't process picture  */
              if ((DDEstrptr->fsStatus && ~DDE_FACK) &&
                  (DDEstrptr->fsStatus && DDE_FRESPONSE)) {
                      DDEstrPtrAck = st_DDE_Alloc(sizeof(DDESTRUCT) + strlen("Graphics") + 1, "DDEFMT_graphics_data");
                      DDEstrPtrAck->offszItemName = (USHORT)sizeof(DDESTRUCT);
                      strcpy(DDES_PSZITEMNAME(DDEstrPtrAck), "Graphics");
                      WinDdePostMsg((HWND)lParam1, hwnd, WM_DDE_UNADVISE, DDEstrPtrAck, TRUE);
                      WinDdePostMsg((HWND)lParam1, hwnd, WM_DDE_TERMINATE, NULL, TRUE);
              }
             break;
        default:
            return(WinDefWindowProc(hwnd,message,lParam1,lParam2));

    }
    return((MRESULT)0L);
}

/*
 *  Function ClientDDEParWndProc
 *
 *  This function is the wndproc for the anchor DDE window and
 *  processes the initialization messages for the client on behalf
 *  of the conversation DDE and the client main winprocs.  The DDE
 *  advise and request for the client are processed here as part of
 *  initialization processing.
 */

MRESULT APIENTRY ClientDDEParWndProc(hwnd,message,lParam1,lParam2)
HWND    hwnd;
USHORT  message;
MPARAM  lParam1;
MPARAM  lParam2;
{
PDDESTRUCT DDEstrptr;
PDDEINIT   DDEInitPtr;
HWND       DDEconversationHWND;
char       itoa_buf[24];

    DDEInitPtr = (PDDEINIT)lParam2;
    switch (message){

        case WM_CREATE:  /* broadcast the initiate message  */
             WinDdeInitiate(hwnd, "", "Graphics_Exchange");
             break;

        case WM_DDE_INITIATE: /* reply with a initiate to specific server  */
             if (hwnd != lParam1) {
                 if ((!strcmp("Client", DDEInitPtr->pszAppName)) &&
                    (!strcmp("Graphics_Exchange", DDEInitPtr->pszTopic))) {
                     itoa((int)lParam1, itoa_buf, 10);
                     WinDdeInitiate(hwnd, itoa_buf, "Graphics_Exchange");
                 }
             }
             DosFreeSeg(PDDEITOSEL(DDEInitPtr));
             break;

        case WM_DDE_INITIATEACK:  /* establish conversation with server  */
             if( ((!strcmp("Client", DDEInitPtr->pszAppName)) &&
                  (!strcmp("Graphics_Exchange", DDEInitPtr->pszTopic))) ||
                ((!strlen(DDEInitPtr->pszAppName)) &&
                  (!strcmp("Graphics_Exchange", DDEInitPtr->pszTopic)))) {
                 /* create a window for the conversation  -  child of DDEanchorHWND  */
                 DDEconversationHWND = WinCreateWindow(hwnd, (PSZ)"DDE_Win", (PSZ)NULL,
                                                      WS_VISIBLE, 0, 0, 0, 0,
                                                      WinQueryWindow(hwnd, QW_PARENT, FALSE), HWND_TOP,
                                                      ++winDDEid, (PVOID)NULL, (PVOID)NULL);
                 WinSetWindowULong(DDEconversationHWND, WW_CONV_HWND, (ULONG)lParam1);
                 WinSetWindowULong(WinQueryWindow(hwnd, QW_PARENT, FALSE), WW_CONVCOUNT,
                                   WinQueryWindowULong(WinQueryWindow(hwnd, QW_PARENT, FALSE), WW_CONVCOUNT) + 1);

                 /* send a request for initial data */
                 DDEstrptr = st_DDE_Alloc(sizeof(DDESTRUCT) + strlen("Graphics") + 1, "DDEFMT_graphics_data");
                 DDEstrptr->offszItemName = (USHORT)sizeof(DDESTRUCT);
                 strcpy(DDES_PSZITEMNAME(DDEstrptr), "Graphics");
                 WinDdePostMsg((HWND)lParam1, DDEconversationHWND, (ULONG)WM_DDE_REQUEST, DDEstrptr, TRUE);

                 /* send an advise to subscribe to receive future data updates */
                 DDEstrptr = st_DDE_Alloc(sizeof(DDESTRUCT) + strlen("Graphics") + 1, "DDEFMT_graphics_data");
                 DDEstrptr->offszItemName = (USHORT)sizeof(DDESTRUCT);
                 strcpy(DDES_PSZITEMNAME(DDEstrptr), "Graphics");
                 WinDdePostMsg((HWND)lParam1, DDEconversationHWND, (ULONG)WM_DDE_ADVISE, DDEstrptr, TRUE);
             }
             /* free the memory */
             DosFreeSeg(PDDEITOSEL(DDEInitPtr));
             break;

        default:
            return(WinDefWindowProc(hwnd,message,lParam1,lParam2));

    }
    return((MRESULT)0L);
}
