;                         *** dosdate.asm ***
;
; IBM-PC microsoft "C" under PC-DOS
;
; Microsoft "C" callable 8088 assembly routine that uses a DOS function
; call to obtain and return the month, day, and year as integers.
;
; synopsis	dosdate(&imo,&iday,&iyr);
;
;		*** no function value returned ***
;		int imo;	1-12
;		int iday;	1-31
;		int iyr;	1980-2099
;
; Written by L. Cuthbertson, May 1984
;
;**********************************************************************
;
PGROUP	GROUP	PROG
PROG	SEGMENT	BYTE PUBLIC 'PROG'
	PUBLIC	DOSDATE
	ASSUME	CS:PGROUP
;
; ********************************************************************
;
DOSDATE	PROC	NEAR
	PUSH	BP
	MOV	BP,SP
	MOV	AH,2AH		; DOS GET DATE FUNCTION NUMBER
	INT	21H		; DOS FUNCTION CALL INTERRUPT
	MOV	AL,DH		; MOVE MONTH INTO AX
	XOR	AH,AH		; CLEAR HIGH BYTE
	MOV	DI,[BP+4]	; ADDRESS OF MONTH VARIABLE
	MOV	[DI],AX		; MOVE MONTH INTO MONTH VARIABLE
	MOV	BL,DL		; MOVE DAY INTO BX
	XOR	BH,BH		; CLEAR HIGH BYTE
	MOV	DI,[BP+6]	; ADDRESS OF DAY VARIABLE
	MOV	[DI],BX		; MOVE DAY INTO DAY VARIABLE
	MOV	DI,[BP+8]	; ADDRESS OF YEAR VARIABLE
	MOV	[DI],CX		; MOVE YEAR INTO YEAR VARIABLE
	POP	BP
	RET
DOSDATE	ENDP
;
PROG	ENDS
	END
