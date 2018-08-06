	name	parent
	title	'PARENT --- demonstrate EXEC call'
;
; PARENT.EXE --- demonstration of EXEC to run process
;
; Uses MS-DOS EXEC (Int 21H, Function 4BH Subfunction 00H)
; to load and execute a child process named CHILD.EXE,
; then displays CHILD's return code.
;
; Ray Duncan, June 1987
;

stdin	equ	0	; standard input
stdout	equ	1	; standard output
stderr	equ	2	; standard error
 
stksize equ	128	; size of stack

cr	equ	0dh	; ASCII carriage return
lf	equ	0ah	; ASCII linefeed


DGROUP	group	_DATA,_ENVIR,_STACK


_TEXT	segment byte public 'CODE'	; executable code segment

	assume  cs:_TEXT,ds:_DATA,ss:_STACK


stk_seg dw	?	; original SS contents
stk_ptr dw	?	; original SP contents


main	proc	far	; entry point from MS-DOS

	mov	ax,_DATA	; set DS = our data segment
	mov	ds,ax

			; now give back extra memory
			; so child has somewhere to 
			; run...
	mov	ax,es	; let AX = segment of PSP base
	mov	bx,ss	; and BX = segment of stack base
	sub	bx,ax	; reserve seg stack - seg psp
	add	bx,stksize/16	; plus paragraphs of stack
	mov	ah,4ah	; fxn 4ah = modify memory 
			; block
	int	21h
	jc	main1
			; display parent message ...
	mov	dx,offset DGROUP:msg1	; DS:DX = address of message
	mov	cx,msg1_len	; CX = length of message
	call	pmsg

	push	ds	; save parent's data segment
	mov	stk_seg,ss	; save parent's stack pointer
	mov	stk_ptr,sp

			; now EXEC the child process...
	mov	ax,ds	; set ES = DS
	mov	es,ax
	mov	dx,offset DGROUP:cname	; DS:DX = child pathname
	mov	bx,offset DGROUP:pars	; EX:BX = parameter block
	mov	ax,4b00h	; function 4BH, subfunction 00H
	int	21h	; transfer to MS-DOS

	cli		; (for bug in some early 8088s)
	mov	ss,stk_seg	; restore parent's stack  
			; pointer
	mov	sp,stk_ptr
	sti		; (for bug in some early 8088s)
	pop	ds	; restore DS = our data segment

	jc	main2	; jump if EXEC failed

			; otherwise EXEC succeeded,
			; convert and display child's
			; termination and return 
			; codes...
	mov	ah,4dh	; fxn 4dh = get return code
	int	21h	; transfer to MS-DOS
	xchg	al,ah	; convert termination code
	mov	bx,offset DGROUP:msg4a
	call	b2hex
	mov	al,ah	; get back return code
	mov	bx,offset DGROUP:msg4b	; and convert it
	call	b2hex
	mov	dx,offset DGROUP:msg4	; DS:DX = address of message
	mov	cx,msg4_len	; CX = length of message
	call	pmsg	; display it

	mov	ax,4c00h	; no error, terminate program
	int	21h	; with return code = 0

main1:	mov	bx,offset DGROUP:msg2a	; convert error code
	call	b2hex
	mov	dx,offset DGROUP:msg2	; display message 'Memory
	mov	cx,msg2_len	; resize failed...'
	call	pmsg
	jmp	main3

main2:	mov	bx,offset DGROUP:msg3a	; convert error code
	call	b2hex
	mov	dx,offset DGROUP:msg3	; display message 'EXEC
	mov	cx,msg3_len	; call failed...'
	call	pmsg

main3:	mov	ax,4c01h	; error, terminate program
	int	21h	; with return code = 1

main	endp		; end of main procedure


b2hex	proc	near	; convert byte to hex ASCII
			; call with AL=binary value
			;    BX=addr to store string
	push	ax
	shr	al,1
	shr	al,1
	shr	al,1
	shr	al,1
	call	ascii	; become first ASCII character
	mov	[bx],al	; store it
	pop	ax
	and	al,0fh	; isolate lower 4 bits, which
	call	ascii	; become the second ASCII 
			; character
	mov	[bx+1],al	; store it
	ret
b2hex	endp


ascii	proc	near	; convert value 00-0FH in AL
	add	al,'0'	; into a "hex ASCII" character
	cmp	al,'9'
	jle	ascii2	; jump if in range 00-09H,
	add	al,'A'-'9'-1	; offset it to range 0A-0FH,

ascii2:	ret		; return ASCII char. in AL.
ascii   endp


pmsg	proc	near	; displays message on 
			; standard output
			; call with DS:DX = address,
			;           	       CX = length

	mov	bx,stdout	; BX = standard output handle
	mov	ah,40h	; function 40h = write 
			; file/device
	int	21h	; transfer to MS-DOS
	ret		; back to caller

pmsg	endp

_TEXT	ends


_DATA	segment para public 'DATA'	; static & variable data segment

cname	db	'CHILD.EXE',0	; pathname of child process

pars	dw	_ENVIR	; segment of environment block
	dd	tail	; long address, command tail
	dd	fcb1	; long address, default FCB #1
	dd	fcb2	; long address, default FCB #2

tail	db	fcb1-tail-2	; command tail for child
	db	'dummy command tail',cr
			
fcb1	db	0	; copied into default FCB #1 in
	db	11 dup (' ')	; child's program segment prefix
	db	25 dup (0)

fcb2	db	0	; copied into default FCB #2 in
	db	11 dup (' ')	; child's program segment prefix
	db	25 dup (0)

msg1	db	cr,lf,'Parent executing!',cr,lf
msg1_len equ 	$-msg1

msg2	db	cr,lf,'Memory resize failed, error code='
msg2a	db	'xxh.',cr,lf
msg2_len equ 	$-msg2

msg3	db	cr,lf,'EXEC call failed, error code='
msg3a	db	'xxh.',cr,lf
msg3_len equ 	$-msg3

msg4	db	cr,lf,'Parent regained control!'
	db	cr,lf,'Child termination type='
msg4a 	db	'xxh, return code='
msg4b	db	'xxh.',cr,lf
msg4_len equ 	$-msg4

_DATA	ends


_ENVIR  segment para public 'DATA'	; example environment block
			; to be passed to child

	db	'PATH=',0	; basic PATH, PROMPT,
	db	'PROMPT=$p$_$n$g',0	; and COMSPEC strings
	db	'COMSPEC=C:\COMMAND.COM',0
	db	0	; extra null terminates block

_ENVIR	ends


_STACK	segment para stack 'STACK'


	db	stksize dup (?)

_STACK	ends


	end	main	; defines program entry point

