
/*
   Test program for checking basic CTask functions.

   Note that strange effects, like duplicated or incomplete output,
   may be due to the concurrent usage of "printf". This is not trapped
   here, since the effects are not fatal, but in real applications,
   resources should be used to channel console output access.
*/

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>

#include "tsk.h"

#if (TSK_NAMED)
extern void snapshot (FILE *f);
#endif

#define STACKSIZE 2048

word _stklen = 5 * STACKSIZE;  /* For Turbo C: Four tasks + main Task Stack */

typedef struct {
               farptr xx;
               char str [20];
               } message;

tcb tcb1, tcb2, tcb3, tcb4;
mailbox box;
message msg;
flag halt;
pipe pip;
buffer buf;

byte pipbuf [10];
word bufbuf [40];

int endrun;


/*
   Task1 sends a mail to Task3, and waits for a response in the buffer.
   The response is then displayed.
   Task1 will stop while the halt flag is set.
*/

void far task1 (void)
{
   char str [20];

   printf ("Task 1 started\n");
   while (!endrun)
      {
      wait_flag_clear (&halt, 0L);

      t_delay (5);
      putch ('1');
      strcpy (msg.str, "From T1");
      send_mail (&box, &msg);

      read_buffer (&buf, str, 20, 0L);
      printf ("Task 1 read buf: <%s>\n", str);
      }
}

/*
   Task2 reads the keyboard. If a character has been read, it is passed
   to Task4 via a pipe. Entering 'h' will set the halt flag (stopping Task1),
   entering 'c' will clear the halt flag.
   'e' stops the program.
*/

void far task2 (void)
{
   int ch;

   printf ("Task 2 started\n");
   while (!endrun)
      {
      ch = t_read_key () & 0xff;
      switch (tolower (ch))
         {
         case 'h':   set_flag (&halt);
                     break;
         case 'c':   clear_flag (&halt);
                     break;
         case 'e':   wake_task (NULL);
                     break;
#if (TSK_NAMED)
         case 'd':   snapshot (stdout);
                     break;
#endif
         }

      if (!endrun)
         write_pipe (&pip, ch, 0L);
      putch ('2');
      }
}


/*
   Task3 waits for mail, then sends it back through a buffer.
*/

void far task3 (void)
{
   message far *m;

   printf ("Task 3 started\n");
   while (!endrun)
      {
      m = wait_mail (&box, 0L);
      printf ("Task 3 received <%Fs>\n", m->str);

      m->str [6] = '3';
      write_buffer (&buf, m->str, 7, 0L);
      }
}


/*
   Task4 waits for a character in the pipe and displays it. To make
   things livelier, it uses a timeout while waiting, and will display
   faces when the timeout occurred before the character.
*/

void far task4 (void)
{
   int ch;

   printf ("Task 4 started\n");
   while (!endrun)
      {
      ch = read_pipe (&pip, 10L);
      if (ch < 0)
         putch (0x02);
      else
         printf ("Task 4 got <%c>\n", ch);
      }
}


main ()
{
   char stack1 [STACKSIZE];
   char stack2 [STACKSIZE];
   char stack3 [STACKSIZE];
   char stack4 [STACKSIZE];

   endrun = 0;
   install_tasker (0, 0);
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
   create_task (&tcb3, task3, stack3, STACKSIZE, PRI_STD, NULL
#if (TSK_NAMEPAR)
                ,"TASK3"
#endif
                );
   create_task (&tcb4, task4, stack4, STACKSIZE, PRI_STD, NULL
#if (TSK_NAMEPAR)
                ,"TASK4"
#endif
                );
   create_mailbox (&box
#if (TSK_NAMEPAR)
                ,"Mailbox"
#endif
                );
   create_flag (&halt
#if (TSK_NAMEPAR)
                ,"Halt"
#endif
                );
   create_pipe (&pip, pipbuf, sizeof (pipbuf)
#if (TSK_NAMEPAR)
                ,"Pipe"
#endif
                );
   create_buffer (&buf, bufbuf, sizeof (bufbuf)
#if (TSK_NAMEPAR)
                ,"Buffer"
#endif
                );

   start_task (&tcb1);
   start_task (&tcb2);
   start_task (&tcb3);
   start_task (&tcb4);
   preempt_on ();

   t_delay (0L);

   endrun = 1;
   puts ("******** Main Task *********");

   set_priority (NULL, 10);
   delete_mailbox (&box);
   delete_pipe (&pip);
   delete_buffer (&buf);
   delete_flag (&halt);

   schedule ();

   preempt_off ();
/*   remove_tasker (); */
   puts ("******** End Run *********");
}
