/* pmaccess.c RHS
 *
 * application that uses PMSERVER for accessing PM services
 *
 *
 */

#define    INCL_SUB
#define    INCL_DOSERRORS
#define    INCL_DOS
#include<os2.h>
#include<mt\stdio.h>
#include<mt\string.h>
#include<mt\stdlib.h>
#include<mt\process.h>
#include<mt\dos.h>
#include"mou.h"
#include"kbd.h"
#include"pmserver.h"
#include"moudefs.h"
#include"kbddefs.h"
#include"msgq.h"
#include"button.h"
#include"errexit.h"
#include"msgs.h"

#define     MAINMSGQUEUE    "\\QUEUES\\PMLITE.QUE"

#define     VIOHDL                  0
#define     KEYTHREADSTACK          500         // keyboard stack
#define     REQUESTTHREADSTACK      500         // request stack
#define     MOUTHREADSTACK          800         // mouse stack

#define     BORDERCOL               79
#define     BORDERROW               17
#define     BORDERROW2              21
#define     STARTROW                0
#define     ENDROW                  20

#define     BUTTON_ON               0x70
#define     BUTTON_OFF              0x0f
#define     NORMAL                  BUTTON_OFF
#define     HIGHLIGHT               BUTTON_ON

#define     beep()                  DosBeep(440,200)
#define     Lastchar(str)           (str[strlen(str)-1])

#define     MAXREQUESTCOUNT         10
#define     REQUESTTHREADSLEEPTIME  3000L

    /********************* Buttons ****************************/

#define     BUTTONLIST              13
BUTTON buttonlist[BUTTONLIST] =
    {
/* text  title  row,col,row,col,attr,     type  left val, right val,
        accelerator    */
{"                         ","Application Name",
              18,16,  0, 0, BUTTON_OFF, INPUT,  MSG_APPNAME,
            MSG_APPNAME,
        ALT_1,  0},
{"                 ",       "Topic Name",
              18,43,  0, 0, BUTTON_OFF, INPUT,  MSG_TOPICNAME,
            MSG_TOPICNAME,
        ALT_2,  0},
{"               ",         "Item Name",
              18,62,  0, 0, BUTTON_OFF, INPUT,  MSG_ITEMNAME,
            MSG_ITEMNAME,
        ALT_3,  0},
{"Copy","",   22, 0, 0, 0,  BUTTON_OFF, BPRESS, MSG_COPY,   MSG_COPY,
        ALT_O,  0},
{"Cut","",    22, 7, 0, 0,  BUTTON_OFF, BPRESS, MSG_CUT,    MSG_CUT,
        ALT_U,  0},
{"Paste","",  22,13, 0, 0,  BUTTON_OFF, BPRESS, MSG_PASTE, MSG_PASTE,
        ALT_P,  0},
{"Initiate","",22,23,0, 0,  BUTTON_OFF, BPRESS, MSG_INIT,   MSG_INIT,
        ALT_I,  0},
{"Request","",22,34, 0, 0,  BUTTON_OFF, BPRESS, MSG_REQUEST,
            MSG_REQUEST,
        ALT_R,  0},
{"Advise","", 22,44, 0, 0,  BUTTON_OFF, BPRESS, MSG_ADVISE,
            MSG_ADVISE,
        ALT_A},
{"Terminate","",22,53,0,0,  BUTTON_OFF, BPRESS, MSG_TERM,   MSG_TERM,
        ALT_T,  0},
{"Clear","",  22,67, 0, 0,  BUTTON_OFF, BPRESS, MSG_CLR,    MSG_CLR,
        ALT_C,  0},
{"Esc","",    22,75, 0, 0,  BUTTON_OFF, BPRESS, MSG_ESCAPE,
            MSG_ESCAPE,
        ESC,    0},
{NULL,   0,     0, 0, 0,  0,          0,      0,          0,
                0,      0}
    };

    /**********************************************************/

typedef struct _screen_label
    {
    PCHAR   text;
    SHORT   row;
    SHORT   column;
    } SCREEN_LABEL;

#define SCREEN_LABELS 4
SCREEN_LABEL screen_labels[SCREEN_LABELS] =
    {
    {"Clipboard Functions",     BORDERROW2,     1},
    {"DDE Functions",           BORDERROW2,     35},
    {"Gen'l",                   BORDERROW2,     72},
    {"DDE Parameters:",         BORDERROW+2,      0}
    };    

PCHAR mainmsgqueue = MAINMSGQUEUE;
BYTE MouThreadStack[MOUTHREADSTACK];
BYTE KbdThreadStack[KEYTHREADSTACK];
BYTE RequestThreadStack[REQUESTTHREADSTACK];

long MouSem = 0L, KbdSem = 0L;

unsigned    BlankCell = 0x0f20;
USHORT  dirrow = BORDERROW;

USHORT  DDEstartrow,DDEstartcol,DDEendrow,DDEendcol;
USHORT  rowBlockBeg,colBlockBeg,startBlock,ScreenRows,ScreenCols,
        blocksize;
USHORT  startrow,startcol,endrow,endcol;
BYTE    highlight_att = HIGHLIGHT,normal_att = NORMAL;
PBYTE   blockptr, bufptr;
HQUEUE  pmshdl;

BYTE    tempbuffer[8192];
USHORT  clipboard_data = TRUE, dde_init = FALSE, Server = FALSE;
PIDINFO pidinfo;
char appname[MAXAPPNAME], topicname[MAXTOPICNAME],
        itemname[MAXITEMLEN];
BUTTON  *app, *topic, *item;

    /********************* Function Prototypes ********************/

void    main(void);
void    RequestThread(void);
void    InitScreen(void);
void    SetBlock(USHORT row, USHORT col);
void    ResetScreen(void);
void    BlankBlock(void);
void    ResetBlock(void);

void    ButtonPress(USHORT *eventcode);
USHORT  prepare_CLPBRDblock(void);
void    PMS_Init(HQUEUE serverhandle,char *qname);
void    PMS_Terminate(HQUEUE serverhandle);
USHORT  readtextblock(PCHAR buffer,
        USHORT srow, USHORT scol, USHORT erow, USHORT ecol);

void    activate_entrybutton(BUTTON *b, char *buttonfield);

void main(void)
    {
    USHORT pause = 0, msg, capture, valid_block, msgsize, retval, i;
    USHORT post_on = FALSE,row,col;
    HQUEUE qhandle;
    PVOID  *msgdata;
    BUTTON *b;
    PBYTE   p;

    app = &buttonlist[0];
    topic = &buttonlist[1];
    item = &buttonlist[2];

    *appname = *topicname = *itemname = '\0';

    DosGetPID(&pidinfo);                        // get process id
    MsgQCreate(&qhandle,MAINMSGQUEUE);
    if(retval = MsgQOpen(&pmshdl,PMSERVERQUE))
        error_exit(retval,
        "MsgQOpen - PM Server probably hasn\'t started...");

    DosSemSet(&MouSem);
    DosSemSet(&KbdSem);
    
                                                //start other threads
    if(_beginthread(MouThread,MouThreadStack,
            sizeof(MouThreadStack),NULL) == -1)
        error_exit(0,"_beginthread(Mouse)");
    if(_beginthread(KbdThread,KbdThreadStack,
            sizeof(KbdThreadStack),NULL) == -1)
        error_exit(0,"_beginthread(Keyboard)");
    if(_beginthread(RequestThread,RequestThreadStack,
            sizeof(RequestThreadStack),NULL) == -1)
        error_exit(0,"_beginthread(Request)");
    
    DosSemWait(&MouSem,SEM_INDEFINITE_WAIT);    // get threads going
    DosSemWait(&KbdSem,SEM_INDEFINITE_WAIT);

    InitButtons();
    InitScreen();
    DisplayButtons();
    VioGetBuf((PULONG)&bufptr, &msg, VIOHDL);
    PMS_Init(pmshdl,MAINMSGQUEUE);

    while(TRUE)
        {
        MsgQGet(qhandle, &msgdata, &msgsize, &msg);

        if(ButtonEvent(msg))
            ButtonPress(&msg);

        switch(msg)
            {
            case MSG_ESCAPE:
            case ESC:
                MsgQClose(qhandle);             // close queue
                                                // clear the screen
                VioScrollUp(0,0,-1,-1,-1,(char *)&BlankCell,VIOHDL);
                PMS_Terminate(pmshdl);
                DosExit(EXIT_PROCESS,0);        // get out of Dodge
                break;

            case MSG_CLR:                       // clear screen
                VioScrollUp(0,0,BORDERROW-1,ScreenCols,0xffff,
                    (char *)&BlankCell,VIOHDL);
                InitScreen();
                DisplayButtons();
                VioWrtCharStr(appname,strlen(appname),
                    app->startrow+1,app->startcol+1,VIOHDL);
                VioWrtCharStr(topicname,strlen(topicname),
                    topic->startrow+1,topic->startcol+1,VIOHDL);
                VioWrtCharStr(itemname,strlen(itemname),
                    item->startrow+1,item->startcol+1,VIOHDL);
                MsgQSend(pmshdl,&pidinfo.pid,sizeof(PID),
                    PMS_CLPBRD_QUERY);
                VioSetCurPos(0,0,VIOHDL);
                MsgQSend(qhandle,0,0,MSG_RESETINITBUTTON);
                break;

            // ************* Mouse Message handling
            case MSG_B1DOWN:
                // clear any existing block and set up a new one
                valid_block = FALSE;
                ResetScreen();
                capture = TRUE;
                rowBlockBeg = MOUMSG(msgdata)->row;
                colBlockBeg = MOUMSG(msgdata)->col;
                startBlock = (rowBlockBeg * ScreenCols)+colBlockBeg;
                SetBlock(rowBlockBeg,colBlockBeg);
                VioSetCurPos(rowBlockBeg,colBlockBeg,VIOHDL);
                break;

            case MSG_MOUSEMOVED:
                if(capture)
                    SetBlock(MOUMSG(msgdata)->row,
                        MOUMSG(msgdata)->col);
                break;

            case MSG_B1UP:
                if(capture)
                    {
                    capture = FALSE;
                    valid_block = TRUE;
                    SetBlock(MOUMSG(msgdata)->row,
                        MOUMSG(msgdata)->col);
                    }
                break;

            case MSG_APPNAME:
                activate_entrybutton(app,appname);
                break;

            case MSG_ITEMNAME:
                activate_entrybutton(item,itemname);
                break;

            case MSG_TOPICNAME:
                activate_entrybutton(topic,topicname);
                break;

            case MSG_RESETINITBUTTON:
                findbutton("Initiate",&b);
                ButtonPaint(b,(BYTE)(dde_init ?
                    BUTTON_ON : BUTTON_OFF));
                break;

            case MSG_RESETPASTEBUTTON:
                findbutton("Paste", &b);
                ButtonPaint(b,(BYTE)(clipboard_data ?
                    BUTTON_ON : BUTTON_OFF));
                break;

            // ************* Keyboard message handling
            case MSG_CHAR:
                {
                int c = KBDMSG(msgdata)->chChar;

                VioGetCurPos(&row,&col,VIOHDL); // get cursor pos.
                b = IsButton(row,col);
                if(b)
                    if(c == '\r')
                        break;
                    else if((c == '\b') && (col == b->startcol+1))
                        break;
                if(!b || ((col > b->startcol) && (col < b->endcol)))
                    VioWrtTTY((PCH)&c,1,VIOHDL);

                switch(c)
                    {
                    case '\b':
                        if(b && (row == b->startcol+1))
                            break;
                        VioWrtTTY(" \b",2,VIOHDL);
                        break;

                    case '\r':
                        if(row == ScreenRows)
                            VioScrollUp(0,0,BORDERROW-1,ScreenCols,1,
                                    (char *)&BlankCell,VIOHDL);
                        else
                            VioWrtTTY("\n",1,VIOHDL);
                        break;
                    }
                }
                break;

            case MSG_UPARROW:
            case MSG_DOWNARROW:
            case MSG_LEFTARROW:
            case MSG_RIGHTARROW:
            case MSG_HOME:
            case MSG_END:
                {
                BUTTON *b;
                USHORT start = 0,end = ScreenCols-1;

                VioGetCurPos(&row,&col,VIOHDL); // get cursor pos.
                if(b = IsButton(row,col))
                    {
                    start = b->startcol+1;
                    end = b->endcol-1;
                    }

                switch(msg)
                    {
                    case MSG_UPARROW:
                        if(!b && (row > 0))
                            row--;
                        break;

                    case MSG_DOWNARROW:
                        if(!b && (row < BORDERROW))
                            row++;
                        break;

                    case MSG_LEFTARROW:
                        if(col > start)
                            col--;
                        break;

                    case MSG_RIGHTARROW:
                        if(col < end)
                            col++;
                        break;

                    case MSG_HOME:
                        col = start;
                        break;

                    case MSG_END:
                        col = end;
                        break;
                    }
                VioSetCurPos(row,col,VIOHDL);
                }
                break;

            // *******************Clipboard handling****************

            case MSG_COPY:              // user copy to clipboard
            case MSG_CUT:               // user cut to clipboard
                if(!valid_block)        // if no block selected
                    {
                    beep();             // warn user
                    break;              // forget it
                    }
                i = prepare_CLPBRDblock();
                                        // pass data to server
                MsgQSend(pmshdl,tempbuffer,i,PMS_CLPBRD_COPY);

                if(msg == MSG_CUT)      // reset screen
                    BlankBlock();
                else
                    ResetBlock();
                valid_block = FALSE;
                break;

            case MSG_PASTE:                     // user paste
                MsgQSend(pmshdl,&pidinfo.pid,sizeof(PID),
                    PMS_CLPBRD_PASTE);
                break;

            // ********************DDE handling*********************

            case MSG_INIT:                      // user wants dde
                if(dde_init)                    // if dde active
                    {
                    beep();
                    break;
                    }
                {
                CLIENTDATA *pclientdata = (CLIENTDATA *)tempbuffer;
                PBYTE p;

                ButtonRead(app,appname);        // get app name
                ButtonRead(topic,topicname);    // get topic name

                pclientdata->pid = pidinfo.pid; // set up PID
                strcpy(pclientdata->qname,appname);// add app name
                p = pclientdata->qname;                     
                p += strlen(p)+1;
                strcpy(p,topicname);            // add topic name
                p += strlen(p)+1;
                if(i = (p-(PBYTE)pclientdata))  // size
                    {
                        // request initiation of DDE conversation
                    dde_init = TRUE;
                    MsgQSend(pmshdl,tempbuffer,i,PMS_DDE_INIT);
                    }
                }
                break;

            case MSG_REQUEST:
            case MSG_ADVISE:
            case MSG_TERM:
                {
                int i;

                if(!dde_init)
                    beep();
                else
                    {
                    CLIENTDATA *pclientdata =
                        (CLIENTDATA *)tempbuffer;
                    pclientdata->pid = pidinfo.pid; // set up PID
                    ButtonRead(item,itemname);
                    strcpy(pclientdata->qname,itemname);
                    i = sizeof(PID)+strlen(pclientdata->qname)+1;

                    switch(msg)
                        {
                        case MSG_REQUEST:
                            MsgQSend(pmshdl,tempbuffer,i,
                                PMS_DDE_REQUEST);
                            break;
                        case MSG_ADVISE:
                            MsgQSend(pmshdl,tempbuffer,i,
                                PMS_DDE_ADVISE);
                            break;
                        case MSG_TERM:
                            MsgQSend(pmshdl,tempbuffer,sizeof(PID),
                                PMS_DDE_TERMINATE);
                            dde_init = FALSE;
                            ButtonDisplay(app);
                            ButtonDisplay(topic);
                            ButtonDisplay(item);
                            MsgQSend(qhandle,0,0,
                                MSG_RESETINITBUTTON);
                            break;
                        }
                    }
                }
                break;

            // **************** PMServer message handling ***********
            case PMS_CLPBRD:                    // data available
            case PMS_CLPBRD_EMPTY:              // clpbrd is empty
                clipboard_data = ((msg == PMS_CLPBRD) ? TRUE :
                    FALSE);
                MsgQSend(qhandle,0,0,MSG_RESETPASTEBUTTON);
                break;

            case PMS_DDE_INITACK:               // DDE available
                MsgQSend(qhandle,0,0,MSG_RESETINITBUTTON);
                beep();
                break;

            case PMS_DDE_INITNAK:               // DDE not available
                dde_init = FALSE;
                beep();
                break;

            case PMS_DDE_TERMINATE:  // DDE conversation terminated
                if(dde_init)
                    {
                    dde_init = FALSE;
                    beep();
                    MsgQSend(qhandle,0,0,MSG_RESETINITBUTTON);
                    }
                break;

            case PMS_CLPBRD_DATA:    // clipboard data rec'd
            case PMS_DDE_DATA:       // dde data rec'd
                {
                PITEMREQ pitemreq = (PITEMREQ)msgdata;
                PBYTE mptr;

                if(msg == PMS_DDE_DATA)
                    {
                    if(stricmp(pitemreq->item,itemname))
                        break;
                    mptr = pitemreq->value;
                    msgsize = strlen(mptr)+1;
                    }
                else
                    mptr = (PBYTE)msgdata;

                VioGetCurPos(&row,&col,VIOHDL); // get cursor pos.
                for(i = 0, p = (PBYTE)mptr; *p; p++)
                    if(*p == 13)                // if a linefeed
                        i++;                    // # of lines in text
                if(row+i > ScreenRows)
                    {
                    int j = (row+i)-ScreenRows;

                    VioScrollUp(0,0,BORDERROW-1,ScreenCols,j,
                            (char *)&BlankCell,VIOHDL);
                    row -= j;
                    VioSetCurPos(row,col,VIOHDL);
                    }
                VioWrtTTY((PCH)mptr,msgsize-1,VIOHDL);
                if(msg == PMS_CLPBRD_DATA)
                    MsgQSend(pmshdl,&pidinfo.pid,sizeof(PID),
                        PMS_CLPBRD_QUERY);
                }
                break;

            case PMS_NO_INIT:                   // server can't init
                error_exit(msg,
                    "PMS_Init - unable to access PM Server");
                break;

            case PMS_SERVER_TERM:          // server has terminated
                error_exit(msg,
                    "PMAccess: Server had terminated, exiting");
                Server = FALSE;
                break;


            case PMS_INIT_ACK:             // server initialized
                Server = TRUE;
                clipboard_data = FALSE;    // then fall thru

            default:
                break;
            }
        if(msgdata != NULL)
            DosFreeSeg(SELECTOROF(msgdata));
        }
    }

// terminates PMServer connection
void PMS_Terminate(HQUEUE serverhandle)
    {
                                            // terminate connection
    MsgQSend(serverhandle,&pidinfo.pid,sizeof(PID),PMS_TERMINATE);
    MsgQClose(serverhandle);                // close queue
    }

// initializes PMServer connection
void PMS_Init(HQUEUE serverhandle,char *qname)
    {
    CLIENTDATA clientdata;

    strcpy(clientdata.qname,qname);         // set up qname
    clientdata.pid = pidinfo.pid;           // set up PID
                                            // initialize connection
    MsgQSend(serverhandle,&clientdata,sizeof(clientdata),PMS_INIT);
    }

// prepare data for clipboard
USHORT prepare_CLPBRDblock(void)
    {
    return readtextblock(tempbuffer,startrow,startcol,endrow,endcol);
    }

// read text from screen
USHORT readtextblock(PCHAR buffer,USHORT srow, USHORT scol,
        USHORT erow, USHORT ecol)
    {    
    USHORT    i,row,len,col;

    for( row = srow, col = scol, i = 0; row <= erow; row++)
        {
        len = ScreenCols;
        if(row != srow)
            col = 0;
        if(row == erow)
            len = ecol+1;
        VioReadCharStr(&buffer[i],&len,row,col,VIOHDL);
        i += len-1;
        while(buffer[i] == ' ')
            i--;
        buffer[++i] = '\r';
        buffer[++i] = '\n';
        i++;
        }
    buffer[i] = '\0';
    return i+1;                            // return length of block
    }

// set screen text block
void SetBlock(USHORT row, USHORT col)
    {
    USHORT endBlock = (row * ScreenCols) + col;
    USHORT presize,postsize,start;

    if(startBlock <= endBlock)          // block end at/after start
        {
        // normal from top to startBlock-1, 
        // highlight from startBlock to endBlock
        // normal from endBlock+1 to bottom
        presize = startBlock;
        startrow = rowBlockBeg;
        startcol = colBlockBeg;
        blocksize = endBlock-startBlock+1;
        endBlock++;
        endrow = (endBlock/ScreenCols);
        endcol = (endBlock%ScreenCols);
        postsize = (ScreenRows*ScreenCols)-endBlock+1;
        }
    else                                // block end before start
        {
        // normal from top to endBlock-1
        // highlight from endBlock to startBlock
        // normal from startBlock+1 to bottom
        presize = endBlock;
        startrow = row;
        startcol = col;
        blocksize = startBlock-endBlock+1;
        start = startBlock+1;
        endrow = (start/ScreenCols);
        endcol = (start%ScreenCols);
        postsize = (ScreenRows*ScreenCols)-start+1;
        }

    blockptr = bufptr+presize;          // set to beginning of block
    // now write pre-block and post-block normal, highlight block
    VioWrtNAttr(&normal_att,presize,0,0,VIOHDL);
    VioWrtNAttr(&highlight_att,blocksize,startrow,startcol,VIOHDL);
    VioWrtNAttr(&normal_att,postsize,endrow,endcol,VIOHDL);
    }

// reset screen work area
void ResetScreen(void)
    {
    USHORT i;

    for( i = STARTROW; i <= ENDROW; i++)
        VioWrtNAttr(&normal_att,BORDERCOL,STARTROW,0,VIOHDL);
    }

// blank screen text block
void BlankBlock(void)
    {
    VioWrtNCell((PBYTE)&BlankCell,blocksize,startrow,startcol,
        VIOHDL);
    }

// reset screen to normal
void ResetBlock(void)
    {
    VioWrtNAttr(&normal_att,(ScreenCols*ScreenRows),0,0,
        VIOHDL);
    }

// initialize screen
void InitScreen(void)
    {
    SHORT i;
    char c = 196;

    VIOMODEINFO modeinfo;

    VioGetMode(&modeinfo,VIOHDL);   // get screen mode, rows, cols
    ScreenCols = modeinfo.col;
    ScreenRows = modeinfo.row;
    if(ScreenRows > BORDERROW-1)
        ScreenRows = BORDERROW-1;

    VioScrollUp(0,0,-1,-1,-1,(char *)&BlankCell,VIOHDL); // clear scr
    VioSetCurPos(0,0,VIOHDL);                   // set cursor
    VioWrtNChar(&c,ScreenCols,BORDERROW,0,VIOHDL); // draw border 
    VioWrtNChar(&c,ScreenCols,BORDERROW2,0,VIOHDL); // draw border
    for( i = 0; i < SCREEN_LABELS; i++)         // write labels
        VioWrtCharStr(screen_labels[i].text,
            strlen(screen_labels[i].text),
            screen_labels[i].row,screen_labels[i].column,VIOHDL);
    }

// manipulate buttons for specific event code and button type
void ButtonPress(USHORT *eventcode)
    {
    BUTTON *b = &buttonlist[0];

    MOUSECODEOFF(*eventcode);                   // turn off mouse bit

    for( ; b->text; b++)                        // find the button            
        if((b->left_button_val == *eventcode) ||
                (b->right_button_val == *eventcode) ||
                (b->accelerator == *eventcode))
            {
            switch(b->type)
                {
                case BPRESS:                    // if a press button            
                    ButtonPaint(b,BUTTON_ON);   // turn it on                   
                    DosSleep(100L);             // wait                            
                    ButtonPaint(b,BUTTON_OFF);  // turn it off                    
                    break;

                case BTOGGLE:                   // if a toggle button        
                    b->state = !b->state;       // toggle it                    
                                                // and toggle color        
                    ButtonPaint(b,(b->state ? (BYTE)BUTTON_ON :
                        b->attribute));
                    break;

                default:
                    break;
                }
            return;
            }
    }


void activate_entrybutton(BUTTON *b, char *buttonfield)
    {
    int i;

    ButtonRead(b,buttonfield);                  // read button
    for(i = 0; buttonfield[i]; i++);            // get # of chars
                                                // cursor to end
    VioSetCurPos(b->startrow+1,b->startcol+1+i,VIOHDL);
    }

    /******* end of main thread code **************************/

    /******* start of request thread code *********************/

// periodically wakes up and queries PMServer about Clipboard
void RequestThread(void)
    {
    USHORT count = 0;

    while(TRUE)
        {
        if(!clipboard_data && Server)           // if invalidated
                                                // ask if paste
            MsgQSend(pmshdl,&pidinfo.pid,sizeof(PID),
                PMS_CLPBRD_QUERY);
        DosSleep(REQUESTTHREADSLEEPTIME);       // sleep a while
        count++;
        if(count >= MAXREQUESTCOUNT)            // count over limit?
            {
            clipboard_data = FALSE;             // invalidate flag
            count = 0;                          // reset count
            }
        }
    }

    /******* end of request thread code ***********************/


    /********************** end of pmlite.c ********************/

