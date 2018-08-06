;
;	CTask - DOS access module.
;
;	Public Domain Software written by
;		Thomas Wagner
;		Patschkauer Weg 31
;		D-1000 Berlin 33
;		West Germany
;
;
; The DOS interrupt (and the related direct disk I/O interrupts) are
; not reentrant. Access to DOS thus has to be channelled such that no
; two tasks use DOS services simultaneously. However, there is one
; exception to this rule. Whenever DOS is waiting for the keyboard, it
; issues a special interrupt, INT 28h, to signal that background
; processing for functions > 0Ch may be performed. This is used in the
; DOS interface for CTask in the following manner:
;
;   A task issuing a DOS interrupt will request one of two resources:
;   "lower_dos" for functions <= 0C, "upper_dos" for functions > 0C.
;   If a task gets access to "lower_dos", it will also request the
;   "upper_dos" resource to lock out other tasks from interrupting.
;   This "upper_dos" resource is shortly released on each INT 28, and
;   then immediately reclaimed, with task priority temporarily raised
;   to the maximum value. The first task waiting to execute a 
;   function > 0C will thus be scheduled to execute the request, but
;   the resource will be reassigned to the INT 28 handler as soon as
;   this request terminates, so the waiting task will not be delayed too
;   long.
;
; There are two additional safety measures which have to be taken to
; avoid getting into conflicts with other resident background programs,
; especially the DOS PRINT background spooler:
;
;   Before requesting any resource, the status of the DOS critical section
;   flag is checked. If this flag is set, the task is made waiting for
;   the flag to be cleared.
;   Only the task that set the flag will be allowed access to DOS.
;
;   Before actually executing the request, the status of the DOS in-use
;   flag is checked. If this flag is set, the task enters a busy waiting
;   loop, calling the scheduler so that the processor is not tied up.
;
; NOTE: The method for checking the status of DOS is described in-depth
;	in the book "The MS-DOS Encyclopedia" from Microsoft Press, in
;	the chapter on TSR programming. The logic in this module was
;	developed without the help of this book, so if you compare
;	the code here with the routines in the Encyclopedia, you may
;	notice that not all re-entry conditions are checked here.
;	According to my experience with debugging TSR's and CTask, the
;	logic should be sufficient for all but the most obscure TSR's.
;	If you want to be completely on the safe side, you might consider
;	adding the more thorough checks listed in the Encyclopedia.
;
;
	name	tskdos
;
	.model	large
;
	public	_tsk_install_dos
	public	_tsk_remove_dos
;
;
	include	tsk.mac
;
CSECT		=	2ah	; Critical Section Interrupt
get_in_use_flag	=	34h	; DOS-function to get in_use_flag address
;
;
intseg	segment at 0
		org	20h*4
termoff		dw	?	; program terminate vector
termseg		dw	?
		org	21h*4
idosoff		dw	?	; dos interrupt
idosseg		dw	?
		org	25h*4
absreadoff	dw	?	; absolute disk read
absreadseg	dw	?
		org	26h*4
abswriteoff	dw	?	; absolute disk write
abswriteseg	dw	?
		org	27h*4
keepoff		dw	?	; Terminate but stay resident
keepseg		dw	?
		org	28h*4
idleoff		dw	?	; dos idle interrupt
idleseg		dw	?
		org	CSECT*4
csectoff	dw	?	; dos critical section
csectseg	dw	?
;
intseg	ends
;
;----------------------------------------------------------------------------
;
;	Variables
;
	.data
;
	extrn	_tsk_current: dword
;
term_err_msg	db	0dh,0ah,"Program terminated - CTask uninstalled"
		db	0dh,0ah,'$'
;
idle_active	db	0    		; Idle-Interrupt active
dos310		db	0		; DOS version >= 3.10
;
	IF	TSK_NAMEPAR
ldos_name	db	"DOSUPPER",0
udos_name	db	"DOSLOWER",0
cdos_name	db	"DOSCRIT",0
	ENDIF
;
	.data?
;
in_use		dd	?		; Adress of DOS in-use-flag
in_error	dd	?		; Adress of DOS error-mode-flag
;
lower_dos	resource <>
upper_dos	resource <>
critical	flag <>
;
idle_ss		dw	?		; Stack save
idle_sp		dw	?
;
		dw	256 dup(?)	; Stack for IDLE-Interrupt
local_stack	label	word
;
	.code
;
;	Original Interrupt-Entries
;
savtermoff	dw	?		; Terminate vector save
savtermseg	dw	?
;
savdos		label	dword		; original DOS-Entry
savdosoff	dw	?
savdosseg	dw	?
;
savidle		label	dword		; original IDLE-Entry
savidleoff	dw	?
savidleseg	dw	?
;
savcsect	label	dword		; Critical Section save
savcsectoff	dw	?
savcsectseg	dw	?
;
savabsread	label	dword		; Absolute Disk Read save
savabsreadoff	dw	?
savabsreadseg	dw	?
;
savabswrite	label	dword		; Absolute Disk Write save
savabswriteoff	dw	?
savabswriteseg	dw	?
;
savkeepoff	dw	?		; Terminate resident vector save
savkeepseg	dw	?
;
critsect_active	db	0		; DOS Critical Section active
ctask_active	db	0		; CTask DOS call active
crit_task	dd	?		; Task requesting critical section
;
;
dos	macro
	pushf
	cli
	call	cs:savdos
	endm
;
;---------------------------------------------------------------------------
;
	.code
;
	extrn	_create_resource: far
	extrn	_delete_resource: far
	extrn	_request_resource: far
	extrn	_release_resource: far
	extrn	_create_flag: far
	extrn	_delete_flag: far
	extrn	_set_flag: far
	extrn	_clear_flag: far
	extrn	_wait_flag_clear: far
	extrn	_remove_tasker: far
	extrn	scheduler: far
;
;
;	void tsk_install_dos (void)
;
;		Install DOS handler
;
_tsk_install_dos	proc
;
;	create needed resources & flags
;
	IF	TSK_NAMEPAR
	mov	ax,offset udos_name
	push	ds
	push	ax
	ENDIF
	mov	ax,offset upper_dos
	push	ds
	push	ax
	call	_create_resource
	IF	TSK_NAMEPAR
	add	sp,8
	ELSE
	add	sp,4
	ENDIF
;
	IF	TSK_NAMEPAR
	mov	ax,offset ldos_name
	push	ds
	push	ax
	ENDIF
	mov	ax,offset lower_dos
	push	ds
	push	ax
	call	_create_resource
	IF	TSK_NAMEPAR
	add	sp,8
	ELSE
	add	sp,4
	ENDIF
;
	IF	TSK_NAMEPAR
	mov	ax,offset cdos_name
	push	ds
	push	ax
	ENDIF
	mov	ax,offset critical
	push	ds
	push	ax
	call	_create_flag
	IF	TSK_NAMEPAR
	add	sp,8
	ELSE
	add	sp,4
	ENDIF
;
;	Get the address of DOS's in_use-flag. This flag indicates that
;	DOS is already active. This might happen if there are other
;	background tasks, like popups or print spoolers, active in
;	parallel to CTask.
;	This is also the address of the critical error flag in DOS. Beginning 
;	with DOS 3.10 the flag is located one byte before the in_use_flag.
;	With older DOS versions, we would have to search through DOS for the 
;	address. This is omitted here, but you could include the code
;	for pre 3.1 versions from pages 378-379 of the MS-DOS Encyclopedia.
;	
	mov	ah,get_in_use_flag
	int	21h
	mov	word ptr in_use,bx
	mov	word ptr in_use+2,es
	mov	word ptr in_error+2,es
;
	mov	ah,30h
	int	21h
	cmp	ah,3
	jb	not_dos3
	cmp	ah,0ah
	je	not_dos3	; OS/2 compatibility box
	cmp	al,10
	jb	not_dos3
	inc	dos310
	dec	bx
	mov	word ptr in_error,bx
	jmp	short save_ints
;
not_dos3:

;
;	Save old interrupt vectors
;
save_ints:
        push    es
	xor	ax,ax
	mov	es,ax
;
        assume  es:intseg
;
	mov	ax,termoff		; DOS
	mov	savtermoff,ax
	mov	ax,termseg
	mov	savtermseg,ax
;
	mov	ax,idosoff		; DOS
	mov	savdosoff,ax
	mov	ax,idosseg
	mov	savdosseg,ax
;
	mov	ax,idleoff		; IDLE
	mov	savidleoff,ax
	mov	ax,idleseg
	mov	savidleseg,ax
;
	mov	ax,csectoff		; Critical Section
	mov	savcsectoff,ax
	mov	ax,csectseg
	mov	savcsectseg,ax
;
	mov	ax,absreadoff		; Absolute Disk read
	mov	savabsreadoff,ax
	mov	ax,absreadseg
	mov	savabsreadseg,ax
;
	mov	ax,abswriteoff		; Absolute Disk write
	mov	savabswriteoff,ax
	mov	ax,abswriteseg
	mov	savabswriteseg,ax
;
	mov	ax,keepoff		; Terminate Resident
	mov	savkeepoff,ax
	mov	ax,keepseg
	mov	savkeepseg,ax
;
;	Enter new Interrupt-Entries
;
	cli
	mov	idosoff,offset dosentry		; DOS-Entry
	mov	idosseg,cs
	mov	idleoff,offset idleentry	; Idle-Entry
	mov	idleseg,cs
	mov	termoff,offset terminate_int	; Terminate Process Entry
	mov	termseg,cs
	mov	csectoff,offset critsectint	; Critical Section Entry
	mov	csectseg,cs
	mov	keepoff,offset keep_int		; Keep Process Entry
	mov	keepseg,cs
	mov	absreadoff,offset absread_int	; Absolute Disk Read Entry
	mov	absreadseg,cs
	mov	abswriteoff,offset abswrite_int	; Absolute Disk Write Entry
	mov	abswriteseg,cs
	sti
        pop     es
;
	ret
;
	assume	es:nothing
;
_tsk_install_dos	endp
;
;
;	void tsk_remove_dos (void)
;
;		Un-install DOS handler
;
_tsk_remove_dos	proc
;
;	Delete resources & flags
;
	mov	ax,offset upper_dos
	push	ds
	push	ax
	call	_delete_resource
	add	sp,4
;
	mov	ax,offset lower_dos
	push	ds
	push	ax
	call	_delete_resource
	add	sp,4
;
	mov	ax,offset critical
	push	ds
	push	ax
	call	_delete_flag
	add	sp,4
;
        push    es
	xor	ax,ax
	mov	es,ax
;
        assume  es:intseg
;
;	Restore interrupt entries
;
	cli
	mov	ax,savtermoff
	mov	termoff,ax
	mov	ax,savtermseg
	mov	termseg,ax
;
	mov	ax,savdosoff
	mov	idosoff,ax
	mov	ax,savdosseg
	mov	idosseg,ax
;
	mov	ax,savidleoff
	mov	idleoff,ax
	mov	ax,savidleseg
	mov	idleseg,ax
;
	mov	ax,savcsectoff
	mov	csectoff,ax
	mov	ax,savcsectseg
	mov	csectseg,ax
;
	mov	ax,savabsreadoff
	mov	absreadoff,ax
	mov	ax,savabsreadseg
	mov	absreadseg,ax
;
	mov	ax,savabswriteoff
	mov	abswriteoff,ax
	mov	ax,savabswriteseg
	mov	abswriteseg,ax
;
	mov	ax,savkeepoff
	mov	keepoff,ax
	mov	ax,savkeepseg
	mov	keepseg,ax
;
	sti
;
        pop     es
	ret
;
	assume	es:nothing
;
_tsk_remove_dos	endp
;
;
;---------------------------------------------------------------------------
;
;	Stack-Offsets relative to BP
;
caller_cs	=	4		; Caller's CS
caller_flags	=	6		; Caller's Flags
;
;
;---------------------------------------------------------------------------
;
;	INT 25: Absolute Disk Read 
;
;	This interrupt is translated into INT 21, function C0.
;	This function is re-translated later after the necessary resources
;	have been requested.
;
absread_int:
	mov	ah,0c0h
	jmp	short absrw_int
;
;
;---------------------------------------------------------------------------
;
;	INT 26: Absolute Disk Write
;
;	This interrupt is translated into INT 21, function C1.
;	This function is re-translated later after the necessary resources
;	have been requested.
;
abswrite_int:
	mov	ah,0c1h
;
;	Interrupts 25 und 26 leave the flag-word on the stack.
;	Since flags are removed in normal processing, the flag-word
;	has to be duplicated on the stack here.
;
absrw_int:
	push	bp			; reserve space
	push	bp			; save BP
	mov	bp,sp
	sti
	push	ax
	mov	ax,4[bp]		; Move return offset, segment down
	mov	2[bp],ax
	mov	ax,6[bp]
	mov	4[bp],ax
	mov	ax,8[bp]		; duplicate flags
	mov	6[bp],ax
	pop	ax
	push	caller_flags[bp]
	jmp	short dosentry_2
;
;---------------------------------------------------------------------------
;
;	INT 27: Terminate But Stay Resident Interrupt
;
;	This interrupt is translated to INT 21, function 31.
;
keep_int:
	push	bp
	mov	bp,sp
	push	caller_flags[bp]	; flags (bp-2)
	sti				; allow interrupts
;
	add	dx,0fh			; last addr + 0f to round
	sub	dx,caller_cs[bp]	; minus CS (= PSP)
	mov	cl,4
	shr	dx,cl			; div 16 = paragraphs

	mov	ax,3100h		; Keep process
	jmp	short dosentry_2
;
;---------------------------------------------------------------------------
;
;	INT 20: Terminate Program interrupt
;
;	This interrupt is translated to INT 21, function 0.
;
terminate_int:
	mov	ah,0
;
;	fall through
;
;---------------------------------------------------------------------------
;
;	INT 21: DOS-Interrupt
;
dosentry        proc    far
;
;	Save registers
;
	push	bp
	mov	bp,sp
	push	caller_flags[bp]	; flags (bp-2)
;
dosentry_2:
	push	es
	push    ds
	push	dx
	push	cx
	push	bx
	push	ax
	push	si
	push	di
;
;	Here we check if the DOS critical region is active.
;	If yes, this means that some outside background process has
;	started DOS (most likely DOS PRINT).
;	To avoid busy waiting, we wait for the "critical" flag to be
;	cleared, if this is *not* the task that set the flag.
;	The task that set the critical task is *not* made waiting.
;
wait_crit_loop:
	cli
	cmp	cs:critsect_active,0
	je	no_crit
	mov	ax,word ptr cs:crit_task
	cmp	word ptr _tsk_current,ax
	jne	wait_crit
	mov	ax,word ptr cs:crit_task+2
	cmp	word ptr _tsk_current+2,ax
	je	no_crit
;
wait_crit:
	xor	ax,ax
	push	ax
	push	ax
	mov	ax,offset critical
	push	ds
	push	ax
	call	_wait_flag_clear
	add	sp,8
;
no_crit:
	mov	cs:ctask_active,1	; mark that we are active
	sti				; Interrupts allowed now
;
	cld
	mov	bx,SEG dgroup
	mov	ds,bx
	mov	es,bx
;
        cmp     ah,4ch                  ; terminate?
        jne     dosent_x
        mov     ah,0                    ; translate to fn 0
dosent_x:
	cmp	ah,0ch
	jbe	lower_funcs
	jmp	upper_funcs
;
;	Functions 00-0C
;
lower_funcs:
;
;	first, request the "lower_dos" resource
;
	mov	bx,offset lower_dos
	xor	cx,cx
	push	cx		; no timeout
	push	cx
	push	ds		; resource address
	push	bx
	call	_request_resource
	add	sp,8
;
;	we have it, now let's get the upper_dos resource, too
;
	mov	bx,offset upper_dos
	xor	cx,cx
	push	cx		; no timeout
	push	cx
	push	ds		; resource address
	push	bx
	call	_request_resource
	add	sp,8
;
;	both resources gained, now we may execute the function if dos is free
;
	pop	di
	pop	si
	pop	ax
	pop	bx
	pop	cx
	pop	dx
;
	call	wait_dos_free
;
	or	ah,ah			; terminate ?
	je	fg_terminate		; special treatment required
	cmp	ah,4ch
	je	fg_terminate		; special treatment required
	pop     ds
	pop	es
	popf
	pop	bp
;
	dos				; execute function
;
;	Now we have to release the resources.
;
	pushf				; save registers again
	sti
	push	es
	push    ds
	push	dx
	push	cx
	push	bx
	push    ax
	push	si
	push	di
;
	mov	bx,SEG dgroup
	mov	ds,bx
	mov	es,bx
;
	mov	bx,offset upper_dos
	push	ds		; resource address
	push	bx
	call	_release_resource
	add	sp,4
;
	mov	bx,offset lower_dos
	push	ds		; resource address
	push	bx
	call	_release_resource
	add	sp,4
;
;	If both resources are free now, clear the ctask_active flag to
;	allow other background processes to gain access to DOS.
;
	cli
	mov	ax,upper_dos.rstate
        or	ax,ax
        jz	no_relc
	cmp	ax,lower_dos.rstate
	jne	no_relc
	mov	cs:ctask_active,0
no_relc:
;
;	All done, restore registers and return.
;
	pop	di
	pop	si
	pop	ax
	pop	bx
	pop	cx
	pop	dx
	pop     ds
	pop	es
	popf
;
	ret	2			; don't restore flags
;
;
;	The terminate request requires special treatment.
;	Since terminating the program without first un-installing
;	would crash the system, we do this here, not without telling
;	the user something went very wrong.
;
fg_terminate:
	cli
	call	_remove_tasker
	mov	dx,offset term_err_msg
	mov	ah,9
	int	21h
	mov	ax,4cffh
	int	21h
;
;--------------------------------------------------------------------------
;
;	Functions 0D and above
;
upper_funcs:
	mov	bx,offset upper_dos
	xor	cx,cx
	push	cx		; no timeout
	push	cx
	push	ds		; resource address
	push	bx
	call	_request_resource
	add	sp,8
;
;	resource gained, now we may execute the function if dos is free
;
	pop	di
	pop	si
	pop	ax
	pop	bx
	pop	cx
	pop	dx
;
	call	wait_dos_free
;
	pop     ds
	pop	es
;
;
;	Filter pseudo-functions C0/C1 (Absolute Read/Write)
;
	cmp	ah,0c0h
	jne	uf_exec1
	popf
	pop	bp
	call	exec_absread
	jmp	short uf_complete
uf_exec1:
	cmp	ah,0c1h
	jne	uf_exec2
	popf
	pop	bp
	call	exec_abswrite
	jmp	short uf_complete
;
uf_exec2:
	popf
	pop	bp
	dos				; execute function
;
;	Now we have to release the resources.
;
uf_complete:
	pushf				; save registers again
	sti
	push	es
	push    ds
	push	dx
	push	cx
	push	bx
	push    ax
	push	si
	push	di
;
	mov	bx,SEG dgroup
	mov	ds,bx
	mov	es,bx
;
	mov	bx,offset upper_dos
	push	ds		; resource address
	push	bx
	call	_release_resource
	add	sp,4
;
;	If both resources are free now, clear the ctask_active flag to
;	allow other background processes to gain access to DOS.
;
	cli
	mov	ax,upper_dos.rstate
        or	ax,ax
        jz	no_relc1
	cmp	ax,lower_dos.rstate
	jne	no_relc1
	mov	cs:ctask_active,0
no_relc1:
;
;	All done, restore registers and return.
;
	pop	di
	pop	si
	pop	ax
	pop	bx
	pop	cx
	pop	dx
	pop     ds
	pop	es
	popf
;
	ret	2			; don't restore flags
;
;
dosentry	endp
;
;--------------------------------------------------------------------------
;
;	Absolute Read/Absolute Write.
;
exec_absread	proc	near

	pushf
	call	cs:savabsread
	inc	sp			; Remove flags (not using ADD,
	inc	sp			; this would clobber Carry)
	ret

exec_absread	endp
;
exec_abswrite	proc	near

	pushf
	call	cs:savabswrite
	inc	sp			; Remove flags (not using ADD,
	inc	sp			; this would clobber Carry)
	ret

exec_abswrite	endp
;
;----------------------------------------------------------------------------
;
wait_dos_free	proc	near
;
	push	es
	push	bx
in_use_loop:
	cmp	idle_active,0		; idle interrupt active?
	jne	dos_free		; then don't check for flag
;
	les	bx,in_use
	cmp	byte ptr es:[bx],0
	jne	is_in_use
	cmp	dos310,0
	je	dos_free
	les	bx,in_error
	cmp	byte ptr es:[bx],0
	je	dos_free
is_in_use:
	pushf
	call	scheduler
	jmp	in_use_loop
;
dos_free:
	pop	bx
	pop	es
	ret
;
wait_dos_free	endp
;
;----------------------------------------------------------------------------
;
;	INT 28: DOS Idle Interrupt
;
idleentry	proc	far
;
	push	ds
	push	es
	push	ax
;
	mov	ax,SEG dgroup
	mov	ds,ax
	mov	es,ax
;
;	Check if someone is waiting for upper_dos. If not, we can return
;	immediately.
;
	mov	ax,word ptr upper_dos.rwaiting
	or	ax,word ptr upper_dos.rwaiting+2
	jz	idle_exit
;
;	Also make sure this is not a second invocation of INT 28.
;	Normally, this should never happen, but better safe than sorry.
;
	cmp	idle_active,0
	jne	idle_exit
	inc	idle_active
;
;	someone is waiting, let's please him by releasing the resource
;
	mov	idle_ss,ss		; Switch to local stack
	mov	idle_sp,sp
	mov	ax,ds
	mov	ss,ax
	mov	sp,offset local_stack
;
	sti				; Interrupts allowed now
;
        push	bx			; save remaining regs
	push	cx
	push	dx
;
;	temporarily increase priority
;
	les	bx,_tsk_current
	push	es:prior[bx]
	mov	es:prior[bx],0ffffh
	push	bx
	push	es
;
;	release resource & request it again
;
	mov	ax,ds
	mov	es,ax
	mov	bx,offset upper_dos
	push	ds
	push	bx
	call	_release_resource
	add	sp,4
;
	mov	bx,offset upper_dos
	xor	cx,cx
	push	cx
	push	cx
	push	ds
	push	bx
	call	_request_resource
	add	sp,8
;
;	ready, restore priority, stack & regs
;
	cli
	pop	es
	pop	bx
	pop	es:prior[bx]
;
	pop	dx
	pop	cx
	pop	bx
;
	mov	ss,idle_ss		; restore stack
	mov	sp,idle_sp
;
	mov	idle_active,0
;
idle_exit:
	pop	ax
        pop     es
	pop	ds
;
	jmp	cs:savidle		; chain to original interrupt
;
idleentry	endp
;
;---------------------------------------------------------------------------
;
;	INT 2A: DOS Critical Section Interrupt.
;
;	Not documented.
;	Is used by DOS PRINT to mark Critical Regions.
;	Usage by PRINT:
;		AX = 8700 - Begin Critical Region
;				Returns:
;				Carry set if already active.
;		AX = 8701 - End Critical Region
;
;	Both these functions are handled here.
;
;	Other usage in DOS, function unknown:
;		AH = 82 (AL undefined)
;			seems to be called on DOS-Functions > 0C
;		AH = 84 (AL undefined)
;			seems to be called when DOS is idle
;
;	These functions are currently ignored.
;
critsectint	proc	far

	cmp	ax,8700h	; Enter critical region
	jne	csi1
	cmp	cs:critsect_active,0
	stc
	jnz	critsect_end
	cmp	cs:ctask_active,0
	stc
	jnz	critsect_end
;
	inc	cs:critsect_active
	push	ax
	push	bx
	push	cx
	push	dx
	push	ds
	push	es
	mov	ax,seg dgroup
	mov	ds,ax
	mov	es,ax
	mov	ax,word ptr _tsk_current
	mov	word ptr cs:crit_task,ax
	mov	ax,word ptr _tsk_current+2
	mov	word ptr cs:crit_task+2,ax
	mov	ax,offset critical
	push	ds
	push	ax
	call	_set_flag
	add	sp,4
	pop	es
	pop	ds
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	clc
critsect_end:
	ret	2
;
csi1:
	cmp	ax,8701h	; Leave critical region
	jne	csi2
	mov	cs:critsect_active,0
	push	ax
	push	bx
	push	cx
	push	dx
	push	ds
	push	es
	mov	ax,seg dgroup
	mov	ds,ax
	mov	es,ax
	mov	ax,offset critical
	push	ds
	push	ax
	call	_clear_flag
	add	sp,4
	pop	es
	pop	ds
	pop	dx
	pop	cx
	pop	bx
	pop	ax
csi2:
	iret
;
critsectint	endp
;
;
;---------------------------------------------------------------------------
;
        end

