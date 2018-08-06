;
;	CTask - Scheduler and miscellaneous utilities
;
;	Public Domain Software written by
;		Thomas Wagner
;		Patschkauer Weg 31
;		D-1000 Berlin 33
;		West Germany
;
 IFDEF at
.186
 ENDIF
	name	tskasm
	.model	large
;
	public	scheduler
	public	_schedule
	public	_c_schedule
	public	_sched_int
;
	public	_tsk_dis_int
	public	_tsk_ena_int
	public	_tsk_cli
	public	_tsk_sti
	public	_tsk_dseg
	public	_tsk_flags
	public	_tsk_outp
	public	_tsk_inp
	public	_tsk_nop
;
;
	include	tsk.mac
;
INT_FLAG	=	2h	; Int enable flag in upper byte of flag reg
r_flags		=	24	; Offset of flag register on task stack
;
	.data?
;
	extrn	_tsk_current: dword
	extrn	_tsk_eligible: dword
	extrn	_tsk_preempt: byte
	extrn	_tsk_pretick: byte
	extrn	_tsk_var_prior: byte
;
	.data
;
in_sched	db	0
;
	.code
;				   
_dgroup		dw	seg dgroup
;
;------------------------------------------------------------------------
;
;	enq	Enqueue tcb in queue. For local use only.
;		entry:	es:di = tcb to enqueue
;		exit:	-
;		uses:	ax, cx, dx, bp, di
;
enq	macro
;
	lds	bx,es:queue[di]		; queue head pointer
	mov	ax,ds
	or	ax,bx	
	jz	enq_end			; nothing left to do if queue null
	mov	dx,es:prior[di]		; load priority of task
enq_l:
	mov	cx,bx			; save last ptr
	mov	bp,ds
	lds	bx,next[bx]		; load next
	mov	ax,ds
	or	ax,bx			; end of chain?
	jz	enq_found		; then insert
	cmp	dx,prior[bx]		; compare priority
	jbe	enq_l			; loop if tasks prior less or equal
;
enq_found:
	mov	word ptr es:next[di],bx		; set tasks next pointer
	mov	word ptr es:next+2[di],ds
	mov	bx,cx				; last pointer
	mov	ds,bp
	mov	word ptr next[bx],di		; last->next = task
	mov	word ptr next+2[bx],es
enq_end:
	mov	ds,_dgroup
;
	endm
;
;
;	upd_prior: Update priority of tasks in eligible queue.
;	           Only activated if tsk_var_prior is nonzero.
;
;	NOTE:	This loop is not protected by interrupt disable.
;		Since it does not modify the queue itself, there
;		is no danger of race conditions.
;
upd_prior	macro
;
	les	di,_tsk_eligible
pinc_loop:
	mov	ax,es
	or	ax,di			; end of chain?
	jz	updp_end
	inc	es:prior[di]
	jnz	pinc_nxt
	dec	es:prior[di]
pinc_nxt:
	les	di,es:next[di]
	jmp	pinc_loop
;
updp_end:
;
	endm
;
;
;	The scheduler. Note that this routine is entered with the stack
;	set up as for an interrupt handler.
;
scheduler	proc	far
;
	cli			; better safe than sorry
	push	ds
	mov	ds,_dgroup
	cmp	in_sched,0	; already in the scheduler?
	je	sched_ok	; continue if not
	pop	ds		; else return immediately
	iret
;
sched_ok:
	inc	in_sched
	pop	ds
	sti			; we can now safely enable interrupts
 IFNDEF at
	push	ax
	push	cx
	push	dx
	push	bx
	push	sp
	push	bp
	push	si
	push	di
 ELSE
	pusha
 ENDIF
	push	ds
	push	es
;
	mov	ds,_dgroup		; establish addressing of our vars
;
	cmp	_tsk_var_prior,0
	je	no_var_pri
	upd_prior			; update priority
;
no_var_pri:
	cli				; the following is critical
;
	les	di,_tsk_current		; get current tcb
	mov	ax,es			; check if NULL (current task killed)
	or	ax,di
	jz	no_current
;
	mov	word ptr es:stack[di],sp ; store stack pointer & seg
	mov	word ptr es:stack+2[di],ss
;
	enq				; Enqueue current task
;
no_current:
	mov	_tsk_pretick,0		; No preemption tick
	and	_tsk_preempt,1		; Turn off temp preempt flag
;
wait_elig:
	cli
	les	di,_tsk_eligible
;
;	If eligible queue empty, enter waiting loop
;
	mov	ax,es
	or	ax,di			; is the eligible queue empty?
	jnz	not_empty		; jump if not
	sti			       	; enable interrupts
	nop
	nop
	jmp	wait_elig
;
;	Eligible queue not empty, activate first eligible task.
;
not_empty:
	mov	ax,word ptr es:next[di]		; load next pointer
	mov	word ptr _tsk_eligible,ax	; remove from queue
	mov	ax,word ptr es:next+2[di]
	mov	word ptr _tsk_eligible+2,ax
;
	mov	word ptr _tsk_current,di 	; set tcb into current
	mov	word ptr _tsk_current+2,es
	mov	ax,es:iniprior[di]		; reset current tasks priority
	mov	es:prior[di],ax
;
	mov	in_sched,0			; reset scheduler active flag
;
	lds	si,es:stack[di] 		; load stack
	or	byte ptr r_flags+1[si],INT_FLAG	; enable interrupts
	mov	es:state[di],ST_RUNNING		; set task state
	mov	cx,ds			; switch stack
	mov	ss,cx
	mov	sp,si
	pop	es			; restore all registers
	pop	ds
 IFNDEF at
	pop	di
	pop	si
	pop	bp
	pop	bx			; don't pop SP
	pop	bx
	pop	dx
	pop	cx
	pop	ax
 ELSE
	popa
 ENDIF
	iret
;
scheduler	endp
;
;
;--------------------------------------------------------------------------
;
;
;	_sched_int  
;
;	Is the scheduler entry for interrupt handlers.
;	It checks if preemption is allowed, returning if not.
;	The stack is assumed to be set up as on interrupt entry.
;	
_sched_int	proc	far
;
	push	ds
	push	bx
	mov	ds,_dgroup
	cmp	_tsk_preempt,0		; preempt flags 0?
	jne	no_sched		; no scheduling if set
	lds	bx,_tsk_current		; current running task
	test	flags[bx],F_CRIT	; preemption allowed for this task?
	jnz	no_sched		; no scheduling if flag set
	pop	bx			; else go schedule
	pop	ds
	jmp	scheduler
;
no_sched:
	mov	ds,_dgroup
	mov	_tsk_pretick,1		; Mark preemption pending
	pop	bx
	pop	ds
	iret
;
_sched_int	endp
;
;
;	void far schedule (void)
;
;	Entry for calling the scheduler. Rearranges the stack to
;	contain flags.
;	NOTE: Uses ax,bx.
;
_schedule	proc	far
;
	pop	ax
	pop	bx
	pushf
	push	bx
	push	ax
	cli
	jmp	scheduler
;
_schedule	endp
;
;
;	void far c_schedule (void)
;
;	Entry for conditionally calling the scheduler. Rearranges 
;	the stack to contain flags, then jumps to _sched_int.
;	NOTE: Uses ax,bx.
;
_c_schedule	proc	far
;
	pop	ax
	pop	bx
	pushf
	push	bx
	push	ax
	cli
	jmp	_sched_int
;
_c_schedule	endp
;
;--------------------------------------------------------------------------
;
;	word tsk_dseg (void)
;
;	Returns current contents of DS register.
;
_tsk_dseg	proc  	far
	mov	ax,ds
	ret
_tsk_dseg	endp
;
;
;	word tsk_flags (void)
;
;	Returns current contents of Flag register.
;
_tsk_flags	proc	far
	pushf
	pop	ax
	ret
_tsk_flags	endp
;
;
;	int tsk_dis_int (void)
;
;	Returns current state of the interrupt flag (1 if ints were 
;	enabled), then disables interrupts.
;
_tsk_dis_int	proc	far
;
	pushf
	pop	ax
	mov	cl,9
	shr	ax,cl
	and	ax,1
	cli
	ret
;
_tsk_dis_int	endp
;
;
;	void far tsk_ena_int (int state)
;
;	Enables interrupts if 'state' is nonzero.
;
_tsk_ena_int	proc	far
;
	push	bp
	mov	bp,sp
	mov	ax,6[bp]
	pop	bp
	or	ax,ax
	jz	teiend
	sti
teiend:
	ret
;
_tsk_ena_int	endp
;
;
;	tsk_cli/tsk_sti: disable/enable int
;
_tsk_cli	proc	far
	cli
	ret
_tsk_cli	endp
;
;
_tsk_sti	proc	far
	sti
	ret
_tsk_sti	endp
;
;
;	tsk_inp/tsk_outp: input/output from/to port
;
_tsk_inp	proc	far
;
	push	bp
	mov	bp,sp
	mov	dx,6[bp]
	in	al,dx
	xor	ah,ah
	pop	bp
	ret
;
_tsk_inp	endp
;
;
_tsk_outp	proc	far
;
	push	bp
	mov	bp,sp
	mov	dx,6[bp]
	mov	al,8[bp]
	out	dx,al
	pop	bp
	ret
;
_tsk_outp	endp
;
;
;	void tsk_nop (void)
;
;	Do nothing. Used for very short delays.
;
_tsk_nop	proc	far
;
	jmp	short tnop1
tnop1:
	jmp	short tnop2
tnop2:
	ret
;
_tsk_nop	endp
;
	end


