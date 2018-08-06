/*
   TSKCNT.C - CTask - Counter handling routines.

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
   create_counter - initialises counter.
*/

counterptr far create_counter (counterptr cnt
#if (TSK_NAMEPAR)
                              ,byteptr name
#endif
                              )
{
#if (TSK_DYNAMIC)
   if (cnt == NULL)
      {
      if ((cnt = tsk_alloc (sizeof (counter))) == NULL)
         return NULL;
      cnt->flags = F_TEMP;
      }
   else
      cnt->flags = 0;
#endif

   cnt->wait_set = cnt->wait_clear = NULL;
   cnt->state = 0;

#if (TSK_NAMED)
   tsk_add_name (&cnt->name, name, TYP_COUNTER, cnt);
#endif

   return cnt;
}


/*
   delete_counter - kills all processes waiting for counter
*/

void far delete_counter (counterptr cnt)
{
   CRITICAL;

   C_ENTER;
   tsk_kill_queue (&(cnt->wait_set));
   tsk_kill_queue (&(cnt->wait_clear));
   cnt->state = 0L;
   C_LEAVE;

#if (TSK_NAMED)
   tsk_del_name (&cnt->name);
#endif

#if (TSK_DYNAMIC)
   if (cnt->flags & F_TEMP)
      tsk_free (cnt);
#endif
}


/*
   clear_counter  - Sets counter to zero. All tasks waiting for
                    Counter zero are made eligible.
*/

void far clear_counter (counterptr cnt)
{
   CRITICAL;

   C_ENTER;
   cnt->state = 0L;
   while (cnt->wait_clear != NULL)
      cnt->wait_clear = tsk_runable (cnt->wait_clear);
   C_LEAVE;
}

/*
   wait_counter_set  - Wait until counter is != 0. If counter is != 0 on
                       entry, the counter is decremented and the task
                       continues to run. If the counter is decremented to 
                       zero, tasks waiting for zero are made eligible.
*/

int far wait_counter_set (counterptr cnt, dword timeout)
{
   CRITICAL;

   C_ENTER;
   if (cnt->state)
      {
      if (!--cnt->state)
         while (cnt->wait_clear != NULL)
            cnt->wait_clear = tsk_runable (cnt->wait_clear);
      C_LEAVE;
      return 0;
      }

   tsk_current->retptr = NULL;
   tsk_wait (&cnt->wait_set, timeout);
   return (int)tsk_current->retptr;
}

/*
   wait_counter_clear - Wait until counter is == 0. If counter is == 0 on
                       entry, the task continues to run.
*/

int far wait_counter_clear (counterptr cnt, dword timeout)
{
   CRITICAL;

   C_ENTER;
   if (!cnt->state)
      {
      C_LEAVE;
      return 0;
      }

   tsk_current->retptr = NULL;
   tsk_wait (&cnt->wait_clear, timeout);
   return (int)tsk_current->retptr;
}


/*
   inc_counter - Increment counter. If there are tasks waiting for the
                 set state, the first task in the queue is made eligible.
*/

void far inc_counter (counterptr cnt)
{
   tcbptr curr;

   CRITICAL;

   C_ENTER;
   if ((curr = cnt->wait_set) == NULL)
      {
      cnt->state++;
      C_LEAVE;
      return;
      }
   cnt->wait_set = tsk_runable (curr);
   C_LEAVE;
}


/*
   check_counter - return current counter state.
*/

dword far check_counter (counterptr cnt)
{
   return cnt->state;
}



