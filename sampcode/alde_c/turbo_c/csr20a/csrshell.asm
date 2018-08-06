
Comment *

	Shell for assembly routine interface to Microsoft C.

	Provided as a convenience for C Spot Run C Add-On Library users.

	Note on return values:  The C function is given the value of AX
	upon return of a char, short, int, or any of these three unsigned.
	On return of a long or unsigned long, the high order word goes in
	DX, and the low in AX.  A struct, union, float, or double must be
	returned with the address in AX and the value in a static area of
	memory.  A near pointer is returned in AX, and a far with the 
	segment selector in DX and the offset in AX.

	For more information on assembly interface, consult Chapter 8
	(Pages 157-172) of the Microsoft 3.0 User's Guide.

*

;=============================================================================
;				Declarations
;=============================================================================

TRUE	EQU	1
FALSE	EQU	0

LARGE	EQU	FALSE			; TRUE for Large Model

if	LARGE
@AB	EQU	6
else
@AB	EQU	4
endif

ARG1	EQU	@AB
ARG2	EQU	@AB+2
ARG3	EQU	@AB+4
ARG4	EQU	@AB+6
ARG5	EQU	@AB+8
ARG6	EQU	@AB+10
ARG7	EQU	@AB+12

;=============================================================================
;				    Data
;=============================================================================

DGROUP	group	_DATA
_DATA	segment word public 'DATA'
	assume	ds:DGROUP

	; Your Data goes here . . .

_DATA	ends

;=============================================================================
;				   Code
;=============================================================================

	assume cs:_text

_text	segment public	byte	'code'

	public	_shell

if	LARGE
_shell	proc	far
else
_shell	proc	near
endif

	push	bp 			; Save Base of Stack
	mov	bp,sp			; Establish Stack Frame

	push	si 			; Save MSC's Register Vars
	push	di 			; Save MSC's Register Vars
	push	ds 			; Save Data and Extra Segs
	push	es

	;===============================
	;	   User Code
	;===============================
	
		mov	bh,[bp+ARG1]		; Sample of Getting Argument
		mov	ah,[bp+ARG2]		; Another Sample

	pop	es			; Restore Data and Extra Segs
	pop	ds
	pop	di			; Restore MSC's Register Vars
	pop	si			; Restore MSC's Register Vars

	mov	sp,bp			; Restore Stack Pointer
	pop	bp 			; and Base of Stack

	ret				; Return to Caller

_shell	endp

_text	ends

end
