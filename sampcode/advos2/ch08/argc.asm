	title	ARGC -- Return Argument Count
        page    55,132
        .286

;
; ARGC.ASM
;
; Return count of command line arguments.  Treats blanks and
; tabs as whitespace.
;
; Assemble with:  C> masm argc.asm;
; 
; Call with:    N/A
;
; Returns:	AX    = argument count (always >= 1)
;
; Uses:         nothing (other registers preserved)
;
; Copyright (C) 1987 Ray Duncan
;

tab     equ     09h             ; ASCII tab
blank   equ     20h             ; ASCII space character

        extrn   DosGetEnv:far

_TEXT   segment word public 'CODE'

        assume  cs:_TEXT

        public  argc            ; make ARGC available to Linker

                                ; local variables
envseg  equ     [bp-2]          ; environment segment
cmdoffs equ     [bp-4]          ; command line offset   

argc    proc    near

        enter   4,0             ; make room for local variables

	push	es		; save original ES, BX, and CX
        push    bx
        push    cx

        push    ss              ; get selector for environment 
        lea     ax,envseg       ; and offset of command line 
        push    ax
        push    ss
        lea     ax,cmdoffs
        push    ax
        call    DosGetEnv       ; transfer to OS/2      
        or      ax,ax           ; check operation status
        mov     ax,1            ; force argc >= 1
        jnz     argc3           ; inexplicable failure

        mov     es,envseg       ; set ES:BX = command line
        mov     bx,cmdoffs

argc0:  inc     bx              ; ignore useless first field
        cmp     byte ptr es:[bx],0      
        jne     argc0

argc1:  mov     cx,-1           ; set flag = outside argument

argc2:  inc     bx              ; point to next character 
        cmp     byte ptr es:[bx],0
        je      argc3           ; exit if null byte
        cmp     byte ptr es:[bx],blank
        je      argc1           ; outside argument if ASCII blank
        cmp     byte ptr es:[bx],tab    
        je      argc1           ; outside argument if ASCII tab

                                ; otherwise not blank or tab,
        jcxz    argc2           ; jump if already inside argument

        inc     ax              ; else found argument, count it
        not     cx              ; set flag = inside argument
        jmp     argc2           ; and look at next character

argc3:  pop     cx              ; restore original BX, CX, ES
        pop     bx
        pop     es
        leave                   ; discard local variables
        ret                     ; return AX = argument count

argc    endp

_TEXT   ends

        end     
