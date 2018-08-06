.XLIST
	page	66,132
;************************************************************
;
;	Header file for inclusion in HERCBIOS assemblies
;
;		Dave Tutelman - 8/86
;
;*************************************************************

;iAPX286	equ	1	; UN-COMMENT FOR A 186 or 286 MACHINE!
				;   Some of the "ugly" constructs are for speed
				;   on the 808X, while the "obvious" constructs
				;   run faster on the 186 & 286.
VIDI		equ	10H	; video interrupt, 10H for real, 50H for debug
pixbase		equ	0B000H	; beginning segment of graphics board
mpx_bios	equ	0F000H	; MPX-16 BIOS segment address
vid_offset	equ	0F065H	; MPX-16 video offset in BIOS
charbase	equ	0FA6EH	; MPX-16 BIOS character table offset
herc_mode	equ	8	; Hercules graphics mode
ibm_mode	equ	6	; IBM Hi-Res color graphics mode.
				; we'll try to handle it.
vid_port	equ	03B4H	; 6845 index register (data reg = 3B5H )
				;		      (control port = 3B8H )
;------------------------------------------
bios	segment at	mpx_bios	; setup call to vid_bios
		org	vid_offset
vid_bios	proc	far
vid_bios	endp
bios		ends
;------------------------------------------------
bios_data    segment at 040h
		org	049h
video_mode	db	?	; current video mode
n_cols		dw	?	; number of columns in video display
		org	050h
curs_pos	dw	8 dup(?) ; cursor for each of 8 pages
curs_mode	dw	?	; cursor mode
active_page	db	?	; current active display page
video_base	dw	?	; video port base
chip_ctrl	db	?	; current setting of 3X8 register
bios_data ends
;------------------------------------
intvec	     segment at 0		; interrupt vector
		org	4*1Fh		; interrupt 1FH
user_table	dd	?
intvec	     ends
;------------------------------------

IFDEF	iAPX286
	.286c			; allow 286/186-only instructions
ENDIF

.LIST
