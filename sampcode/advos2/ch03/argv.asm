	title	ARGV -- Return Argument Pointer
        page    55,132
        .286

;
; ARGV.ASM
;
; Return address and length of specified command line argument
; or fully qualified program name.  Treats blanks and tabs as
; whitespace.
;
; Assemble with:  C> masm argv.asm;
;
; Call with:    AX    = argument number (0 based)
;
; Returns:      ES:BX = argument address
;		AX    = argument length (0 = no argument)
;
; Uses:         nothing (other registers preserved)
;
; Note:  If called with AX=0 (argv[0]), returns ES:BX
; pointing to fully qualified program name in environment 
; block and AX=length.
;
; Copyright (C) 1987 Ray Duncan
;

tab     equ     09h             ; ASCII tab
blank   equ     20h             ; ASCII space character

        extrn   DosGetEnv:far

_TEXT   segment word public 'CODE'

        assume  cs:_TEXT

        public  argv            ; make ARGV available to Linker

                                ; local variables...
envseg  equ     [bp-2]          ; environment segment
cmdoffs equ     [bp-4]          ; command line offset   

argv    proc    near

        enter   4,0             ; make room for local variables
        push    cx              ; save original CX and DI 
        push    di

        push    ax              ; save argument number

        push    ss              ; get selector for environment 
        lea     ax,envseg       ; and offset of command line 
        push    ax
        push    ss
        lea     ax,cmdoffs
        push    ax
        call    DosGetEnv       ; transfer to OS/2      
        or      ax,ax           ; test operation status
        pop     ax              ; restore argument number
        jnz     argv7           ; jump if DosGetEnv failed

        mov     es,envseg       ; set ES:BX = command line
        mov     bx,cmdoffs

	or	ax,ax		; is requested argument = 0?
        jz      argv8           ; yes, jump to get program name

argv0:  inc     bx              ; scan off first field
        cmp     byte ptr es:[bx],0      
        jne     argv0

        xor     ah,ah           ; initialize argument counter

argv1:  mov     cx,-1           ; set flag = outside argument

argv2:  inc     bx              ; point to next character 
        cmp     byte ptr es:[bx],0
        je      argv7           ; exit if null byte
        cmp     byte ptr es:[bx],blank
        je      argv1           ; outside argument if ASCII blank
        cmp     byte ptr es:[bx],tab    
        je      argv1           ; outside argument if ASCII tab

                                ; if not blank or tab...
        jcxz    argv2           ; jump if already inside argument

        inc     ah              ; else count arguments found
        cmp     ah,al           ; is this the one we're looking for?
        je      argv4           ; yes, go find its length
        not     cx              ; no, set flag = inside argument
        jmp     argv2           ; and look at next character

argv4:                          ; found desired argument, now
                                ; determine its length...
	mov	ax,bx		; save parameterr starting address

argv5:  inc     bx              ; point to next character
        cmp     byte ptr es:[bx],0
        je      argv6           ; found end if null byte
        cmp     byte ptr es:[bx],blank
        je      argv6           ; found end if ASCII blank
        cmp     byte ptr es:[bx],tab    
        jne     argv5           ; found end if ASCII tab

argv6:  xchg    bx,ax           ; set ES:BX = argument address
        sub     ax,bx           ; and AX = argument length
        jmp     argvx           ; return to caller

argv7:  xor     ax,ax           ; set AX = 0, argument not found
        jmp     argvx           ; return to caller

argv8:                          ; special handling for argv=0
        xor     di,di           ; find the program name by
        xor     al,al           ; first skipping over all the
        mov     cx,-1           ; environment variables...
        cld
argv9:  repne scasb             ; scan for double null (can't use
	scasb			; SCASW since might be odd address)
        jne     argv9           ; loop if it was a single null
        mov     bx,di           ; save program name address
        mov     cx,-1           ; now find its length... 
        repne scasb             ; scan for another null byte
        not     cx              ; convert CX to length 
        dec     cx
        mov     ax,cx           ; return length in AX

argvx:                          ; common exit point
        pop     di              ; restore original CX and DI
        pop     cx
        leave                   ; discard stack frame
        ret                     ; return to caller

argv    endp

_TEXT   ends

        end     
