/*
    MOUDEMO.C   Simple demo of the OS/2 mouse API.
    Copyright (C) 1988 Ray Duncan

    Compile with:  C> cl moudemo.c

    Usage is:  C> moudemo
*/

#include <stdio.h>

#define API unsigned extern far pascal

API MouReadEventQue(void far *, unsigned far *, unsigned);
API MouOpen(void far *, unsigned far *);
API MouClose(unsigned);
API MouDrawPtr(unsigned);
API VioScrollUp(unsigned, unsigned, unsigned, unsigned, 
                unsigned, char far *, unsigned);
API VioWrtCharStr(void far *, unsigned, unsigned, unsigned, unsigned);

struct _MouEventInfo {  unsigned Flags;
                        unsigned long Timestamp;
                        unsigned Row;
                        unsigned Col;
                     }  MouEvent ;

main(int argc, char *argv[])
{
    char OutStr[40];                    /* for output formatting */ 
    
    unsigned Cell = 0x0720;             /* ASCII space and
                                           normal attribute */

    unsigned MouHandle;                 /* mouse logical handle */
    unsigned Status;                    /* returned from API */
    int WaitOption = 1;                 /* 1 = block for event,
                                           0 = do not block */

    Status = MouOpen(0L, &MouHandle);   /* open mouse device */

    if(Status)                          /* exit if no mouse */
    {   
        printf("\nMouOpen failed.\n");
        exit(1);
    }
                                        /* clear the screen */  
    VioScrollUp(0, 0, -1, -1, -1, &(char)Cell, 0);

    puts("Press Both Mouse Buttons To Exit");

    MouDrawPtr(MouHandle);              /* display mouse cursor */

    do                                  
    {                                   /* format mouse position */    
        sprintf(OutStr, "X=%2d Y=%2d", MouEvent.Col, MouEvent.Row);

                                        /* display mouse position */
        VioWrtCharStr(OutStr, strlen(OutStr), 0, 0, 0);

                                        /* wait for a mouse event */
        MouReadEventQue(&MouEvent, &WaitOption, MouHandle);
        
                                        /* exit if both buttons down */
    } while((MouEvent.Flags & 0x14) != 0x14) ;

    MouClose(MouHandle);                /* release mouse handle */

    puts("Have a Mice Day!");           /* hail & farewell */
}
