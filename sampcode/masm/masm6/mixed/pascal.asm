; Assemble with ML /c PASCAL.ASM
; Called by PASMAIN.PAS 

        .MODEL  medium, PASCAL
        .386
Power2  PROTO PASCAL  factor:WORD, power:WORD
        .CODE

Power2  PROC    factor:WORD, power:WORD

        mov     ax, factor    ; Load Factor into AX
        mov     cx, power     ; Load Power into CX
        shl     ax, cl        ; AX = AX * (2 to power of CX)
        ret                   ; Leave return value in AX

Power2  ENDP
        END
