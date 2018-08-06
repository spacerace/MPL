/*
**
** BYTE Small-C Video Support Routines
** NOTE: Many of these routines were adapted from code presented
**       in Richard Wilton's PROGRAMMER'S GUIDE TO PC & PS/2
**       VIDEO SYSTEMS (Microsoft Press, ISBN 1-55615-103-9)
**       (A *fabulous* book, by the way.)
**       Also, Robert Jourdain's PROGRAMMER'S PROBLEM SOLVER
**       FOR THE IBM PC,XT & AT (Brady Books, ISBN 0-89303-787-7)
**       (Another fine book.)
**
** setp(x,y,color,mode)
** Set pixel at x,y to color. Mode is mode #.
** This function assumes that ES points to the video segment.
*/
setp(x,y,color,mode)
int x,y,color,mode;
{
#asm
	MOV	BP,SP		;Get pointer to arguments
	MOV	AX,6[BP]	;y coord
	MOV	BX,8[BP]	;x coord
	MOV     CX,2[BP]	;Mode
;Determine which routine to execute using mode
;First check for Hercules
	CMP	CL,255
	JNE	SETP10
;
; Hercules mode
	CALL CALCPH
SETP01:
	MOV	AL,4[BP]	;Get color
	SHL	AX,CL		;Shift in
	NOT	AH
	AND	ES:[BX],AH	;Zero the pixel
	OR	ES:[BX],AL	;Set it
	XOR	CX,CX		;For small c
	RET
;
SETP10:
	CMP	CL,6
	JGE	SETP20
;
;Modes 4 and 5
;
	CALL CALCP4
	JMP	SHORT SETP01	;Borrow code
;
SETP20:
	CMP	CL,13
	JGE	SETP30
;
;Mode 6
;
	CALL CALCP6
	JMP	SHORT SETP01	;Borrow code
;
SETP30:
	CMP	CL,14
	JGE	SETP40
;
;Mode 13
;
	CALL CALCP13
SETP31:
	SHL	AH,CL		;Position mask
	MOV	DX,3CEH		;Address register port
	MOV	AL,8		;Mask register number
	OUT	DX,AX
	MOV	AX,205H		;Mode register number 2, write mode 2
	OUT	DX,AX
	MOV	AH,0		;Replace mode
	MOV	AL,3		;Data rotate/function select register
	OUT	DX,AX
	MOV	AL,ES:[BX]	;Load the latches
	MOV	AL,4[BP]	;Get the color
	MOV	ES:[BX],AL	;Store it
	MOV	AX,0FF08H	;Restore defaults
	OUT	DX,AX
	MOV	AX,5
	OUT	DX,AX
	MOV	AX,3
	OUT	DX,AX
	XOR	CX,CX
	RET
;
SETP40:
	CMP	CL,17
	JGE	SETP50
;
; Modes 14,15,16
;
SETP41:
	CALL CALCP14
	JMP	SHORT SETP31	;Borrow code
;
SETP50:
	CMP	CL,18
	JGE	SETP60
;
; Mode 17
;
	CALL CALCP14
	JMP SHORT SETP01	;Borrow code
;
SETP60:
	CMP	CL,18
	JZ	SETP41			;18 same as EGA
;
; Mode 19
;
	CALL CALCP19
	MOV	AL,4[BP]	;Get color
	MOV	ES:[BX],AL	;set it
	XOR	CX,CX		;For small c
	RET
;
; Calculate pixel address for modes 4 and 5
; Entry: AX = y coord
;        BX = x coord
; Exit:  AH = bit mask
;        BX = byte offset
;        CL = number of bits to shift
	PUBLIC CALCP4
CALCP4:
	MOV CL,BL
	XCHG AH,AL
	SHR AX,1
	ADD BH,AL
	XOR AL,AL
	ADD BX,AX
	SHR AX,1
	SHR AX,1
	ADD BX,AX
	SHR BX,1
	SHR BX,1
	MOV AH,3
	AND CL,AH
	XOR CL,AH
	SHL CL,1
	RET
;
; Calculate address of pixel for mode 6
; Entry: See abov
; Exit: See above
;
	PUBLIC CALCP6
CALCP6:
	MOV CL,BL
	XCHG AH,AL
	SHR BX,1
	SHR AX,1
	ADD BH,AL
	XOR AL,AL
	ADD BX,AX
	SHR AX,1
	SHR AX,1
	ADD BX,AX
	SHR BX,1
	SHR BX,1
	AND CL,7
	XOR CL,7
	MOV AH,1
	RET
;
; Calculate pixel address for mode 13
; Entry: See above
; Exit: See above
;
	PUBLIC CALCP13
CALCP13:
	MOV CL,BL
	MOV DX,40
CP13COM:
	MUL DX 
	SHR BX,1
	SHR BX,1
	SHR BX,1
	ADD BX,AX
	AND CL,7
	XOR CL,7
	MOV AH,1
	RET
;
; Calculate pixel address for modes 14,15,16,17,18
; Entry: See above
; Exit: See above
;
	PUBLIC CALCP14
CALCP14:
	MOV CL,BL
	MOV DX,80
	JMP SHORT CP13COM
;
; Calculate pixel address for mode 19
; Entry: See above
; Exit: See above
; Note: we don't need a bit mask in AH or a shift count in CL 
;       for this mode.
;
	PUBLIC CALCP19
CALCP19:
	XCHG AH,AL
	ADD BX,AX
	SHR AX,1
	SHR AX,1
	ADD BX,AX
	RET
;
; Calculate pixel address for Hercules graphics mode
;
	PUBLIC CALCPH
CALCPH:
	MOV	CL,BL
	SHR AX,1
	RCR BX,1
	SHR AX,1
	RCR BX,1
	SHR BX,1
	MOV AH,90
	MUL AH
	ADD BX,AX
	AND CL,7
	XOR CL,7
	MOV AH,1
	RET
#endasm
}

/*
** getp(x,y,mode)
** Returns value of pixel at x,y
** mode holds current graphics mode
*/
getp(x,y,mode)
int x,y,mode; {
#asm
	MOV BP,SP	;Get pointer to arguments
	MOV CX,2[BP]	;Get mode
	MOV AX,4[BP]	;Get y coord
	MOV BX,6[BP]	;Get x coord
;
	CMP	CL,255		;Hercules mode selected by 255
	JNE	GETP10
;
; Hercules monochrome mode
;
	CALL CALCPH
GETP01:
	MOV	BL,ES:[BX]
	SHR BL,CL
	AND BL,AH
	XOR BH,BH
	XOR CX,CX
	RET
;
GETP10:
	CMP CL,6
	JGE GETP20
;
; Modes 4 and 5
;
	CALL CALCP4
	JMP SHORT GETP01	;Borrow code
;
GETP20:
	CMP CL,7
	JGE	GETP30
;
; Mode 6
;
	CALL CALCP6
	JMP SHORT GETP01	;Borrow code
;
GETP30:
	CMP	CL,14
	JGE	GETP40
;
; Mode 13
;
	CALL CALCP13
GETP31:
	MOV CH,AH
	SHL CH,CL
	MOV SI,BX
	MOV DX,3CEH
	MOV AX,304H
	XOR BL,BL
GETP32:
	OUT DX,AX
	MOV BH,ES:[SI]
	AND BH,CH
	NEG BH
	ROL BX,1
	DEC AH
	JGE GETP32
	XOR BH,BH
	XOR CX,CX
	RET
;
GETP40:
	CMP	CL,15
	JNE GETP50
;
; Special for mode 15
;
	CALL CALCP14
	MOV CH,AH
	SHL CH,CL
	MOV SI,BX
	MOV DX,3CEH
	MOV AX,204H
	XOR BL,BL
GETP41:
	OUT DX,AX
	MOV BH,ES:[SI]
	AND BH,CH
	NEG BH
	ROL BX,1
	SUB AH,2
	JGE GETP41
	XOR BH,BH
	XOR CX,CX
	RET
;
GETP50:
	CMP	CL,17
	JGE	GETP60
;
; Modes 14 and 16
;
GETP51:
	CALL CALCP14
	JMP GETP31		;Borrow code
;
GETP60:
	CMP	CL,18
	JZ	GETP51		;Mode 18 same as EGA 14 and 16
;
; Mode 19
;
	CALL CALCP19
	MOV BL,ES:[BX]
	XOR	BH,BH
	XOR CX,CX
#endasm
}

/*
** 8088/8086 version
** svmode(mode) int mode; {
** Set video mode.
** Mode is:
** 0 - 40 x 25 chars b&w (cga,ega,mcga,vga)
** 1 - 40 x 25 chars color (cga,ega,mcga,vga)
** 2 - 80 x 25 chars b&w (cga,ega,mcga,vga)
** 3 - 80 x 25 chars color (cga,ega,mcga,vga)
** 4 - 320 x 200 4-color graph (cga,ega,mcga,vga)
** 5 - 320 x 200 b&w graph (cga,ega,mcga,vga)
** 6 - 640 x 200 b&w graph (cga,ega,mcga,vga)
** 7 - 80 x 25 chars b&w (mda,ega,vga)
** 8 - 160 x 200 16-color graph (jr)
** 9 - 320 x 200 16-color graph (jr)
** 10 - 640 x 200 4-color graph (jr)
** 11 - reserved by EGA video BIOS
** 12 - reserved by EGA video BIOS
** 13 - 320 x 200 16-color graph (ega,vga)
** 14 - 640 x 200 16-color graph (ega,vga)
** 15 - 640 x 350 2-color graph (ega,vga)
** 16 - 640 x 350 4- or 16-color (ega,vga)
** 17 - 640 x 480 2-color (mcga,vga)
** 18 - 640 x 480 16-color (vga)
** 19 - 320 x 200 256-color (mcga,vga)
** 255 - Heculese 720x348 monochrome graphics
*/
svmode(mode) int mode; {
#asm
	MOV	BP,SP
	MOV	AX,2[BP]
	CMP	AX,255		;Hercules?
	JZ	HERC
	PUSH	DS
	MOV	BX,40H		;Video display data area
	MOV	DS,BX
	MOV	BL,32		;Presence of CGA
	MOV	AX,2[BP]
	MOV	DL,AL
	AND	DL,7
	CMP	DL,7
	JNE	SVM1
	MOV	BL,48		;Presence of MDA
SVM1:	AND	BYTE PTR DS:[10H],11001111B
	OR	BYTE PTR DS:[10H],BL
	INT	10H	;Set the mode
	POP	DS		;Restore DS
	XOR	CX,CX	;Clear for Small-C
	RET
HERC:	
;Update video bios data area
	MOV	AX,40H
	MOV	ES,AX
	MOV	DI,49H	;ES:DI is video bios data area
	MOV	SI,OFFSET BDATA
	MOV	CX,BDLEN
	REP	MOVSB		;Update video bios data
;Set config. switch
	MOV	DX,3BFH		;Config switch port
	MOV	AL,1		;Exclude second 32K of video buffer
	OUT	DX,AL
;Blank the screen
	MOV	DX,3B8H
	XOR	AL,AL
	OUT	DX,AL
;Program the CRTC
	SUB	DL,4		;3b4H is addres reg port of CRTC
	MOV	SI,OFFSET CRTCP
	MOV	CX,9		;9 parameters to load
HERC1:	LODSW			;Al=crtc register number/AH=VALUE
	OUT	DX,AX
	LOOP	HERC1
;Set graphics mode
	ADD	DL,4
	MOV	AL,CRTMD
	OUT 	DX,AL
;Clear the video buffer
	MOV	AX,0B000H
	MOV	ES,AX
	XOR	AX,AX		;Clear to zero
	XOR	DI,DI
	MOV	CX,4000H
	REP	STOSW
	XOR	CX,CX		;Clear for small c
	RET			;Go home
;Configurations stuff for Hercules
CRTCP	DB	00H,35H		;54 chars horizontal
	DB	01,2DH		;45 displayed
	DB	02,2EH		;Sync at 46th char
	DB	03,07H		;Sync width 7 char clocks
	DB	4,5BH		;Vert total of 92 characters
	DB	05,02H		;Vert adjust of 2 scan lines
	DB	06,57H		;Vert displayed of 87 char rows
	DB	07,57H		;Vert. sync after 87th char row
	DB	09,03H		;Max scan line, 4 scan lines per char
BDATA	DB	7		;CRT_MODE
	DW	80		;CRT_COLS
	DW	8000H		;CRT_LEN
	DW	0		;CRT_START
	DW	8 DUP (0)	;CURSOR_POSN
	DW	0		;CURSOR_MODE
	DB	0		;ACTIVE_PAGE
	DW	3B4H		;ADDR_6845
CRTMD	DB	0AH		;CRT_MODE_SET
	DB	0		;PALLETTE
BDLEN	EQU	$-BDATA
#endasm
}

/*
** 8088/8086 version
** gvmode(vlock) int vblock[3] ; { - Get current video mode
** Returns current video mode info
** vblock[0] = current video mode #
** vblock[1] = character columns on screen
** vblock[2] = current display page #
*/
gvmode() {
#asm
	MOV	AH,0FH
	INT	10H
	MOV	BP,SP
	MOV	SI,2[BP]	;Pointer to tblock
	XOR	CH,CH		;Clear hi part
	MOV	CL,AL
	MOV	[SI],CX		;Mode #
	MOV	CL,AH
	MOV	2[SI],CX	;Char columns on screen
	MOV	CL,BH
	MOV	4[SI],CX	;Current display page
	XOR	CX,CX		;Clear
#endasm
}
