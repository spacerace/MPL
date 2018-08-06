		TITLE	'Listing C-1'
		NAME	VideoID
		PAGE	55,132

;
; Name:		VideoID
;
; Function:	Detects the presence of various video subsystems and associated
;		monitors.
;
; Caller:	Microsoft C:
;
;			void VideoID(VIDstruct);
;
;			struct
;			{
;			  char VideoSubsystem;
;			  char Display;
;			}
;				*VIDstruct[2];
;
;		Subsystem ID values:
;				 0  = (none)
;				 1  = MDA
;				 2  = CGA
;				 3  = EGA
;				 4  = MCGA
;				 5  = VGA
;				80h = HGC
;				81h = HGC+
;				82h = Hercules InColor
;
;		Display types:	 0  = (none)
;				 1  = MDA-compatible monochrome
;				 2  = CGA-compatible color
;				 3  = EGA-compatible color
;				 4  = PS/2-compatible monochrome
;				 5  = PS/2-compatible color
;				 
;
;	The values returned in VIDstruct[0].VideoSubsystem and
;	VIDstruct[0].Display indicate the currently active subsystem.
;

ARGpVID		EQU	word ptr [bp+4]	; stack frame addressing


VIDstruct	STRUC			; corresponds to C data structure

Video0Type	DB	?		; first subsystem type
Display0Type	DB	? 		; display attached to first subsystem

Video1Type	DB	?		; second subsystem type
Display1Type	DB	?		; display attached to second subsystem

VIDstruct	ENDS


Device0		EQU	word ptr Video0Type[di]
Device1		EQU	word ptr Video1Type[di]


MDA		EQU	1		; subsystem types		
CGA		EQU	2
EGA		EQU	3
MCGA		EQU	4
VGA		EQU	5
HGC		EQU	80h
HGCPlus		EQU	81h
InColor		EQU	82h

MDADisplay	EQU	1		; display types
CGADisplay	EQU	2
EGAColorDisplay	EQU	3
PS2MonoDisplay	EQU	4
PS2ColorDisplay	EQU	5

TRUE		EQU	1
FALSE		EQU	0


DGROUP		GROUP	_DATA

_TEXT		SEGMENT	byte public 'CODE'
		ASSUME	cs:_TEXT,ds:DGROUP

		PUBLIC	_VideoID
_VideoID	PROC	near

		push	bp		; preserve caller registers
		mov	bp,sp
		push	si
		push	di

; initialize the data structure that will contain the results

		mov	di,ARGpVID	; DS:DI -> start of data structure

		mov	Device0,0	; zero these variables
		mov	Device1,0

; look for the various subsystems using the subroutines whose addresses are
;  tabulated in TestSequence; each subroutine sets flags in TestSequence
;  to indicate whether subsequent subroutines need to be called

		mov	cx,NumberOfTests
		mov	si,offset DGROUP:TestSequence

L01:		lodsb			; AL := flag
		test	al,al
		lodsw			; AX := subroutine address
		jz	L02		; skip subroutine if flag is false

		push	si
		push	cx
		call	ax		; call subroutine to detect subsystem
		pop	cx
		pop	si

L02:		loop	L01

; determine which subsystem is active

		call	FindActive

		pop	di		; restore caller registers and return
		pop	si
		mov	sp,bp
		pop	bp
		ret

_VideoID	ENDP


;
; FindPS2
;
; 	This subroutine uses INT 10H function 1Ah to determine the video BIOS
;	 Display Combination Code (DCC) for each video subsystem present.
;

FindPS2		PROC	near

		mov	ax,1A00h
		int	10h		; call video BIOS for info

		cmp	al,1Ah
		jne	L13		; exit if function not supported (i.e.,
					;  no MCGA or VGA in system)

; convert BIOS DCCs into specific subsystems & displays

		mov	cx,bx
		xor	bh,bh		; BX := DCC for active subsystem

		or	ch,ch
		jz	L11		; jump if only one subsystem present

		mov	bl,ch		; BX := inactive DCC
		add	bx,bx
		mov	ax,[bx+offset DGROUP:DCCtable]

		mov	Device1,ax

		mov	bl,cl
		xor	bh,bh		; BX := active DCC

L11:		add	bx,bx
		mov	ax,[bx+offset DGROUP:DCCtable]

		mov	Device0,ax

; reset flags for subsystems that have been ruled out

		mov	byte ptr CGAflag,FALSE
		mov	byte ptr EGAflag,FALSE
		mov	byte ptr Monoflag,FALSE

		lea	bx,Video0Type[di]  ; if the BIOS reported an MDA ...
		cmp	byte ptr [bx],MDA
		je	L12

		lea	bx,Video1Type[di]
		cmp	byte ptr [bx],MDA
		jne	L13

L12:		mov	word ptr [bx],0    ; ... Hercules can't be ruled out
		mov	byte ptr Monoflag,TRUE

L13:		ret

FindPS2		ENDP


;
; FindEGA
;
; Look for an EGA.  This is done by making a call to an EGA BIOS function
;  which doesn't exist in the default (MDA, CGA) BIOS.

FindEGA		PROC	near		; Caller:	AH = flags
					; Returns:	AH = flags
					;		Video0Type and
					;		 Display0Type updated

		mov	bl,10h		; BL := 10h (return EGA info)
		mov	ah,12h		; AH := INT 10H function number
		int	10h		; call EGA BIOS for info
					; if EGA BIOS is present,
					;  BL <> 10H
					;  CL = switch setting
		cmp	bl,10h
		je	L22		; jump if EGA BIOS not present

		mov	al,cl
		shr	al,1		; AL := switches/2
		mov	bx,offset DGROUP:EGADisplays
		xlat			; determine display type from switches
		mov	ah,al		; AH := display type
		mov	al,EGA		; AL := subystem type
		call	FoundDevice

		cmp	ah,MDADisplay
		je	L21		; jump if EGA has a monochrome display

		mov	CGAflag,FALSE	; no CGA if EGA has color display
		jmp	short L22

L21:		mov	Monoflag,FALSE	; EGA has a mono display, so MDA and
					;  Hercules are ruled out
L22:		ret

FindEGA		ENDP


;
; FindCGA
;
;	This is done by looking for the CGA's 6845 CRTC at I/O port 3D4H.
;

FindCGA		PROC	near		; Returns:	VIDstruct updated

		mov	dx,3D4h		; DX := CRTC address port
		call	Find6845
		jc	L31		; jump if not present

		mov	al,CGA
		mov	ah,CGADisplay
		call	FoundDevice

L31:		ret

FindCGA		ENDP


;
; FindMono
;
;	This is done by looking for the MDA's 6845 CRTC at I/O port 3B4H.  If
;	a 6845 is found, the subroutine distinguishes between an MDA
;	and a Hercules adapter by monitoring bit 7 of the CRT Status byte.
;	This bit changes on Hercules adapters but does not change on an MDA.
;
;	The various Hercules adapters are identified by bits 4 through 6 of
;	the CRT Status value:
;
;		000b = HGC
;		001b = HGC+
;		101b = InColor card
;

FindMono	PROC	near		; Returns:	VIDstruct updated

		mov	dx,3B4h		; DX := CRTC address port
		call	Find6845
		jc	L44		; jump if not present

		mov	dl,0BAh		; DX := 3BAh (status port)
		in	al,dx
		and	al,80h
		mov	ah,al		; AH := bit 7 (vertical sync on HGC)

		mov	cx,8000h	; do this 32768 times 
L41:		in	al,dx
		and	al,80h		; isolate bit 7
		cmp	ah,al
		loope	L41		; wait for bit 7 to change

		jne	L42		; if bit 7 changed, it's a Hercules

		mov	al,MDA		; if bit 7 didn't change, it's an MDA
		mov	ah,MDADisplay
		call	FoundDevice
		jmp	short L44

L42:		in	al,dx
		mov	dl,al		; DL := value from status port

		mov	ah,MDADisplay	; assume it's a monochrome display

		mov	al,HGC		; look for an HGC
		and	dl,01110000b	; mask off bits 4 thru 6
		jz	L43

		mov	al,HGCPlus	; look for an HGC+
		cmp	dl,00010000b
		je	L43		; jump if it's an HGC+

		mov	al,InColor	; it's an InColor card
		mov	ah,EGAColorDisplay

L43:		call	FoundDevice

L44:		ret

FindMono	ENDP


;
; Find6845
;
;	This routine detects the presence of the CRTC on a MDA, CGA or HGC.
;	The technique is to write and read register 0Fh of the chip (cursor
;	low).  If the same value is read as written, assume the chip is
;	present at the specified port addr.
;

Find6845	PROC	near		; Caller:	DX = port addr
					; Returns:	cf set if not present
		mov	al,0Fh
		out	dx,al		; select 6845 reg 0Fh (Cursor Low)
		inc	dx
		in	al,dx		; AL := current Cursor Low value
		mov	ah,al		; preserve in AH
		mov	al,66h		; AL := arbitrary value
		out	dx,al		; try to write to 6845

		mov	cx,100h
L51:		loop	L51		; wait for 6845 to respond

		in	al,dx
		xchg	ah,al		; AH := returned value
					; AL := original value
		out	dx,al		; restore original value

		cmp	ah,66h		; test whether 6845 responded
		je	L52		; jump if it did (cf is reset)

		stc			; set carry flag if no 6845 present

L52:		ret	
	
Find6845	ENDP


;
; FindActive
;
;	This subroutine stores the currently active device as Device0.  The
;	current video mode determines which subsystem is active.
;

FindActive	PROC	near

		cmp	word ptr Device1,0
		je	L63			; exit if only one subsystem

		cmp	Video0Type[di],4	; exit if MCGA or VGA present
		jge	L63			;  (INT 10H function 1AH
		cmp	Video1Type[di],4	;  already did the work)
		jge	L63

		mov	ah,0Fh
		int	10h			; AL := current BIOS video mode

		and	al,7
		cmp	al,7			; jump if monochrome
		je	L61			;  (mode 7 or 0Fh)

		cmp	Display0Type[di],MDADisplay
		jne	L63			; exit if Display0 is color
		jmp	short L62

L61:		cmp	Display0Type[di],MDADisplay
		je	L63			; exit if Display0 is monochrome

L62:		mov	ax,Device0		; make Device0 currently active
		xchg	ax,Device1
		mov	Device0,ax

L63:		ret

FindActive	ENDP


;
; FoundDevice
;
;	This routine updates the list of subsystems.
;

FoundDevice	PROC	near			; Caller:    AH = display #
						;	     AL = subsystem #
						; Destroys:  BX
		lea	bx,Video0Type[di]
		cmp	byte ptr [bx],0
		je	L71			; jump if 1st subsystem

		lea	bx,Video1Type[di]	; must be 2nd subsystem

L71:		mov	[bx],ax			; update list entry
		ret

FoundDevice	ENDP

_TEXT		ENDS


_DATA		SEGMENT	word public 'DATA'

EGADisplays	DB	CGADisplay	; 0000b, 0001b	(EGA switch values)
		DB	EGAColorDisplay	; 0010b, 0011b
		DB	MDADisplay	; 0100b, 0101b
		DB	CGADisplay	; 0110b, 0111b
		DB	EGAColorDisplay	; 1000b, 1001b
		DB	MDADisplay	; 1010b, 1011b

DCCtable	DB	0,0		; translate table for INT 10h func 1Ah
		DB	MDA,MDADisplay
		DB	CGA,CGADisplay
		DB	0,0
		DB	EGA,EGAColorDisplay
		DB	EGA,MDADisplay
		DB	0,0
		DB	VGA,PS2MonoDisplay
		DB	VGA,PS2ColorDisplay
		DB	0,0
		DB	MCGA,EGAColorDisplay
		DB	MCGA,PS2MonoDisplay
		DB	MCGA,PS2ColorDisplay

TestSequence	DB	TRUE		; this list of flags and addresses
		DW	FindPS2		;  determines the order in which this
					;  program looks for the various
EGAflag		DB	TRUE		;  subsystems
		DW	FindEGA

CGAflag		DB	TRUE
		DW	FindCGA

Monoflag	DB	TRUE
		DW	FindMono

NumberOfTests	EQU	($-TestSequence)/3

_DATA		ENDS

		END
