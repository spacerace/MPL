
/*
   Test program for checking CTask printer buffering.
*/

#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <ctype.h>

#include "tsk.h"
#include "prt.h"

#define PORT 0    /* LPT1 (mono card) */
#define POLL 0    /* Set to 1 to use polling */

#if (TSK_NAMED)
extern void snapshot (FILE *f);
#endif

#define STACKSIZE 2048

word _stklen = 3 * STACKSIZE;  /* For Turbo C: Two tasks + main Task Stack */

tcb tcb1, tcb2;
flag halt, ready;

byte xmtbuf [1024];
int endrun;

FILE *pfile;


void far task1 (void)
{
   int i;

   printf ("Task 1 started\n");

   /* Copy file into printer pipe. Using a larger file buffer would be more
      efficient, but this is only a demo.
   */

   while (!endrun && ((i = getc (pfile)) != EOF))
      prt_write (PORT, (byte)i, 0L);

   if (endrun)
      return;

   /* Wait for printer to complete output. */

   if (!prt_complete (PORT))
      puts ("Waiting for Printer");
   prt_wait_complete (PORT, 0L);

   /* Stop program */

   set_flag (&halt);
   set_flag (&ready);
}


/*
   Second task just for fun. Echoes keyboard characters, aborts printing
   when 'e' is entered. Also outputs snapshot dump on 'd'.
*/

void far task2 (void)
{
   int ch;

   printf ("Task 2 started\n");
   while (!endrun)
      {
      ch = t_read_key () & 0xff;
      putch (ch);
      ch = tolower (ch);
      endrun = (ch == 'e');
#if (TSK_NAMED)
      if (ch == 'd')
         snapshot (stdout);
#endif
      }

   set_flag (&halt);
}



void main (int argc, char **argv)
{
   char stack1 [STACKSIZE];
   char stack2 [STACKSIZE];

   if ((pfile = fopen (argv [1], "rb")) == NULL)
      {
      printf ("Error opening file '%s'\n", argv [1]);
      exit (1);
      }

   endrun = 0;
   install_tasker (0, 0);

   prt_install (PORT, POLL, PRI_STD, xmtbuf, sizeof (xmtbuf));

   create_task (&tcb1, task1, stack1, STACKSIZE, PRI_STD, NULL
#if (TSK_NAMEPAR)
                ,"TASK1"
#endif
                );
   create_task (&tcb2, task2, stack2, STACKSIZE, PRI_STD, NULL
#if (TSK_NAMEPAR)
                ,"TASK2"
#endif
                );
   create_flag (&halt
#if (TSK_NAMEPAR)
                ,"Halt"
#endif
                );
   create_flag (&ready
#if (TSK_NAMEPAR)
                ,"Ready"
#endif
                );

   start_task (&tcb1);
   start_task (&tcb2);

   preempt_on ();
   wait_flag_set (&halt, 0L);

   endrun = 1;
   puts ("******** Main Task *********");
   wait_flag_set (&ready, 0L);

   puts ("******** End Run *********");
   preempt_off ();
   prt_remove (PORT);
   remove_tasker ();

   fclose (pfile);
}


