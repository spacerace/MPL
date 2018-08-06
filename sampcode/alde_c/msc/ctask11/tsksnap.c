/*
   TSKSNAP.C - CTask State Snaphot

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany

   This file is new with Version 1.1
*/

#include "tsk.h"
#include "tsklocal.h"

#if (TSK_NAMED)

local char *sn_state [] = { "*???*", "Stop", "Delay", "Wait", "Elig", "Run" };
local char *sn_kind [] = { "*Undef*", "Task", "Flag", "Resource", "Counter",
                           "Mailbox", "Pipe", "Wpipe", "Buffer" };

local int sn_wlist (FILE *f, char *str, tqueptr list, int col)
{
   tcbptr curr;

   if ((curr = *list) == NULL)
      return col;
   if (col)
      fprintf (f, "\n%51c", ' ');
   col = 52;
   fprintf (f, "%6s(%Fp):  ", str, list);
   while (curr != NULL)
      {
      if (col >= 60)
         {
         fprintf (f, "\n51c", ' ');
         col = 52;
         }
      fprintf (f, "%-8s(%Fp) ", curr->name.name, curr);
      col += 20;
      curr = curr->next;
      }
   return col;
}

local void dump_events (FILE *f)
{
   nameptr curr;
   int col;
   flagptr fl;
   counterptr ct;
   resourceptr rs;
   mailboxptr mb;
   pipeptr pp;
   wpipeptr wp;
   bufferptr bp;

   fprintf (f, "List of Events:\n");
   /*           1234567890123456789012345678901234567890123456789012345
                12345678   12345678  12345678  123456(123456789):        */
   fprintf (f, "Name       Type         State     Waitfor(Queue)   Tasks\n");

   for (curr = tsk_name_list.follow; curr->nkind; curr = curr->follow)
      if (curr->nkind != TYP_TCB)
         {
         fprintf (f, "%-8s   %-8s  ", curr->name, sn_kind [curr->nkind]);
         switch (curr->nkind)
            {
            case TYP_FLAG: fl = (flagptr) curr->strucp;
                           fprintf (f, "%8s  ", (fl->state) ? "Set" : "Clear");
                           col = sn_wlist (f, "Set", &fl->wait_set, 0);
                           sn_wlist (f, "Clear", &fl->wait_clear, col);
                           break;

            case TYP_COUNTER: ct = (counterptr) curr->strucp;
                           fprintf (f, "%8ld  ", ct->state);
                           col = sn_wlist (f, "Set", &ct->wait_set, 0);
                           sn_wlist (f, "Clear", &ct->wait_clear, col);
                           break;

            case TYP_RESOURCE: rs = (resourceptr) curr->strucp;
                           fprintf (f, "%8s  ", (rs->state) ? "Free" : "Busy");
                           col = sn_wlist (f, "Owner", &rs->owner, 0);
                           sn_wlist (f, "Wait", &rs->waiting, col);
                           break;

            case TYP_MAILBOX: mb = (mailboxptr) curr->strucp;
                           fprintf (f, "%8s  ", (mb->mail_first == NULL)
                                    ? "Empty" : "Mail");
                           sn_wlist (f, "Mail", &mb->waiting, 0);
                           break;

            case TYP_PIPE: pp = (pipeptr) curr->strucp;
                           fprintf (f, "%8u  ", pp->filled);
                           col = sn_wlist (f, "Read", &pp->wait_read, 0);
                           col = sn_wlist (f, "Write", &pp->wait_write, col);
                           sn_wlist (f, "Clear", &pp->wait_clear, col);
                           break;

            case TYP_WPIPE: wp = (wpipeptr) curr->strucp;
                           fprintf (f, "%8u  ", wp->filled);
                           col = sn_wlist (f, "Read", &wp->wait_read, 0);
                           col = sn_wlist (f, "Write", &wp->wait_write, col);
                           sn_wlist (f, "Clear", &wp->wait_clear, col);
                           break;

            case TYP_BUFFER: bp = (bufferptr) curr->strucp;
                           fprintf (f, "%8u  ", bp->msgcnt);
                           col = sn_wlist (f, "Read", &bp->buf_read.waiting, 0);
                           sn_wlist (f, "Write", &bp->buf_write.waiting, col);
                           break;

            default:       fprintf (f, "*Invalid Type %02x", curr->nkind);
                           break;
            }
         fprintf (f, "\n");
         }
   fprintf (f, "\n");
}


local void dump_tasks (FILE *f)
{
   nameptr curr;
   tcbptr task;
   fprintf (f, "\nTask List:\n");
   fprintf (f, "Name     State Queue      Timeout TCB-addr  Stackptr  Stackbot  Instrptr  Prior\n");

   for (curr = tsk_name_list.follow; curr->nkind; curr = curr->follow)
      if (curr->nkind == TYP_TCB)
         {
         task = (tcbptr) curr->strucp;

         fprintf (f, "%-8s %-5s %Fp %8ld %Fp %Fp %Fp %Fp %5u\n",
                 curr->name, 
                 sn_state [(task->state <= 5) 
                           ? task->state : 0],
                 task->queue,
                 task->timerq.timeout, 
                 task, 
                 task->stack, 
                 task->stkbot,
                 ((struct task_stack *)task->stack)->retn, 
                 task->prior);
         }
   fprintf (f, "\n");
}


void snapshot (FILE *f)
{
   word oldpri;

   oldpri = get_priority (tsk_current);
   set_priority (tsk_current, 0xffff);

   dump_tasks (f);
   dump_events (f);

   set_priority (tsk_current, oldpri);
}

#endif

