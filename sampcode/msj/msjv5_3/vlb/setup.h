; 
;  setup.h    header file for C utility functions
;
 
_DATA   SEGMENT  WORD PUBLIC 'DATA'
_DATA   ENDS
CONST   SEGMENT  WORD PUBLIC 'CONST'
CONST   ENDS
_BSS    SEGMENT  WORD PUBLIC 'BSS'
_BSS    ENDS
DGROUP  GROUP CONST, _BSS, _DATA
        ASSUME  DS: DGROUP, SS: DGROUP

ENTRY MACRO   X
            push    ds
            pop     ax
            nop        
            inc     bp
            push    bp
            mov     bp, sp
            push    ds
            mov     ds, ax
            ASSUME      DS:NOTHING
            sub     sp, X
            push    es
            push    si
            push    di

        ENDM


EXIT MACRO      X       ; X should always be even -
                        ;   can't pass a single byte.
                        ; X is used for PASCAL type stack clean-up.
            pop     di
            pop     si
            pop     es
            sub     bp, 2
            mov     sp, bp
            pop     ds
            ASSUME      DS:DGROUP
            pop     bp
            dec     bp
            ret     X

        ENDM
