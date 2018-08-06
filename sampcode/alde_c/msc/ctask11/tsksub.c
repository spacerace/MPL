/*
   CTask Subroutines

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany

   Version 1.1  88-07-01

      Changes from V0.1: - Handling of timeout queue rewritten
                         - Timeouts in milliseconds (optional)
                         - Support for dynamically allocated control blocks.
*/

#include <stdio.h>

#include "tsk.h"
#include "tsklocal.h"

/*
   tsk_enqueue  inserts a task into a queue based on priority.
*/

void far tsk_enqueue (tcbptr task, tqueptr que)
{
   tcbptr last, curr;

   last = NULL;
   curr = *que;

   while (curr != NULL && curr->prior >= task->prior)
      {
      last = curr;
      curr = curr->next;
      }
   task->next = curr;
   if (last == NULL)
      *que = task;
   else
      last->next = task;
   task->queue = que;
}


/*
   tsk_unqueue 
      Removes a task from somewhere in the middle of a queue. It is only
      used when stopping or prematurely waking a task, since in all other 
      circumstances a task is only removed from the head of a queue.
*/

void far tsk_unqueue (tcbptr task)
{
   tcbptr last, curr;

   if (task->state == ST_RUNNING || task->queue == NULL)
      return;

   last = NULL;
   curr = *task->queue;

   while (curr != task)
      {
      if (curr == NULL)
         return;
      last = curr;
      curr = curr->next;
      }
   if (last == NULL)
      *task->queue = curr->next;
   else
      last->next = curr->next;
   task->queue = NULL;
}


/*
   tsk_enqtimer inserts a task into the timer queue.
*/

void far tsk_enqtimer (tcbptr task, dword tout)
{
   tlinkptr curr;

   if (tout == 0)
      return;

   /*
      Tasks are not sorted in the timer queue, so the task is inserted
      at the queue head. The timer task has to step through all tasks
      in the queue to cont down the timeout, so sorting would not bring
      any advantages.
   */

   curr = &task->timerq;
   curr->timeout = tsk_timeout(tout);
   if (curr->tstate == TSTAT_IDLE)
      {
      curr->next = tsk_timer;
      tsk_timer = curr;
      }
   curr->tstate = TSTAT_COUNTDOWN;
}


/*
   tsk_runable
      make a task eligible for running. The task is removed from the
      timer queue and enqueued in the eligible queue. The old "next"
      pointer of the tcb is returned. This assumes that the task
      is removed from the head of a queue.
*/

tcbptr far tsk_runable (tcbptr task)
{
   tcbptr nxt;

   nxt = task->next;
   task->state = ST_ELIGIBLE;
   tsk_unqtimer (task);
   tsk_enqueue (task, &tsk_eligible);
   return nxt;
}


/*
   tsk_wakeup
      make a task eligible for running. The task is removed from the
      timer queue and enqueued in the eligible queue. 
      This routine assumes that the task is removed from the middle of
      a queue.
*/

void far tsk_wakeup (tcbptr task)
{
   task->state = ST_ELIGIBLE;
   tsk_unqueue (task);
   tsk_unqtimer (task);
   tsk_enqueue (task, &tsk_eligible);
}


/*
   tsk_wait - put current running task in wait state.
              Note that the task is NOT enqueued in the respective queue
              here, this is done by the scheduler based on the queue head
              pointer. Only the timeout queue is affected directly.
*/

void far tsk_wait (tqueptr que, dword timeout)
{
   tsk_current->state = ST_WAITING;
   tsk_current->queue = que;
   tsk_enqtimer (tsk_current, timeout);
   schedule ();
}


/*
   tsk_kill - mark task as killed.
*/

void far tsk_kill (tcbptr task)
{
#if (TSK_NAMED)
   tsk_del_name (&task->name);
#endif

   task->state = ST_KILLED;
   if (task->timerq.tstate != TSTAT_IDLE)
      {
      task->timerq.tstate = (byte) TSTAT_REMOVE;
#if (TSK_DYNAMIC)
      if (task->flags & F_TEMP)
         task->timerq.tkind |= TKIND_TEMP;
      }
   else 
      {
      if (task->flags & F_STTEMP)
         tsk_free (task->stack);
      if (task->flags & F_TEMP)
         tsk_free (task);
      }
#else
      }
#endif
}

/*
   tsk_kill_queue
      Removes all tasks from a queue. For internal use only, critical
      section assumed entered.
*/

void far tsk_kill_queue (tqueptr que)
{
   tcbptr curr;

   for (curr = *que; curr != NULL; curr = curr->next)
      {
      tsk_unqtimer (curr);
      tsk_kill (curr);
      }
   *que = NULL;
}


#if (CLOCK_MSEC)

dword tsk_timeout (dword tout)
{
   dword t;

   t = (dword) (((double)tout / tick_factor) + 0.5);
   return (t) ? t : 1; 
}

#endif

#if (TSK_NAMED)

/*
   tsk_add_name
      Initialise name-list element and insert it at the end of the list.
      NOTE: no check is made for duplicate names; names are not sorted.
*/

void far tsk_add_name (nameptr elem, byteptr name, byte kind, farptr strucp)
{
   byteptr n;
   int i;
   CRITICAL;

   elem->nkind = kind;
   elem->strucp = strucp;
   n = elem->name;
   if (name != NULL)
      for (i = 0; i < 8; i++)
         if (!(*n++ = *name++))
            break;
   *n = 0;

   C_ENTER;
   elem->prev = tsk_name_list.prev;
   tsk_name_list.prev = elem;
   elem->follow = &tsk_name_list;
   elem->prev->follow = elem;
   C_LEAVE;
}


/*
   tsk_del_name
      delete name-element from the name-list.
*/

void far tsk_del_name (nameptr elem)
{
   CRITICAL;

   C_ENTER;
   elem->follow->prev = elem->prev;
   elem->prev->follow = elem->follow;
   C_LEAVE;
}


/*
   find_name
      find structure, given name and type. 
      If type is zero, the first name-element matching the name is returned.
      If type is nonzero, the first structure matching the name and type is
      returned.
*/

local int tsk_streq (byteptr n1, byteptr n2)
{
   while (*n1 && *n1 == *n2)
      {
      n1++;
      n2++;
      }
   return *n1 == *n2; 
}

farptr far find_name (byteptr name, byte kind)
{
   nameptr curr;

   for (curr = tsk_name_list.follow; curr->nkind; curr = curr->follow)
      if (!kind || curr->nkind == kind)
         if (tsk_streq (name, curr->name))
            return (kind) ? curr->strucp : curr;

   return NULL;
}
#endif

