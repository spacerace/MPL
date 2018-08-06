/*
   TSKMAIN.C - CTask - Main routines for task handling.

   CTask - a Multitasking Kernel for C

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany

   No rights reserved.

   Version 1.1  88-07-01

      Changes from V0.1: - Separated timeout handling from INT9 chaining
                           for better control over TSR response.
                         - Handling of timeout queue rewritten.
                         - More features for timeouts (can handle
                           flags, counters, function calls, and repetititve
                           timeouts).
                         - Support for dynamically allocated control blocks.
                         - Idle task removed.

*/

#include <stdio.h>

#include "tsk.h"
#include "tsklocal.h"

#define STACKSIZE 512

/*
   The task queues:
      All tasks using a timeout, either through "t_delay" or an event wait,
      are enqueued into the "tsk_timer" queue, using the "timerq" link.
      All tasks eligible for running are enqueued in "tsk_eligible".
      The tcb-address of the current running task is stored in "tsk_current".
*/

tlinkptr _Near tsk_timer;
tcbptr   _Near tsk_eligible;
tcbptr   _Near tsk_current;

/*
   System flags:
      tsk_preempt is zero if preemption is allowed.
                  Bit 0 is set if preemption has been disabled globally.
                  Bit 1 is set for temporary disabling preemption.
                  Temporary preemption is automatically removed by the
                  scheduler.

      tsk_pretick is nonzero if a schedule request from an interrupt handler
                  was rejected due to tsk_preempt nonzero. This allows
                  an immediate scheduling whenever tsk_preempt is set to 0.

      tsk_var_prior Can be set nonzero to enable variable priority.
                    Variable priority will increase the priority of
                    eligible tasks on each scheduler call while they 
                    are waiting to be executed, so that low priority 
                    tasks will slowly get to the head of the eligible
                    queue, getting a chance to be run. With variable
                    priority off, lower priority tasks will never be
                    executed while higher priority tasks are eligible.
                    
*/

byte _Near tsk_preempt;
byte _Near tsk_pretick;
byte _Near tsk_var_prior;

/* --------------------------------------------------------------------- */

/*
   The tcb's of the standard tasks.

      timer_tcb   is the tcb for the timer task.
                  This task waits for the tsk_timer_counter, which is
                  increased on every timer tick. It processes the entries
                  in the timeout queue.

      int9_tcb    is the tcb for the int9 chain task.
                  This task waits for the tsk_int9_counter, which is
                  increased on every system timer tick. It then chains to
                  the previous timer interrupt entry.

      main_tcb    is the "main" task which called "install_tasker". This
                  task has no separate stack, rather the stack on entry
                  to the scheduler is used.

*/

counter  _Near tsk_timer_counter;

local tcb timer_tcb;
local tcb main_tcb;

local char timer_stack [STACKSIZE];

#if (IBM)
counter  _Near tsk_int9_counter;
local tcb int9_tcb;
local char int9_stack [STACKSIZE];
#endif

#if (CLOCK_MSEC)
double tick_factor;
#endif

word _Near ticks_per_sec;

#if (TSK_NAMED)
namerec tsk_name_list;
#endif


/*
   Un-Install-Function pointers for the optional serial and printer 
   drivers. If ports are installed, the driver inserts the address
   of a remove-function here, to be called on removal of the main
   tasker.
*/

funcptr v24_remove_func = NULL;
funcptr prt_remove_func = NULL;


/* --------------------------------------------------------------------- */

#pragma check_stack(off)

/*
   Killretn kills the current active task. It is used internally, but
   can also be called from outside.
*/

void far killretn (void)
{
   tsk_cli ();
   tsk_kill (tsk_current);
   tsk_current = NULL;
   schedule ();
}

/*
   tsk_timer_action performs the necessary action when a timeout occurred.
*/

local void tsk_timer_action (tlinkptr elem)
{
   tcbptr task;
   byte st;

   switch (elem->tkind & 0x7f)
      {
      case TKIND_WAKE:
      case TKIND_TASK:  task = (tcbptr) elem->strucp;
                        st = task->state;

                        if (st == ST_WAITING || st == ST_DELAYED)
                           {
                           task->retptr = TTIMEOUT;
                           tsk_wakeup (task);
                           }
                        break;

      case TKIND_FLAG:  set_flag ((flagptr) elem->strucp);
                        break;

      case TKIND_COUNTER: inc_counter ((counterptr) elem->strucp);
                        break;

      case TKIND_PROC:  ((funcptr) elem->strucp)();
                        break;

      default:          break;
      }
}


/*
   The timer task handles all timeouts.
   It maintains a single timer queue, which contains elements of the
   "tlink" structure. No other task is allowed to manipulate this queue,
   except for the insertion of new elements at the queue head. This allows
   stepping through the queue with interrupts enabled.
   CAUTION: This assumes that the operation of loading a far pointer
            is indivisible!
*/

local void far timer (void)
{
   tlinkptr curr;
   tlinkptr last;
   byte state;
   CRITICAL;

   while (1)
      {
      wait_counter_set (&tsk_timer_counter, 0L);

      last = (tlinkptr) &tsk_timer;

      while ((curr = last->next) != NULL)
         {
         /* Enter critical section for access to state and timeout
            variables. The timer action is also critical.
         */
         C_ENTER;
         if ((state = curr->tstate) >= TSTAT_COUNTDOWN)
            if (!--curr->timeout)
               {
               if (state == TSTAT_COUNTDOWN)
                  state = (byte) TSTAT_REMOVE;
               else
                  curr->timeout = curr->reload;

               tsk_timer_action (curr);
               }
         if (state == (byte) TSTAT_REMOVE)
            {
            last->next = curr->next;
            curr->tstate = TSTAT_IDLE;

#if (TSK_DYNAMIC)
            if (curr->tkind & TKIND_TEMP)
               {
               if ((curr->tkind & 0x7f) == TKIND_TASK)
                  tsk_kill ((tcbptr) curr->strucp);
               else
                  tsk_free (curr);
               }
#endif
            curr = last;
            }
         C_LEAVE;
         last = curr;
         }
      }
}

/*
   int9 is the timer interrupt chain task.
*/

#if (IBM)

local void far int9 (void)
{
   while (1)
      {
      wait_counter_set (&tsk_int9_counter, 0L);
      tsk_chain_timer ();
      }
}

#endif

/* ---------------------------------------------------------------------- */

/*
   create_task
      Initialises a tcb. The task is in stopped state initially.
*/

tcbptr far create_task (tcbptr task,
                        funcptr func,
                        byteptr stack,
                        word stksz,
                        word prior,
                        farptr arg
#if (TSK_NAMEPAR)
                        ,byteptr name
#endif
                        )

{
   struct task_stack far *stk;

#if (TSK_DYNAMIC)
   if (task == NULL)
      {
      if ((task = (tcbptr) tsk_alloc (sizeof(tcb))) == NULL)
         return NULL;
      task->flags = F_TEMP;
      }
   else
      task->flags = 0;

   if (stack == NULL)
      {
      if ((stack = (byteptr) tsk_alloc (stksz)) == NULL)
         {
         if (task->flags & F_TEMP)
            tsk_free (task);
         return NULL;
         }
      task->flags |= F_STTEMP;
      }
#else
   task->flags = 0;
#endif

   stk = (struct task_stack far *)(stack + stksz - sizeof (struct task_stack));
   stk->r_ds = stk->r_es = tsk_dseg ();
   stk->r_bp = 0;
   stk->r_flags = tsk_flags ();
   stk->retn = func;
   stk->dummyret = killretn;
   stk->arg = arg;

   task->stkbot = stack;
   task->stack = (byteptr) stk;
   task->next = NULL;
   task->queue = NULL;
   task->state = ST_STOPPED;
   task->prior = task->initprior = prior;
   task->timerq.timeout = task->timerq.reload = 0;
   task->timerq.strucp = (farptr) task;
   task->timerq.tkind = TKIND_TASK;
   task->timerq.tstate = TSTAT_IDLE;

#if (TSK_NAMED)
   tsk_add_name (&task->name, name, TYP_TCB, task);
#endif

   return task;
}


/*
   kill_task
      Removes a task from the system.
      CAUTION: The task control block may *not* be immediately re-used
               if it was enqueued in the timer queue. Check for
               task->timerq.tstate == TSTAT_IDLE before modifying
               the tcb.
*/

void far kill_task (tcbptr task)
{
   byte st;
   CRITICAL;

   C_ENTER;
   if ((st = task->state) != ST_RUNNING)
      tsk_unqueue (task);

   task->queue = NULL;
   tsk_kill (task);
   if (st == ST_RUNNING)
      {
      tsk_current = NULL;
      schedule ();
      }
   C_LEAVE;
}


/*
   start_task
      Starts a stopped task. Returns -1 if the task was not stopped.
*/

int far start_task (tcbptr task)
{
   CRITICAL;

   if (task == NULL)
      task = &main_tcb;

   if (task->state == ST_STOPPED)
      {
      task->state = ST_ELIGIBLE;
      C_ENTER;
      tsk_enqueue (task, &tsk_eligible);
      C_LEAVE;
      return 0;
      }
   return -1;
}


/*
   wake_task
      Restarts a task waiting for an event or timeout. 
      Returns -1 if the task was not waiting or stopped.
*/

int far wake_task (tcbptr task)
{
   CRITICAL;

   if (task == NULL)
      task = &main_tcb;

   C_ENTER;
   if (task->state >= ST_ELIGIBLE)
      {
      C_LEAVE;
      return -1;
      }

   task->retptr = TWAKE;
   tsk_wakeup (task);
   C_LEAVE;
   return 0;
}



/*
   get_priority
      Returns the priority of a task.
*/

word far get_priority (tcbptr task)
{
   if (task == NULL)
      task = &main_tcb;

   return task->prior;
}


/*
   set_priority
      Changes the priority of a task. If the task is enqueued in a
      queue, its position in the queue is affected.
*/

void far set_priority (tcbptr task, word prior)
{
   tqueptr que;
   CRITICAL;

   if (task == NULL)
      task = &main_tcb;

   C_ENTER;
   task->prior = task->initprior = prior;

   if ((task->state != ST_RUNNING) && ((que = task->queue) != NULL))
      {
      tsk_unqueue (task);
      tsk_enqueue (task, que);
      }
   C_LEAVE;
}

/*
   set_task_flags
      Changes the user modifiable flags of the task.
*/

void far set_task_flags (tcbptr task, byte flags)
{
   CRITICAL;

   if (task == NULL)
      task = &main_tcb;

   C_ENTER;
   task->flags = (task->flags & FL_SYSM) | (flags & FL_USRM);
   C_LEAVE;
}


/* --------------------------------------------------------------------- */


/*
   t_delay
      delay the current task by "ticks" clock ticks.
      If ticks is zero, the task is stopped.
*/

int far t_delay (dword ticks)
{
   tsk_cli ();
   tsk_current->queue = NULL;
   if (ticks)
      {
      tsk_current->state = ST_DELAYED;
      tsk_enqtimer (tsk_current, ticks);
      }
   else
      tsk_current->state = ST_STOPPED;

   schedule ();
   return (int)tsk_current->retptr;
}


/* --------------------------------------------------------------------- */


/*
   install_tasker
      Installs the Ctask system. The internal tasks are created,
      the queues are initialised, and the interrupt handler installation
      routines are called. Task preemption is initially off.

      Handling of the speedup parameter is system dependent.
*/

void far install_tasker (byte varpri, int speedup)
{
   word divisor, sys_ticks;

   tsk_current = &main_tcb;
   tsk_eligible = NULL;
   tsk_timer = NULL;
   tsk_preempt = 1;
   tsk_pretick = 0;
   tsk_var_prior = varpri;

#if (TSK_NAMED)
   tsk_name_list.follow = tsk_name_list.prev = &tsk_name_list;
   tsk_name_list.nkind = 0;
   tsk_name_list.name [0] = 0;
#endif

   create_task (&timer_tcb, timer, timer_stack, STACKSIZE, PRI_TIMER, NULL
#if (TSK_NAMEPAR)
                , "-TIMER-"
#endif
                );
   create_counter (&tsk_timer_counter
#if (TSK_NAMEPAR)
                , "TIMCOUNT"
#endif
                );

#if (TSK_DYNAMIC)
   create_resource (&alloc_resource
#if (TSK_NAMEPAR)
                , "ALLOCRSC"
#endif
                );
#endif

   main_tcb.prior = main_tcb.initprior = PRI_TIMER - 1;
   main_tcb.queue = &tsk_eligible;
   main_tcb.flags = F_CRIT;
   main_tcb.state = ST_RUNNING;
   start_task (&timer_tcb);

#if (IBM)
   create_task (&int9_tcb, int9, int9_stack, STACKSIZE, PRI_INT9, NULL
#if (TSK_NAMEPAR)
                , "-INT9-"
#endif
                );
   create_counter (&tsk_int9_counter
#if (TSK_NAMEPAR)
                , "INT9CNT"
#endif
                );
   start_task (&int9_tcb);

   if (speedup <= 0 || speedup > 8)
      {
      divisor = 0;
      sys_ticks = 1;
      }
   else
      {
      divisor = 0x8000 >> (speedup - 1);
      sys_ticks = 1 << speedup;
      }

   ticks_per_sec = 18 * sys_ticks;  /* rough number only */

#if (CLOCK_MSEC)
   tick_factor = (65536.0 / (double)sys_ticks) / 1193.18;
#endif

   tsk_install_timer (divisor, sys_ticks);
   tsk_install_kbd ();
#endif
#if (AT_BIOS)
   tsk_install_bios ();
#endif

#if (DOS)
   tsk_install_dos ();
#endif
}


/*
   preempt_off
      Turns off task preemption (will stay off until explicitly enabled).
*/

void far preempt_off (void)
{
   tsk_preempt = 1;
}


/*
   preempt_on
      Resets permanent and temporary task preemption flag. If 
      preemption is pending, the scheduler is called.
*/

void far preempt_on (void)
{
   tsk_preempt = 0;
   tsk_cli ();
   if (tsk_pretick)
      schedule ();
   tsk_sti ();
}


/*
   remove_tasker
      Calls the interrupt handler un-install routines.
*/

void far remove_tasker (void)
{
   tsk_preempt = 0;

#if (AT_BIOS)
   tsk_remove_bios ();
#endif
#if (IBM)

   if (v24_remove_func != NULL)
      v24_remove_func ();
   if (prt_remove_func != NULL)
      prt_remove_func ();

   /* Allow all stored clock ticks to be processed */
   set_priority (&int9_tcb, 0xffff);
   while (check_counter (&tsk_int9_counter))
      schedule();

   tsk_remove_timer ();
   tsk_remove_kbd ();
#endif

#if (DOS)
   tsk_remove_dos ();
#endif
}


/*
   tsk_ena_preempt
      Resets temporary task preemption flag. If preemption is pending,
      the scheduler is called.
*/

void far tsk_ena_preempt (void)
{
   tsk_cli ();
   if (!(tsk_preempt &= ~2))
      if (tsk_pretick)
         schedule ();
   tsk_sti ();
}


/* --------------------------------------------------------------------- */

/*
   create_timer
      Creates a timer queue element. The element is inserted into
      the timeout queue.
*/

tlinkptr far create_timer (tlinkptr elem, dword tout, farptr strucp,
                           byte kind, byte rept)
{
   CRITICAL;

   if (kind <= TKIND_TASK || kind > TKIND_COUNTER)
      return NULL;

#if (TSK_DYNAMIC)

   if (elem == NULL)
      {
      if ((elem = tsk_alloc (sizeof (tlink))) == NULL)
         return NULL;
      kind |= TKIND_TEMP;
      }

#endif

   elem->tkind = kind;
   elem->strucp = strucp;
   elem->tstate = (byte)((rept) ? TSTAT_REPEAT : TSTAT_COUNTDOWN);
   elem->timeout = elem->reload = tsk_timeout(tout);

   C_ENTER;
   elem->next = tsk_timer;
   tsk_timer = elem;
   C_LEAVE;

   return elem;
}


/*
   delete_timer
      Deletes a timeout element.
*/

void far delete_timer (tlinkptr elem)
{
   CRITICAL;

   C_ENTER;
   if (elem->tstate != TSTAT_IDLE)
      {
      elem->tstate = (byte)TSTAT_REMOVE;
      C_LEAVE;
      return;
      }
   C_LEAVE;

#if (TSK_DYNAMIC)
   if (elem->tkind & TKIND_TEMP)
      tsk_free (elem);
#endif
}


/*
   change_timer
      Changes the timeout and/or repeat-flag in a timer element.
      If the timer was idle, it is inserted into the timeout queue.

      If 0 is passed as timeout, the element is removed from the
      timeout queue (same as delete_timer).

      This routine should *not* be used for dynamically allocated
      timer elements.
*/

void far change_timer (tlinkptr elem, dword tout, byte rept)
{
   byte st;
   CRITICAL;

   if (!tout)
      {
      delete_timer (elem);
      return;
      }

   C_ENTER;
   elem->timeout = elem->reload = tsk_timeout(tout);
   st = elem->tstate;
   elem->tstate = (byte)((rept) ? TSTAT_REPEAT : TSTAT_COUNTDOWN);

   if (st == TSTAT_IDLE)
      {
      elem->next = tsk_timer;
      tsk_timer = elem;
      }
   C_LEAVE;
}

