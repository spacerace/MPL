        TITLE   CH1A.ASM

; CH1A.ASM -- support file for CTERM.C terminal emulator
;       this set of routines replaces Ctrl-C/Ctrl-BREAK
;       usage: void set_int(), rst_int();
;               int broke();    /* boolean if BREAK     */
;       for use with Microsoft C and SMALL model only...

_TEXT   segment byte public 'CODE'
_TEXT   ends
_DATA   segment byte public 'DATA'
_DATA   ends
CONST   segment byte public 'CONST'
CONST   ends
_BSS    segment byte public 'BSS'
_BSS    ends

DGROUP  GROUP   CONST, _BSS, _DATA
        ASSUME  CS:_TEXT, DS:DGROUP, ES:DGROUP, SS:DGROUP

_DATA   SEGMENT BYTE PUBLIC 'DATA'

OLDINT1B DD     0               ; storage for original INT 1BH vector

_DATA   ENDS

_TEXT   SEGMENT

        PUBLIC  _set_int,_rst_int,_broke

myint1b:
        mov     word ptr cs:brkflg,1Bh          ; make it nonzero
        iret

myint23:
        mov     word ptr cs:brkflg,23h          ; make it nonzero
        iret

brkflg  dw      0               ; flag that BREAK occurred

_broke  proc    near            ; returns 0 if no break
        xor     ax,ax           ; prepare to reset flag
        xchg    ax,cs:brkflg    ; return current flag value
        ret
_broke  endp

_set_int proc near
        mov     ax,351bh        ; get interrupt vector for 1BH
        int     21h             ; (don't need to save for 23H)
        mov     word ptr oldint1b,bx     ; save offset in first word
        mov     word ptr oldint1b+2,es   ; save segment in second word

        push    ds              ; save our data segment
        mov     ax,cs           ; set DS to CS for now
        mov     ds,ax
        lea     dx,myint1b      ; DS:DX points to new routine
        mov     ax,251bh        ; set interrupt vector
        int     21h
        mov     ax,cs           ; set DS to CS for now
        mov     ds,ax
        lea     dx,myint23      ; DS:DX points to new routine
        mov     ax,2523h        ; set interrupt vector
        int     21h
        pop     ds              ; restore data segment
        ret
_set_int endp

_rst_int proc near
        push    ds              ; save our data segment
        lds     dx,oldint1b     ; DS:DX points to original
        mov     ax,251bh        ; set interrupt vector
        int     21h
        pop     ds              ; restore data segment
        ret
_rst_int endp

_TEXT   ends

        END
