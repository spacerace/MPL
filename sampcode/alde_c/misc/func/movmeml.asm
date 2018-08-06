	TITLE 'MOVMEML Move memory long'
	COMMENT /*
*  Name:
*	movmeml
*
*  Synopsis:
*
*	movmeml(ss,s,ds,d,n);
*	unsigned ss;		source segment
*	char *s;		source memory block (offset)
*	unsigned ds;		destination segment
*	char *d;		destination memory block (offset)
*	unsigned n;		number of bytes to transfer.
*
*  Description
*	Moves memory from one location to another.  The function
*	calls executes the same code as movmem,but sets the segment regesters
*	ES and DS to the source segment and the destination segment.
*
*  Cautions
*	Care must be taken in where the data is moved..
*/
PGROUP	GROUP	PROG
MOVMEMA STRUC
OLD_BP	DW	?	;CALLER'S BP SAVE
RETN	DW	?	;RETURN ADDRESS
FROMS	DW	?	;FROM SEGMENT
FROMO	DW	?	;FROM OFFSET
TOARGS	DW	?	;TO SEGMENT
TOARGO	DW	?	;TO OFFSET
LEN	DW	?	;LENGTH TO MOVE (UNSIGNED)
MOVMEMA ENDS
PROG	SEGMENT BYTE PUBLIC 'PROG'
	PUBLIC	MOVMEML,RETDSEG
	EXTRN	MOVMEM:NEAR
	ASSUME	CS:PGROUP
SAVEDS	DW	?
SAVEES	DW	?
MOVMEML PROC	NEAR
	PUSH	BP
	MOV	BP,SP
	MOV	CX,[BP].LEN
	MOV	SI,[BP].FROMO
	MOV	DI,[BP].TOARGO
;
	PUSH	DS
	PUSH	ES
	MOV	AX,SS:[BP].TOARGS
	MOV	ES,AX
	MOV	AX,SS:[BP].FROMS
	MOV	DS,AX
;
	 CMP	   SI,DI
	 JB	   X1
	 CLD
	 JMP	   X2
X1:	 STD
	 ADD	   SI,CX
	 ADD	   DI,CX
	 DEC	   SI
	 DEC	   DI
X2:  REP MOVSB
	POP	ES
	POP	DS
	POP	BP
	RET
MOVMEML ENDP
	COMMENT /*
*  Name:
*	retdseg
*
*  Synopsis:
*
*	n = retdseg();
*	unsigned n;		contains the current data segment address
*
*  Description:
*	Returns the curren data segment (DS) address.
*/
RETDSEG PROC	NEAR
	MOV	AX,DS
	RET
RETDSEG ENDP
PROG	ENDS
	END
