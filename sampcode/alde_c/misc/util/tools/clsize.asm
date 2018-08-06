;   clsize.asm  Read cluster size of a disk.
;   K.van Houten 230987
;   Static Name Aliases
;
    TITLE   clsize

_TEXT   SEGMENT  BYTE PUBLIC 'CODE'
_TEXT   ENDS
CONST   SEGMENT  WORD PUBLIC 'CONST'
CONST   ENDS
_BSS    SEGMENT  WORD PUBLIC 'BSS'
_BSS    ENDS
_DATA   SEGMENT  WORD PUBLIC 'DATA'
_DATA   ENDS
DGROUP  GROUP   CONST,  _BSS,   _DATA
    ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP
PUBLIC  _clsize
_DATA   SEGMENT
_DATA   ENDS
_TEXT      SEGMENT
    PUBLIC  _clsize
_clsize PROC NEAR
    push    bp
    mov bp,sp
    push di
    push si
    push bx
    push cx
    push dx
    push bp
    push ds

 ;  get info
    mov dl,[bp+4d]
    inc dl
    mov ah,1ch
    int 21h
    mov ah,0h
    mul cx

    pop ds
    pop bp
    pop dx
    pop cx
    pop bx
    pop si
    pop di
    mov sp,bp
    pop bp
    ret
_clsize endP

_TEXT   ENDS
END
