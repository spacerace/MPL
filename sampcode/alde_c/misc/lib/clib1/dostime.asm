;                         *** dostime.asm ***
;
; IBM-PC microsoft "C" under PC-DOS
;
; Microsoft "C" callable 8088 assembly routine that uses a DOS function
; call to obtain and return the hours, minutes, seconds, and hundreths
; of a second as integers.
;
; synopsis	dostime(&hour,&min,&sec,&ths);
;
;		*** no function value returned ***
;		int hour;	0-23  military time
;		int min;	0-59
;		int sec;	0-59
;		int ths;	0-99
;
;
; Written by L. Cuthbertson, May 1984
;
;**********************************************************************
;
PGROUP	GROUP	PROG
PROG	SEGMENT	BYTE PUBLIC 'PROG'
	PUBLIC	DOSTIME
	ASSUME	CS:PGROUP
;
; ********************************************************************
;
DOSTIME	PROC	NEAR
	PUSH	BP
	MOV	BP,SP
	MOV	AH,2CH		; DOS GET TIME FUNCTION NUMBER
	INT	21H		; DOS FUNCTION CALL INTERRUPT
	MOV	AL,CH		; MOVE HOURS INTO AX
	XOR	AH,AH		; CLEAR HIGH BYTE
	MOV	DI,[BP+4]	; ADDRESS OF HOURS VARIABLE
	MOV	[DI],AX		; MOVE HOURS INTO HOURS VARIABLE
	MOV	BL,CL		; MOVE MINUTES INTO BX
	XOR	BH,BH		; CLEAR HIGH BYTE
	MOV	DI,[BP+6]	; ADDRESS OF MINUTES VARIABLE
	MOV	[DI],BX		; MOVE MINUTES INTO MINUTES VARIABLE
	MOV	CL,DH		; MOVE SECONDS INTO CX
	XOR	CH,CH		; CLEAR HIGH BYTE
	MOV	DI,[BP+8]	; ADDRESS OF SECONDS VARIABLE
	MOV	[DI],CX		; MOVE SECONDS INTO SECONDS VARIABLE
	XOR	DH,DH		; CLEAR HIGH BYTE
	MOV	DI,[BP+10]	; ADDRESS OF THS VARIABLE
	MOV	[DI],DX		; MOVE 1/100THS INTO THS VARIABLE
	POP	BP
	RET
DOSTIME	ENDP
;
PROG	ENDS
	END
