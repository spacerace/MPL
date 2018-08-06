;Source Module MODULE_B

;Constant declarations *******************************************************

CONST   SEGMENT WORD PUBLIC 'CONST'

CONST_FIELD_B   DB      'Constant B'    ;declare a MODULE_B constant

CONST   ENDS


;Preinitialized data fields **************************************************

_DATA   SEGMENT WORD PUBLIC 'DATA'

DATA_FIELD_B    DB      'Data B'        ;declare a MODULE_B preinitialized field

_DATA   ENDS


;Uninitialized data fields ***************************************************

_BSS    SEGMENT WORD PUBLIC 'BSS'

BSS_FIELD_B     DB      5 DUP(?)        ;declare a MODULE_B uninitialized field

_BSS    ENDS


;Program text ****************************************************************

DGROUP  GROUP   _DATA,CONST,_BSS

_TEXT   SEGMENT BYTE PUBLIC 'CODE'

        ASSUME  CS:_TEXT,DS:DGROUP,ES:NOTHING,SS:NOTHING

        PUBLIC  PROC_B                  ;reference in MODULE_A
PROC_B  PROC    NEAR

        RET

PROC_B  ENDP

_TEXT   ENDS

        END
