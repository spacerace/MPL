;****************************************************************************
;*									    *
;*  SCRXMS.ASM -							    *
;*									    *
;*	Adapted from the XMM C Library Routines (c) 1988 Microsoft Corp.    *
;*									    *
;****************************************************************************


;============================================================================
;   DATA SEGMENT
;============================================================================

_DATA	segment WORD PUBLIC 'DATA'

XMM_Initialized dw	0		; 1 if the XMS driver has been found
XMM_Control	label	dword		; Points to the XMS control function
		dw	offset _XMM_NotInitialized
		dw	seg _TEXT
_DATA	ends


DGROUP	GROUP _DATA

;============================================================================
;   CODE SEGMENT
;============================================================================

_TEXT	segment WORD PUBLIC 'CODE'

assume cs:_TEXT
assume ds:DGROUP

;*--------------------------------------------------------------------------*
;*									    *
;*  _XMM_NotInitialized -						    *
;*									    *
;*	Called by default if the program hasn't called _XMM_Installed or    *
;*	if no XMS driver is found.					    *
;*									    *
;*--------------------------------------------------------------------------*

_XMM_NotInitialized proc far

	xor	ax,ax		   ; Simulate an XMS "Not Implemented" error
	mov	bl,80h
	ret

_XMM_NotInitialized endp


;*--------------------------------------------------------------------------*
;*									    *
;*  BOOL _XMM_Installed(void);						    *
;*									    *
;*	Called to initialize the XMM library routines.			    *
;*									    *
;*--------------------------------------------------------------------------*

public _XMM_Installed

_XMM_Installed proc near

	; Have we already been initialized?
	cmp	[XMM_Initialized],0
	jne	Already_Initialized		; Yup, return TRUE

	; Is an XMS driver installed?
	mov	ax,4300h
	int	2Fh
	cmp	al,80h
	jne	NoDriver			; Nope, return FALSE

	; Get and store the address of the driver's control function.
	mov	ax,4310h
	int	2Fh
	mov	word ptr [XMM_Control],bx
	mov	word ptr [XMM_Control+2],es

	; Set XMM_Initialized to TRUE.
	inc	[XMM_Initialized]

Already_Initialized:
NoDriver:
	mov	ax,[XMM_Initialized]
	ret

_XMM_Installed endp


;*--------------------------------------------------------------------------*
;*									    *
;*  WORD _XMM_Version(void);						    *
;*									    *
;*	Returns the driver's XMS version number.			    *
;*									    *
;*--------------------------------------------------------------------------*

public _XMM_Version

_XMM_Version proc near

	xor	ah,ah				; Call XMS Function 0
	call	[XMM_Control]			; Sets AX to XMS Version
	ret

_XMM_Version endp


;*--------------------------------------------------------------------------*
;*									    *
;*  long _XMM_AllocateExtended(int SizeK);				    *
;*									    *
;*	Allocates an Extended Memory Block and returns its handle.	    *
;*									    *
;*--------------------------------------------------------------------------*

public _XMM_AllocateExtended

_XMM_AllocateExtended proc near

	push	bp				; Create a stack frame
	mov	bp,sp

	; Call the XMS Allocate function.
	mov	ah,9
	mov	dx,[bp+4]			; [bp+4] is the parameter
	call	[XMM_Control]

	; Was there an error?
	or	ax,ax
	jz	AEFail				; Yup, fail
	mov	ax,dx				; Return XMS Handle in AX
	mov	dx,0				; Zero out DX
	jnz	AESuccess
AEFail:
	mov	dh,bl				; Put error return in DH

AESuccess:
	pop	bp				; Restore the stack
	ret

_XMM_AllocateExtended endp


;*--------------------------------------------------------------------------*
;*									    *
;*  long _XMM_FreeExtended(WORD Handle);				    *
;*									    *
;*	Deallocates an Extended Memory Block.				    *
;*									    *
;*--------------------------------------------------------------------------*

public _XMM_FreeExtended

_XMM_FreeExtended proc near

	push	bp				; Create a stack frame
	mov	bp,sp

	; Call the XMS FreeExtended function.
	mov	ah,0Ah
	mov	dx,[bp+4]			; [bp+4] is the parameter
	call	[XMM_Control]

	xor	dx,dx				; Zero DX

	; Was there an error?
	or	ax,ax
	jz	FESuccess			; Nope, return
	mov	dh,bl				; Yup, return err code in DH
FESuccess:
	pop	bp				; Restore the stack
	ret

_XMM_FreeExtended endp


;*--------------------------------------------------------------------------*
;*									    *
;*  long _XMM_MoveExtended(XMSMOVE *pInfo);				    *
;*									    *
;*	Moves a block of data into/outof/within extended memory.	    *
;*									    *
;*--------------------------------------------------------------------------*

public _XMM_MoveExtended

_XMM_MoveExtended proc near

	push	bp				; Create a stack frame
	mov	bp,sp

	; Call the XMS MoveExtended function.
	mov	ah,0Bh
	mov	si,[bp+4]			; [bp+4] is the parameter
	call	[XMM_Control]

	xor	dx,dx				; Zero DX

	; Was there an error?
	or	ax,ax
	jz	MESuccess			; Nope, return
	mov	dh,bl				; Yup, return err code in DH
MESuccess:
	pop	bp				; Restore the stack
	ret

_XMM_MoveExtended endp


_TEXT	ends

end
