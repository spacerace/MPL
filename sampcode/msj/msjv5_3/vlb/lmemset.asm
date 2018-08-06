;
;  lmemset      far version of memset
;

INCLUDE         SETUP.H

CLIB            SEGMENT  WORD PUBLIC 'CODE'
CLIB            ENDS

CLIB            SEGMENT
ASSUME          CS: CLIB

PUBLIC          lmemset

lmemset         PROC    FAR

Destination     EQU     DWORD   PTR [bp] + 10
FillChar        EQU     BYTE    PTR [bp] + 8
Count           EQU     WORD    PTR [bp] + 6

                ENTRY   0


                les     di, Destination ; ES:DI = Destination
                mov     bx, di          ; save a copy of DST

                mov     cx, Count
                jcxz    toend           ; if no work to do

                mov     al, FillChar    ; the byte FillChar to store
                mov     ah, al          ; store it as a word

                test    di, 1           ; is Destination address odd?
                jz      dowords         ;   yes: proceed

                stosb                   ; store byte for word align
                dec     cx
dowords:
                shr     cx, 1
                rep     stosw           ; store word at a time
                adc     cx, cx
                rep     stosb           ; store final ("odd") byte
toend:
                mov     di, dx          ; Restore DI

                xchg    ax, bx          ; AX = Destination
                mov     dx, es          ; segment part of addr

                EXIT    8

lmemset         ENDP

CLIB            ENDS

                END
