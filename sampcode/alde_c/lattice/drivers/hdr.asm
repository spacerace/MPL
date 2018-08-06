	PAGE	60, 132
TITLE	HDR	22-Feb-85	Device Driver Header			|

;-----------------------------------------------------------------------|
;									|
;	Device Driver Library						|
;		Device Driver Header					|
;									|
;-----------------------------------------------------------------------|
;	REVISION HISTORY						|
;									|
; Number    DD-MMM-YY		  WHO			WHY		|
;-------|---------------|-----------------------|-----------------------|
; 0.0	|   22-Feb-85	| Frank Whaley		| Initial Release	|
;-----------------------------------------------------------------------|

	PAGE
;-----------------------------------------------------------------------|
;	Equates								|
;-----------------------------------------------------------------------|

StkSiz	EQU	2048			; local stack size

	PAGE
;-----------------------------------------------------------------------|
;	Group Selection							|
;-----------------------------------------------------------------------|

PGROUP	Group	PROG, TAIL
DGROUP	Group	DATA, DTAIL

PROG	Segment Para Public 'PROG'
PROG	EndS

DATA	Segment Para Public 'DATA'	; define first
DATA	EndS

	Assume	CS:PROG, DS:DATA, ES:DATA, SS:DATA

	PAGE
;-----------------------------------------------------------------------|
;	Program Segment							|
;-----------------------------------------------------------------------|

PROG	Segment Para Public 'PROG'

	Extrn	Init:Near,	MediaChe:Near,	BuildBPB:Near
	Extrn	IoCtlIn:Near,	Input:Near,	ndInput:Near
	Extrn	InputSta:Near,	InputFlu:Near,	Output:Near
	Extrn	OutVerif:Near,	OutStatu:Near,	OutFlush:Near
	Extrn	IoCtlOut:Near,	DevOpen:Near,	DevClose:Near
	Extrn	RemMedia:Near

	ORG	0

HDR	Proc	Far

;-----------------------------------------------------------------------|
;	Device Header							|
;-----------------------------------------------------------------------|

	DD	-1			; -> next device
	DW	theAttribute		; you must enter attribute field
	DW	Strategy		; -> device strategy
	DW	Interrupt		; -> device interrupt
	DB	theName			; you must put something here

;-----------------------------------------------------------------------|
;	Code Segment Variables						|
;-----------------------------------------------------------------------|

RHptr	DD	(?)			; -> Request Header
ssEntry	DW	(?)			; entry SS
spEntry	DW	(?)			; entry SP

	PAGE
;-----------------------------------------------------------------------|
;	Device Strategy							|
;									|
;	ENTRY :	ES:BX -> Request Header					|
;									|
;	EXIT :	Request Header copied to ReqHdr				|
;		all registers preserved					|
;									|
;-----------------------------------------------------------------------|

Strategy:

	MOV	Word Ptr CS:RHptr,BX	; save request header ptr
	MOV	Word Ptr CS:RHptr + 2,ES

	PUSHF				; (+1) save the world
	PUSH	ES			; (+2)
	PUSH	DS			; (+3)
	PUSH	SI			; (+4)
	PUSH	DI			; (+5)
	PUSH	CX			; (+6)
	PUSH	BX			; (+7)

	MOV	SI,BX
	MOV	BX,ES
	MOV	DS,BX			; DS:SI -> Request Header

	MOV	BX,Offset PGROUP:TAIL
	MOV	CL,4
	SHR	BX,CL
	MOV	CX,CS
	ADD	BX,CX
	MOV	ES,BX
	MOV	DI,Offset DGROUP:ReqHdr	; ES:DI -> ReqHdr

	CLD
	XOR	CH,CH
	MOV	CL,[SI]
	REP	MOVSB			; copy Request Header
	
	POP	BX			; (+6) restore
	POP	CX			; (+5)
	POP	DI			; (+4)
	POP	SI			; (+3)
	POP	DS			; (+2)
	POP	ES			; (+1)
	POPF				; (+0)
	RET

	PAGE
;-----------------------------------------------------------------------|
;	Device Interrupt						|
;									|
;	ENTRY :	anything						|
;									|
;	EXIT :	all registers preserved					|
;									|
;-----------------------------------------------------------------------|

Interrupt:

	PUSH	DS			; (+1) save the world
	PUSH	ES			; (+2)
	PUSH	AX			; (+3)
	PUSH	BX			; (+4)
	PUSH	CX			; (+5)
	PUSH	DX			; (+6)
	PUSH	SI			; (+7)
	PUSH	DI			; (+8)
	PUSH	BP			; (+9)

	MOV	CS:ssEntry,SS		; save entry SS
	MOV	CS:spEntry,SP		; and SP

	MOV	AX,Offset PGROUP:TAIL	; set our DS, SS, BP, and SP
	MOV	CL,4
	SHR	AX,CL
	MOV	CX,CS
	ADD	AX,CX
	MOV	BX,Offset DGROUP:MyStack
	MOV	DS,AX
	MOV	ES,AX
	MOV	SS,AX
	MOV	SP,BX
	MOV	BP,BX

	;
	; call our function
	;
	MOV	AL,ReqHdr + 2		; AL = Command Code
	SHL	AL,1
	CBW
	MOV	SI,Offset DGROUP:FuncTab
	ADD	SI,AX
	CALL	Word Ptr [SI]

	;
	; copy back Request Header
	;
	LES	DI,RHptr		; ES:DI -> original space
	MOV	SI,Offset DGROUP:ReqHdr	; DS:SI -> our (updated) copy
	CLD
	XOR	CH,CH
	MOV	CL,[SI]
	REP	MOVSB			; copy Request Header

	MOV	SS,CS:ssEntry		; restore original stuff
	MOV	SP,CS:spEntry

	POP	BP			; (+8) restore
	POP	DI			; (+7)
	POP	SI			; (+6)
	POP	DX			; (+5)
	POP	CX			; (+4)
	POP	BX			; (+3)
	POP	AX			; (+2)
	POP	ES			; (+1)
	POP	DS			; (+0)
	RET

HDR	EndP

PROG	EndS

TAIL	Segment Public 'PROG'		; for finding end of code segment
TAIL	EndS

	PAGE
;-----------------------------------------------------------------------|
;	Data Segment							|
;-----------------------------------------------------------------------|

DATA	Segment Para Public 'DATA'

	Public	ReqHdr

	DB	StkSiz DUP (?)		; our stack, overflows into code
MyStack	Label	Word

FuncTab	Label Word
	DW	Offset PGROUP:Init
	DW	Offset PGROUP:MediaChe
	DW	Offset PGROUP:BuildBPB
	DW	Offset PGROUP:IoCtlIn
	DW	Offset PGROUP:Input
	DW	Offset PGROUP:ndInput
	DW	Offset PGROUP:InputSta
	DW	Offset PGROUP:InputFlu
	DW	Offset PGROUP:Output
	DW	Offset PGROUP:OutVerif
	DW	Offset PGROUP:OutStatu
	DW	Offset PGROUP:OutFlush
	DW	Offset PGROUP:IoCtlOut
	DW	Offset PGROUP:DevOpen
	DW	Offset PGROUP:DevClose
	DW	Offset PGROUP:RemMedia

ReqHdr	DB	256 DUP (?)		; copy of Request Header

DATA	EndS

DTAIL	Segment Public 'DATA'		; for finding end of data segment
DTAIL	EndS

	END	HDR			; of HDR.ASM
