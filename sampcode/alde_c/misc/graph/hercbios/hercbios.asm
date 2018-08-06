;*******************************************************
;*
;*	Main program file for HERCBIOS
;*
;*		Dave Tutelman - 8/86
;*
;*------------------------------------------------------
;*	
;*	INT10 -- acts as pre-handler for video interrupts
;*	(BIOS calls) for Hercules & SuperComputer
;*	monochrome graphics board.  Calls real BIOS
;*	if not a Hercules special. Handled here are:
;*
;*	Fn 0	Set mode (only 6 & 8)
;*	and all functions, when in mode 6 or 8.
;*	Actually, we've only implemented:
;*	Fn 2	Set cursor position
;*	Fn 3	Read cursor position
;*	Fn 5	New display page
;*	Fn 9	Write character with attribute
;*	Fn 10	Write character
;*	Fn 12	Write pixel
;*	Fn 13	Read pixel
;*	Fn 14	Teletypewriter-style character write
;*	Fn 15	Get video status
;*
;*	The only allowable modes for these boards are:
;*	6	IBM graphics (we handle it, but poor aspect ratio).
;*	7	Monochrome (with 2 pages)
;*	8	Hercules graphics mode.
;*
;**********************************************************
;
INCLUDE	hercbios.h

extrn	writechar:near,tty:near,scroll_up:near,scroll_down:near
extrn	scr_start:word,scr_length:word,num_rows:byte
extrn	wr_pixel:near
extrn	end_herc:near
public	exit_herc_bios,int10,vid_vector
public	set_mode,set_curs,read_curs,new_page,status
;-----------------------------------------
page
;************************************************************
;*
;*	Install the Hercules video handler
;*
;************************************************************
cseg	segment	public
	assume	cs:cseg,ds:cseg

	ORG	100h
;
START:
;
	push	ax
	push	es
	push	ds
;
	xor	ax,ax		; zero the acc
	mov	es,ax		; ES points to zero (interrupt vector)
    ; Get ROM BIOS video entry point, and save in "rom_bios"
	mov	ax,es:[4*VIDI]
	mov	word ptr cs:rom_bios,ax
	mov	ax,es:[4*VIDI+2]
	mov	word ptr cs:rom_bios+2,ax
    ; Now plant the HERCBIOS entry point in interrupt vector
	mov	ax,offset int10	; address of video handler to AX
	mov	es:[4*VIDI],ax	; and store it in interrupt vector 10H
	mov	ax,cs		; same for the segment of video handler
	mov	es:[4*VIDI+2],ax	; ...
;
;   Leave the message that we're installed
	mov	ax,cs		; DS:DX pointing to message
	mov	ds,ax
	mov	dx,offset install_msg
	mov	ah,9		; display-string function
	int	21h

	pop	ds
	pop	es
	pop	ax
;
	mov	dx,offset end_herc	; set dx to end of this program
	int	27H		; terminate, but stay resident
;
install_msg	db	"BIOS for Hercules-compatible Graphics - "
	db		"(DMT Aug 1986)",10,13,'$'
page
;************************************************************
;*
;*	Beginning of video interrupt handler
;*
;************************************************************
;
;
int10	proc	far
;
	sti			; allow further interrupts
	push	ds		; save DS
	push	ax		; save AX
	mov	ax,bios_data	; bios data segment --> AX
	mov	ds,ax		; now put bios data segment in DS
	assume	ds:bios_data	; and tell assembler about it
;			; check current mode
	mov	ah,video_mode	; get current mode
	cmp	ah,herc_mode	; test for a graphics mode
	je	herc_bios
	cmp	ah,ibm_mode
	je	herc_bios
;			; setmode to a graphics mode?
	pop	ax		; restore AX
	push	ax		; ...but leave it on stack
	cmp	ax,0006		; Fn = set to IBM hi-res mode?
	je	herc_bios
	cmp	ax,0008		; Fn = set to Herc graphics mode?
	je	herc_bios
;
norm_bios:		; if we get here, just go to real BIOS
	pop	ax		; restore stack to pre-interrupt state
	pop	ds
	db	0EAh	; opcode for FAR JUMP to rom_bios
rom_bios	dd	?	; normal video bios address (in ROM)
;
herc_bios:		; jump table for special Hercules BIOS
;
	pop	ax	; restore ax
	push	bx	; save regs
	push	cx
	push	dx
	push	si
	push	di
	push	es
;
	push	ax
	mov	al,ah		; function # to AX
	xor	ah,ah
	shl	ax,1		; *2 for word vector
	mov	si,ax		; put in SI to index into vector
	pop	ax		; restore old AX
	cmp	si,offset vid_vec_end-offset vid_vector
				; function number within range?
	jge	exit_herc_bios	; function number out of range
	add	si,offset vid_vector
	jmp	word ptr cs:[si]
				; jump to routine via vector
;

vid_vector:		; jump vector for hercules video routines
	dw	offset set_mode			; 0 = set mode
	dw	offset exit_herc_bios		; 1 = cursor type (NA)
	dw	offset set_curs			; 2 = set cursor position
	dw	offset read_curs		; 3 = read cursor position
	dw	offset exit_herc_bios		; 4 = light pen (NA)
	dw	offset new_page			; 5 = choose active page
	dw	offset scroll_up		; 6 = scroll up
	dw	offset scroll_down		; 7 = scroll down
	dw	offset exit_herc_bios		; 8 = read character (NA)
	dw	offset writechar		; 9 = write char & attribute
	dw	offset writechar		;10 = write character
	dw	offset exit_herc_bios		;11 = set color palette (NA)
	dw	offset wr_pixel			;12 = write pixel
	dw	offset wr_pixel			;13 = read pixel
	dw	offset tty			;14 = teletype write
	dw	offset status			;15 = return video state
vid_vec_end:
;
exit_herc_bios:
	pop	es	; restore regs
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ds
	iret		; and return
page
;********************************************************************
;*
;*	FUNCTION 0 - SET VIDEO MODE
;*
;*	Only gets here to set a hi-res graphics mode
;*		6=IBM
;*		8=Hercules
;*	[ AX destroyed ]
;*
;*******************************************************************
;
set_mode:
;			; is it a graphics mode?
	cmp	al,herc_mode	; set to hercules mode?
	je	g_mode
	cmp	al,ibm_mode	; set to IBM mode?
	je	g_mode
				; Neither. Leave it to normal BIOS
	pop	es		; restore regs
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ds
	jmp	vid_bios	; and go to MPX-16 BIOS
;
g_mode:	mov	video_mode,al	; save video mode in BIOS data area
	mov	active_page,ah	; zero the active page in BIOS data
	mov	n_cols,90	; 90 character columns in graphics mode
;
;*  clear display area
	mov	ax,pixbase	; get starting address
	mov	es,ax		; page base to ES
	mov	cx,8000H	; word counter
	mov	di,0		; start at beginning of display
	cld			; direction "up"
	xor	ax,ax		; zero AX
	rep	stosw		; write zero to both pages
;
;*  load the 6845 internal registers
;
;			; first set up the loop
	push	ds		; save DS
	mov	ax,cs		; get cseg into DS
	mov	ds,ax
	assume	ds:cseg
	mov	dx,vid_port	; 6845 index port address to DX
	mov	si,offset vid_parm_table
				; table pointer to SI
	mov	cx,16		; 16 parameters in table
	xor	ax,ax		; zero AX (AH will be index counter)
;			; now execute the loop
init_6845_loop:
	mov	al,ah		; index counter to AL
	out	dx,al		; output index to 6845
	inc	dx		; point DX to data reg
	mov	al,[si]		; get table entry
	out	dx,al		; output to 6845 data reg
	dec	dx		; point DX back to index reg
	inc	ah		; bump index counter
	inc	si		; bump table pointer
	loop	init_6845_loop
	pop	ds		; restore DS
	assume	ds:bios_data	; restore DS assumed
;
;*  now set the 6845 control register
	mov	dx,vid_port+4	; control port address
	mov	al,0AH		; graphics control byte
	cmp	active_page,0	; get current page
	je	pg0_F0		; skip if zero
	or	al,80H		; page 1 to control byte
pg0_F0:	out	dx,al		; and ship to 6845
	mov	chip_ctrl,al	; also save in bios data area
;
;*  save cursor position (0,0) in bios data area
	xor	ax,ax		; zero AX
	mov	curs_pos,ax	; write zero to pg.0 cursor postion
	mov	curs_pos+2,ax	; write zero to pg.1 cursor postion
;
;*  initialize scrolling parameters
;
	cmp	video_mode,herc_mode
	je	h_parm
	mov	cs:scr_start,180	; IBM parameters
	mov	cs:scr_length,3690	;
	mov	cs:num_rows,42		;
	jmp	parm_done
h_parm:	mov	cs:scr_start,270	; Herc parameters
	mov	cs:scr_length,3645	;
	mov	cs:num_rows,28		;
parm_done:
;
	jmp	exit_herc_bios
;
;*  table of 6845 chip parameters
;
vid_parm_table	db	53,45,46,7,91,2,87,87,2,3,62H,3,0,0,0,0
;vid_parm_table	db	54,45,46,8,91,2,87,87,2,3,62H,3,0,0,0,0
						;DMT's monitor
page
;********************************************************************
;*
;*	FUNCTION 2 - SET CURSOR POSITION
;*
;*	DX = new row (DH) and column (DL)
;*	BH = display page number
;*
;********************************************************************
;
set_curs:
;
; *   save in BIOS data area
	mov	bl,bh		; page # to bl
	xor	bh,bh		; zero bh (page # = BX)
	shl	bx,1		; times 2 for word
	mov	curs_pos[bx],dx	; store in data area
;
; *   if page # = active page, then we should actually move cursor
; *  		However, cursor doesn't show in graphics mode, so we won't.
;
	jmp	exit_herc_bios
;
page
;********************************************************************
;*
;*	FUNCTION 3 - READ CURSOR POSITION
;*
;*	on entry
;*		BH = display page number
;*
;*	on exit
;*		CX = cursor type/size
;*		DX = current row (DH) and column (DL)
;*
;********************************************************************
;
read_curs:
;			; uncover the return portion of the stack
	pop	es
	pop	di
	pop	si
	pop	dx
	pop	cx
;			; now get the data and push onto stack
	push	curs_mode	; cursor type to CX position in stack
	mov	bl,bh		; page # to BX
	xor	bh,bh
	shl	bx,1		; *2 for word offset
	push	curs_pos[bx]	; cursor position for specified page
				;   to DX position in stack
;			; refill the stack for return
	push	si
	push	di
	push	es
;
	jmp	exit_herc_bios
page
;********************************************************************
;*
;*	FUNCTION 5 - SELECT NEW DISPLAY PAGE
;*
;*	AL = new display page number
;*
;********************************************************************
;
new_page:
	cmp	al,2		; page < 2?
	jl	f5_1		; yup
	jmp	exit_herc_bios	; nope. can't do it.
;
f5_1:	mov	active_page,al	; put away active page in bios data
;  * put starting address in 6845 register 12
	mov	ah,al		; save page number in AH
	mov	dx,vid_port	; index pointer address
	mov	al,12		; save in register 12
	out	dx,al		; and set index pointer to 12
	mov	al,ah		; restore page to AL
	ror	al,1		; page to high-order bit
	shr	al,1		; two bytes per word
	inc	dx
	out	dx,al		; ...and output it to register
;  * put control byte in 6845 port
	mov	al,ah		; get back page number
	ror	al,1		; page to high-order bit
	or	al,0AH		; create chip control byte
	mov	chip_ctrl,al	; save it in data area
	mov	dx,vid_port+4	; control port address
	out	dx,al		; send control byte to chip
;
	jmp	exit_herc_bios
page
;***********************************************************************
;*
;*	FUNCTION 15 - RETURN VIDEO STATUS
;*
;*	On exit:
;*		AL = current video mode
;*		AH = number of active display columns
;*		BH = current active page number
;*
;***********************************************************************
;
status:
;			; first uncover the stack
	pop	es
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
;			; now get the parameters needed
	mov	al,video_mode
	mov	ah,90		; all our graphics modes have 90 cols
	mov	bh,active_page
;			; and push back onto the stack for return
	push	bx
	push	cx
	push	dx
	push	si
	push	di
	push	es
;
	jmp	exit_herc_bios
;
int10	endp
;
cseg	ends
	end	START
