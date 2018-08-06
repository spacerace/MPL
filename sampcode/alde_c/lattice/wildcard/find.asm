		NAME	FIND
		TITLE	Find file and attributes
		PAGE	66, 132

DGROUP	GROUP	DATA
DATA	SEGMENT	WORD	PUBLIC	'DATA'
	ASSUME	DS:DGROUP
	ASSUME	ES:DGROUP
DATA	ends

PGROUP	GROUP PROG
PROG	segment	byte public 'PROG'
	assume	cs:PGROUP


		PUBLIC	FIND1ST, FINDNEXT

FIND1ST 	PROC	NEAR
;
;    This procedure finds the first file that matches the name and set of
;    attributes passed in.  If the file is found, a pointer to the information
;    block is returned, otherwise, zero is returned.
;
WILDCARD	EQU	WORD PTR [BP + 4]
ATTR_MASK	EQU	WORD PTR [BP + 6]
DTA_P		EQU	WORD PTR [BP + 8]

		push	bp
		mov	bp, sp
		mov	dx, DTA_P			; point to info block
		mov	ah, 1Ah 			; request new DTA
		int	21h				; from MSDOS
		mov	dx, WILDCARD			; filename to find
		mov	cx, ATTR_MASK			; with these attributes
		mov	ah, 4Eh 			; request search
		int	21h				; of DOS
		mov	ax, 0				; assume 0 (failure)
		jc	DONE				; if CY, no match
		inc	ax				; else success
DONE:		pop	bp
		ret
FIND1ST 	ENDP

FINDNEXT	PROC	NEAR
;
;    This procedure finds the next file that matches the name and set of
;    attributes passed in to FIND1ST.  This routine uses the information block
;    pointer that was passed to FIND1ST, and no changes to the block may
;    have taken place.	FINDNEXT false (0) when there are no more matches.
;
INFO_BLOCK	EQU	WORD PTR [BP + 4]

		push	bp
		mov	bp, sp
		mov	dx, INFO_BLOCK			; point to info block
		mov	ah, 1Ah 			; request new DTA
		int	21h				; from MSDOS
		mov	ah, 4Fh 			; request search
		int	21h				; of DOS
		mov	ax, 0				; assume 0 (failure)
		jc	QUIT				; if CY, no match
		inc	ax				; else TRUE (success)
QUIT:		pop	bp
		ret
FINDNEXT	ENDP

PROG		ENDS
		END
