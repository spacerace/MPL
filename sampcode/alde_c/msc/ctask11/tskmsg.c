/*
   TSKMSG.C - CTask - Message handling routines.

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany
*/

#include <stdio.h>

#include "tsk.h"
#include "tsklocal.h"


/*
   create_mailbox - initialises mailbox.
*/

mailboxptr far create_mailbox (mailboxptr box
#if (TSK_NAMEPAR)
                              ,byteptr name
#endif
                              )
{
#if (TSK_DYNAMIC)
   if (box == NULL)
      {
      if ((box = tsk_alloc (sizeof (mailbox))) == NULL)
         return NULL;
      box->flags = F_TEMP;
      }
   else
      box->flags = 0;
#endif

   box->waiting = NULL;
   box->mail_first = box->mail_last = NULL;

#if (TSK_NAMED)
   tsk_add_name (&box->name, name, TYP_MAILBOX, box);
#endif

   return box;
}


/*
   delete_mailbox - kills all processes waiting for mail
*/

void far delete_mailbox (mailboxptr box)
{
   CRITICAL;

   C_ENTER;
   tsk_kill_queue (&(box->waiting));
   box->mail_first = box->mail_last = NULL;
   C_LEAVE;

#if (TSK_NAMED)
   tsk_del_name (&box->name);
#endif

#if (TSK_DYNAMIC)
   if (box->flags & F_TEMP)
      tsk_free (box);
#endif
}


/*
   wait_mail - Wait until mail arrives. If there is mail in the box on
               entry, the first mail block is assigned to the caller,
               and the task continues to run.
*/

farptr far wait_mail (mailboxptr box, dword timeout)
{
   msgptr msg;
   CRITICAL;

   C_ENTER;
   if ((msg = box->mail_first) != NULL)
      {
      if ((box->mail_first = msg->next) == NULL)
         box->mail_last = NULL;
      C_LEAVE;
      return msg;
      }

   tsk_wait (&box->waiting, timeout);
   return tsk_current->retptr;
}

/*
   c_wait_mail - If there is mail in the box on entry, the first mail 
                 block is assigned to the caller, else an error is returned.
*/

farptr far c_wait_mail (mailboxptr box)
{
   msgptr msg;
   CRITICAL;

   C_ENTER;
   if ((msg = box->mail_first) != NULL)
      if ((box->mail_first = msg->next) == NULL)
         box->mail_last = NULL;
   C_LEAVE;
   return msg;
}


/*
   send_mail - Send a mail block to a mailbox. If there are tasks waiting
               for mail, the first waiting task is assigned the block and
               is made eligible.
*/

void far send_mail (mailboxptr box, farptr msg)
{
   tcbptr curr;
   CRITICAL;

   C_ENTER;
   if ((curr = box->waiting) == NULL)
      {
      if (box->mail_first == NULL)
         box->mail_first = (msgptr)msg;
      else
         box->mail_last->next = (msgptr)msg;
      ((msgptr)msg)->next = NULL;
      box->mail_last = (msgptr)msg;
      C_LEAVE;
      return;
      }
   box->waiting = tsk_runable (curr);
   curr->retptr = msg;
   C_LEAVE;
}


/*
   check_mailbox - returns TRUE if there is mail in the box.
*/

int far check_mailbox (mailboxptr box)
{
   return box->mail_first != NULL;
}


