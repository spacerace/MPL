         name      root
         title     'ROOT --- demonstrate EXEC overlay'
;
; ROOT.EXE --- demonstration of EXEC for overlays
;
; Uses MS-DOS EXEC (Int 21H Function 4BH Subfunction 03H)
; to load an overlay named OVERLAY.EXE, calls a routine
; within the OVERLAY, then recovers control and terminates.
;
; Ray Duncan, June 1987
;

stdin   equ     0                       ; standard input
stdout  equ     1                       ; standard output
stderr  equ     2                       ; standard error

stksize equ     128                     ; size of stack

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII linefeed


DGROUP  group   _DATA,_STACK


_TEXT   segment byte public 'CODE'      ; executable code segment

        assume  cs:_TEXT,ds:_DATA,ss:_STACK


stk_seg dw      ?                       ; original SS contents
stk_ptr dw      ?                       ; original SP contents


main    proc    far                     ; entry point from MS-DOS

        mov     ax,_DATA                ; set DS = our data segment
        mov     ds,ax

                                        ; now give back extra memory
        mov     ax,es                   ; AX = segment of PSP base
        mov     bx,ss                   ; BX = segment of stack base
        sub     bx,ax                   ; reserve seg stack - seg psp
        add     bx,stksize/16           ; plus paragraphs of stack
        mov     ah,4ah                  ; fxn 4AH = modify memory block
        int     21h                     ; transfer to MS-DOS
        jc      main1                   ; jump if resize failed

                                        ; display message 'Root
                                        ; 'segment executing...'
        mov     dx,offset DGROUP:msg1   ; DS:DX = address of message
        mov     cx,msg1_len             ; CX = length of message
        call    pmsg

                                        ; allocate memory for overlay
        mov     bx,1000h                ; get 64 KB (4096 paragraphs)
        mov     ah,48h                  ; fxn 48h, allocate mem block
        int     21h                     ; transfer to MS-DOS
        jc      main2                   ; jump if allocation failed

        mov     pars,ax                 ; set load address for overlay
        mov     pars+2,ax               ; set relocation segment for overlay
        mov     word ptr entry+2,ax     ; set segment of entry point

        push    ds                      ; save root's data segment
        mov     stk_seg,ss              ; save root's stack pointer
        mov     stk_ptr,sp

                                        ; now use EXEC to load overlay
        mov     ax,ds                   ; set ES = DS
        mov     es,ax
        mov     dx,offset DGROUP:oname  ; DS:DX = overlay pathname
        mov     bx,offset DGROUP:pars   ; EX:BX = parameter block
        mov     ax,4b03h                ; function 4BH, subfunction 03H
        int     21h                     ; transfer to MS-DOS

        cli                             ; (for bug in some early 8088s)
        mov     ss,stk_seg              ; restore root's stack pointer
        mov     sp,stk_ptr
        sti                             ; (for bug in some early 8088s)
        pop     ds                      ; restore DS = our data segment

        jc      main3                   ; jump if EXEC failed

                                        ; otherwise EXEC succeeded...
        push    ds                      ; save our data segment
        call    dword ptr entry         ; now call the overlay
        pop     ds                      ; restore our data segment

                                        ; display message that root
                                        ; segment regained control...
        mov     dx,offset DGROUP:msg5   ; DS:DX = address of message
        mov     cx,msg5_len             ; CX = length of message
        call    pmsg                    ; display it

        mov     ax,4c00h                ; no error, terminate program
        int     21h                     ; with return code = 0

main1:  mov     bx,offset DGROUP:msg2a  ; convert error code
        call    b2hex
        mov     dx,offset DGROUP:msg2   ; display message 'Memory
        mov     cx,msg2_len             ; resize failed...'
        call    pmsg
        jmp     main4

main2:  mov     bx,offset DGROUP:msg3a  ; convert error code
        call    b2hex
        mov     dx,offset DGROUP:msg3   ; display message 'Memory
        mov     cx,msg3_len             ; allocation failed...'
        call    pmsg
        jmp     main4

main3:  mov     bx,offset DGROUP:msg4a  ; convert error code
        call    b2hex
        mov     dx,offset DGROUP:msg4   ; display message 'EXEC
        mov     cx,msg4_len             ; call failed...'
        call    pmsg

main4:  mov     ax,4c01h                ; error, terminate program
        int     21h                     ; with return code = 1

main    endp                            ; end of main procedure


b2hex   proc    near                    ; convert byte to hex ASCII
                                        ; call with AL = binary value
                                        ; BX = addr to store string
        push    ax
        shr     al,1
        shr     al,1
        shr     al,1
        shr     al,1
        call    ascii                   ; become first ASCII character
        mov     [bx],al                 ; store it
        pop     ax
        and     al,0fh                  ; isolate lower 4 bits, which
        call    ascii                   ; become the second ASCII character
        mov     [bx+1],al               ; store it
        ret
b2hex   endp


ascii   proc    near                    ; convert value 00-0FH in AL
        add     al,'0'                  ; into a "hex ASCII" character
        cmp     al,'9'
        jle     ascii2                  ; jump if in range 00-09H,
        add     al,'A'-'9'-1            ; offset it to range 0A-0FH,
ascii2: ret                             ; return ASCII char. in AL.
ascii   endp


pmsg    proc    near                    ; displays message on standard output
                                        ; call with DS:DX = address,
                                        ;              CX = length

        mov     bx,stdout               ; BX = standard output handle
        mov     ah,40h                  ; function 40H = write file/device
        int     21h                     ; transfer to MS-DOS
        ret                             ; back to caller

pmsg    endp

_TEXT   ends


_DATA   segment para public 'DATA'      ; static & variable data segment

oname   db      'OVERLAY.OVL',0         ; pathname of overlay file

pars    dw      0                       ; load address (segment) for file
        dw      0                       ; relocation (segment) for file

entry   dd      0                       ; entry point for overlay

msg1    db      cr,lf,'Root segment executing!',cr,lf
msg1_len equ    $-msg1

msg2    db      cr,lf,'Memory resize failed, error code='
msg2a   db      'xxh.',cr,lf
msg2_len equ    $-msg2

msg3    db      cr,lf,'Memory allocation failed, error code='
msg3a   db      'xxh.',cr,lf
msg3_len equ    $-msg3

msg4    db      cr,lf,'EXEC call failed, error code='
msg4a   db      'xxh.',cr,lf
msg4_len equ    $-msg4

msg5    db      cr,lf,'Root segment regained control!',cr,lf
msg5_len equ    $-msg5

_DATA   ends


_STACK  segment para stack 'STACK'

        db      stksize dup (?)

_STACK  ends


        end     main                    ; defines program entry point
