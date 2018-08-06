        TITLE   engine

CODE    SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CODE,DS:CODE,ES:CODE,SS:CODE

        ORG     0100h

START:  mov     dx,offset devnm ; open named device (ASY1)
        mov     ax,3d02h
        int     21h
        mov     handle,ax       ; save the handle
        jc      quit
alltim: call    getmdm          ; main engine loop
        call    putcrt
        call    getkbd
        call    putmdm
        jmp     alltim
quit:   mov     ah,4ch          ; come here to quit
        int     21h

getmdm  proc                    ; get input from modem
        mov     cx,256
        mov     bx,handle
        mov     dx,offset mbufr
        mov     ax,3F00h
        int     21h
        jc      quit
        mov     mdlen,ax
        ret
getmdm  endp

getkbd  proc                    ; get input from keyboard
        mov     kblen,0         ; first zero the count
        mov     ah,11           ; key pressed?
        int     21h
        inc     al
        jnz     nogk            ; no
        mov     ah,7            ; yes, get it
        int     21h
        cmp     al,3            ; was it Ctrl-C?
        je      quit            ; yes, get out
        mov     kbufr,al        ; no, save it
        inc     kblen
        cmp     al,13           ; was it Enter?
        jne     nogk            ; no
        mov     byte ptr kbufr+1,10 ; yes, add LF
        inc     kblen
nogk:   ret
getkbd  endp

putmdm  proc                    ; put output to modem
        mov     cx,kblen
        jcxz    nopm
        mov     bx,handle
        mov     dx,offset kbufr
        mov     ax,4000h
        int     21h
        jc      quit
nopm:   ret
putmdm  endp

putcrt  proc                    ; put output to CRT
        mov     cx,mdlen
        jcxz    nopc
        mov     bx,1
        mov     dx,offset mbufr
        mov     ah,40h
        int     21h
        jc      quit
nopc:   ret
putcrt  endp

devnm   db      'ASY1',0        ; miscellaneous data and buffers
handle  dw      0
kblen   dw      0
mdlen   dw      0
mbufr   db      256 dup (0)
kbufr   db      80 dup (0)

CODE    ENDS
        END     START
