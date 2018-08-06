;	Static Name Aliases
;
	TITLE   ov.c
	NAME    ov

	.8087
OV_TEXT	SEGMENT  WORD PUBLIC 'CODE'
OV_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME  CS: OV_TEXT, DS: DGROUP, SS: DGROUP
EXTRN	__acrtused:ABS
EXTRN	_printf:FAR
EXTRN	__chkstk:FAR
EXTRN	_strcpy:FAR
EXTRN	_p1:FAR
EXTRN	_p2:FAR
EXTRN	_executable_name:BYTE
_DATA      SEGMENT
$SG197	DB	'This is main',  0aH,  00H
_DATA      ENDS
OV_TEXT      SEGMENT
	ASSUME	CS: OV_TEXT
; Line 10
	PUBLIC	_main
_main	PROC FAR
	push	bp
	mov	bp,sp
	mov	ax,2
	call	FAR PTR __chkstk
	push	di
	push	si
;	argc = 6
;	argv = 8
;	i = -2
; Line 11
; Line 13
	les	bx,DWORD PTR [bp+8]	;argv
	push	WORD PTR es:[bx+2]
	push	WORD PTR es:[bx]
	mov	ax,OFFSET _executable_name
	mov	dx,SEG _executable_name
	push	dx
	push	ax
	call	FAR PTR _strcpy
	add	sp,8
; Line 14
	mov	ax,WORD PTR [bp+6]	;argc
	mov	WORD PTR [bp+6],ax	;argc
; Line 16
	mov	ax,OFFSET DGROUP:$SG197
	push	ds
	push	ax
	call	FAR PTR _printf
	add	sp,4
; Line 17
	mov	WORD PTR [bp-2],0	;i
	jmp	$F198
$FC199:
	inc	WORD PTR [bp-2]	;i
$F198:
	cmp	WORD PTR [bp-2],1000	;i
	jl	$JCC74
	jmp	$FB200
$JCC74:
; Line 18
	call	FAR PTR _p1
; Line 19
	call	FAR PTR _p2
; Line 20
	jmp	$FC199
$FB200:
; Line 21
$EX195:
	pop	si
	pop	di
	mov	sp,bp
	pop	bp
	ret	

_main	ENDP
OV_TEXT	ENDS
END
