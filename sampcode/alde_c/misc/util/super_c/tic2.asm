;                       hasWon(p)
;
; Returns TRUE if side p has won the game.

_TEXT   SEGMENT  BYTE PUBLIC 'CODE'
_TEXT   ENDS
CONST   SEGMENT  WORD PUBLIC 'CONST'
CONST   ENDS
_BSS    SEGMENT  WORD PUBLIC 'BSS'
_BSS    ENDS
_DATA   SEGMENT  WORD PUBLIC 'DATA'
_DATA   ENDS

DGROUP  GROUP   CONST,_BSS,_DATA

        ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP

        PUBLIC  _hasWon

_DATA   SEGMENT

        EXTRN   _board:BYTE
        EXTRN   __chkstk:NEAR

_DATA   ENDS

_TEXT      SEGMENT

; hasWon(p)
;
; int p;

        p = 4

        PUBLIC  _hasWon

_hasWon PROC NEAR
        push    bp
        mov     bp,sp
        mov     ax,4
        call    __chkstk
        push    di
        push    si

;       w = 3*p
        mov     ax,3
        imul    WORD PTR [bp+p]
        mov     di,ax

;       for (i = 0; i < 9; i += 3)
        xor     si,si
$F13:
;               if ((board[i]+board[i+1]+board[i+2]) == w)
;                       return(TRUE);
        mov     al,BYTE PTR _board[si+2]
        cbw
        mov     cx,ax
        mov     al,BYTE PTR _board[si+1]
        cbw
        add     ax,cx
        mov     cx,ax
        mov     al,BYTE PTR _board[si]
        cbw
        add     cx,ax
        cmp     cx,di
        je      $L20001
        add     si,3
        cmp     si,9
        jl      $F13

;       for (i = 0; i < 3; i++)
        xor     si,si
$F18:

;               if ((board[i]+board[i+3]+board[i+6]) == w)
;                       return(TRUE);
        mov     al,BYTE PTR _board[si+6]
        cbw
        mov     cx,ax
        mov     al,BYTE PTR _board[si+3]
        cbw
        add     ax,cx
        mov     cx,ax
        mov     al,BYTE PTR _board[si]
        cbw
        add     cx,ax
        cmp     cx,di
        je      $L20001
        inc     si
        cmp     si,3
        jl      $F18

;       if ((board[0]+board[4]+board[8]) == w) return(TRUE);
        mov     al,BYTE PTR _board+4
        cbw
        mov     cx,ax
        mov     al,BYTE PTR _board
        cbw
        add     ax,cx
        mov     cx,ax
        mov     al,BYTE PTR _board+8
        cbw
        add     cx,ax
        cmp     cx,di
        jne     $I23
$L20001:
        mov     ax,1
        jmp     SHORT $EX10
$I23:

;       if ((board[2]+board[4]+board[6]) == w) return(TRUE);
        mov     al,BYTE PTR _board+4
        cbw
        mov     cx,ax
        mov     al,BYTE PTR _board+2
        cbw
        add     ax,cx
        mov     cx,ax
        mov     al,BYTE PTR _board+6
        cbw
        add     cx,ax
        cmp     cx,di
        je      $L20001

;       return(FALSE);
        xor     ax,ax
$EX10:
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_hasWon ENDP

_TEXT   ENDS

        END

