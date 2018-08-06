        title   GETENV get environment string
        page    55,132
        .286

; GETENV --- Return address and length of variable
;            portion of environment string (OS/2 version).
;            This version uses OS/2 API function DosScanEnv.
;
; Copyright (C) 1988 Ray Duncan
;
; Call with:    DS:SI = ASCIIZ env. variable name
;
; Returns:      ES:DI = address of env. variable
;               AX    = length (0 = not found)
;
; Uses:         nothing

        extrn   DosScanEnv:far  ; OS/2 API function

_TEXT   segment word public 'CODE'

        assume  cs:_TEXT
                                ; local variables...
valptr  equ     [bp-4]          ; receives pointer to 
                                ; env. value string

        public  getenv          ; make visible to Linker

getenv  proc    near

        enter   4,0             ; allocate local variable
        push    cx              ; save register

                                ; call OS/2 to search
                                ; environment block...
        push    ds              ; address of name string
        push    si
        push    ss              ; address to receive 
        lea     ax,valptr       ; pointer to value string
        push    ax
        call    DosScanEnv      ; transfer to OS/2
        or      ax,ax           ; env. variable found?
        jz      get1            ; jump if it exists

        xor     ax,ax           ; else return length=0
        jmp     get2

get1:                           ; load value string addr.
        les     di,dword ptr valptr

        mov     cx,-1           ; find length of string
        cld                     ; by scanning for null
        xor     al,al
        repnz scasb
        not     cx
        dec     cx              ; and let AX = length,
        mov     ax,cx           ; ES:DI = address
        mov     di,word ptr valptr

get2:                           ; common exit point
        pop     cx              ; restore registers
        leave                   ; discard local variables
        ret                     ; return to caller

getenv  endp

_TEXT   ends

        end
