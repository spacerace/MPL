/* kbd.c RHS 9/30/89
 *
 * keyboard thread code for PMAccess
 */

#define INCL_SUB
#define INCL_DOS
#include<os2.h>
#include"kbd.h"
#include"kbddefs.h"
#include"msgq.h"
#include"button.h"
#include"msgs.h"

extern long KbdSem;
extern PCHAR mainmsgqueue;
extern BUTTON buttonlist[];

USHORT AcceleratorPressed(unsigned char key);

void KbdThread(void)
    {
    KBDINFO     kbdinfo;
    KBDKEYINFO  KbdKeyInfo;
    HKBD        KbdHandle = 0;
    HQUEUE      qhandle;
    USHORT      event;

    MsgQOpen(&qhandle,mainmsgqueue);

    KbdFlushBuffer(KbdHandle);                  // flush keyboard buffer    
    KbdGetStatus(&kbdinfo,KbdHandle);           // get keyboard status      
    kbdinfo.fsMask &= ~COOKED;                  // turn off COOKED bit    
    kbdinfo.fsMask |= RAW;                      // turn on RAW bit            
    KbdSetStatus(&kbdinfo,KbdHandle);           // set the keyboard status    
    DosSemClear(&KbdSem);                       // notify main thread

    while(TRUE)
        {
        KbdCharIn(&KbdKeyInfo,IO_WAIT,KbdHandle);// get a character            
        if(KbdKeyInfo.chChar)                    // if Ascii code                
            MsgQSend(qhandle,&KbdKeyInfo,sizeof(KbdKeyInfo),MSG_CHAR);
        else if(event = AcceleratorPressed(KbdKeyInfo.chScan))  // Accelerator?
            MsgQSend(qhandle,NULL,0,event);      // if so, pass it on            
        else
            MsgQSend(qhandle,&KbdKeyInfo,sizeof(KbdKeyInfo),
                SCANCODE(KbdKeyInfo.chScan));
        DosSleep(32L);
        }
    }

USHORT AcceleratorPressed(unsigned char key)
    {
    BUTTON *b = &buttonlist[0];

    for( ; b->text; b++)
        if(key == (unsigned char)b->accelerator)
            return MOUSECODE(b->left_button_val);
    return 0;
    }

    /********** end of keyboard thread code *******************/



