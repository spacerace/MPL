         title     ZERODIV --- Divide by zero handler
         page      55,132

; 
; ZERODIV.ASM --- Terminate-and-stay-resident handler
;                 for divide-by-zero interrupts
; 
; Copyright 1988 Ray Duncan
; 
; Build:	C>MASM ZERODIV; 
; 		C>LINK ZERODIV; 
; 		C>EXE2BIN ZERODIV.EXE ZERODIV.COM
;        	C>DEL ZERODIV.EXE
;
; Usage:	C>ZERODIV
;

cr	equ 	0dh       	; ASCII carriage return
lf      equ     0ah       	; ASCII line feed
beep	equ	07h		; ASCII bell code
backsp	equ	08h		; ASCII backspace code

_TEXT 	segment	word public 'CODE'

	org	100H      

	assume 	cs:_TEXT,ds:_TEXT,es:_TEXT,ss:_TEXT


init	proc 	near      	; entry point at load time	

				; capture vector for
				; interrupt zero ...
	mov	dx,offset zdiv	; DS:DX = handler address
	mov	ax,2500h	; Fxn 25H = set vector
				; interrupt type = 0
	int	21h		; transfer to MS-DOS

				; print sign-on message
	mov	dx,offset msg1	; DS:DX = message address
	mov	ah,9		; Fxn 09H = display string
	int	21h		; transfer to MS-DOS

				; DX = paragraphs to reserve
	mov	dx,((offset pgm_len+15)/16)+10h
	mov	ax,3100h	; Fxn 31H = terminate and
				; stay resident
	int	21h		; transfer to MS-DOS

init	endp


zdiv	proc	far		; this is the divide by 
				; zero interrupt handler

	sti			; enable interrupts

	push	ax		; save registers
	push	bx
	push	cx
	push	dx
	push	si
	push	di
	push	bp
	push	ds
	push	es

	mov	ax,cs      	; make data addressable
	mov	ds,ax

				; display message
				; "Continue or Quit?"
	mov	dx,offset msg2	; DS:DX = message address	
	mov	ah,9		; Fxn 09H = display string
	int	21h		; transfer to MS-DOS

zdiv1:	mov	ah,1		; Fxn 01H = read keyboard
	int	21h		; transfer to MS-DOS

	or	al,20h		; fold char. to lower case

	cmp	al,'c'		; is it C or Q?
	je	zdiv3		; jump, it's a C

	cmp	al,'q'
	je	zdiv2		; jump, it's a Q

				; illegal entry, send beep
				; and erase the character
	mov	dx,offset msg3	; DS:DX = message address
	mov	ah,9		; Fxn 09H = display string
	int	21h		; transfer to MS-DOS

	jmp	zdiv1		; try again

zdiv2:				; user picked "Quit"
	mov	ax,4cffh	; terminate current program
	int	21h		; with return code = 255

zdiv3:				; user picked "Continue"
				; send CR-LF pair
	mov	dx,offset msg4  ; DS:DX = message address
	mov	ah,9		; Fxn 09H = print string
	int	21h		; transfer to MS-DOS

				; what CPU type is this?
	xor	ax,ax		; to find out, we'll put
	push	ax		; zero in the CPU flags
	popf			; and see what happens
	pushf
	pop	ax
	and	ax,0f000h	; 8086/88 forces 
	cmp	ax,0f000h	; bits 12-15 true
      	je	zdiv5		; jump if 8086/88

				; otherwise we must adjust
				; return address to bypass
				; the divide instruction...
	mov	bp,sp		; make stack addressable

	lds	bx,[bp+18]	; get address of the
				; faulting instruction

	mov	bl,[bx+1]	; get addressing byte
	and	bx,0c7h		; isolate mod & r/m fields

	cmp	bl,6		; mod 0, r/m 6 = direct
	jne	zdiv4		; not direct, jump

	add	word ptr [bp+18],4
	jmp	zdiv5

zdiv4:	mov	cl,6		; otherwise isolate mod
	shr	bx,cl		; field and get instruction
	mov	bl,cs:[bx+itab]	; size from table
	add	[bp+18],bx

zdiv5:	pop	es		; restore registers
	pop	ds
	pop	bp
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	iret			; return from interrupt

zdiv	endp


msg1	db   	cr,lf		; load-time signon message
	db	'Divide by Zero Interrupt '
	db	'Handler installed.'
	db	cr,lf,'$'

msg2	db	cr,lf,lf	; interrupt-time message
	db	'Divide by Zero detected: '
	db	cr,lf,'Continue or Quit (C/Q) ? '
	db	'$'

msg3	db	beep		; used if bad entry
	db	backsp,' ',backsp,'$'

msg4	db	cr,lf,'$'	; carriage return-line feed

				; instruction size table
itab	db	2		; mod = 0
	db	3		; mod = 1
	db	4		; mod = 2
	db	2		; mod = 3

pgm_len	equ	$-init		; program length

_TEXT 	ends

	end	init

