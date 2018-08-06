/* pmaccess.c RHS 7/14/89
 *
 * application that uses PMSERVER for accessing PM services
 *
 *
    This application is a VIO app that offers the following services:
    1. It allows the user to position the cursor any where in the input screen
    2. It allows the user to type text into any part of the input screen
    3. It allows the user to mark any text in the input screen
    4. It allows the user to copy, cut or paste any part of the input screen.
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

#define     MAINMSGQUEUE    "\\QUEUES\\PMACCESS.QUE"

#define     VIOHDL                  0
#define     KEYTHREADSTACK          500         // keyboard thread stack
#define     REQUESTTHREADSTACK      500         // request thread stack
#define     MOUTHREADSTACK          800         // mouse thread stack

#define     BORDERCOL               79
#define     BORDERROW               21
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

#define     BUTTONLIST              6
BUTTON buttonlist[BUTTONLIST] =
    {
/* text  title  row,col,row,col,attr,     type    left val,   right val,
        accelerator    */
{" Copy ","",   22, 0, 0, 0,  BUTTON_OFF, BPRESS, MSG_COPY,   MSG_COPY,
        ALT_O,  0},
{" Cut ","",    22, 9, 0, 0,  BUTTON_OFF, BPRESS, MSG_CUT,    MSG_CUT,
        ALT_U,  0},
{" Paste ","",  22,17, 0, 0,  BUTTON_OFF, BPRESS, MSG_PASTE,  MSG_PASTE,
        ALT_P,  0},

{" Clear ","",  22,63, 0, 0,  BUTTON_OFF, BPRESS, MSG_CLR,    MSG_CLR,
        ALT_C,  0},
{" Esc ","",    22,73, 0, 0,  BUTTON_OFF, BPRESS, MSG_ESCAPE, MSG_ESCAPE,
        ESC,    0},

{NULL,           0, 0, 0, 0,  0,          0,      0,          0,
                0,      0}
    };

    /**********************************************************/

typedef struct _screen_label
    {
    PCHAR   text;
    SHORT   row;
    SHORT   column;
    } SCREEN_LABEL;

#define SCREEN_LABELS 2
SCREEN_LABEL screen_labels[SCREEN_LABELS] =
    {
    {"Clipboard Functions",     BORDERROW,     3},
    {"General Functions",       BORDERROW,     63}
    };    

PCHAR mainmsgqueue = MAINMSGQUEUE;
BYTE MouThreadStack[MOUTHREADSTACK];
BYTE KbdThreadStack[KEYTHREADSTACK];
BYTE RequestThreadStack[REQUESTTHREADSTACK];

long MouSem = 0L, KbdSem = 0L;

unsigned    BlankCell = 0x0f20;
USHORT  dirrow = BORDERROW;

USHORT  DDEstartrow,DDEstartcol,DDEendrow,DDEendcol;
USHORT  rowBlockBeg,colBlockBeg,startBlock,ScreenRows,ScreenCols,blocksize;
USHORT  startrow,startcol,endrow,endcol;
BYTE    highlight_att = HIGHLIGHT,normal_att = NORMAL;
PBYTE   blockptr, bufptr;
HQUEUE  pmshdl;

BYTE    tempbuffer[8192];
USHORT  clipboard_data = TRUE;
PIDINFO pidinfo;

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

void main(void)
    {
    USHORT pause = 0, msg, capture, valid_block, msgsize, retval, i,j;
    USHORT post_on = FALSE,row,col;
    HQUEUE qhandle;
    PVOID  *msgdata;
    BUTTON *b;
    PBYTE   p;

    DosGetPID(&pidinfo);                        // get process id
    MsgQCreate(&qhandle,MAINMSGQUEUE);
    if(retval = MsgQOpen(&pmshdl,PMSERVERQUE))
        error_exit(retval,"MsgQOpen - PM Server probably hasn\'t started...");

    DosSemSet(&MouSem);
    DosSemSet(&KbdSem);
    
                                                // start other threads
    if(_beginthread(MouThread,MouThreadStack,
            sizeof(MouThreadStack),NULL) == -1)
        error_exit(0,"_beginthread(Mouse)");
    if(_beginthread(KbdThread,KbdThreadStack,
            sizeof(KbdThreadStack),NULL) == -1)
        error_exit(0,"_beginthread(Keyboard)");
    if(_beginthread(RequestThread,RequestThreadStack,
            sizeof(RequestThreadStack),NULL) == -1)
        error_exit(0,"_beginthread(Request)");
    
    DosSemWait(&MouSem,SEM_INDEFINITE_WAIT);    // let threads get going
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
                MsgQClose(qhandle);             // close the message queue
                                                // clear the screen
                VioScrollUp(0,0,-1,-1,-1,(char *)&BlankCell,VIOHDL);
                PMS_Terminate(pmshdl);
                DosExit(EXIT_PROCESS,0);        // get out of dodge
                break;

            case MSG_CLR:                       // user clearing screen
                VioScrollUp(0,0,BORDERROW-1,ScreenCols,0xffff,(char *)&BlankCell,VIOHDL);
                InitScreen();
                DisplayButtons();
                MsgQSend(pmshdl,&pidinfo.pid,sizeof(PID),PMS_CLPBRD_QUERY);
                VioSetCurPos(0,0,VIOHDL);
                break;

            // ************* Mouse Message handling
            case MSG_B1DOWN:
                // clear any existing block and set up a new one
                valid_block = FALSE;
                ResetScreen();
                capture = TRUE;
                rowBlockBeg = MOUMSG(msgdata)->row;
                colBlockBeg = MOUMSG(msgdata)->col;
                startBlock = (rowBlockBeg * ScreenCols) + colBlockBeg;
                SetBlock(rowBlockBeg,colBlockBeg);
                break;

            case MSG_MOUSEMOVED:
                if(capture)
                    SetBlock(MOUMSG(msgdata)->row,MOUMSG(msgdata)->col);
                break;

            case MSG_B1UP:
                if(capture)
                    {
                    capture = FALSE;
                    valid_block = TRUE;
                    SetBlock(MOUMSG(msgdata)->row,MOUMSG(msgdata)->col);
                    }
                break;

            // ************* Keyboard message handling
            case MSG_CHAR:
                VioWrtTTY(&(KBDMSG(msgdata)->chChar),1,VIOHDL);

                switch(KBDMSG(msgdata)->chChar)
                    {
                    case '\b':
                        VioWrtTTY(" \b",2,VIOHDL);
                        break;

                    case '\r':
                        VioGetCurPos(&row,&col,VIOHDL);
                        if(row == ScreenRows)
                            VioScrollUp(0,0,BORDERROW-1,ScreenCols,1,
                                    (char *)&BlankCell,VIOHDL);
                        else
                            VioWrtTTY("\n",1,VIOHDL);
                        break;

                    }

                break;

            case MSG_UPARROW:
            case MSG_DOWNARROW:
            case MSG_LEFTARROW:
            case MSG_RIGHTARROW:
            case MSG_HOME:
            case MSG_END:
                VioGetCurPos(&row,&col,VIOHDL); // get current cursor pos.
                switch(msg)
                    {
                    case MSG_UPARROW:
                        if(row > 0)
                            row--;
                        break;

                    case MSG_DOWNARROW:
                        if(row < ScreenRows)
                            row++;
                        break;

                    case MSG_LEFTARROW:
                        if(col > 0)
                            col--;
                        break;

                    case MSG_RIGHTARROW:
                        if(col < ScreenCols)
                            col++;
                        break;

                    case MSG_HOME:
                        col = 0;
                        break;

                    case MSG_END:
                        col = ScreenCols-1;
                        break;
                    }
                VioSetCurPos(row,col,VIOHDL);
                break;

            // *******************Clipboard handling*****************

            case MSG_COPY:                      // user copy to clipboard
            case MSG_CUT:                       // user cut to clipboard
                if(!valid_block)                // if no block selected
                    {
                    beep();                     // warn user
                    break;                      // forget it
                    }
                i = prepare_CLPBRDblock();
                                                // pass data to server
                MsgQSend(pmshdl,tempbuffer,i,PMS_COPY);

                if(msg == MSG_CUT)              // reset screen
                    BlankBlock();
                else
                    ResetBlock();
                valid_block = FALSE;
                break;

            case MSG_PASTE:                     // user wants to paste
                MsgQSend(pmshdl,&pidinfo.pid,sizeof(PID),PMS_PASTE);
                break;

            // **************** PMServer message handling ***********
            case PMS_CLPBRD:                    // clpbrd data is available
                clipboard_data = TRUE;
                findbutton(" Paste ", &b);
                ButtonPaint(b,BUTTON_ON);
                break;

            case PMS_CLPBRD_EMPTY:              // clpbrd is empty
                clipboard_data = FALSE;
                findbutton(" Paste ", &b);
                ButtonPaint(b,BUTTON_OFF);
                break;

            case PMS_CLPBRD_DATA:               // clipboard data rec'd
                VioGetCurPos(&row,&col,VIOHDL); // get current cursor pos.
                for(i = 0, p = (PBYTE)msgdata; *p; p++)
                    if(*p == 13)                // if a linefeed
                        i++;                    // # of lines in text
                if(row+i >= ScreenRows)
                    {
                    VioScrollUp(0,0,BORDERROW-1,ScreenCols,(row+i)-ScreenRows,
                            (char *)&BlankCell,VIOHDL);
                    row -= i;
                    VioSetCurPos(row,col,VIOHDL);
                    }
                VioWrtTTY((PCH)msgdata,msgsize-1,VIOHDL);

                MsgQSend(pmshdl,&pidinfo.pid,sizeof(PID),PMS_CLPBRD_QUERY);
                break;

            case PMS_NO_INIT:                   // server can't initialize
                error_exit(msg,"PMS_Init - unable to access PM Server");
                break;

            case PMS_INIT_ACK:                  // server initialized
                clipboard_data = FALSE;         // then fall thru

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
    MsgQClose(serverhandle);                    // close queue
    }

// initializes PMServer connection
void PMS_Init(HQUEUE serverhandle,char *qname)
    {
    CLIENTDATA clientdata;

    strcpy(clientdata.qname,qname);             // set up qname
    clientdata.pid = pidinfo.pid;               // set up PID

                                                // initialize connection
    MsgQSend(serverhandle,&clientdata,sizeof(clientdata),PMS_INIT);
    }

// prepare data for clipboard
USHORT prepare_CLPBRDblock(void)
    {
    return readtextblock(tempbuffer,startrow,startcol,endrow,endcol);
    }

// read text from screen
USHORT readtextblock(PCHAR buffer,USHORT srow, USHORT scol, USHORT erow, USHORT ecol)
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
    return i+1;                                 // return length of block
    }

// set screen text block
void SetBlock(USHORT row, USHORT col)
    {
    USHORT endBlock = (row * ScreenCols) + col;
    USHORT presize,postsize,start;

    if(startBlock <= endBlock)                  // block end at/after start
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
    else                                        // block end before start
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

    blockptr = bufptr+presize;                  // set to beginning of block
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
    VioWrtNCell((PBYTE)&BlankCell,blocksize,startrow,startcol,VIOHDL);
    }

// reset screen to normal
void ResetBlock(void)
    {
    VioWrtNAttr(&normal_att,(ScreenCols*ScreenRows),0,0,VIOHDL);
    }

// initialize screen
void InitScreen(void)
    {
    SHORT i;

    VIOMODEINFO modeinfo;

    VioGetMode(&modeinfo,VIOHDL);               // get screen mode, rows, cols
    ScreenCols = modeinfo.col;
    ScreenRows = modeinfo.row;
    if(ScreenRows > BORDERROW-1)
        ScreenRows = BORDERROW-1;

    VioScrollUp(0,0,-1,-1,-1,(char *)&BlankCell,VIOHDL);    // clear screen
    VioSetCurPos(0,0,VIOHDL);                   // set cursor
    VioWrtNChar("Ä",ScreenCols,BORDERROW,0,VIOHDL); // draw border line
    for( i = 0; i < SCREEN_LABELS; i++)         // write labels
        VioWrtCharStr(screen_labels[i].text,strlen(screen_labels[i].text),
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
                                                // and toggle the color        
                    ButtonPaint(b,(b->state ? (BYTE)BUTTON_ON : b->attribute));
                    break;

                default:
                    break;
                }
            return;
            }
    }
    /******* end of main thread code **************************/

    /******* start of request thread code *********************/

// periodically wakes up and queries PMServer about Clipboard
void RequestThread(void)
    {
    USHORT count = 0;

    while(TRUE)
        {
        if(!clipboard_data)                     // if flag invalidated
                                                // ask if pasting possible
            MsgQSend(pmshdl,&pidinfo.pid,sizeof(PID),PMS_CLPBRD_QUERY);
        DosSleep(REQUESTTHREADSLEEPTIME);       // sleep a while
        count++;
        if(count >= MAXREQUESTCOUNT)            // if count exceeded limit
            {
            clipboard_data = FALSE;             // invalidate flag
            count = 0;                          // reset count
            }
        }
    }

    /******* end of request thread code ***********************/


    /********************** end of pmaccess.c ********************/

