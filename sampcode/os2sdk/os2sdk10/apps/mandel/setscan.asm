page	,132
;***************************************************************************
;									   *
;		Copyright (C) 1987 by Microsoft Inc.			   *
;									   *
;***************************************************************************
title	SetScan - Set An EGA Scan to the Given Colors




.286c					;Allow iAPX80286 instructions

memM	equ	1			;define middle model program
?PLM=1
?WIN=0



.xlist
include cmacros.inc
include ega.inc
.list


ScreenWidthB	equ	80		;Width of screen in bytes
ScreenSize	equ	14000		;Size of screen plane in words

;**	define segment for ring 2 IOPL code

createSeg	_SCRIO,  SCRIO,  byte,	 public, CODE
scrioOFFSET equ OFFSET _SCRIO:

sBegin	data

;**	These routines assume that the following two external values are
;	initialized before any routine is called.


	extrn	_dpoffset:word		;EGA display page offset
	extrn	_psel:word		;selector array for EGA memory

sEnd	data

sBegin	scrio
assumes cs,_scrio
assumes ds,data




;**	SetDVideo  - disable video
;
;	SetDVideo disable video to prevent snow and flicker during state
;	changes of the EGA.  A call to SetEVideo must be made to reenable
;	the video.
;
;	SetDVideo ();
;
;	Entry	none
;	Exit	EGA video blanked
;	Returns none
;	Calls	dvideo


cProc	SetDvideo,<FAR,PUBLIC>,<>

cBegin
	call	bvd			;blank video display
cEnd




;**	SetEVideo  - enable video
;
;	SetEVideo renables video
;
;	SetEVideo ();
;
;	Entry	none
;	Exit	EGA video restored
;	Returns none
;	Calls	evideo


cProc	SetEvideo,<FAR,PUBLIC>,<>

cBegin
	call	evd			;enable video display
cEnd




;**	SetScan - Set One Scan to Given Colors
;
;	The given scan line is set to the colors passed in.
;
;	SetScan (row, count, values);
;
;	Entry	row = row to write
;		count = number of bytes of display data
;		values = pointer to character array of bit values
;	Exit	display page forced to 0
;		display row updated
;	Returns none
;	The display adapter must be initialized before invoking this
;	routine.



cProc	SetScan,<FAR,PUBLIC>,<si,di,es>
	parmW	Y
	parmW	count
	parmW	pMapped

cBegin
	cld
	xor	bx,bx			;force display to page 0
	call	sdp			;set display page
	mov	ax,Y			;Compute start of scan that will
	mov	di,ScreenWidthB 	;  be set to the passed colors
	mul	di
	mov	di,ax
	mov	si,_psel		;es:di --> scan on the EGA
	mov	es,word ptr [si]
	mov	dx,rGraphics		;Set the Graphics Controller's
	mov	al,rBitmask		;  address register to point to
	out	dx,al			;  the bitmask register
	inc	dx			;--> Graphics Controller's data register
	mov	ah,10000000b		;Set mask for initial pixel
	mov	cx,count		;Set count of pixels
	jcxz	ssr2			;None, exit now
	mov	si,offset pMapped	;ds:si --> pixel colors
	xor	bx,bx			;Zero is always handy!

ssr1:	mov	al,ah			;Set BitMask register to only alter
	out	dx,al			;  pixel of interest
	ror	ah,1			;Set up for next pixel
	lodsb				;Get color for this pixel


;	The BitMask register allows bits to be protected from changes.
;	It works in conjunction with some internal EGA latches.  Every
;	time a read operation from EGA memory is performed, the latches
;	are loaded with the data read from each plane (one 8-bit latch
;	per plane).  Wherever the BitMask register is a "0", the EGA
;	writes data from the latches.  Wherever the BitMask register is
;	a "1", the data from the host is written.  This is not implemented
;	in the hardware as a read/modify/write operation, so the user
;	must load the latches explicitly.


	xchg	es:[di],al		;Load latches, write color to the pixel
	adc	di,bx			;Possibly --> next destination byte
	loop	ssr1			;Until all pixels drawn.

ssr2:	mov	al,0ffh 		;Set mask for all pixel
	out	dx,al
cEnd




;**	SetScanClear - clear video display memory
;
;	SetScanClear clears the 28000 bytes of video memory starting at
;	the page offset specified in _dpoffset.
;
;	SetScanClear ();
;
;	Entry	none
;	Exit	EGA video memory cleared
;	Returns none
;	Calls	cdm


cProc	SetScanClear,<FAR,PUBLIC>,<>

cBegin
	call	bvd			;blank video display
	mov	cx,_dpoffset
	call	cdm			;clear display memory
	call	evd			;enable video display
cEnd




;**	SetScanRestore - restore graphics display
;
;	Restore active EGA display page from saved image
;	SetScanRestore (ressel);
;
;	Entry	ressel = far pointer to array of selectors to segments
;			 containing graphics image to be restored
;		_dpoffset = offset of active display page
;	Exit	graphics image restored
;	Returns none


cProc	SetScanRestore,<FAR,PUBLIC>,<si,di,es,ds>
	parmD	ressel

cBegin
	mov	cx,_dpoffset		;(cx) = display page to clear
	call	cdm			;clear display memory
	mov	bx,_psel		;(es) = display selector
	mov	es,word ptr [bx]

;	Restore graphics memory.
;	This is done by enabling each 32k bank of graphics memory
;	and copying memory from the restore segments.

	mov	dx,rGraphics		;Set the Graphics Controller's
	mov	ax,0*256+rMode
	out	dx,ax
	mov	al,rBitmask		;  address register to point to
	mov	ah,0ffh 		;Set mask for initial pixel
	out	dx,ax			;  the bitmask register
	mov	dx,rSequencer		;(dx) = sequencer register address
	mov	al,rMapMask		;write to sequencer map mask register
	mov	ah,C0			;select plane C0
	out	dx,ax
	push	ds			;save (ds) for later
	mov	bx,_dpoffset		;move display offset to stack
	push	bx
	lds	bx,ressel		;(ds:bx) = address of restore selector
	mov	ds,[bx] 		;(ds) = first selector
	xor	si,si			;(si) = offset into restore segment
	pop	di			;(di) = offset into display memory
	push	di
	mov	cx,ScreenSize		;copy image to plane C0
	rep	movsw
	mov	ah,C1			;select plane C1
	out	dx,ax
	pop	di			;(di) = offset into display memory
	push	di
	mov	cx,ScreenSize		;copy image to plane C1
	rep	movsw
	lds	bx,ressel		;(ds:bx) = address of restore selector
	mov	ds,[bx+2]		;(ds) = second selector
	xor	si,si			;(si) = offset into restore segment
	mov	ah,C2			;select plane C2
	out	dx,ax
	pop	di			;(di) = offset into display memory
	push	di
	mov	cx,ScreenSize		;copy image to plane C2
	rep	movsw
	mov	ah,C3			;select plane C3
	out	dx,ax
	pop	di			;(di) = offset into display memory
	mov	cx,ScreenSize		;copy image to C3
	rep	movsw
	pop	ds

;	set start address register

	mov	bx,_dpoffset
	call	sdp			;set display page
cEnd




;**	SetScanRFont - restore EGA font table
;
;	SetScanRFont restores the EGA font tables saved by SetScanSFont.
;
;	SetScanRFont (fontsel);
;
;	Entry	fontsel = selector for font save memory
;	Exit	EGA font table restored from font save segment
;	Returns none
;	Calls	ifa, tfa


cProc	SetScanRFont,<FAR,PUBLIC>,<si,di,es>
	parmW	frsel

cBegin
	mov	si,_psel		;(es) = display selector
	mov	es,word ptr [si]
	push	ds
	call	ifa			;initialize font addressing
	mov	bx,frsel		;(bx) = font restore selector
	mov	ds,bx			;(ds) = font selector
	xor	si,si			;(si) = offset into restore segment
	xor	di,di			;(di) = offset into display
	mov	cx,08000h		;copy 32k words to plane C2
	rep	movsw			;copy memory
	pop	ds
	call	tfa			;terminate font table addressing
cEnd



;**	SetScanSave - Save graphics display
;
;	Copy active page of display memory to RAM save buffer.
;
;	SetScanSave (savesel);
;
;	Entry	savsel = far pointer to array of selectors for segments
;			 used to save graphics image
;		_dpoffset = offset of active EGA display page
;	Exit	graphics image saved
;	Returns none


cProc	SetScanSave,<FAR,PUBLIC>,<si,di,ds,es>
	parmD	savesel 		;far pointer to array of save selectors

cBegin
	les	bx,savesel		;(es:bx) = address of save selector
	mov	es,es:[bx]		;(es) = first selector
	mov	bx,_dpoffset		;(bx) = offset into display memory
	push	bx
	mov	dx,rGraphics		;(dx) = address of graphics controller
	mov	al,rReadMap
	mov	ah,rmC0 		;set read map select to plane 0
	out	dx,ax			;
	mov	si,_psel		;(ds) = display selector
	mov	ds,word ptr [si]
	mov	si,bx			;(si) = offset into display
	xor	di,di			;offset into save memory
	mov	cx,ScreenSize		;copy image from plane C0
	rep	movsw			;copy memory
	mov	ah,rmC1 		;select plane 1
	out	dx,ax
	mov	si,bx			;(si) = offset into display
	mov	cx,ScreenSize		;copy image from plane C1
	rep	movsw
	les	bx,savesel		;(es:bx) = address of save selector
	mov	es,es:[bx+2]		;(es) = second selector
	pop	bx			;(bx) = offset into display memory
	mov	ah,rmC2 		;select plane 2
	out	dx,ax
	mov	si,bx			;(si) = offset into display
	xor	di,di			;offset into save memory
	mov	cx,ScreenSize		;copy image from plane C2
	rep	movsw			;copy memory
	mov	ah,rmC3 		;select plane 3
	out	dx,ax
	mov	si,bx			;(si) = offset into display
	mov	cx,ScreenSize		;copy image from C3
	rep	movsw
	mov	ah,rmC0 		;set read map select to plane 0
	out	dx,ax			;
cEnd




;**	SetScanSFont - save font tables
;
;	SetScanSFont (fontsel);
;
;	Entry	fontsel = selector for font save memory
;	Exit	EGA font table saved in font save segment
;	Returns none
;	Calls	ifa, tfa


cProc	SetScanSFont,<FAR,PUBLIC>,<si,di,es>
	parmW	fssel

cBegin
	call	ifa			;initialize font addressing
	push	ds
	mov	bx,fssel		;(bx) = font save selector
	mov	es,bx			;(es) = font save selector
	mov	si,_psel		;(ds) = display selector
	mov	ds,word ptr [si]
	xor	si,si			;(si) = offset into display
	xor	di,di			;offset into save memory
	mov	cx,8000h		;copy 32k words from plane C0
	rep	movsw			;copy memory
	pop	ds			;(ds) = data segment selector
cEnd




;*	bvd - blank video display
;
;	bvd blanks the video display by preventing access to the
;	palette registers.
;
;	Entry	none
;	Exit	display blanked
;	Returns none
;	Calls	wvs
;	Uses	al, dx

cProc	bvd,<NEAR>,<>

cBegin
	call	wvs			;wait for vertical resynch
	mov	dx,rAttread		;read feature control to reset
	in	al,dx			;internal flipflop to force address
	mov	dx,rAttwrite
	mov	al,000H 		;disable access to palette
	out	dx,al
cEnd




;*	cdm - clear display memory
;
;	cdm clears the 28000 bytes of memory starting at the address
;	pointed to by (cx)
;
;	Entry	cx = offset of page to clear
;	Exit	display memory cleared
;	Returns none
;	Calls	sdm
;	Uses	ax, bx, cx, dx


cProc	cdm,<NEAR,PUBLIC>,<di, es>

cBegin
	mov	bx,_psel		;(es) = display selector
	mov	es,word ptr [bx]
	mov	bx,cx			;(bx) = display offset


;	Clear graphics memory.
;	This is done by enabling each 32k bank of graphics memory
;	and storing zeroes.

	mov	dx,rGraphics		;Set the Graphics Controller's
	mov	al,rBitmask		;  address register to point to
	mov	ah,0ffh 		;Set mask for initial pixel
	out	dx,ax			;  the bitmask register
	mov	dx,rSequencer		;(dx) = sequencer register address
	mov	al,rMapMask		;write to sequencer map mask register
	mov	ah,C0			;select plane C0
	out	dx,ax
	mov	di,bx			;(di) = offset into display memory
	mov	cx,ScreenSize		;copy image to plane C0
	xor	ax,ax
	rep	stosw
	mov	al,rMapMask		;write to sequencer map mask register
	mov	ah,C1			;select plane C1
	out	dx,ax
	mov	di,bx			;(di) = offset into display memory
	mov	cx,ScreenSize		;copy image to plane C1
	xor	ax,ax
	rep	stosw
	mov	al,rMapMask		;write to sequencer map mask register
	mov	ah,C2			;select plane C2
	out	dx,ax
	mov	di,bx			;(di) = offset into display memory
	mov	cx,ScreenSize		;copy image to plane C2
	xor	ax,ax
	rep	stosw
	mov	al,rMapMask		;write to sequencer map mask register
	mov	ah,C3			;select plane C3
	out	dx,ax
	mov	di,bx			;(di) = offset into display memory
	mov	cx,ScreenSize		;copy image to C3
	xor	ax,ax
	rep	stosw
	call	sdm			;set display mode
cEnd




;*	evd - enable video display
;
;	evd enables the video display by reenabling access to the
;	palette registers
;
;	Entry	none
;	Exit	video display restored
;	Returns none
;	Calls	none
;	Uses	al, dx


cProc	evd,<NEAR>,<>

cBegin
	mov	dx,rAttread		;read feature control to reset
	in	al,dx			;internal flipflop to force address
	mov	dx,rAttwrite
	mov	al,020H 		;allow access to palette
	out	dx,al
cEnd




;**	ifa - initialize font addressing
;
;	initialize EGA to allow read of font tables
;
;	Entry	none
;	Exit	EGA addressing set up
;	Returns none


cProc	ifa,<NEAR>,<>

cBegin
	mov	dx,rGraphics
	mov	ax,0204h
	cli
	out	dx,ax			;Read Map Select register
	mov	ax,0005h
	out	dx,ax			;Graphics Mode register
	mov	ax,0406h
	out	dx,ax			;Graphics Misc register
	mov	dx,rSequencer
	mov	ax,0C02h
	out	dx,ax			;Write Map Select
	mov	ax,0404h
	out	dx,ax			;Turn on odd/even
	sti
cEnd




;*	sdm - set display mode
;
;	sdm restores the EGA to the default graphics mode
;
;	Entry	none
;	Exit	default graphics display mode reestablished
;	Returns none
;	Calls	none
;	Uses	ax, dx


cProc	sdm,<NEAR>,<si,di,es>

cBegin
	mov	dx,rSequencer		;Enable all planes for writing
	mov	ah,C0+C1+C2+C3
	mov	al,rMapMask
	out	dx,ax
	mov	dx,rGraphics		;-->Graphics Controller
	mov	ax,0*256+rEnableSR	;Clear any set/reset enable
	out	dx,ax


;	Color Write Mode takes data from the host as a color.  D0 of
;	the host data is taken as the color for plane C0, D1 for plane
;	C1, etc.  The color for each plane is written to all bits of
;	the destination byte which are enabled in the BitMask Register.


	mov	ax,(mColorWrite+mDataRead)*256+rMode
	out	dx,ax


;	The Data Rotate register allows a boolean function between
;	the data from the host and the data in the EGA's latches
;	to occur.  It also allows the data from the host to be
;	rotated by some count before the operation occurs.


	mov	ax,(drSet*256)+rDataRotate
	out	dx,ax
cEnd




;*	sdp - set display page
;
;	sdp sets the current display page by setting the display offset
;	to the value in bx
;
;	Entry	bx = page offset
;	Exit	disply page address set
;	Returns none
;	Calls	wvs
;	Uses	ax,dx


cProc	sdp,<NEAR>,<>

cBegin
	call	wvs			;wait for vertical synch
	mov	dx,rCRTC		;(dx) = CRT controller
	mov	al,rSahr		;(al) = high address register
	mov	ah,bh			;(ah) = upper byte of address
	out	dx,ax
	mov	al,rSalr
	mov	ah,bl			;(ah) = low byte of address
	out	dx,ax
cEnd




;**	tfa - terminate font addressing
;
;	terminate font table addressing mode
;
;	Entry	none
;	Exit	EGA returned to normal addressing mode
;	Returns none


cProc	tfa,<NEAR>,<>

cBegin
	mov	cx,0e06h		;Setup color buffer with chaining
	mov	dx,rGraphics
	mov	ax,0004h
	cli
	out	dx,ax			;Read Map Select register
	mov	ax,1005h
	out	dx,ax			;Graphics Mode register
	mov	ax,0e06h		;Setup color buffer with chaining
	out	dx,ax			;Graphics Misc register
	mov	dx,rSequencer
	mov	ax,0302h
	out	dx,ax			;Write Map Select
	mov	ax,0004h
	out	dx,ax			;Turn off odd/even
	sti
cEnd




;*	wvs - wait for vertical synch
;
;	wvs waits for the leading edge of the vertical synch pulse
;
;	Entry	none
;	Exit	leading edge of vertical synch encountered
;	Returns none
;	Calls	none
;	Uses	al, dx


cProc	wvs,<NEAR>,<>

cBegin
	mov	dx,rAttread		;read feature control to reset

;	wait until not in vertical retrace

wvs1:	in	al,dx			;internal flipflop to force address
	test	al,mVRetrace		;check for vertical retrace
	jnz	wvs1			;if vertical retrace set

;	wait for start of vertical retrace

wvs2:	in	al,dx			;internal flipflop to force address
	test	al,mVRetrace		;check for vertical retrace
	jz	wvs2			;if vertical retrace not set
cEnd


sEnd	scrio
end
