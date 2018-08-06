	page	60,132

title IBM PC Communications I/O Routines -- Curt Klinsing

interrupts 	segment at 0h
		org	9h*4
keyboard_int	label	word
interrupts	ends


	include dos.mac


	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF




	PSEG

;
;
;A set	of Lattice C callable functions	to support
;interrupt driven character I/O on the	IBM PC.	Input
;is buffered, output is polled.
;
public	key_intercept	;initialize the	comm port,
public	key_nointercept	;remove	initialization,
public	key_intbcnt	;returns count of rcv chars,
public	key_intbread	;get one char from buffer,
public	key_intbflush	;flush input buffer,



;
FALSE	EQU	0
TRUE	EQU	NOT FALSE
;
;
;	MISCELLANEOUS EQUATES
;
BUFSIZ	EQU	512	;Max NUMBER OF CHARS
SetIntVect  EQU	25H	;SET INTERRUPT VECTOR FUNCTION NUMBER

;
;	DUMP BUFFER, COUNT AND POINTER.	 
;
CIRC_BUF 	DB	BUFSIZ DUP(?)	;ALLOW 512 MaxIMUM BUFFERED CHARACTERS
BUF_TOP		EQU	$ - 1		;KEEP TRACK OF THE TOP OF THE BUFFER
CIRC_TOP	DW	BUF_TOP		;
;
CIRC_IN 	DW	OFFSET CIRC_BUF	;POINTER TO LAST CHAR. PLACED IN BUFFER
CIRC_CUR DW	OFFSET CIRC_BUF	;POINTER TO NEXT CHAR. TO BE RETRIEVED FROM
				; BUFFER
CIRC_CT	DW	0		;COUNT OF CHARACTERS USED IN BUFFER
junk	dw	0,0,0
old_keyboard_int   dw 0,0,0,0	; storage for old keyboard vector
junk2	dw	0,0,0


;--------------------------------------------------------------
;
; name		key_intercept - intercept hardware key interrupt
;
; synopsis	key_intercept();
;
; description	Intercepts the hardware keyboard interrupt.  You can
;		use this routine to gain absolute control over the 
;		keyboard.  The disadvantage is that you have to work
;		with scan codes directly, not nice ascii characters.
;		Up to 512 scan codes are buffered.
;
; notes
;		The key_nointercept routine MUST be called before
;		leaving your program, or the machine will hang.
;
;		You must not attempt to use any other routine that
;		reads the keyboard, or your program will hang.  This
;		routine absolutely intercepts the keyboard.
;
;		You are in control of the keyboard, but that means that
;		it is your responsibility to manage the state of the
;		shift keys, the caps lock, and numlock keys and any other
;		command keys, like the control key.
;
;		Each key returns two scan codes, one when pressed, one
;		when released.  The code returned when the key is pressed
;		is called a MAKE code, the one returned when a key is
;		released is called a BREAK code.  The BREAK code is 
;		the same as the make code, except that the high order bit
;		is set in the BREAK code.
;
;--------------------------------------------------------------------- 	



	IF	LPROG
key_intercept	PROC	FAR
	ELSE
key_intercept	PROC	NEAR
	ENDIF



	push	bp
	cli
	push	ds

	assume ds:interrupts

	mov	ax,interrupts
	mov	ds,ax
	mov	ax,keyboard_int
	mov	old_keyboard_int,ax
	mov	ax,keyboard_int[2]
	mov	old_keyboard_int[2],ax
	mov	ax,offset IntHdlr
	mov	keyboard_int,ax
	mov	keyboard_int[2],cs

	sti
	pop	ds
	pop	bp
	ret

key_intercept endp



;------------------------------------------------------------------------
; name		key_nointercept - remove keyboard interception
;
; synopsis	VOID key_nointercept()
;
; description	Removes	the interrupt structure	installed by key_intercept.
;		Must be done before passing control to DOS, else your 
;		machine will hang.
;-----------------------------------------------------------------------

	IF	LPROG
key_nointercept	PROC	FAR
	ELSE
key_nointercept	PROC	NEAR
	ENDIF


	push	ds
	assume	ds:interrupts
	cli
	mov	ax,interrupts
	mov	ds,ax
	mov	ax,old_keyboard_int
	mov	keyboard_int,ax
	mov	ax,old_keyboard_int[2]
	mov	keyboard_int[2],ax
	pop	ds
	sti
	ret

key_nointercept endp





;----------------------------------------------------------
; 
; name		key_intbcnt - Return # chars in interrupt buffer
;
; synopsis	count = key_intbcnt();
;
; description	Returns the number of scan codes available in the
;		keyboard intercept interrupt buffer.
;
; notes		Call this to make sure that there are keystrokes
;		available before attempting to read a code out of
;		the buffer.  If there are no codes available, DO NOT
;		attempt to read the buffer.
;
;--------------------------------------------------------------------  
;


	IF	LPROG
key_intbcnt	PROC	FAR
	ELSE
key_intbcnt	PROC	NEAR
	ENDIF

	push	bp
	push	ds		; save data segment
	push	cs
	pop	ds		; move code seg	addr to	data seg reg
	mov	ax,circ_ct
	pop	ds
	pop	bp
	ret
key_intbcnt	endp





;----------------------------------------------------------------
;
; name		key_intbflush - Flush the interrupt buffer.
;
; synopsis	VOID key_intbflush()
;
; description	Flushes any key strokes in the keyboard interrupt
;		intercept buffer.
;
;------------------------------------------------------------------


	IF	LPROG
key_intbflush	PROC	FAR
	ELSE
key_intbflush	PROC	NEAR
	ENDIF


	push	bp
	push	ds		; save data reg
	push	cs
	pop	ds		; move code seg	addr to	data seg reg.
	mov	bx,offset circ_buf
	mov	circ_in,bx	
	mov	circ_cur,bx
	xor	ax,ax
	mov	circ_ct,ax
	pop	ds
	pop	bp
	ret
key_intbflush endp



;------------------------------------------------------
;
; name		key_intbread - get a char from the input buffer.
;
; synopsis	ch = key_intbread();
;
; description	returns the next character from the keyboard
;		interrupt intercept buffer. Assumes you have called
;		key_intbcnt to see if there are any characters to get.
;
; notes:
;		Will probably crash if called when the buffer is
;		empty.  Call key_intbcnt first to see if there is
;		anything available to read !!!!!!!!
;
;--------------------------------------------------------------------



	IF	LPROG
key_intbread	PROC	FAR
	ELSE
key_intbread	PROC	NEAR
	ENDIF


	push	bp
	push	ds		; save data reg
	push	cs
	pop	ds		; move code seg	addr to	data seg reg.
	mov	bx,circ_cur
	xor	ax,ax
	mov	al,[bx]		;get next char from circ_buf
	DEC	circ_ct		;decrement circ_buf COUNT
	CMP	bx,circ_top	;ARE WE	AT THE TOP OF THE circ_buf?
	JZ	reset_cur	;JUMP IF SO
	INC	bx		;ELSE, BUMP PTR
	JMP SHORT upd_cur
reset_cur:
	mov	bx,OFFSET circ_buf	;RESET circ_in TO BOTTOM OF BUF.
upd_cur:
	mov	circ_cur,bx		;SAVE NEW PTR
	pop	DS		;GET BACK ENTERING DS
	pop	bp
	ret
key_intbread endp
;


kb_ctl	equ	61h

;
IntHdlr proc far
	CLI
	push	cx
	push	dx
	push	bx
	push	ax
	push	ds
	mov	ax,cs		;get cur code segment
	mov	ds,ax		; and set it as	data segment
	mov	bx,circ_in	;GET circ_buf IN PTR


	in	al,60h
	push	ax		; save scan code
	in	al,kb_ctl
	mov	ah,al
	or	al,80h		; set "reset" bit
	out	kb_ctl,al
	xchg	ah,al
	out	kb_ctl,al
	pop	ax
	mov	ah,al

ck_full:
	cmp	circ_ct,BUFSIZ	;SEE IF	circ_buf ALREADY FULL
	jz	clnup		; JUMP IF SO, DO NOT PLACE CHARACTER IN	BFR
savch:				
	mov	[bx],AL		;SAVE NEW CHARACTER IN circ_buf
	inc	circ_ct		;BUMP circ_buf COUNT
	CMP	bx,circ_top	;ARE WE	AT THE TOP OF THE circ_buf?
	JZ	reset_in	;JUMP IF SO
	inc	bx		;ELSE, BUMP PTR
	JMP 	SHORT into_buf
reset_in:
	mov	bx,OFFSET circ_buf	;RESET circ_in TO BOTTOM OF BUF.
into_buf:
	mov	circ_in,bx		;SAVE NEW PTR
clnup:
	mov	AL,20h
	OUT	020h,AL		;ISSUE SPECIFIC	EOI FOR	8259
	pop	ds		;GET BACK ENTERING DS
	pop	ax
	pop	bx
	pop	dx
	pop	cx
	sti
	iret

IntHdlr endp

	endps
	end
