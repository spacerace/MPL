/*
   TSKLOCAL.H - CTask - Internal definitions and prototypes.

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany
*/

/*
   struct task_stack describes the contents of a tasks stack after creation.
   The first 10 words are the registers to be restored by the scheduler.
   Only the segment registers are significant initially.
   The next three words contain the function address plus the CPU flags
   setup as if an interrupt had occurred at the function's entry address.

   This setup is the same as the stack of an interrupted task after
   scheduling.

   The following two words contain a dummy return address, which points
   to the routine "killretn". Thus, if the task main function should ever
   return, the task is automatically killed. The last doubleword is
   used for the optional argument to the task.
*/

struct task_stack {
                  word     r_es;
                  word     r_ds;
                  word     r_di;
                  word     r_si;
                  word     r_bp;
                  word     r_sp;
                  word     r_bx;
                  word     r_dx;
                  word     r_cx;
                  word     r_ax;
                  funcptr  retn;
                  word     r_flags;
                  funcptr  dummyret;
                  farptr   arg;
                  };

extern tcbptr     _Near tsk_eligible;
extern tcbptr     _Near tsk_current;
extern tlinkptr   _Near tsk_timer;
extern byte       _Near tsk_preempt;
extern byte       _Near tsk_pretick;

#if (CLOCK_MSEC)
extern dword tsk_timeout (dword tout);
extern double tick_factor;
#else
#define tsk_timeout(tout)  tout
#endif

extern void far tsk_enqueue (tcbptr task, tqueptr que);
extern void far tsk_unqueue (tcbptr task);
extern void far tsk_enqtimer (tcbptr task, dword tout);
extern void far tsk_unqtimer (tcbptr task);

extern void far tsk_kill (tcbptr task);
extern void far tsk_kill_queue (tqueptr que);

extern void far tsk_install_timer (word divisor, word sys_ticks);
extern void far tsk_remove_timer (void);
extern void far tsk_install_dos (void);
extern void far tsk_remove_dos (void);
extern void far tsk_install_kbd (void);
extern void far tsk_remove_kbd (void);
extern void far tsk_chain_timer (void);

extern void far tsk_install_bios (void);
extern void far tsk_remove_bios (void);

#if (TURBO)
#define tsk_dseg()   _DS
#else
extern word far tsk_dseg (void);
#endif

extern word far tsk_flags (void);

extern tcbptr far tsk_runable (tcbptr task);
extern void far tsk_wakeup (tcbptr task);
extern void far tsk_wait (tqueptr que, dword timeout);


/*
   tsk_unqtimer 
      Marks a task for removal from the timer queue. 
*/

#define tsk_unqtimer(task) { if (task->timerq.tstate != TSTAT_IDLE) \
                                task->timerq.tstate = (byte) TSTAT_REMOVE; }

#if (TSK_NAMED)
extern namerec tsk_name_list;
extern void far tsk_add_name (nameptr elem, byteptr name, byte kind, 
                              farptr strucp);
extern void far tsk_del_name (nameptr elem);
#endif

