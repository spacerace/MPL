; Assemble with ML /c QPEX.ASM

Power2  PROTO PASCAL  factor:WORD, power:WORD

CODE	SEGMENT WORD PUBLIC
	ASSUME  CS:CODE


Power2  PROC PASCAL   factor:WORD, power:WORD

	mov     ax, factor        ; Load factor into AX
	mov     cx, power         ; Load power into CX
	shl	ax, cl	          ; AX = AX * (2 to power of CX)
				  ; Leave return value in AX
	ret
Power2  ENDP

CODE    ENDS
	END
