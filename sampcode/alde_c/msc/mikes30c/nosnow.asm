; THIS IS A BOILER PLATE THAT ALLOWS YOU TO INTERFACE ASSEMBLY ROUTINES
; WITH A MICRSOFT 3.0 C PROGRAM.
; THIS PROGRAM CONTAINS TWO FUNCTIONS CALLED IN THE FOLLOWING MANOR:
;
;            scrnsave(seg_ptr, offset_ptr)
;            flash(seg_ptr, offset_ptr)
;            char far seg_ptr, offset_ptr;
;
;      Example:
;            flash(FP_SEG(holdscn1), FP_OFF(holdscn1));
;
; These pointers should point to a 4000 byte buffer ie. 
;     char far savebuf[4000];
; The first function will save the current screen into the buffer and 
; the second function will put a formatted screen (attribute, char,...etc)
; onto the screen very fast without the "snow" usually associated with
; the movedata() function. FP_SEG & FP_OFF are documented in the 
; Microsoft manual. I have used this this routine to create professional
; looking and fast screens that overcome the snails pace that printf()
; is known for. These routines automatically check for Mono or graphic
; adapter cards. Don't forget the /Fe compiler switch to enable the 
; "far" keyword and that the first byte of every character on the screen
; is an attribute value. See scrnput.c for further examples.
;
;                  by Garland Wong & Mike Elkins
;                  Mike's "C" Board  619-722-8724
;                       Oceanside, Calif.
;

SCREEN		SEGMENT	AT 0B000H
SCREEN		ENDS

_TEXT	SEGMENT	BYTE	PUBLIC	'CODE'
	ASSUME	CS:_TEXT
	PUBLIC	_FLASH,_SCRNSAVE                   

_FLASH 	PROC 	NEAR
	ASSUME DS:SCREEN
	PUSH	BP			;SAVE CALLER'S BP
	MOV	BP,SP			;ALLOWS US TO SEE THE STACK

	PUSH 	DS			;SAVE THE REGISTERS FOR GOOD FORM
	PUSH 	ES
	PUSH 	DI
	PUSH 	DX
	PUSH	SI
	PUSH 	CX
	PUSH	BX
	PUSH	AX
	
	MOV	DX,[BP+4]		;GET OFFSET 
	MOV	CS:SEG_SCRN,DX
	MOV	DX,[BP+6]		;GET SEGMENT
	MOV	CS:PNT_TO_OLD_SCRN,DX
 
	MOV	AH,15
	INT	10H
	SUB	AH,8
	SHL	AH,1
	MOV	CS:VIDEO_PORT,03BAH	;THIS IS FOR MONOCHROME PORT
	MOV	CS:POINT_TO_SCREEN,0	;MAKE OFFSET THE MONOCHROME BUFFER
	TEST	AL,4
	JNZ	NOT_GRAPHICS
	MOV	CS:VIDEO_PORT,03DAH	;THIS IS FOR COLOR GRAPHICS PORT
	MOV	CS:POINT_TO_SCREEN,8000H	;MAKE OFFSET THE GRAPHICS BUFFER

NOT_GRAPHICS:
	MOV	CX,SCREEN
	MOV	DS,CX
	MOV	DI,CS:POINT_TO_SCREEN
	MOV	CX,2000
	MOV	AX,CS:SEG_SCRN
	MOV	ES,AX
	MOV	SI,CS:PNT_TO_OLD_SCRN		;MOVE OFFSET OF SCREEN TO SI
	MOV	DX,CS:VIDEO_PORT		;mike this can be either monochrome or graphics
	CLI

SCAN_LOW:
	IN	AL,DX
	TEST	AL,1
	JNZ	SCAN_LOW
	MOV	BX,ES:[SI]			;here is where you move the character into a temporary holder until okay to send
	
SCAN_HIGH:
	IN	AL,DX
	TEST	AL,1
	JZ	SCAN_HIGH
	MOV	[DI],BX				;here is the actual move to the screen memory map
	ADD	DI,2
	ADD	SI,2
	LOOP	SCAN_LOW			;DO UNTIL SCREEN UPDATED
	STI

	POP	AX
	POP	BX
	POP	CX
	POP 	SI
	POP	DX
	POP 	DI
	POP	ES
	POP	DS

	MOV	SP,BP			;GET BACK CALLER'S STACK POINTER
	POP	BP			;GET BACK BASE POINTER
	RET
_FLASH	ENDP	

_SCRNSAVE	PROC	NEAR
	PUSH	BP			;SAVE CALLER'S BP
	MOV	BP,SP			;ALLOWS US TO SEE THE STACK
	ASSUME 	DS:SCREEN
	PUSH 	DS
	PUSH 	ES
	PUSH 	DI
	PUSH 	DX
	PUSH	SI
	PUSH 	CX
	PUSH	BX
	PUSH	AX

	MOV	AH,15
	INT	10H
	SUB	AH,8
	SHL	AH,1
	MOV	CS:VIDEO_PORT,03BAH	;THIS IS FOR MONOCHROME PORT
	MOV	CS:POINT_TO_SCREEN,0	;MAKE OFFSET THE MONOCHROME BUFFER
	TEST	AL,4
	JNZ	IS_MONO
	MOV	CS:VIDEO_PORT,03DAH	;THIS IS FOR COLOR GRAPHICS PORT
	MOV	CS:POINT_TO_SCREEN,8000H	;MAKE OFFSET THE GRAPHICS BUFFER

IS_MONO:
	
	MOV	DX,[BP+4]		;GET OFFSET 
	MOV	CS:SEG_SCRN,DX
	MOV	DX,[BP+6]		;GET SEGMENT
	MOV	CS:PNT_TO_OLD_SCRN,DX
	MOV	CX,SCREEN
	MOV	DS,CX
	MOV	SI,CS:POINT_TO_SCREEN
	MOV	CX,2000
	MOV	AX,CS:SEG_SCRN
	MOV	ES,AX
	MOV	DI,CS:PNT_TO_OLD_SCRN		;MOVE OFFSET OF SCREEN TO SI
	MOV	DX,CS:VIDEO_PORT
	CLI

SCAN_LOW_READ:
	IN	AL,DX
	TEST	AL,1
	JNZ	SCAN_LOW_READ
	
SCAN_HIGH_READ:
	IN	AL,DX
	TEST	AL,1
	JZ	SCAN_HIGH_READ
	MOV	BX,WORD PTR [SI]
	MOV	ES:[DI],BX
	ADD	DI,2
	ADD	SI,2
	LOOP	SCAN_LOW_READ		;DO UNTIL SCREEN UPDATED
	STI

	POP	AX	
	POP	BX	
	POP	CX
	POP 	SI
	POP	DX
	POP 	DI
	POP	ES
	POP	DS

	MOV	SP,BP			;GET BACK CALLER'S STACK POINTER
	POP	BP			;GET BACK BASE POINTER
	RET
_SCRNSAVE	ENDP

POINT_TO_SCREEN 	DW	0
PNT_TO_OLD_SCRN		DW	0
VIDEO_PORT		DW	0
OLD_SCREEN		DW	0
SEG_SCRN		DW	0
_TEXT	ENDS
	END



