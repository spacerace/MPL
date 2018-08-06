;
;	CTask - Timer interrupt handler (IBM specific)
;
;	Public Domain Software written by
;		Thomas Wagner
;		Patschkauer Weg 31
;		D-1000 Berlin 33
;		West Germany
;
	name	tsktim
	.model	large
;
	public	_tsk_install_timer
	public	_tsk_remove_timer
	public	_tsk_chain_timer
;
	include	tsk.mac
;
timer	equ	40h			; 8253 timer base I/O address
inta00	equ	20h			; 8259 int controller base
eoi	equ	20h			; unspecific EOI
;
intseg	segment at 0
;
		org	8*4
tintofs		dw	?		; timer interrupt entry
tintseg		dw	?

intseg	ends
;
;
	.data?
;
	extrn	_tsk_timer_counter:word
	extrn	_tsk_int9_counter:word
;
divisor		dw	?
timflag		dw	?
timcnt		dw	?
sys_ticks	dw	?
;
	.code
;
	extrn	_sched_int: far
	extrn	_inc_counter: far
;
timer_save	label	dword		; in CSEG to allow addressing
tsofs		dw	?
tsseg		dw	?
;
;----------------------------------------------------------------------
;
;	timer interrupt handler
;
timer_int	proc	far
	sti
	push	ds
	push	ax
	mov	ax,SEG dgroup
	mov	ds,ax
;
	cmp	timflag,1		; initialisation ?
	jne	no_init
;
;	Install timer
;
	mov	timflag,0		; signal init ready
	mov	timcnt,1
	mov	al,36h
	out	timer+3,al		; setup to load divisor
	mov	al,byte ptr divisor
	out	timer,al		; lsb
	mov	al,byte ptr divisor+1
	out	timer,al		; msb
	jmp	short no_uninit
;
no_init:
	cmp	timflag,2		; un-install flag set?
	jne	no_uninit		; no un-install if not
;
;       Un-Install timer (wait until system tick count reached)
;
timdec:
	cli
	dec	timcnt			; decrement tick count
	jz	uninit			; go un-install if zero
	mov	al,eoi			; else just issue EOI
	out	inta00,al
	pop	ax
	pop	ds
	iret				; do nothing while waiting for uninit
;
;--------------------------------------------------------------------
;
;	Normal timer tick. The tick counter is incremented, and
;	the interrupt controller is checked for other pending interrupts.
;	If the timer tick occurred during processing of another interrupt,
;	we may not call the scheduler, since this would delay the
;	interrupt handler.
;
;	Note that an EOI is issued here to allow interrupts to occur
;	during further processing. The original INT 9 handler will be
;	chained to from a special task. The reason behind this is that
;	some TSR's link into the timer interrupt and may do some lengthy
;	processing there. To allow the TSR to be preempted, we must use
;	a task for the INT 9 processing.
;
no_uninit:
	push	es			; save other regs
	push	bx
	push	cx
	push	dx
	mov	ax,ds
	mov	es,ax
	mov	ax,offset _tsk_timer_counter
	push	ds
	push	ax
	call	_inc_counter		; increase timer tick counter
	add	sp,4
;
;	Now the timer counter is decremented. If it is zero,
;	we must chain to the original INT 9, so the counter for
;	the chain task is incremented.
;
	dec	timcnt			; decrement tick count
	jnz	no_pass			; pass on this int if zero
;
	mov	ax,sys_ticks
	mov	timcnt,ax		; re-init tick counter
;
	mov	ax,offset _tsk_int9_counter
	push	ds
	push	ax
	call	_inc_counter		; increase timer tick counter
	add	sp,4
;
no_pass:
	pop	dx
	pop	cx
	pop	bx
	pop	es
;
	cli
	mov	al,eoi			; issue EOI
	out	inta00,al
	mov	al,0bh			; access int control reg
	out	inta00,al
	in	al,inta00		; ints pending?
	or	al,al
	pop	ax
	pop	ds
	jnz	no_sch			; don't schedule if other ints active
	jmp	_sched_int		; else schedule
;
no_sch:
	iret
;
;------------------------------------------------------------------------
;
;	Uninstall timer int handler
;
uninit:
	mov	timflag,0		; mark un-install complete
	mov	al,36h			; setup to load divisor
	out	timer+3,al
	mov	al,0			; divisor 0 means 65536
	out	timer,al		; lsb
	out	timer,al		; msb
	push	es
	xor	ax,ax
	mov	es,ax
	assume	es:intseg
	mov	ax,cs:tsofs		; restore vector
	mov	tintofs,ax
	mov	ax,cs:tsseg
	mov	tintseg,ax
	assume	es:nothing
	pop	es
	pop	ax
	pop	ds
	jmp	cs:timer_save		; pass on interrupt
;
timer_int	endp
;
;----------------------------------------------------------------------
;
;	void far tsk_chain_timer (void)
;
;       Pass timer tick on to interrupt 9 chain.
;
_tsk_chain_timer	proc	far
;
	pushf
	cli
	call	cs:timer_save
	ret
;
_tsk_chain_timer	endp
;
;
;	void far tsk_install_timer (word divisor, word sys_ticks)
;
;	This routine installs the timer tick int handler.
;	The timer chip is reprogrammed on the next tick.
;
_tsk_install_timer	proc	far
;
	push	bp
	mov	bp,sp
	mov	ax,6[bp]
	mov	divisor,ax
	mov	ax,8[bp]
	mov	sys_ticks,ax
	mov	timflag,1		; set init-flag
	xor	ax,ax
	mov	es,ax			; establish addressing for intseg
	assume	es:intseg
;
	mov	ax,tintofs		; save old timer int addr
	mov	tsofs,ax
	mov	ax,tintseg
	mov	tsseg,ax
	cli
	mov	tintofs,offset timer_int ; set new timer int addr
	mov	tintseg,cs
	sti
	assume	es:nothing
wait_set:
	cmp	timflag,0		; wait until timer started
	jne	wait_set
	pop	bp
	ret
;
_tsk_install_timer	endp
;
;
;	void far tsk_remove_timer (void)
;
;	This routine un-installs the timer tick int handler.
;	The timer chip is reprogrammed & the interrupt vector
;	restored when the system tick count reaches zero.
;
_tsk_remove_timer	proc	far
;
	mov	timflag,2		; set un-init flag for timer
wait_tim:
        sti                             ; just to be safe
	cmp	timflag,0		; wait until int un-installed
	jne	wait_tim
	ret
;
_tsk_remove_timer	endp
;
	end

