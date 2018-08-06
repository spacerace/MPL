        name    int24
        title   INT24 Critical Error Handler
 
;
; INT24.ASM -- Replacement critical error handler
; by Ray Duncan, September 1987
;
 
cr      equ     0dh             ; ASCII carriage return
lf      equ     0ah             ; ASCII linefeed
 
DGROUP  group   _DATA
 
_DATA   segment word public 'DATA'
 
save24  dd      0               ; previous contents of Int 24H
                                ; critical error handler vector
 
                                ; prompt message used by
                                ; critical error handler
prompt  db      cr,lf,'Critical Error Occurred: '
        db      'Abort, Retry, Ignore, Fail? $'
 
keys    db      'aArRiIfF'      ; possible user response keys
keys_len equ    $-keys          ; (both cases of each allowed)
 
codes   db      2,2,1,1,0,0,3,3 ; codes returned to MS-DOS kernel
                                ; for corresponding response keys
 
_DATA   ends
 
 
_TEXT   segment word public 'CODE'
 
        assume  cs:_TEXT,ds:DGROUP
 
        public  get24           
get24   proc    near            ; set Int 24H vector to point
                                ; to new critical error handler
 
        push    ds              ; save segment registers
        push    es
 
        mov     ax,3524h        ; get address of previous
        int     21h             ; INT 24H handler and save it
 
        mov     word ptr save24,bx
        mov     word ptr save24+2,es
 
        push    cs              ; set DS:DX to point to
        pop     ds              ; new INT 24H handler
        mov     dx,offset _TEXT:int24
        mov     ax,2524h        ; then call MS-DOS to
        int     21h             ; set the INT 24H vector
 
        pop     es              ; restore segment registers
        pop     ds
        ret                     ; and return to caller
 
get24   endp
 
 
        public  res24
res24   proc    near            ; restore original contents
                                ; of Int 24H vector
 
        push    ds              ; save our data segment
 
        lds     dx,save24       ; put address of old handler
        mov     ax,2524h        ; back into INT 24H vector
        int     21h
 
        pop     ds              ; restore data segment
        ret                     ; and return to caller
 
res24   endp
 
;
; This is the replacement critical error handler.  It
; prompts the user for Abort, Retry, Ignore, or Fail and
; returns the appropriate code to the MS-DOS kernel.
; 
int24   proc    far             ; entered from MS-DOS kernel
 
        push    bx              ; save registers
        push    cx
        push    dx
        push    si
        push    di
        push    bp
        push    ds
        push    es
 
int24a: mov     ax,DGROUP       ; display prompt for user
        mov     ds,ax           ; using function 09H (print string
        mov     es,ax           ; terminated by $ character)
        mov     dx,offset prompt
        mov     ah,09h
        int     21h
 
        mov     ah,01h          ; get user's response
        int     21h             ; Function 01H = read one character
 
        mov     di,offset keys  ; look up code for response key
        mov     cx,keys_len
        cld
        repne scasb
        jnz     int24a          ; prompt again if bad response
 
                                ; set AL = action code for MS-DOS
                                ; according to key that was entered:
                                ; 0 = ignore, 1 = retry, 2 = abort, 3 = fail
        mov     al,[di+keys_len-1]
 
        pop     es              ; restore registers
        pop     ds
        pop     bp
        pop     di
        pop     si
        pop     dx
        pop     cx
        pop     bx
        iret                    ; exit critical error handler
 
int24   endp
 
_TEXT   ends
 
        end
