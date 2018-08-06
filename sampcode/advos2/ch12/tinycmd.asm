	title	TINYCMD -- Simple Command Interpreter
        page    55,132
	.286

;
; TINYCMD.ASM
;
; A simple command interpreter for OS/2.
;
; Assemble with:  C> masm tinycmd.asm;
; Link with:  C> link tinycmd,,,os2,tinycmd
; 
; Usage is:  C> tinycmd
;
; Copyright (C) 1988 Ray Duncan
;

cr      equ     0dh                     ; ASCII carriage return
lf	equ	0ah			; ASCII linefeed
tab     equ     09h                     ; ASCII tab
blank   equ     20h                     ; ASCII blank code
escape  equ     01bh                    ; ASCII escape code

inpsize equ     80                      ; maximum input length

        extrn   DosExecPgm:far          ; OS/2 API functions
        extrn   DosExit:far
        extrn   DosGetVersion:far
        extrn   KbdStringIn:far
        extrn   VioWrtTTY:far

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

; Intrinsic commands table: Each entry is an ASCIIZ string
; followed by the offset of the corresponding procedure.

commands label  byte
	db	'CLS',0 		; clear screen command
        dw      cls_cmd
        db      'VER',0                 ; display OS/2 version
        dw      ver_cmd
        db      'EXIT',0                ; exit from TINYCMD
        dw      exit_cmd
        db      0                       ; end of table

input   db      (inpsize+2) dup (0)     ; keyboard input buffer
ibinfo  dw      inpsize,0               ; input buffer info

pbuff   db      (inpsize+2) dup (0)     ; program name moved here

cinfo   dw      0                       ; child termination type
        dw      0                       ; child return code

prompt  db      cr,lf,'>> '             ; TINYCMD's user prompt
pr_len  equ     $-prompt

delims  db      0,blank,tab,';,='       ; delimiters for first  
de_len  equ     $-delims                ; command line token

pext    db      '.EXE',0                ; program file extension
pe_len equ      $-pext

wlen    dw      0                       ; receives bytes written
verinfo db      0,0                     ; receives OS/2 version

msg1    db      cr,lf,lf                ; error message
        db      'Bad command or filename'
        db      cr,lf
msg1_len equ    $-msg1

msg2    db      escape,'[2J'            ; ANSI escape sequence
msg2_len equ    $-msg2                  ; to clear the screen

msg3    db      cr,lf,lf                ; version number message
        db      'OS/2 version '
msg3a   db      'nn.'
msg3b   db      'nn'
        db      cr,lf
msg3_len equ    $-msg3

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP,es:DGROUP

main    proc    far                     ; entry point from OS/2

        push    ds                      ; make DGROUP addressable
        pop     es                      ;  with ES too
        cld                             ; clear direction flag

main1:                                  ; main interpreter loop
        call    gcmd                    ; get command from user

        call    intrinsic               ; check if intrinsic function
        jnc     main1                   ; yes, it was processed

	call	extrinsic		; no, run EXE file, then
        jmp     main1                   ; get another command

main    endp

; Try to match first command token against COMMANDS table.
; If match, run the routine, return Carry = False.
; If no match, return Carry = True.

intrinsic proc  near
                                
                                        ; point to command table
        mov     si,offset DGROUP:commands

intr1:                                  ; try next table entry...
        cmp     byte ptr [si],0         ; end of entire table?
        je      intr6                   ; jump if table exhausted

        mov     di,offset DGROUP:input  ; point to user's command

intr2:  mov     al,[si]                 ; next character from table

        or      al,al                   ; end of table entry?
        jz      intr3                   ; yes, jump

        cmp     al,[di]                 ; compare to input character
        jnz     intr4                   ; jump, found mismatch

        inc     si                      ; advance string pointers
        inc     di
        jmp     intr2

intr3:  cmp     byte ptr [di],0         ; user's entry same length?
        jne     intr5                   ; no, not a match

        call    word ptr [si+1]         ; run the command routine

        clc                             ; return Carry = False
        ret                             ; as success signal

intr4:  lodsb                           ; look for end of this
        or      al,al                   ; command string (null byte)
        jnz     intr4                   ; not end yet, loop

intr5:  add     si,2                    ; skip over routine address
        jmp     intr1                   ; try to match next command

intr6:  stc                             ; command not matched, exit
        ret                             ; with Carry = True

intrinsic endp

; Append .EXE to first token and attempt to execute program, 
; passing address of argument strings block and null pointer 
; for environment (child process inherits TINYCMD's environment).

extrinsic proc  near

        mov     si,offset DGROUP:input  ; copy first token of
        mov     di,offset DGROUP:pbuff  ; user command to pbuff

extr1:  lodsb                           ; get next character
        or      al,al                   ; found null?
        jz      extr2                   ; yes, end of token
        stosb                           ; no, copy character
        jmp     extr1                   ; and get next char.

extr2:  mov     si,offset DGROUP:pext   ; append .EXE extension
        mov     cx,pe_len               ; to token, forming 
        rep movsb                       ; program filename

                                        ; try and run program...
        push    0                       ; object name buffer
        push    0
        push    0                       ; object name buffer length
	push	0			; asynchronous execution mode
	push	ds			; address of argument strings
        push    offset DGROUP:input
        push    0                       ; environment pointer
        push    0
        push    ds                      ; receives termination info
        push    offset DGROUP:cinfo
        push    ds                      ; address of program name
        push    offset DGROUP:pbuff
        call    DosExecPgm              ; transfer to OS/2
        or      ax,ax                   ; was function successful?
        jz      extr3                   ; yes, jump

                                        ; no, display error message...
        push    ds                      ; message address
        push    offset DGROUP:msg1
        push    msg1_len                ; message length
        push    0                       ; default video handle
        call    VioWrtTTY               ; transfer to OS/2

extr3:  ret                             ; back to caller

extrinsic endp

; Get input from user, convert it to argument strings block
; by breaking out first token with null and appending double
; null to entire line, and convert first token to uppercase.

gcmd    proc    near                    ; prompt user, get command

                                        ; display TINYCMD prompt...
        push    ds                      ; address of prompt
        push    offset DGROUP:prompt
        push    pr_len                  ; length of prompt
        push    0                       ; default video handle
        call    VioWrtTTY

        mov     ibinfo+2,0              ; disable buffer editing

                                        ; get entry from user...
        push    ds                      ; address of input buffer
        push    offset DGROUP:input
        push    ds                      ; input buffer info
        push    offset DGROUP:ibinfo
        push    0                       ; 0 = wait for input
        push    0                       ; default keyboard handle
        call    KbdStringIn

        mov     cx,inpsize              ; look for carriage return
        mov     al,cr                   ; if any...     
        mov     di,offset DGROUP:input
        repnz scasb
        jnz     gcmd1
        dec     di
gcmd1:	mov	word ptr [di],0 	; and replace with 2 nulls
        mov     si,offset DGROUP:input  ; break out first token...

gcmd2:  lodsb                           ; compare next character 
        mov     di,offset DGROUP:delims ; to delimiter table
        mov     cx,de_len
        repnz scasb
        jnz     gcmd2                   ; jump, no match
        mov     byte ptr [si-1],0       ; replace delimiter with 0

	mov	si,offset DGROUP:input	; fold token to uppercase

gcmd3:  cmp     byte ptr [si],0         ; end of token?
        jz      gcmd5                   ; found end, jump
        cmp     byte ptr [si],'a'       
        jb      gcmd4                   ; jump if not 'a-z'
        cmp     byte ptr [si],'z'
        ja      gcmd4                   ; jump if not 'a-z'
	sub	byte ptr [si],'a'-'A'	; convert to uppercase

gcmd4:  inc     si                      ; go to next character
        jmp     gcmd3

gcmd5:  ret                             ; back to caller

gcmd    endp


cls_cmd proc    near                    ; intrinsic CLS command 

                                        ; send ANSI escape sequence
                                        ; to clear the screen...
        push    ds                      ; escape sequence address
        push    offset DGROUP:msg2
        push    msg2_len                ; escape sequence length
        push    0                       ; default video handle
        call    VioWrtTTY               ; transfer to OS/2
        ret                             ; back to caller

cls_cmd endp


ver_cmd proc    near                    ; intrinsic VER command

                                        ; get OS/2 version number...
        push    ds                      ; receives version info
        push    offset DGROUP:verinfo
        call    DosGetVersion           ; transfer to OS/2

                                        ; format the version...
        mov     al,verinfo              ; convert minor version 
        aam                             ; number to ASCII
        add     ax,'00'
        xchg    al,ah
        mov     word ptr msg3b,ax       ; store ASCII characters

        mov     al,verinfo+1            ; convert major version 
        aam                             ; number to ASCII
        add     ax,'00'
        xchg    al,ah
        mov     word ptr msg3a,ax       ; store ASCII characters

                                        ; display version number...     
        push    ds                      ; message address
        push    offset DGROUP:msg3
        push    msg3_len                ; message length
        push    0                       ; default video handle
        call    VioWrtTTY               ; transfer to OS/2

        ret                             ; back to caller

ver_cmd endp


exit_cmd proc   near                    ; intrinsic EXIT Command 

                                        ; final exit to OS/2...
        push    1                       ; terminate all threads
        push    0                       ; return code = 0
        call    DosExit                 ; transfer to OS/2

exit_cmd endp

_TEXT   ends

        end     main                    ; defines entry point

