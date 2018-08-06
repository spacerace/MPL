
          PAGE  60,132

;                  Multi-tasking subroutines for Lattice C
;                          (large model only)
;
;
;       These functions are based on the technique presented by
;       Richard Foard in PC Tech Journal, March 1986 in the article
;       "Multitasking Methods".  The basic mechanism is the same; I
;       have changed the names of the functions to suit my own
;       background.  The only real differences are as follows:
;
;         1.  The interfacing is specific to Lattice V2.15, using the
;             large memory model.  Although the DOS.MAC header is
;             referenced, much of the code is dependent on 4-byte
;             pointers and FAR calls.
;
;         2.  In addition to the stack frame, each of the subtasks is
;             allowed (required?) to have a static, global data area
;             independent of all others.  The truly global data item
;             COMANCH should be defined as a pointer to an appropriate
;             structure.  The task switch routines will assure that
;             COMANCH always points to the proper area.  The inittask
;             and attach functions accept a pointer to the area in
;             their parameter lists.
;
;         3.  Instead of wait and post, I have implemented enq and
;             deq functions to allow serialization of resources.
;             For me, at least, these are a more natural mechanism.
;
;         4.  The yield function will skip over any task which does not have
;             a non-zero data pointer, assuming that the task has terminated.
;
;         5.  The function taskcnt returns the number of active tasks - this
;             can be checked by the "main" task to decide whether termination
;             is reasonable.  When a value of 1 is returned, only the calling
;             task is active.
;
;     Notes:
;
;         If an attempt is made to attach more than MAXTASKS, return is made
;         to DOS via _exit with a return code of 1.
;
;         If the last remaining task invokes the stop function, yield will
;         loop forever.
;
;         A subtask should *never* attempt to return to its caller;
;         the stack used for the subtask does not carry the caller's
;         environment, and the system would hang for sure.   The proper
;         way to terminate a subtask is via stop().
;
;         It is unwise for any task to return to DOS unless it is known
;         that no other tasks remain;  use taskcnt() to find out.
;
;
;      March 1986                       Ed Legowski
;                                       24 Cannonade Dr
;                                       Marlboro, NJ 07746
;
;
;         void inittask(comptr)
;                  char *comptr    - pointer to tcb's common area
;
;                  prepares environment for use - on exit caller is the
;                  first active task.  Note: the public pointer COMANCH
;                  must have already been initialized to point to the
;                  proper data area.
;
;         int attach(comptr, stackptr, stacksize)
;                  char *comptr    - pointer to tcb's common area
;                  char *stackptr  - pointer to stack space
;                  int stacksize   - size of stack
;
;                  creates and activates a subtask.  returns FALSE to
;                  the calling task, and true to the new task.
;
;              Typical sequence is:
;                 if (attach(&area,&stack,sizeof stack)) subtask();
;
;         void yield()
;                  allows a task switch to occur - the next task (in
;                  round-robin order) is 'dispatched'.
;
;         int enq(lockword)
;                  int *lockword;
;
;                  waits unit the specified lockword becomes free, then
;                  takes ownership.
;                  Returns 0 if ok, !0 if already owned.
;
;         void deq(lockword)
;                  int *lockword;
;
;                  releases the specified lock
;
;         void stop()
;                  destroys the calling task
;
;         int taskcnt()
;                  returns the number of active tasks
;
;
;
;

          EXTRN    COMANCH:FAR
          EXTRN    _EXIT:FAR

                if1
                include L:\l\DOS.MAC
                endif

TCB       STRUC
tcbstack  dd       ?               ;saved stack frame
tcbcommon dd       ?               ;tasks common block
TCB       ENDS

ATTACHPL  STRUC
attbp     dw       ?              ;callers bp
attret    dd       ?              ;return address
attcomptr dd       ?              ;pointer to common
attstack  dd       ?              ;pointer to stack
attsize   dw       ?              ;size of stack
ATTACHPL  ENDS

EVENTPL   STRUC
evnbp     dw       ?              ;caller's bp
evnret    dd       ?              ;return address
evnlock   dd       ?              ;pointer to lockword
EVENTPl   ENDS

          DSEG
MAXTASKS  equ      4              ;maximum number of tasks

tasktable equ      this byte
          REPT     MAXTASKS
          TCB      <0,0>
          ENDM
endtable  equ      this byte

curtask   dw       0               ;ptr to current tcb
actvtasks dw       0               ;current task count
commonptr dd       COMANCH
          ENDDS

          PSEG

          BEGIN    inittask

          push     bp             ;save callers reg
          mov      bp,sp          ;set addressing to parm list

;    first clear the task table

          sub      ax,ax
          mov      bx,offset tasktable
          mov      cx,MAXTASKS
initl:    mov      [bx],ax         ;clear the stack reference
          mov      [bx+2],ax
          mov      [bx+4],ax       ;clear the common ptr
          mov      [bx+6],ax
          add      bx,SIZE TCB     ;point next tcb
          loop     initl           ;do them all

;    now establish us as the current task

          mov      curtask,offset tasktable
          mov      actvtasks,1
          mov      bx,offset tasktable
          mov      ax,word ptr [bp].attcomptr  ;set the common pointer
          mov      word ptr [bx].tcbcommon,ax
          mov      ax,word ptr [bp].attcomptr+2
          mov      word ptr [bx].tcbcommon+2,ax

          pop      bp             ;restore caller's reg
          ret
inittask  ENDP

          BEGIN    yield

;   first save status of current task

          push     bp             ;preserve frame
          mov      bx,curtask     ;point to current tcb
          mov      word ptr [bx].tcbstack,sp  ;preserve stack
          mov      word ptr [bx].tcbstack+2,ss

;   locate next task to dispatch

yield0:   add      bx,SIZE TCB    ;point next TCB
          cmp      bx,offset endtable  ;q/end of round robin table
          jb       yield1              ;bin - test the entry
          mov      bx,offset tasktable ;point back to the beginning
yield1:   cmp      word ptr [bx].tcbcommon+2,0   ;q/is task active
          je       yield0              ;bin - keep trying

;  restore status of this task
          mov      curtask,bx     ;make new task current
          les      ax,[bx].tcbcommon ; point to task common
          mov      si,es          ;save segment for common
          les      di,commonptr   ;point to the C pointer
          mov      es:[di],ax
          mov      es:[di+2],si

;  restore the stack - this is done disabled to prevent possible
;     problems with sp and ss regs being out of synch if an interrupt
;     should occur

          cli
          mov      ss,word ptr [bx].tcbstack+2
          mov      sp,word ptr [bx].tcbstack
          sti
          pop      bp
          mov      ax,1
          ret
yield     ENDP

          BEGIN    attach

          mov      ax,actvtasks   ;verify task
          cmp      ax,MAXTASKS    ;     count
          jne      attach0        ;        not exceeded
          mov      ax,1           ; all tcb's in use
          push     ax             ;   terminate with error
          call     _exit

attach0:
          inc      ax             ;number of tasks
          mov      actvtasks,ax   ;preserve it

;   locate first available tcb


          mov      bx,offset tasktable
          mov      cx,MAXTASKS
attach1:  cmp      word ptr [bx].tcbcommon+2,0   ;q/is it in use?
          je       attach2        ;bin - use this one
          add      bx,SIZE TCB    ;point next one
          loop     attach1        ;keep looking
          mov      ax,2           ;empty tcb not found
          push     ax             ;   terminate with error 2
          call     _exit

;   initialize the new tcb

attach2:  push     bp             ;save callers bp
          mov      bp,sp          ;point to parm frame
          mov      ax,word ptr [bp].attcomptr  ;set the common pointer
          mov      word ptr [bx].tcbcommon,ax
          mov      ax,word ptr [bp].attcomptr+2
          mov      word ptr [bx].tcbcommon+2,ax
          mov      ax,word ptr [bp].attstack+2  ;stack segment
          mov      word ptr [bx].tcbstack+2,ax
          mov      ax,word ptr [bp].attstack  ;  stack offset
          add      ax,word ptr [bp].attsize   ;      adjust by size
          mov      dx,ax                      ;save stack origin
          sub      ax,SIZE ATTACHPL  ;  allow pop of parms
          mov      word ptr [bx].tcbstack,ax

;   initialize the stack for the new task

          les      di,[bx].tcbstack  ;point to the stack
          mov      cx,SIZE ATTACHPL
          cld
attach3:  mov      al,[bp]
          stosb
          inc      bp
          loop     attach3
          les      di,[bx].tcbstack  ;point to the stack
          mov      es:[di],dx        ;set bp for new task = top of stack

;    return to calling task

          pop      bp
          sub      ax,ax
          ret

attach    ENDP

          BEGIN    enq
          push     bp             ;save bp
          mov      bp,sp          ;origin of parm list

enq0:     les      di,[bp].evnlock ;point event counter
          mov      ax,es:[di]     ;get the counter
          or       ax,ax          ;q/do we need to wait
          jz       enq1           ;bin - return to caller
          cmp      ax,curtask     ;q/caller already own it
          je       enq1           ;biy
          call     yield          ;allow transfer
          jmp      enq0           ;try again

enq1:     mov      bx,curtask     ;show which task owns it
          mov      es:[di],bx     ;take control of the lock
          pop      bp             ;return caller's bp
          ret
enq       ENDP

          BEGIN    deq
          push     bp             ;save user's bp
          mov      bp,sp
          les      di,[bp].evnlock     ;point user's event counter
          mov      word ptr es:[di],0  ;clear the lock
          pop      bp
          ret
deq       ENDP

          BEGIN    stop

          mov      bx,curtask     ;point to the current task
          mov      word ptr [bx].tcbcommon+2,0   ;show task inactive
          dec      actvtasks      ;reduce count of tasks
          call     yield          ;we will not return from here

stop      ENDP

          BEGIN    taskcnt
          mov      ax,actvtasks   ;return number of active tasks
          ret
taskcnt   ENDP

          ENDPS
          END
