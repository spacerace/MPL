/*
    SIGNAL.C  Example signal handler in Microsoft C.
              Registers a signal handler then goes to
              sleep for 10 seconds.  During the sleep
              you can enter ^C and the signal handler
              will display a message.

    Copyright (C) 1988 Ray Duncan

    Compile:    CL SIGNAL.C

    Usage:      SIGNAL
*/

#include <stdio.h>

#define SIGINTR 1                       /* Ctrl-C signal number */

void far pascal handler(unsigned, int); /* function prototypes */       

#define API unsigned extern far pascal

API DosSetSigHandler(void (pascal far *)(unsigned, int), 
                     unsigned long far *, unsigned far *, int, int);
API DosSleep(unsigned long);

main()
{
    unsigned long prevhandler;          /* previous handler address */
    unsigned prevaction;                /* previous handler action */
    int i;                              /* scratch variable */

                                        /* register Ctrl-C handler */
    DosSetSigHandler(handler, &prevhandler, &prevaction, 2, SIGINTR);

    fprintf(stderr,"\nSignal handler registered.");
    fprintf(stderr,"\nEnter Ctrl-C to demonstrate handler.\n");

    for(i=0; i<10; i++)                 /* sleep for 10 seconds */
        DosSleep(1000L);    
}

void far pascal handler(unsigned sigarg, int signum)
{
    unsigned long dummy1;               /* scratch variables */
    unsigned dummy2;

                                        /* reset signal */
    DosSetSigHandler(handler, &dummy1, &dummy2, 4, SIGINTR);

                                        /* show something happened */
    fprintf(stderr,"\nSignal Received!\n");
}

