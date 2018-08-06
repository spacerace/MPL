/* mou.c
 *
 * mouse thread code for PMAccess
 *
 */

#define INCL_DOS
#define INCL_SUB
#include<os2.h>
#include"mou.h"
#include"moudefs.h"
#include"msgq.h"
#include"button.h"
#include"msgs.h"
#include"errexit.h"

extern long MouSem;
extern PCHAR mainmsgqueue;
extern BUTTON buttonlist[];
extern USHORT dirrow;

typedef struct _moubuttons
    {
    USHORT    mask;
    USHORT    movedmask;
    USHORT    down;
    USHORT    upmsg;
    USHORT    downmsg;
    } MOUBUTTONS;

MOUBUTTONS buttons[3] =
        {
{ MOU_B1, MOUSE_MOTION_WITH_BN1_DOWN, FALSE, MSG_B1UP, MSG_B1DOWN },
{ MOU_B2, MOUSE_MOTION_WITH_BN2_DOWN, FALSE, MSG_B2UP, MSG_B2DOWN },
{ MOU_B3, MOUSE_MOTION_WITH_BN3_DOWN, FALSE, MSG_B3UP, MSG_B3DOWN }
        };

USHORT ButtonPressed(MOUEVENTINFO *ev);

void MouThread(void)
    {
    MOUEVENTINFO MouEvent;
    USHORT       WaitOption = MOU_WAIT;      // set to block on input
    HQUEUE       qhandle;
    USHORT       buttondown = FALSE, numbuttons,i;
    USHORT       retval, mouse_moved,event;
    HMOU         MouHandle;

    MsgQOpen(&qhandle,mainmsgqueue);

    if((retval = MouOpen((PSZ)NULL,(PHMOU)&MouHandle)))
        error_exit(retval,"MouOpen");

    MouDrawPtr(MouHandle);                   // display mouse pointer
    MouFlushQue(MouHandle);                  // flush mouse queue
    MouGetNumButtons(&numbuttons,MouHandle); // get button count
    DosSemClear(&MouSem);                    // notify main thread

    while(TRUE)
        {                                       // read the queue
        MouReadEventQue(&MouEvent,&WaitOption,MouHandle);
        MouEventDropLowBit(MouEvent);      // turn off the low bit

            // notify if screen button pressed
                                           // if mouse button pressed
        if(!buttondown && IsMouButtonPressed(MouEvent))
                                          // while on screen button
            if(event = ButtonPressed(&MouEvent))
                {
                MsgQSend(qhandle,NULL,0,event);
                buttondown = TRUE;
                DosSleep(32L);
                continue;
                }
        buttondown = FALSE;

        if(MouEvent.row >= dirrow)     // non-PM-like line:
            continue;                  // protecting mouse buttons

        for( i = 0, mouse_moved = FALSE; i < numbuttons; i++)
                                       // if the button is down now
            if(MouButtonPressed(MouEvent,buttons[i].mask))
                {
                if(!buttons[i].down)   // if button was previously up
                    {
                    MsgQSend(qhandle,&MouEvent,sizeof(MouEvent),
                             buttons[i].downmsg);
                    buttons[i].down = TRUE;
                    }
                    // if button was previously down but mouse moved
                else if(MouButtonPressed(
                        MouEvent,buttons[i].movedmask))
                    mouse_moved = TRUE;
                }
            else                       // if button is not down now
                {
                if(buttons[i].down)    // if button previously down
                    {
                    MsgQSend(qhandle,&MouEvent,sizeof(MouEvent),
                             buttons[i].upmsg);
                    buttons[i].down = FALSE;
                    }
                }
    // notify of all mouse movement if button is down
    // (PM gets all movement)
        if(mouse_moved)
            MsgQSend(qhandle,&MouEvent,sizeof(MouEvent),
                MSG_MOUSEMOVED);

        DosSleep(32L);
        }

    MouClose(MouHandle);
    }

USHORT ButtonPressed(MOUEVENTINFO *ev)
    {
    register USHORT row = ev->row, col = ev->col;
    BUTTON    *b = &buttonlist[0];

    for( ; b->text; b++)
        {
        if((row >= b->startrow) && (row <= b->endrow) // if on button
                && (col >= b->startcol) && (col <= b->endcol))

            if(MouB1Pressed(ev->fs))
                return MOUSECODE(b->left_button_val);
            else if(MouB2Pressed(ev->fs))
                return MOUSECODE(b->right_button_val);
            else
                return 0;
        }
    return 0;
    }

    /******* end of mouse thread code *************************/

