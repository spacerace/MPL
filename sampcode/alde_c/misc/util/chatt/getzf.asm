
;ASM routine to return flags (need zero flag) register to input()

.MODEL  SMALL
.CODE
        PUBLIC  _getzf

_getzf  PROC    NEAR
        push    bp              ; don't really need standard proc in this
        mov     bp, sp          ;   program but will be reminder
        pushf                   ; push flags register and
        pop     ax              ; pop it to AX for return value
        pop     bp
        ret
_getzf  ENDP
        END

