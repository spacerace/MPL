LPROG EQU 0
LDATA EQU 0
COM   EQU 0
INCLUDE DOS.MAC
	PSEG
	PUBLIC	TIME
        IF LPROG
TIME	PROC	FAR
        ELSE
TIME	PROC	NEAR
	ENDIF
	PUSH	BP
	MOV	BP,SP
	MOV	BX,[BP+X]
	MOV	AH,2CH
	INT	21H
	XOR	AX,AX
	MOV	[BX],CH
	MOV	[BX+1],AL
	MOV	[BX+2],CL
	MOV	[BX+3],AL
	MOV	[BX+4],DH
	MOV	[BX+5],AL
	MOV	[BX+6],DL
	MOV	[BX+7],AL
	POP	BP
	RET
TIME	ENDP
        ENDPS
	END
S
	END
S
	END
