;
;	CTask - BIOS INT 15 interrupt handler (IBM AT specific)
;
;	Public Domain Software written by
;		Thomas Wagner
;		Patschkauer Weg 31
;		D-1000 Berlin 33
;		West Germany
;
	name	tskbios
	.model	large
;
	public	_tsk_install_bios
	public	_tsk_remove_bios
;
	include	tsk.mac
;
intseg	segment at 0
;
		org	15h*4
pintofs		dw	?		; interrupt entry
pintseg		dw	?

intseg	ends
;
;
	.data?
;
	extrn	_ticks_per_sec: word
;
floppy  flag    <>
fdisk   flag    <>
kbd     flag    <>
;
	.data
;
flag_tab	label	word
	dw	offset fdisk
	dw	offset floppy
	dw	offset kbd
;
	IF	TSK_NAMEPAR
fflname	db	"FLOPPY",0
ffdname	db	"HARDDISK",0
fkbname	db	"KBDPOST",0
	ENDIF
;
	.code
;
	extrn	_clear_flag_wait_set: far
	extrn	_set_flag: far
        extrn   _create_flag: far
        extrn   _delete_flag: far
        extrn   _t_delay: far
;
post_save	label	dword		; in CSEG to allow addressing
psofs		dw	?
psseg		dw	?
;
;----------------------------------------------------------------------
;
;	interrupt handler
;
bios_int	proc	far
        pushf
	sti
        cmp     ah,90h			; WAIT
        jb      bios_pass
        cmp     ah,91h			; POST
        ja	bios_pass
	cmp	al,2			; fdisk/floppy/keyboard
        jbe	process_bios
	cmp	ah,91h
	je	bios_pass
	cmp	al,0fdh			; floppy motor
	jb	bios_pass
	cmp	al,0feh			; printer
        jbe	process_bios
;
bios_pass:
	popf
	jmp	cs:post_save
;
process_bios:
	popf
	sti
	push	es
	push	ds
	push	dx
	push	cx
	push	bx
	push	ax
	mov	bx,SEG dgroup
	mov	ds,bx
	mov	es,bx
;
	cmp	al,2
	jbe	flag_ops
;
	cmp	al,0fdh
	je	wait_motor
;
;	wait for printer ready.
;	delay for some ticks, then check printer.
;	CAUTION: this assumes the BIOS is *truly* AT compatible,
;		 with the printer port address in DX on entry to INT15.
;
wait_prn:
	push	dx
	mov	ax,3
	push	ax
	xor	ax,ax
	push	ax
	call	_t_delay
	add	sp,4
	pop	dx	
	in	al,dx
	test	al,80h
	jz	wait_prn
	stc
;
bios_ret:
	pop	ax
	pop	bx
	pop	cx
	pop	dx
	pop	ds
	pop	es
	ret	2
;
;	Wait for diskette motor start (1 sec.)
;
wait_motor:
	xor	ax,ax
	push	ax
	mov	ax,_ticks_per_sec
	push	ax
	call	_t_delay
	add	sp,4
	stc
	jmp	bios_ret
;
;	Floppy/Fdisk/Keyboard wait/post
;
flag_ops:
	xor	bh,bh
	mov	bl,al	
	add	bx,bx
	mov	bx,flag_tab[bx]
	cmp	ah,90h
	je	fl_wait
	push	ds
	push	bx
	call	_set_flag
	add	sp,4
	clc
	jmp	bios_ret
;
fl_wait:
	xor	dx,dx
	cmp	al,2
	je	fl_dowait
	mov	dx,_ticks_per_sec
	add	dx,dx
	cmp	al,1
	je	fl_dowait
	mov	cx,dx
	add	dx,cx
	add	dx,cx
fl_dowait:
	xor	ax,ax
	push	ax
	push	dx
	push	ds
	push	bx
	call	_clear_flag_wait_set
	add	sp,8
	cmp	ax,0
	je	bios_ret
	stc
	jmp	bios_ret
;
bios_int       endp
;
;------------------------------------------------------------------------
;
;	void far _tsk_remove_bios (void)
;
;	This routine un-installs the int handler.
;
_tsk_remove_bios    proc    far
;
	push	es
	xor	ax,ax
	mov	es,ax
	assume	es:intseg
        cli
	mov	ax,cs:psofs		; restore vector
	mov	pintofs,ax
	mov	ax,cs:psseg
	mov	pintseg,ax
        sti
	assume	es:nothing
	pop	es
;
        mov     ax,offset floppy
        push    ds
        push    ax
        call    _delete_flag
        add     sp,4
;
        mov     ax,offset fdisk
        push    ds
        push    ax
        call    _delete_flag
        add     sp,4
;
        mov     ax,offset kbd
        push    ds
        push    ax
        call    _delete_flag
        add     sp,4
;
        ret
;
_tsk_remove_bios	endp
;
;----------------------------------------------------------------------
;
;	void far _tsk_install_bios (void)
;
;	This routine installs the int handler.
;
_tsk_install_bios	proc	far
;
	IF	TSK_NAMEPAR
	mov	ax,offset fflname
	push	ds
	push	ax
	ENDIF
        mov     ax,offset floppy
        push    ds
        push    ax
        call    _create_flag
	IF	TSK_NAMEPAR
	add	sp,8
	ELSE
        add     sp,4
	ENDIF
;
	IF	TSK_NAMEPAR
	mov	ax,offset ffdname
	push	ds
	push	ax
	ENDIF
        mov     ax,offset fdisk
        push    ds
        push    ax
        call    _create_flag
	IF	TSK_NAMEPAR
	add	sp,8
	ELSE
        add     sp,4
	ENDIF
;
	IF	TSK_NAMEPAR
	mov	ax,offset fkbname
	push	ds
	push	ax
	ENDIF
        mov     ax,offset kbd
        push    ds
        push    ax
        call    _create_flag
	IF	TSK_NAMEPAR
	add	sp,8
	ELSE
        add     sp,4
	ENDIF
;
        push    es
	xor	ax,ax
	mov	es,ax			; establish addressing for intseg
	assume	es:intseg
;
	mov	ax,pintofs		; save old timer int addr
	mov	psofs,ax
	mov	ax,pintseg
	mov	psseg,ax
	cli
	mov	pintofs,offset bios_int ; set new timer int addr
	mov	pintseg,cs
	sti
	assume	es:nothing
        pop     es
	ret
;
_tsk_install_bios	endp
;
	end

