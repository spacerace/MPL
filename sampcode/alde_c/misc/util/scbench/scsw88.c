/*
** 8088/8086 version
** Routine to get the time of day -- BYTE Small C
** MS-DOS version for 8088.
** tblock[] is assumed to be a 4-element int array.
** tblock[0]=hours
** tblock[1]=minutes
** tblock[2]=seconds
** tblcok[3]=hundredths of seconds
*/
gtime(tblock) int tblock[];
{
#asm
	MOV	AH,2CH	;Function to get time
	INT	21H	;Do it
	XOR	AX,AX	;Clear high part
	MOV	BP,SP
	MOV	SI,2[BP] ;Get pointer to array
	MOV	AL,CH
	MOV	[SI],AX	;Hours
	MOV	AL,CL
	MOV	2[SI],AX ;Minutes
	MOV	AL,DH
	MOV	4[SI],AX ;Seconds
	MOV	AL,DL
	MOV	6[SI],AX ;Hundredths
	XOR	CX,CX	;Clear CX
#endasm
}

/*
** 8088/8086 version
** calctim() - calculate time difference between time stored in
** tblock[] and current time.
** tblock[] is a 4-element array:
** tblock[0] = hours
** tblock[1] = minutes
** tblock[2] = seconds
** tblcok[3] = 100/ths of a second
** calctim() returns its results in tblock[]
*/
calctim(tblock) int tblock[]; {
#asm
	MOV	AH,2CH	;Get time
	INT	21H
	MOV	BP,SP
	MOV	SI,2[BP] ;Pointer to tblock[]
	MOV	AX,6[SI] ;Get 100/ths of a second
	SUB	DL,AL
	JNS	CAL1
	ADD	DL,100
	DEC	DH
CAL1:	MOV	6[SI],DL
	MOV	AX,4[SI] ;Get seconds
	SUB	DH,AL
	JNS	CAL2
	ADD	DH,60
	DEC	CL
CAL2:	MOV	4[SI],DH
	MOV	AX,2[SI] ;Get minutes
	SUB	CL,AL
	JNS	CAL3
	ADD	CL,60
	DEC	CH
CAL3:	MOV	2[SI],CL
	MOV	AX,[SI]  ;Get hours
	SUB	CH,AL
	MOV	[SI],CH
	XOR	CX,CX
#endasm
}
