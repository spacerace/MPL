; The following example restores the default settings of the EGA registers.

	mov	ah, 0f6h	; f6 = revert to default registers
	int	10h		; do it now!
