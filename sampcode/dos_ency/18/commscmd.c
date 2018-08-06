/***********************************************************************
*                                                                      *
*  COMMSCMD                                                            *
*                                                                      *
*  This routine controls the COMMSCOP program that has been in-        *
*  stalled as a resident routine.  The operation performed is de-      *
*  termined by the command line.  The COMMSCMD program is invoked      *
*  as follows:                                                         *
*                                                                      *
*                COMMSCMD [[cmd][ port]]                               *
*                                                                      *
*  where cmd is the command to be executed                             *
*            STOP   -- stop trace                                      *
*            START  -- flush trace buffer and start trace              *
*            RESUME -- resume a stopped trace                          *
*        port is the COMM port to be traced (1=COM1, 2=COM2, etc.)     *
*                                                                      *
*  If cmd is omitted, STOP is assumed.  If port is omitted, 1 is       *
*  assumed.                                                            *
*                                                                      *
***********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#define COMMCMD 0x60

main(argc, argv)
int argc;
char *argv[];
{
    int cmd, port, result;
    static char commands[3] [10] = {"STOPPED", "STARTED", "RESUMED"};
    union REGS inregs, outregs;

    cmd = 0;
    port = 0;

    if (argc > 1)
        {
        if (0 == stricmp(argv[1], "STOP"))
            cmd = 0;
        else if (0 == stricmp(argv[1], "START"))
            cmd = 1;
        else if (0 == stricmp(argv[1], "RESUME"))
            cmd = 2;
        }

    if (argc == 3)
        {
        port = atoi(argv[2]);
        if (port > 0)
            port = port - 1;
        }

    inregs.h.ah = cmd;
    inregs.x.dx = port;
    result = int86(COMMCMD, &inregs, &outregs);


    printf("\nCommunications tracing %s for port COM%1d:\n",
            commands[cmd], port + 1);
}
