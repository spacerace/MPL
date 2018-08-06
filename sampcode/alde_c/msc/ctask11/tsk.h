/*
   TSK.H - CTask - Type definitions and global routine prototypes.

   Public Domain Software written by
      Thomas Wagner
      Patschkauer Weg 31
      D-1000 Berlin 33
      West Germany
*/


#include "tskconf.h"
#include <stdio.h>

#if defined(__TURBOC__)
#include <dos.h>
#define _Near
#define TURBO  1
#define MSC    0
#else
#include <dos.h>
#include <conio.h>
#define _Near near
#define TURBO  0
#define MSC    1
#endif

#define local static      /* Comment out the "static" for debugging */

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef void (cdecl far *funcptr)();
typedef void far *farptr;
typedef byte far *byteptr;
typedef word far *wordptr;

#define TTIMEOUT ((farptr) -1L)
#define TWAKE    ((farptr) -2L)

/* Task states */

#define	ST_KILLED   0
#define  ST_STOPPED  1
#define  ST_DELAYED  2
#define  ST_WAITING  3
#define  ST_ELIGIBLE 4
#define  ST_RUNNING  5

/* Task flags */

#define  F_TEMP   0x80     /* Task is temporary, free on kill */
#define  F_STTEMP 0x40     /* Task stack is temporary, free on kill */
#define  F_CRIT   0x01     /* Task is critical, may not be preempted */

#define  FL_SYSM  0xf0     /* Mask for system flags */
#define  FL_USRM  0x0f     /* Mask for user flags */

/* Timer queue element states */

#define  TSTAT_REMOVE      -1    /* Remove element from queue */
#define  TSTAT_IDLE        0     /* Not in queue */
#define  TSTAT_COUNTDOWN   1     /* Count down timeout, then remove */
#define  TSTAT_REPEAT      2     /* Count down, reload when done */

/* Timer queue element control structure pointer kinds */

#define  TKIND_TASK        1     /* tcbptr, Wakeup associated task */
#define  TKIND_WAKE        2     /* tcbptr, but not same task */
#define  TKIND_PROC        3     /* call function */
#define  TKIND_FLAG        4     /* flagptr, set flag */
#define  TKIND_COUNTER     5     /* counterptr, increment counter */

#define  TKIND_TEMP        0x80  /* Hi bit set means temporary element */

/* Name link structure types */

#define  TYP_TCB        1
#define  TYP_FLAG       2
#define  TYP_RESOURCE   3
#define  TYP_COUNTER    4
#define  TYP_MAILBOX    5
#define  TYP_PIPE       6
#define  TYP_WPIPE      7
#define  TYP_BUFFER     8

#define  NAMELENGTH     9  /* For structure names: 8 bytes + zero */

typedef struct name_rec far *nameptr;

struct name_rec {
                nameptr    follow;        /* Next name in list */
                nameptr    prev;          /* Previous name in list */
                farptr     strucp;        /* Top of structure pointer */
                byte       nkind;         /* Kind of structure */
                char       name [NAMELENGTH];
                };

typedef struct name_rec namerec;

typedef struct tcb_rec far *tcbptr;
typedef tcbptr far *tqueptr;

typedef struct tlink_rec far *tlinkptr;

struct tlink_rec {
                  tlinkptr next;
                  dword    timeout;    /* Timeout counter */
                  dword    reload;     /* Timeout counter reload value */
                  farptr   strucp;     /* Pointer to control structure */
                  byte     tstate;     /* Element state */
                  byte     tkind;      /* Kind of control structure */
                 };

typedef struct tlink_rec tlink;


struct tcb_rec {
				   tcbptr   next;       /* Next in queue */
				   tqueptr  queue;	   /* Queue head pointer */
				   byteptr  stack;      /* Task stack */
				   byteptr  stkbot;     /* Task stack bottom */
				   word     prior;      /* Task priority */
				   word     initprior;  /* Initial Task priority */
               byte     state;      /* Task state */
               byte     flags;      /* Task flags */
               tlink    timerq;     /* Timer queue link */
               farptr   retptr;     /* Event return pointer */
               int      retsize;    /* Return buffer size for pipes */
#if (TSK_NAMED)
               namerec  name;
#endif
			      };

typedef struct tcb_rec tcb;

typedef struct {
               tcbptr  wait_set;
               tcbptr  wait_clear;
               int     state;
#if (TSK_DYNAMIC)
               byte    flags;
#endif
#if (TSK_NAMED)
               namerec name;
#endif
               } flag;

typedef flag far *flagptr;

typedef struct {
               tcbptr  wait_set;
               tcbptr  wait_clear;
               dword   state;
#if (TSK_DYNAMIC)
               byte    flags;
#endif
#if (TSK_NAMED)
               namerec name;
#endif
               } counter;

typedef counter far *counterptr;

typedef struct {
               tcbptr   waiting;
               tcbptr   owner;
               int      state;
#if (TSK_DYNAMIC)
               byte     flags;
#endif
#if (TSK_NAMED)
               namerec  name;
#endif
               } resource;

typedef resource far *resourceptr;

struct msg_header {
                  struct msg_header far *next;
                  };

typedef struct msg_header far *msgptr;

typedef struct {
               tcbptr  waiting;
               msgptr  mail_first;
               msgptr  mail_last;
#if (TSK_DYNAMIC)
               byte    flags;
#endif
#if (TSK_NAMED)
               namerec name;
#endif
               } mailbox;

typedef mailbox far *mailboxptr;

typedef struct {
               tcbptr   wait_read;
               tcbptr   wait_write;
               tcbptr   wait_clear;
               word     bufsize;
               word     filled;
               word     inptr;
               word     outptr;
               byteptr  contents;
#if (TSK_DYNAMIC)
               byte     flags;
#endif
#if (TSK_NAMED)
               namerec  name;
#endif
               } pipe;

typedef pipe far *pipeptr;

typedef struct {
               tcbptr   wait_read;
               tcbptr   wait_write;
               tcbptr   wait_clear;
               word     bufsize;
               word     filled;
               word     inptr;
               word     outptr;
               wordptr  wcontents;
#if (TSK_DYNAMIC)
               byte     flags;
#endif
#if (TSK_NAMED)
               namerec  name;
#endif
               } wpipe;

typedef wpipe far *wpipeptr;

typedef struct {
               resource    buf_write;
               resource    buf_read;
               wpipe       pip;
               word        msgcnt;
#if (TSK_DYNAMIC)
               byte    flags;
#endif
#if (TSK_NAMED)
               namerec     name;
#endif
               } buffer;

typedef buffer far *bufferptr;


extern void far install_tasker (byte varpri, int speedup);
extern void far remove_tasker (void);
extern void far preempt_on (void);
extern void far preempt_off (void);

extern void far schedule (void);
extern tcbptr far create_task (tcbptr task, funcptr func, byteptr stack,
                               word stksz, word prior, farptr arg
#if (TSK_NAMED)
                               ,byteptr name
#endif
                               );
extern void far kill_task (tcbptr task);
extern int far start_task (tcbptr task);
extern int far wake_task (tcbptr task);
extern word far get_priority (tcbptr task);
extern void far set_priority (tcbptr task, word prior);

extern int far t_delay (dword ticks);

extern tlinkptr far create_timer (tlinkptr elem, dword tout, farptr strucp,
                                  byte kind, byte rept);
extern void far delete_timer (tlinkptr elem);
extern void far change_timer (tlinkptr elem, dword tout, byte rept);

extern resourceptr far create_resource (resourceptr rsc
#if (TSK_NAMED)
                               ,byteptr name
#endif
                               );
extern void far delete_resource (resourceptr rsc);
extern void far release_resource (resourceptr rsc);
extern int far request_resource (resourceptr rsc, dword timeout);
extern int far c_request_resource (resourceptr rsc);
extern int far check_resource (resourceptr rsc);

extern flagptr far create_flag (flagptr flg
#if (TSK_NAMED)
                               ,byteptr name
#endif
                               );
extern void far delete_flag (flagptr flg);
extern void far set_flag (flagptr flg);
extern void far clear_flag (flagptr flg);
extern int far clear_flag_wait_set (flagptr flg, dword timeout);
extern int far wait_flag_set (flagptr flg, dword timeout);
extern int far wait_flag_clear (flagptr flg, dword timeout);
extern int far check_flag (flagptr flg);

extern counterptr far create_counter (counterptr cnt
#if (TSK_NAMED)
                               ,byteptr name
#endif
                               );
extern void far delete_counter (counterptr cnt);
extern void far clear_counter (counterptr cnt);
extern int far wait_counter_set (counterptr cnt, dword timeout);
extern int far wait_counter_clear (counterptr cnt, dword timeout);
extern void far inc_counter (counterptr cnt);
extern dword far check_counter (counterptr cnt);

extern mailboxptr far create_mailbox (mailboxptr box
#if (TSK_NAMED)
                               ,byteptr name
#endif
                               );
extern void far delete_mailbox (mailboxptr box);
extern void far send_mail (mailboxptr box, farptr msg);
extern farptr far wait_mail (mailboxptr box, dword timeout);
extern farptr far c_wait_mail (mailboxptr box);
extern int far check_mailbox (mailboxptr box);

extern pipeptr far create_pipe (pipeptr pip, farptr buf, word bufsize
#if (TSK_NAMED)
                               ,byteptr name
#endif
                               );
extern void far delete_pipe (pipeptr pip);
extern int far read_pipe (pipeptr pip, dword timeout);
extern int far c_read_pipe (pipeptr pip);
extern int far write_pipe (pipeptr pip, byte ch, dword timeout);
extern int far c_write_pipe (pipeptr pip, byte ch);
extern int far wait_pipe_empty (pipeptr pip, dword timeout);
extern int far check_pipe (pipeptr pip);
extern word far pipe_free (pipeptr pip);

extern wpipeptr far create_wpipe (wpipeptr pip, farptr buf, word bufsize
#if (TSK_NAMED)
                               ,byteptr name
#endif
                               );
extern void far delete_wpipe (wpipeptr pip);
extern word far read_wpipe (wpipeptr pip, dword timeout);
extern word far c_read_wpipe (wpipeptr pip);
extern int far write_wpipe (wpipeptr pip, word ch, dword timeout);
extern int far c_write_wpipe (wpipeptr pip, word ch);
extern int far wait_wpipe_empty (wpipeptr pip, dword timeout);
extern word far check_wpipe (wpipeptr pip);
extern word far wpipe_free (wpipeptr pip);

extern bufferptr far create_buffer (bufferptr buf, farptr pbuf, word bufsize
#if (TSK_NAMED)
                               ,byteptr name
#endif
                               );
extern void far delete_buffer (bufferptr buf);
extern int far read_buffer (bufferptr buf, farptr msg, int size, dword timeout);
extern int far c_read_buffer (bufferptr buf, farptr msg, int size);
extern int far write_buffer (bufferptr buf, farptr msg, int size, dword timeout);
extern int far c_write_buffer (bufferptr buf, farptr msg, int size);
extern word far check_buffer (bufferptr buf);

extern word far t_read_key (void);
extern word far t_wait_key (dword timeout);
extern word far t_keyhit (void);

extern int far  tsk_dis_int (void);
extern void far tsk_ena_int (int);
extern void far tsk_nop (void);

#if (TURBO)
#define tsk_cli()       disable()
#define tsk_sti()       enable()
#define tsk_outp(p,b)   outportb(p,b)
#define tsk_inp(p)      ((byte) inportb(p))
#elif MSC
#pragma intrinsic(_disable,_enable,outp,inp)
#define tsk_cli()       _disable()
#define tsk_sti()       _enable()
#define tsk_outp(p,b)   outp(p,b)
#define tsk_inp(p)      ((byte) inp(p))

#define MK_FP(seg,ofs)	((farptr)(((dword)(seg) << 16) | (word)(ofs)))
#else
extern int far  tsk_dis_int (void);
extern void far tsk_ena_int (int);
extern void far tsk_cli (void);
extern void far tsk_sti (void);
#endif

#define CRITICAL  int crit_intsav
#define C_ENTER   crit_intsav = tsk_dis_int()
#define C_LEAVE   tsk_ena_int (crit_intsav)

#if (TSK_DYNAMIC)
extern farptr tsk_alloc (word size);
extern void tsk_free (farptr item);
extern resource alloc_resource;
#endif

#if (TSK_NAMED)
extern farptr far find_name (byteptr name, byte kind);
#endif

extern word _Near ticks_per_sec;

