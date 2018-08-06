        name    divzero
        title   'DIVZERO - Interrupt 00H Handler'
;
; DIVZERO.ASM: Demonstration Interrupt 00H Handler
; 
; To assemble, link, and convert to COM file:
;
;       C>MASM DIVZERO;  <Enter>
;       C>LINK DIVZERO;  <Enter>
;       C>EXE2BIN DIVZERO.EXE DIVZERO.COM  <Enter>
;       C>DEL DIVZERO.EXE  <Enter>
;

cr      equ     0dh             ; ASCII carriage return
lf      equ     0ah             ; ASCII linefeed
eos     equ     '$'             ; end of string marker

_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:_TEXT,es:_TEXT,ss:_TEXT

        org     100h

entry:  jmp     start           ; skip over data area

intmsg  db      'Divide by Zero Occurred!',cr,lf,eos

divmsg  db      'Dividing '     ; message used by demo
par1    db      '0000h'         ; dividend goes here
        db      ' by '
par2    db      '00h'           ; divisor goes here
        db      ' equals '
par3    db      '00h'           ; quotient here
        db      ' remainder '
par4    db      '00h'           ; and remainder here
        db      cr,lf,eos

oldint0 dd      ?               ; save old Int 00H vector

intflag db      0               ; nonzero if divide by
                                ; zero interrupt occurred

oldip   dw      0               ; save old IP value


;
; The routine 'int0' is the actual divide by zero
; interrupt handler.  It gains control whenever a
; divide by zero or overflow occurs.  Its action
; is to set a flag and then increment the instruction
; pointer saved on the stack so that the failing 
; divide will not be reexecuted after the IRET.
;
; In this particular case we can call MS-DOS to 
; display a message during interrupt handling
; because the application triggers the interrupt
; intentionally. Thus, it is known that MS-DOS or
; other interrupt handlers are not in control
; at the point of interrupt.
; 

int0:   pop     cs:oldip        ; capture instruction pointer

        push    ax
        push    bx
        push    cx
        push    dx
        push    di
        push    si
        push    ds
        push    es

        push    cs              ; set DS = CS
        pop     ds

        mov     ah,09h          ; print error message
        mov     dx,offset _TEXT:intmsg
        int     21h

        add     oldip,2         ; bypass instruction causing
                                ; divide by zero error

        mov     intflag,1       ; set divide by 0 flag

        pop     es              ; restore all registers
        pop     ds
        pop     si
        pop     di
        pop     dx
        pop     cx
        pop     bx
        pop     ax

        push    cs:oldip        ; restore instruction pointer

        iret                    ; return from interrupt


;
; The code beginning at 'start' is the application
; program.  It alters the vector for Interrupt 00H to
; point to the new handler, carries out some divide
; operations (including one that will trigger an
; interrupt) for demonstration purposes, restores
; the original contents of the Interrupt 00H vector,
; and then terminates.
;

start:  mov     ax,3500h        ; get current contents 
        int     21h             ; of Int 00H vector

                                ; save segment:offset
                                ; of previous Int 00H handler
        mov     word ptr oldint0,bx
        mov     word ptr oldint0+2,es

                                ; install new handler...
        mov     dx,offset int0  ; DS:DX = handler address
        mov     ax,2500h        ; call MS-DOS to set
        int     21h             ; Int 00H vector
        
                                ; now our handler is active,
                                ; carry out some test divides.

        mov     ax,20h          ; test divide
        mov     bx,1            ; divide by 1
        call    divide      

        mov     ax,1234h        ; test divide
        mov     bx,5eh          ; divide by 5EH
        call    divide      

        mov     ax,5678h        ; test divide
        mov     bx,7fh          ; divide by 127
        call    divide      

        mov     ax,20h          ; test divide
        mov     bx,0            ; divide by 0
        call    divide          ; (triggers interrupt)

                                ; demonstration complete,
                                ; restore old handler

        lds     dx,oldint0      ; DS:DX = handler address
        mov     ax,2500h        ; call MS-DOS to set
        int     21h             ; Int 00H vector

        mov     ax,4c00h        ; final exit to MS-DOS
        int     21h             ; with return code = 0

;
; The routine 'divide' carries out a trial division,
; displaying the arguments and the results.  It is
; called with AX = dividend and BL = divisor.
;

divide  proc    near

        push    ax              ; save arguments
        push    bx

        mov     di,offset par1  ; convert dividend to 
        call    wtoa            ; ASCII for display

        mov     ax,bx           ; convert divisor to
        mov     di,offset par2  ; ASCII for display
        call    btoa

        pop     bx              ; restore arguments
        pop     ax

        div     bl              ; perform the division
        cmp     intflag,0       ; divide by zero detected?
        jne     nodiv           ; yes, skip display

        push    ax              ; no, convert quotient to
        mov     di,offset par3  ; ASCII for display
        call    btoa

        pop     ax              ; convert remainder to 
        xchg    ah,al           ; ASCII for display
        mov     di,offset par4
        call    btoa

        mov     ah,09h          ; show arguments, results 
        mov     dx,offset divmsg
        int     21h

nodiv:  mov     intflag,0       ; clear divide by 0 flag
        ret                     ; and return to caller

divide  endp



wtoa    proc    near            ; convert word to hex ASCII
                                ; call with AX = binary value
                                ;           DI = addr for string
                                ; returns AX, CX, DI destroyed

        push    ax              ; save original value
        mov     al,ah
        call    btoa            ; convert upper byte
        add     di,2            ; increment output address
        pop     ax
        call    btoa            ; convert lower byte
        ret                     ; return to caller

wtoa    endp



btoa    proc    near            ; convert byte to hex ASCII
                                ; call with AL = binary value
                                ;           DI = addr to store string
                                ; returns AX, CX destroyed

        mov     ah,al           ; save lower nibble
        mov     cx,4            ; shift right 4 positions
        shr     al,cl           ; to get upper nibble
        call    ascii           ; convert 4 bits to ASCII
        mov     [di],al         ; store in output string
        mov     al,ah           ; get back lower nibble

        and     al,0fh          ; blank out upper one
        call    ascii           ; convert 4 bits to ASCII
        mov     [di+1],al       ; store in output string
        ret                     ; back to caller

btoa    endp



ascii   proc    near            ; convert AL bits 0-3 to 
                                ; ASCII {0...9,A...F}
        add     al,'0'          ; and return digit in AL
        cmp     al,'9'
        jle     ascii2     
        add     al,'A'-'9'-1    ; "fudge factor" for A-F
ascii2: ret                     ; return to caller

ascii   endp

_TEXT   ends

        end     entry
