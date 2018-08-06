/*
**          TEST PROGRAM FOR INTERRUPT HANDLER FUNCTIONS
**  This program demonstrates the installable interrupt
**  service routines contained in the functions ticker, ctlbrk(),
**  and in SMDLx.LIB.
**  The ticker routine installs on
**  interrupt 1CH, and chains after completion to whatever
**  routine may have already been hung on that interrupt.
**  Ticker is not a C function as such, but a stand-alone assembly
**  language routine which is installed by installtick() and removed
**  by removetick().
**  The ctlbrk() routine is used to exit the program and cleanup
**  the interrupt vector table upon exit.  It installs upon int 23H.
**
**  Copyright 1986, S.E. Margison
**
*/

#include <stdio.h>
#include <dos.h>
#include <smdefs.h>

/* first, we must declare our control-break handler routine.
** this routine could do almost anything, but we will just
** set a flag that the main program can test, and print a message.
** Note that this should be declared BEFORE main() routine!
*/

int killit;

handler() {    /* handler cannot return a value, so declare void */
   killit = YES;
   puts("Control-C received; exiting program");
   return(0);       /* return 0 to continue program */
   }

int count;       /* this is the variable which ticker will manipulate */
extern void installtick(), removetick();

main() {
   int i;

/* BEFORE we change the interrupt vector table, we MUST
** install our control-C trap.  If not, it would be possible
** for a fast typist to control-C out of the program after
** we mess up the vector table and before we grab the control-C
** handler vector.   Result:  Well, just turn off the power switch
** and start all over again!!!
*/

   ctlbrk(&handler);       /* install our trap */
   killit = NO;            /* and clear our trap flag */



/* Use installtick() to pass the desired variable address to the ticker
** process and install it upon the interrupt.
*/

   installtick(&count);



/* this loop demonstrates the use of the ticker function.  As long as
** the count variable is not 0, it will be decremented 18.21 times
** per second.  ticker tests the value for zero and if it is,
** does not decrement it again.  This makes it unnecessary to try to
** "trap" count exactly at zero before the next interrupt occurs.
** To use ticker, place a value in count (18 = 1 second more or less).
** Then, check it every so often to see if it is zero (or whatever
** your heart desires).
*/

   i = 0;
   while(!killit) {          /* abort when control-C trap sets this flag */
      count = 18;            /* set a value of 1 second */
      while(count != 0);     /* loop until timed out */
      printf("This is loop #%d\n", i++);  /* do something useful here */
      }                      /* and go again */

/* if we are here, then our control-C trap worked! */

/* now, we have to clean up after ourselves.  It is necessary
** to restore the original vector table contents.  Alternate
** option is to turn off the power!!
*/

/* first, remove the ticker from the interrupt table.
** WARNING:  FAILURE TO REMOVE TICKER BEFORE EXITING PROGRAM
**           WILL DEFINITELY (as in positively!) CRASH THE
**           SYSTEM ALMOST IMMEDIATELY!!!!!!
*/
	removetick();

/* finally, let's restore the original control-C handler by uninstalling
** our own.  A NULL parameter does the job!
** Yes, DOS restores the original vector upon exit from our program, but
** this shows that we can also do that ourselves.
*/
   ctlbrk(0);

   }

/* If using ticker, the following ctlbrk handler would be sufficient
** if it was desired to abort the program cleanly when a control-break
** was pressed:
   handler {
      removetick();
      return(1);
      }
*/

