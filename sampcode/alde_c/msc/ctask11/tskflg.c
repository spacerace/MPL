/*
   TSKFLG.C - CTask - Flag handling routines.

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
   create_flag - initialises flag.
*/

flagptr far create_flag (flagptr flg
#if (TSK_NAMEPAR)
                        ,byteptr name
#endif
                        )
{
#if (TSK_DYNAMIC)
   if (flg == NULL)
      {
      if ((flg = tsk_alloc (sizeof (flag))) == NULL)
         return NULL;
      flg->flags = F_TEMP;
      }
   else
      flg->flags = 0;
#endif

   flg->wait_set = flg->wait_clear = NULL;
   flg->state = 0;

#if (TSK_NAMED)
   tsk_add_name (&flg->name, name, TYP_FLAG, flg);
#endif

   return flg;
}

/*
   delete_flag - kills all processes waiting for flag
*/

void far delete_flag (flagptr flg)
{
   CRITICAL;

   C_ENTER;

   tsk_kill_queue (&(flg->wait_set));
   tsk_kill_queue (&(flg->wait_clear));
   flg->state = 0;
   C_LEAVE;

#if (TSK_NAMED)
   tsk_del_name (&flg->name);
#endif

#if (TSK_DYNAMIC)
   if (flg->flags & F_TEMP)
      tsk_free (flg);
#endif
}


/*
   wait_flag_set  - Wait until flag is != 0. If flag is != 0 on
                    entry, the task continues to run.
*/

int far wait_flag_set (flagptr flg, dword timeout)
{
   CRITICAL;

   C_ENTER;
   if (flg->state)
      {
      C_LEAVE;
      return 0;
      }
   tsk_current->retptr = NULL;
   tsk_wait (&flg->wait_set, timeout);
   return (int)tsk_current->retptr;
}


/*
   wait_flag_clear - Wait until flag is == 0. If flag is == 0 on
                     entry, the task continues to run.
*/

int far wait_flag_clear (flagptr flg, dword timeout)
{
   CRITICAL;

   C_ENTER;
   if (!flg->state)
      {
      C_LEAVE;
      return 0;
      }

   tsk_current->retptr = NULL;
   tsk_wait (&flg->wait_clear, timeout);
   return (int)tsk_current->retptr;
}


/*
   set_flag - Set flag to 1. If there are tasks waiting for the
              set state, all tasks in the queue are made eligible.
*/

void far set_flag (flagptr flg)
{
   CRITICAL;

   C_ENTER;
   flg->state = 1;

   while (flg->wait_set != NULL)
      flg->wait_set = tsk_runable (flg->wait_set);
   C_LEAVE;
}


/*
   clear_flag - Set flag to 0. If there are tasks waiting for the
                clear state, all tasks in the queue are made eligible.
*/

void far clear_flag (flagptr flg)
{
   CRITICAL;

   C_ENTER;
   flg->state = 0;

   while (flg->wait_clear != NULL)
      flg->wait_clear = tsk_runable (flg->wait_clear);

   C_LEAVE;
}


/*
   clear_flag_wait_set - Set flag to 0, then wait for set state.
*/

int far clear_flag_wait_set (flagptr flg, dword timeout)
{
   clear_flag (flg);
   return wait_flag_clear (flg, timeout);
}


/*
   check_flag - return current flag state.
*/

int far check_flag (flagptr flg)
{
   return flg->state;
}



