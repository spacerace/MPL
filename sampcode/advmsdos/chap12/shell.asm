	name  	shell    
	page  	55,132
	title 	SHELL.ASM -- simple MS-DOS shell
; 
; SHELL.ASM	Simple extendable command interpreter
; 		for MS-DOS version 2.0 and later
; 
; Copyright 1988 by Ray Duncan
; 
; Build:	C>MASM SHELL; 
; 		C>LINK SHELL; 
; 
; Usage:	C>SHELL;
;

stdin	equ	0			; standard input handle
stdout	equ	1			; standard output handle
stderr	equ	2			; standard error handle

cr	equ	0dh 			; ASCII carriage return
lf      equ     0ah       		; ASCII line feed
blank	equ	20h			; ASCII blank code
escape	equ	01bh			; ASCII escape code

_TEXT 	segment	word public 'CODE'

	assume 	cs:_TEXT,ds:_DATA,ss:STACK

shell	proc	far			; at entry DS = ES = PSP

	mov	ax,_DATA		; make our data segment
	mov	ds,ax			; addressable

	mov	ax,es:[002ch]		; get environment segment
	mov	env_seg,ax		; from PSP and save it

					; release unneeded memory...
					; ES already = PSP segment
	mov  	bx,100h   		; BX = paragraphs needed
        mov     ah,4ah    		; Function 4AH = resize block
        int     21h			; transfer to MS-DOS
	jnc	shell1			; jump if resize OK

	mov	dx,offset msg1		; resize failed, display
	mov	cx,msg1_length		; error message and exit
	jmp	shell4

shell1:	call	get_comspec		; get COMMAND.COM filespec
	jnc	shell2			; jump if it was found 

	mov	dx,offset msg3		; COMSPEC not found in 
	mov	cx,msg3_length		; environment, display error
	jmp	shell4			; message and exit

shell2:	mov	dx,offset shell3	; set Ctrl-C vector (Int 23H)
	mov	ax,cs			; for this program's handler
	mov	ds,ax			; DS:DX = handler address
	mov	ax,2523H		; Fxn 25H = set vector
	int	21h			; transfer to MS-DOS

	mov	ax,_DATA		; make our data segment
	mov	ds,ax			; addressable again
	mov	es,ax

shell3:					; main interpreter loop

	call	get_cmd			; get a command from user

	call	intrinsic		; check if intrinsic function
	jnc	shell3			; yes, it was processed

	call	extrinsic		; no, pass it to COMMAND.COM 
	jmp	shell3			; then get another command

shell4: 				; come here if error detected
					; DS:DX = message address
					; CX = message length
	mov	bx,stderr		; BX = standard error handle
	mov	ah,40h			; Fxn 40H = write
	int	21h			; transfer to MS-DOS

	mov	ax,4c01h		; Fxn 4CH = terminate with
					; return code = 1
	int	21h			; transfer to MS-DOS

shell	endp


intrinsic proc	near			; Decode user entry against
					; the table "COMMANDS" 
					; If match, run the routine,
					; and return Carry = False 
					; If no match, Carry = True
					; return Carry = True.

	mov	si,offset commands	; DS:SI = command table

intr1:	cmp	byte ptr [si],0		; end of table?
	je	intr7			; jump, end of table found

	mov	di,offset inp_buf	; no, let DI = addr of user input

intr2:	cmp	byte ptr [di],blank	; scan off any leading blanks 
	jne	intr3

	inc	di			; found blank, go past it
	jmp	intr2

intr3:	mov	al,[si]			; next character from table

	or	al,al			; end of string?
	jz	intr4			; jump, entire string matched

	cmp	al,[di]			; compare to input character
	jnz	intr6			; jump, found mismatch

	inc	si			; advance string pointers
	inc	di
	jmp	intr3

intr4:	cmp	byte ptr [di],cr	; make sure user's entry
	je	intr5			; is the same length...
	cmp	byte ptr [di],blank	; next character in entry
	jne	intr6			; must be blank or Return

intr5:	call	word ptr [si+1]		; run the command routine

	clc				; return Carry Flag = False
	ret				; as success flag

intr6:	lodsb				; look for end of this
	or	al,al			; command string (null byte)
	jnz	intr6			; not end yet, loop

	add	si,2			; skip over routine address
	jmp	intr1			; try to match next command

intr7:	stc				; command not matched, exit
	ret				; with Carry = True

intrinsic endp


extrinsic proc	near			; process extrinsic command
					; by passing it to 
					; COMMAND.COM with a
					; " /C " command tail.

	mov	al,cr			; find length of command
	mov	cx,cmd_tail_length	; by scanning for carriage
	mov	di,offset cmd_tail+1	; return
	cld
	repnz scasb

	mov	ax,di			; calculate command tail
	sub	ax,offset cmd_tail+2	; length without carriage
	mov	cmd_tail,al		; return, and store it

					; set command tail address
	mov	word ptr par_cmd,offset cmd_tail
	call	exec			; and run COMMAND.COM
	ret

extrinsic endp


get_cmd	proc	near			; prompt user, get command

					; display the shell prompt
	mov	dx,offset prompt	; DS:DX = message address
	mov	cx,prompt_length	; CX = message length
	mov	bx,stdout		; BX = standard output handle
	mov	ah,40h			; Fxn 40H = write
	int	21h			; transfer to MS-DOS

					; get entry from user
	mov	dx,offset inp_buf	; DS:DX = input buffer
	mov	cx,inp_buf_length	; CX = max length to read
	mov	bx,stdin		; BX = standard input handle
	mov	ah,3fh			; Fxn 3FH = read
	int	21h			; transfer to MS-DOS

	mov	si,offset inp_buf	; fold lower case characters
	mov	cx,inp_buf_length	; in entry to upper case

gcmd1:	cmp	byte ptr [si],'a'	; check if 'a-z'
	jb	gcmd2			; jump, not in range
	cmp	byte ptr [si],'z'	; check if 'a-z'	
	ja	gcmd2			; jump, not in range
	sub	byte ptr [si],'a'-'A'	; convert to upper case

gcmd2:	inc	si			; advance through entry
	loop	gcmd1
	ret				; back to caller

get_cmd	endp


get_comspec proc near			; get location of COMMAND.COM
					; from environment "COMSPEC=" 
					; Returns Carry = False 
					; if COMSPEC found.
					; Returns Carry=True 
					; if no COMSPEC.

	mov	si,offset com_var	; DS:SI = string to match...
	call	get_env			; search Environment Block
	jc	gcsp2			; jump if COMSPEC not found

					; ES:DI points past "="
	mov 	si,offset com_spec	; DS:SI = local buffer

gcsp1:	mov	al,es:[di]		; copy COMSPEC variable
	mov	[si],al			; to local buffer
	inc	si
	inc	di
	or	al,al			; null char? (turns off Carry)
	jnz	gcsp1			; no, get next character

gcsp2:	ret				; back to caller

get_comspec endp


get_env	proc	near			; search environment
					; Call DS:SI = "NAME=" 
					; uses contents of "ENV_SEG" 
					; Returns Carry=False and ES:DI 
					; pointing to parameter if found,
					; Returns Carry=True if no match

	mov	es,env_seg		; get environment segment
	xor	di,di			; initialize env. offset 

genv1:	mov	bx,si			; initialize pointer to name
	cmp	byte ptr es:[di],0	; end of environment?
	jne	genv2			; jump, end not found

	stc				; no match, return Carry set
	ret

genv2:	mov	al,[bx]			; get character from name
	or	al,al			; end of name? (turns off Carry)
	jz	genv3			; yes, name matched

	cmp	al,es:[di]		; compare to environment
	jne	genv4			; jump if match failed

	inc	bx			; advance environment
	inc	di			; and name pointers
	jmp	genv2

genv3:					; match found, Carry = clear,
	ret 				; ES:DI = variable

genv4:	xor	al,al			; scan forward in environment
	mov	cx,-1			; for zero byte
	cld
	repnz	scasb
	jmp	genv1			; go compare next string

get_env	endp


exec	proc	near			; call MS-DOS EXEC function
					; to run COMMAND.COM. 

	mov  	stkseg,ss		; save stack pointer
        mov     stkptr,sp

					; now run COMMAND.COM
	mov	dx,offset com_spec	; DS:DX = filename
	mov	bx,offset par_blk	; ES:BX = parameter block
	mov	ax,4b00h		; Fxn 4BH = EXEC
					; Subf. 0 = load and execute
	int	21h			; transfer to MS-DOS

	mov	ax,_DATA		; make data segment
	mov	ds,ax			; addressable again
	mov	es,ax

	cli		 		; (for bug in some 8088s)
	mov	ss,stkseg		; restore stack pointer
	mov	sp,stkptr
	sti				; (for bug in some 8088s)

	jnc	exec1			; jump if no errors

					; display error message
	mov	dx,offset msg2		; DS:DX = message address
	mov	cx,msg2_length		; CX = message length
	mov	bx,stderr		; BX = standard error handle
	mov	ah,40h			; Fxn 40H = write
	int	21h			; transfer to MS-DOS

exec1:	ret				; back to caller

exec	endp


cls_cmd	proc	near			; intrinsic CLS command 

	mov	dx,offset cls_str	; send the ANSI escape
	mov	cx,cls_str_length	; sequence to clear
	mov	bx,stdout		; the screen	
	mov	ah,40h
	int	21h
	ret

cls_cmd	endp


dos_cmd	proc	near			; intrinsic DOS command 

					; set null command tail
	mov	word ptr par_cmd,offset nultail
	call	exec			; and run COMMAND.COM
	ret

dos_cmd	endp


exit_cmd proc	near			; intrinsic EXIT Command 

	mov	ax,4c00h		; call MS-DOS terminate
	int	21h			; function with 
					; return code of zero
exit_cmd endp

_TEXT	ends


STACK	segment	para stack 'STACK'	; declare stack segment

	dw	64 dup (?)

STACK	ends


_DATA	segment	word public 'DATA'

commands equ $				; "intrinsic" commands table
					; each entry is ASCIIZ string
					; followed by the offset
					; of the procedure to be 
					; executed for that command.
	db	'CLS',0
	dw	cls_cmd

	db	'DOS',0
	dw	dos_cmd

	db	'EXIT',0
	dw	exit_cmd

	db	0			; end of table

com_var db	'COMSPEC=',0		; environment variable

					; COMMAND.COM filespec 
com_spec db	80 dup (0)		; from environment COMSPEC=

nultail	db	0,cr			; null command tail for
					; invoking COMMAND.COM
					; as another shell

cmd_tail db	0,' /C '		; command tail for invoking 
					; COMMAND.COM  as a transient

inp_buf	db	80 dup (0)		; command line from Standard Input

inp_buf_length equ $-inp_buf		
cmd_tail_length equ $-cmd_tail-1

prompt	db	cr,lf,'sh: '		; SHELL's user prompt
prompt_length equ $-prompt

env_seg	dw	0			; segment of Environment Block

msg1	db	cr,lf
	db	'Unable to release memory.'
	db	cr,lf
msg1_length equ $-msg1

msg2	db	cr,lf
	db	'EXEC of COMMAND.COM failed.'
	db	cr,lf
msg2_length equ $-msg2

msg3	db	cr,lf
	db	'No COMSPEC variable in environment.'
	db	cr,lf
msg3_length equ $-msg3

cls_str	db	escape,'[2J'		; ANSI escape sequence
cls_str_length equ $-cls_str		; to clear the screen

					; EXEC parameter block
par_blk	dw	0			; environment segment
par_cmd	dd	cmd_tail		; command line
	dd	fcb1			; file control block #1
	dd	fcb2			; file control block #2

fcb1	db	0			; file control block #1
	db	11 dup (' ')
	db	25 dup (0)

fcb2	db	0			; file control block #2
	db	11 dup (' ')
	db	25 dup (0)

stkseg	dw	0         		; original SS contents
stkptr 	dw      0         		; original SP contents

_DATA	ends

	end	shell
