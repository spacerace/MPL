        title   GETENV get environment string
        page    55,132
        .286

; GETENV --- Return address and length of variable
;            portion of environment string (OS/2 version).
;
; Copyright (C) 1988 Ray Duncan
;
; Call with:    DS:SI = ASCIIZ env. variable name
;
; Returns:      ES:DI = address of env. variable
;               AX    = length (0 = not found)
;
; Uses:         nothing

        extrn   DosGetEnv:far   ; OS/2 API function

_TEXT   segment word public 'CODE'

        assume  cs:_TEXT
                                ; local variables...
envseg  equ     [bp-2]          ; environment segment
cmdoffs equ     [bp-4]          ; command line offset   

        public  getenv          ; make visible to Linker

getenv  proc    near

        enter   4,0             ; allocate local variables
        push    cx              ; save registers
        push    si

        push    ss              ; get selector for environment 
        lea     ax,envseg       ; and offset of command line 
        push    ax
        push    ss
        lea     ax,cmdoffs
        push    ax
        call    DosGetEnv       ; transfer to OS/2      
        or      ax,ax           ; did function succeed?
        jz      get1            ; jump if successful

        xor     ax,ax           ; DosGetEnv failed, 
        jmp     get5            ; return AX = 0

get1:   mov     es,envseg       ; set ES:BX = command line
        mov     cx,8000h        ; assume max env. = 32 KB       
        xor     di,di           ; initial env. offset
        xor     ax,ax           ; default length result 

get2:                           ; check for end of environment
        cmp     byte ptr es:[di],0
        je      get5            ; end reached, return AX = 0

        pop     si              ; initialize address of target
        push    si              ; variable to be found

        repe cmpsb              ; compare target and env. strings
        cmp     byte ptr [si-1],0       
        jne     get3            ; jump if incomplete match
        cmp     byte ptr es:[di-1],'='
        je      get4            ; jump if match was complete

get3:                           ; match was incomplete
        repne scasb             ; scan for end of env. string
        jmp     get2            ; and try again to match

get4:   push    di              ; save address after = sign
        repne scasb             ; look for end of this string
        pop     ax              ; get back starting address 
        xchg    di,ax           ; find string length
        sub     ax,di
        dec     ax              ; don't include null byte

get5:                           ; common exit point
        pop     si              ; restore registers
        pop     cx
        leave                   ; discard local variables
        ret                     ; return to caller

getenv  endp

_TEXT   ends

        end
