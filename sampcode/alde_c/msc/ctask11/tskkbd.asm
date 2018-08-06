;
;	CTask - Keyboard handler module.
;
;	Public Domain Software written by
;		Thomas Wagner
;		Patschkauer Weg 31
;		D-1000 Berlin 33
;		West Germany
;
;	This module traps the keyboard interrupts to allow task switching
;	on waiting for a character.
;	All characters are put into a (word)pipe, and can either be
;	read directly from the pipe, or through the normal INT 16 channels.
;
	name	tskkbd
	.model	large
;
	public	_tsk_install_kbd
	public	_tsk_remove_kbd
;
	public	_t_read_key
	public	_t_wait_key
	public	_t_keyhit
;
	include	tsk.mac
;
BUFSIZE	=	80	; max. number of chars in pipe
;
intseg	segment at 0
		org	09h*4
hwdoff		dw	?	; keyboard hardware interrupt
hwdseg		dw	?
		org	16h*4
kbdoff		dw	?	; keyboard I/O interrupt
kbdseg		dw	?
;
intseg	ends
;
;----------------------------------------------------------------------------
;
;	Variables
;
	IF	TSK_NAMEPAR
	.data
kbd_name	db	"KBDBUF",0
	ENDIF
;
	.data?
;
key_avail	wpipe <>
;
buffer		dw	BUFSIZE dup(?)
;
	.code
;
kbd_flag	db	0
;
;	Original Interrupt-Entries
;
savhwd		label	dword		; original hardware int entry
savhwdoff	dw	?
savhwdseg	dw	?
;
savkbd		label	dword		; original keyboard I/O entry
savkbdoff	dw	?
savkbdseg	dw	?
;
;
;---------------------------------------------------------------------------
;
	.code
;
	extrn	_create_wpipe: far
	extrn	_delete_wpipe: far
	extrn	_read_wpipe: far
	extrn	_check_wpipe: far
	extrn	_c_write_wpipe: far
;
;
;	void tsk_install_kbd (void)
;
;		Install keyboard handler
;
_tsk_install_kbd	proc	far
;
	IF	TSK_NAMEPAR
	mov	ax,offset kbd_name
	push	ds
	push	ax
	ENDIF
	mov	ax,BUFSIZE
	push	ax
	mov	ax,offset buffer
	push	ds
	push	ax
	mov	ax,offset key_avail
	push	ds
	push	ax
	call	_create_wpipe
	IF	TSK_NAMEPAR
	add	sp,14
	ELSE
	add	sp,10
	ENDIF
;
;	Save old interrupt vectors
;
        push    es
	xor	ax,ax
	mov	es,ax
;
        assume  es:intseg
;
	mov	ax,kbdoff
	mov	savkbdoff,ax
	mov	ax,kbdseg
	mov	savkbdseg,ax
;
	mov	ax,hwdoff
	mov	savhwdoff,ax
	mov	ax,hwdseg
	mov	savhwdseg,ax
;
;	Enter new Interrupt-Entries
;
	cli
	mov	kbdoff,offset kbdentry
	mov	kbdseg,cs
	mov	hwdoff,offset hwdentry
	mov	hwdseg,cs
	sti
        pop     es
;
	ret
;
	assume	es:nothing
;
_tsk_install_kbd	endp
;
;
;	void tsk_remove_kbd (void)
;
;		Un-install keyboard handler
;
_tsk_remove_kbd	proc	far
;
;	Delete pipe
;
	mov	ax,offset key_avail
	push	ds
	push	ax
	call	_delete_wpipe
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
;
	mov	ax,savkbdoff
	mov	kbdoff,ax
	mov	ax,savkbdseg
	mov	kbdseg,ax
;
	mov	ax,savhwdoff
	mov	hwdoff,ax
	mov	ax,savhwdseg
	mov	hwdseg,ax
;
	sti
;
        pop     es
	ret
;
	assume	es:nothing
;
_tsk_remove_kbd	endp
;
;
;---------------------------------------------------------------------------
;
;	int t_read_key (void)
;
;	Waits for key from keyboard. Returns char in lower byte,
;	Scan-Code in upper byte.
;
_t_read_key	proc	far
;
	xor	ax,ax		; no timeout
	push	ax
	push	ax
	mov	ax,offset key_avail
	push	ds
	push	ax
	call	_read_wpipe
	add	sp,8
	ret
;	
_t_read_key	endp
;
;
;	int t_wait_key (dword timeout)
;
;	Waits for key from keyboard. Returns char in lower byte,
;	Scan-Code in upper byte.
;
_t_wait_key	proc	far
;
	push	bp
	mov	bp,sp
	push	8[bp]		; timeout
	push	6[bp]
	mov	ax,offset key_avail
	push	ds
	push	ax
	call	_read_wpipe
	add	sp,8
	pop	bp
	ret
;	
_t_wait_key	endp
;
;
;	int t_keyhit (void)
;
;	Checks if char is available. Returns -1 if not, else the
;	character value. The character remains in the buffer.
;
_t_keyhit	proc	far
;
	mov	ax,offset key_avail
	push	ds
	push	ax
	call	_check_wpipe
	add	sp,4
	ret
;
_t_keyhit	endp
;
;---------------------------------------------------------------------------
;
;	INT 9 - Keyboard hardware interrupt
;
hwdentry	proc	far
;
	pushf
	call	cs:savhwd	; process key
	push	ds
	push	es
	push	ax
	push	bx
	push	cx
	push	dx
	mov	ax,seg dgroup
	mov	ds,ax
	mov	es,ax
;
	mov	ah,1
	pushf
	call	cs:savkbd	; check if char available
	jz	hwd_ret		; return if not
;
	xor	ah,ah	
	pushf
	call	cs:savkbd	; get the key
;
	push	ax		; push key
	mov	ax,offset key_avail
	push	ds
	push	ax
	call	_c_write_wpipe
	add	sp,6
;
hwd_ret:
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	pop	es
	pop	ds
	iret
;
hwdentry	endp
;
;---------------------------------------------------------------------------
;
;	INT 16 - Keyboard I/O
;
kbdentry	proc	far
;
	cmp	ah,2
	jb	kbd_funcs
	jmp	cs:savkbd	; pass on functions >= 2
;	
kbd_funcs:
	sti
	push	ds
	push	es
	push	dx
	push	cx
	push	bx
	mov	bx,seg dgroup
	mov	ds,bx
	mov	es,bx
;
	cmp	ah,1
	jb	kbd_read
;
	mov	bx,offset key_avail
	push	ds
	push	bx
	call	_check_wpipe
	add	sp,4
	cmp	ax,0ffffh
	jne	kbd_gotone
	xor	ax,ax
kbd_gotone:
	pop	bx
	pop	cx
	pop	dx
	pop	es
	pop	ds
	ret	2
;
kbd_read:
	xor	ax,ax
	push	ax
	push	ax
	mov	bx,offset key_avail
	push	ds
	push	bx
	call	_read_wpipe
	add	sp,8
	pop	bx
	pop	cx
	pop	dx
	pop	es
	pop	ds
	iret
;
kbdentry	endp
;
	end

