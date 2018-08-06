        TITLE   CH2.ASM

; CH2.ASM -- support file for CTERM.C terminal emulator
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
        assume  CS:_TEXT, DS:DGROUP, ES:DGROUP, SS:DGROUP

_TEXT   segment

        public  __cls,__color,__deol,__i_v,__key,__wrchr,__wrpos

atrib   DB      0               ; attribute
_colr   DB      0               ; color
v_bas   DW      0               ; video segment
v_ulc   DW      0               ; upper left corner cursor
v_lrc   DW      184Fh           ; lower right corner cursor
v_col   DW      0               ; current col/row

__key   proc    near            ; get keystroke
        PUSH    BP
        MOV     AH,1            ; check status via BIOS
        INT     16h
        MOV     AX,0FFFFh
        JZ      key00           ; none ready, return EOF
        MOV     AH,0            ; have one, read via BIOS
        INT     16h
key00:  POP     BP
        RET
__key   endp

__wrchr proc    near
        PUSH    BP
        MOV     BP,SP
        MOV     AL,[BP+4]       ; get char passed by C
        CMP     AL,' '
        JNB     prchr           ; printing char, go do it
        CMP     AL,8
        JNZ     notbs
        DEC     BYTE PTR v_col  ; process backspace
        MOV     AL,byte ptr v_col
        CMP     AL,byte ptr v_ulc
        JB      nxt_c           ; step to next column
        JMP     norml

notbs:  CMP     AL,9
        JNZ     notht
        MOV     AL,byte ptr v_col       ; process HTAB
        ADD     AL,8
        AND     AL,0F8h
        MOV     byte ptr v_col,AL
        CMP     AL,byte ptr v_lrc
        JA      nxt_c
        JMP     SHORT   norml

notht:  CMP     AL,0Ah
        JNZ     notlf
        MOV     AL,byte ptr v_col+1     ; process linefeed
        INC     AL
        CMP     AL,byte ptr v_lrc+1
        JBE     noht1
        CALL    scrol
        MOV     AL,byte ptr v_lrc+1
noht1:  MOV     byte ptr v_col+1,AL
        JMP     SHORT   norml

notlf:  CMP     AL,0Ch
        JNZ     ck_cr
        CALL    __cls           ; process formfeed
        JMP     SHORT   ignor

ck_cr:  CMP     AL,0Dh
        JNZ     ignor           ; ignore all other CTL chars
        MOV     AL,byte ptr v_ulc       ; process CR
        MOV     byte ptr v_col,AL
        JMP     SHORT   norml

prchr:  MOV     AH,_colr        ; process printing char
        PUSH    AX
        XOR     AH,AH
        MOV     AL,byte ptr v_col+1
        PUSH    AX
        MOV     AL,byte ptr v_col
        PUSH    AX
        CALL    wrtvr
        MOV     SP,BP
nxt_c:  INC     BYTE PTR v_col  ; advance to next column
        MOV     AL,byte ptr v_col
        CMP     AL,byte ptr v_lrc
        JLE     norml
        MOV     AL,0Dh          ; went off end, do CR/LF
        PUSH    AX
        CALL    __wrchr
        POP     AX
        MOV     AL,0Ah
        PUSH    AX
        CALL    __wrchr
        POP     AX
norml:  CALL    set_cur
ignor:  MOV     SP,BP
        POP     BP
        RET
__wrchr endp

__i_v   proc    near            ; establish video base segment
        PUSH    BP
        MOV     BP,SP
        MOV     AX,0B000h       ; mono, B800 for CGA
        MOV     v_bas,AX        ; could be made automatic
        MOV     SP,BP
        POP     BP
        RET
__i_v   endp

__wrpos proc    near            ; set cursor position
        PUSH    BP
        MOV     BP,SP
        MOV     DH,[BP+4]       ; row from C program
        MOV     DL,[BP+6]       ; col from C program
        MOV     v_col,DX        ; cursor position
        MOV     BH,atrib        ; attribute
        MOV     AH,2
        PUSH    BP
        INT     10h
        POP     BP
        MOV     AX,v_col        ; return cursor position
        MOV     SP,BP
        POP     BP
        RET
__wrpos endp

set_cur proc    near            ; set cursor to v_col
        PUSH    BP
        MOV     BP,SP
        MOV     DX,v_col        ; use where v_col says
        MOV     BH,atrib
        MOV     AH,2
        PUSH    BP
        INT     10h
        POP     BP
        MOV     AX,v_col
        MOV     SP,BP
        POP     BP
        RET
set_cur endp

__color proc    near            ; _color(fg, bg)
        PUSH    BP
        MOV     BP,SP
        MOV     AH,[BP+6]       ; background from C
        MOV     AL,[BP+4]       ; foreground from C
        MOV     CX,4
        SHL     AH,CL
        AND     AL,0Fh
        OR      AL,AH           ; pack up into 1 byte
        MOV     _colr,AL        ; store for handler's use
        XOR     AH,AH
        MOV     SP,BP
        POP     BP
        RET
__color endp

scrol   proc    near            ; scroll CRT up by one line
        PUSH    BP
        MOV     BP,SP
        MOV     AL,1            ; count of lines to scroll
        MOV     CX,v_ulc
        MOV     DX,v_lrc
        MOV     BH,_colr
        MOV     AH,6
        PUSH    BP
        INT     10h             ; use BIOS
        POP     BP
        MOV     SP,BP
        POP     BP
        RET
scrol   endp

__cls   proc    near            ; clear CRT
        PUSH    BP
        MOV     BP,SP
        MOV     AL,0            ; flags CLS to BIOS
        MOV     CX,v_ulc
        MOV     v_col,CX        ; set to HOME
        MOV     DX,v_lrc
        MOV     BH,_colr
        MOV     AH,6
        PUSH    BP
        INT     10h             ; use BIOS scroll up
        POP     BP
        CALL    set_cur         ; cursor to HOME
        MOV     SP,BP
        POP     BP
        RET
__cls   endp

__deol  proc    near            ; delete to end of line
        PUSH    BP
        MOV     BP,SP
        MOV     AL,' '
        MOV     AH,_colr        ; set up blanks
        PUSH    AX
        MOV     AL,byte ptr v_col+1
        XOR     AH,AH           ; set up row value
        PUSH    AX
        MOV     AL,byte ptr v_col

deol1:  CMP     AL,byte ptr v_lrc
        JA      deol2           ; at RH edge 
        PUSH    AX              ; current location
        CALL    wrtvr           ; write a blank
        POP     AX
        INC     AL              ; next column
        JMP     deol1           ; do it again

deol2:  MOV     AX,v_col        ; return cursor position
        MOV     SP,BP
        POP     BP
        RET
__deol  endp

wrtvr   proc    near            ; write video RAM (col, row, char/atr)
        PUSH    BP
        MOV     BP,SP           ; set up arg ptr
        MOV     DL,[BP+4]       ; column
        MOV     DH,[BP+6]       ; row
        MOV     BX,[BP+8]       ; char/atr
        MOV     AL,80           ; calc offset
        MUL     DH
        XOR     DH,DH
        ADD     AX,DX
        ADD     AX,AX           ; adjust bytes to words
        PUSH    ES              ; save seg reg
        MOV     DI,AX
        MOV     AX,v_bas        ; set up segment
        MOV     ES,AX
        MOV     AX,BX           ; get the data
        STOSW                   ; put on screen
        POP     ES              ; restore regs
        MOV     SP,BP
        POP     BP
        RET
wrtvr   endp

_TEXT   ends

        END
